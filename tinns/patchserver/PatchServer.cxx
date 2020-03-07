#include <cstring>
#include <sstream>
#include "patchserver/Includes.hxx"
#include "common/Includes.hxx"

const uint16_t PATCH_PORT = 7000;

struct PPatchState {
	enum State
	{
		PS_UNKNOWN,
		PS_CONNECTED,
		PS_HANDSHAKE0,
		PS_HANDSHAKE1,
		PS_VERSIONREQUEST,
		PS_GETPATCHORFILE,
		PS_SENDPATCH,
		PS_SENDFILE
	} mState;

	uint16_t mSerial;

	uint32_t mCurrentPatch;
	uint32_t mPatchOffset;
	uint32_t mPatchSize;
	std::FILE *mPatchFile;

	std::string mCurrentFile;
	uint32_t mFileOffset;
	uint32_t mFileSize;
	PFile *mSendFile;

	bool mWaitSend;	// wait-for-completition flag
	PPatchState()
	{
		mState = PS_UNKNOWN;
		mSerial = 0;
		mCurrentPatch = 0;
		mPatchOffset = 0;
		mPatchFile = 0;

		mCurrentFile = "";
		mFileOffset = 0;
		mFileSize = 0;
		mSendFile = 0;

		mWaitSend = false;
	};

	~PPatchState()
	{
		if(mPatchFile)
			std::fclose(mPatchFile);
		if(mSendFile)
			Filesystem->Close(mSendFile);
	}
};

// -------------------------------------------------------

PPatchServer::PPatchServer()
{
	//mNumClients = 1;
	mNumFileTransfers = 0;
}

PPatchServer::~PPatchServer()
{
    ServerSock->closeServer();

	for(PatchStateMap::iterator i=ClientStates.begin(); i!=ClientStates.end(); i++)
		delete i->second;
}

void PPatchServer::Start()
{
	uint16_t Port = Config->GetOptionInt("patchserver_port");
	if(Port==0)
		Port=PATCH_PORT;

	Console->LPrint("Starting Patchserver on port %i...", Port);
	if(ServerSock->open(Port))
	{
	    Console->LPrint(GREEN, BLACK, "Success");
	}
	else
	{
		Console->LPrint(RED, BLACK, "ERROR");
	}
	Console->LClose();
	ServerSock->settimeout(0, 10000);
}

void PPatchServer::Update()
{
    //ServerSock->update();

	if(ServerSock->newConnection())
	{
		int32_t clid = Server->NewClient();
		if(clid!=-1)
		{
			Console->Print(GREEN, BLACK, "Patchserver: client [%i] connected", clid);
			PClient *Client = Server->GetClient(clid);

			ConnectionTCP* tcpConn = ServerSock->getTCPConnection();
			Client->setTCPConnection(tcpConn);

			Console->Print("Client address: %s", Client->GetAddress());
			//++mNumClients;

			PPatchState *state = new PPatchState();
			ClientStates.insert(std::make_pair(Client, state));
			state->mState = PPatchState::PS_CONNECTED;
		} else
		{
			Console->Print("Patchserver: Client connection refused (server full?)");
		}
	}

	for(PatchStateMap::iterator i=ClientStates.begin(); i!=ClientStates.end();)
	{
		PClient *Client = i->first;
		PPatchState *State = i->second;
		// node gets erased in FinalizeClient, increment iterator now
		++i;
		if(!ProcessClient(Client, State))
		{
			FinalizeClient(Client, State);
		}
	}
}

void PPatchServer::FinalizeClient(PClient *Client, PPatchState *State)
{
	Console->Print(RED, BLACK, "Patchserver: client %s disconnected", Client->GetAddress());
	Client->PatchDisconnect();
	ClientStates.erase(Client);
	if(State->mPatchFile)
		--mNumFileTransfers;
	if(State->mSendFile)
		--mNumFileTransfers;
	delete State;
}

// completes pending packets before disconnect
void PPatchServer::FinalizeClientDelayed(PClient *Client, PPatchState *State)
{
	Console->Print("Patchserver: client %i is about to be disconnected", Client->GetIndex());
	State->mWaitSend = true;
}

void PPatchServer::ClientDisconnected(PClient *Client)
{
	PatchStateMap::iterator node = ClientStates.find(Client);
	if(node == ClientStates.end())
		return;

	PPatchState *State = node->second;
	FinalizeClient(Client, State);
}

bool PPatchServer::HandleHandshake(PClient *Client, PPatchState *State, const uint8_t *Packet, int32_t PacketSize)
{
	static const uint8_t HANDSHAKE1A[6]={0xfe, 0x03, 0x00, 0x80, 0x01, 0x73};

	ConnectionTCP *Socket = Client->getTCPConn();

	switch(State->mState)
	{
		case PPatchState::PS_HANDSHAKE0 :
		{
			if(PacketSize==6 && *(uint16_t*)&Packet[3]==0x0280 && Packet[5]==0x64)
			{
				Socket->write(HANDSHAKE1A, sizeof(HANDSHAKE1A));
				State->mState = PPatchState::PS_HANDSHAKE1;
			} else
			{
				Console->Print("Patchserver protocol error (PS_HANDSHAKE0): invalid packet [%04x]", *(uint16_t*)&Packet[3]);
				return false;
			}

			break;
		}

		case PPatchState::PS_HANDSHAKE1 :
		{
			if(PacketSize==6 && *(uint16_t*)&Packet[3]==0x0080 && Packet[5]==0x6c)
				State->mState = PPatchState::PS_VERSIONREQUEST;
			else
			{
				Console->Print("Patchserver protocol error (PS_HANDSHAKE1): invalid packet [%04x]", *(uint16_t*)&Packet[3]);
				return false;
			}
			break;
		}
		default:
			break;
	}

	return true;
}

bool PPatchServer::HandleVersionRequest(PClient *Client, PPatchState *State, const uint8_t *Packet, int32_t PacketSize)
{
	static uint8_t VERSIONPACKET[13]={0xfe, 0x0a, 0x00, 0x37, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	ConnectionTCP *Socket = Client->getTCPConn();
	if(PacketSize==9 && *(uint16_t*)&Packet[3]==0x007b)
	{
		State->mSerial = *(uint16_t*)&Packet[7];
		*(uint16_t*)&VERSIONPACKET[7]=State->mSerial;
		uint32_t ver = Config->GetOptionInt("server_version");
		*(uint32_t*)&VERSIONPACKET[9]=ver;
		Socket->write(VERSIONPACKET, 13);
		State->mState = PPatchState::PS_GETPATCHORFILE;
	} else
	{
		Console->Print("Patchserver protocol error (PS_VERSIONREQUEST): invalid packet [%04x]", *(uint16_t*)&Packet[3]);
		return false;
	}

	return true;
}

bool PPatchServer::HandleFileRequests(PClient *Client, PPatchState *State, const uint8_t *Packet, int32_t PacketSize)
{
	static uint8_t STARTPATCH[13]={0xfe, 0x0a, 0x00, 0x38, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static uint8_t STARTFILE[13]={0xfe, 0x0a, 0x00, 0x3b, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static uint8_t FILEERROR[9]={0xfe, 0x06, 0x00, 0x3d, 0x02, 0x00, 0x00, 0x00, 0x00};

	ConnectionTCP *Socket = Client->getTCPConn();
	// request patch
	if(PacketSize==13 && *(uint16_t*)&Packet[3]==0x007c)
	{
		int32_t nmax = Config->GetOptionInt("max_file_xfers");
		if(mNumFileTransfers>=nmax)
		{
			Console->Print("Patchserver: max file xfers exceed, killing client %i", Client->GetIndex());
			return false;
		}
		if(State->mPatchFile)
		{
			std::fclose(State->mPatchFile);
			State->mPatchFile=0;
			--mNumFileTransfers;
		}
		if(State->mSendFile)
		{
			Filesystem->Close(State->mSendFile);
			State->mSendFile=0;
			--mNumFileTransfers;
		}
		State->mSerial = *(uint16_t*)&Packet[7];
		State->mCurrentPatch = *(uint32_t*)&Packet[9];
		Console->Print("Patchserver: Patch request from client %i (v%i)", Client->GetIndex(), State->mCurrentPatch);
		if((bool)(State->mPatchSize = StartPatch(State)))
		{
			Console->Print("Patchserver: Patch is available, %d bytes", State->mPatchSize);
			*(uint16_t*)&STARTPATCH[7]=State->mSerial;
			*(uint32_t*)&STARTPATCH[9]=State->mPatchSize;
			Socket->write(STARTPATCH, 13);
			State->mState = PPatchState::PS_SENDPATCH;
		} else
		{
			Console->Print("Patchserver: Patch not available");
			*(uint16_t*)&FILEERROR[7]=State->mSerial;
			Socket->write(FILEERROR, 9);
			FinalizeClientDelayed(Client, State);
			State->mState=PPatchState::PS_UNKNOWN;
			return true;
		}
	} else
	// request file
	if(PacketSize > 9 && *(uint16_t*)&Packet[3]==0x004d)
	{
		int32_t nmax = Config->GetOptionInt("max_file_xfers");
		if(mNumFileTransfers>=nmax)
		{
			Console->Print("Patchserver: max file xfers exceed, killing client %i", Client->GetIndex());
			return false;
		}
		if(State->mPatchFile)
		{
			std::fclose(State->mPatchFile);
			State->mPatchFile=0;
			--mNumFileTransfers;
		}
		if(State->mSendFile)
		{
			Filesystem->Close(State->mSendFile);
			State->mSendFile=0;
			--mNumFileTransfers;
		}
		// request file
		State->mSerial = *(uint16_t*)&Packet[7];
		char fn[256];
		strncpy(fn, (const char*)&Packet[10], Packet[9]);
		fn[Packet[9]]=0;
		State->mCurrentFile = fn;

		Console->Print("Patchserver: File request from client %i (%s)", Client->GetIndex(), fn);
		if((bool)(State->mFileSize = StartFile(State)))
		{
			Console->Print("Patchserver: File %s is available, %d bytes", State->mCurrentFile.c_str(), State->mFileSize);
			*(uint16_t*)&STARTFILE[7]=State->mSerial;
			*(uint32_t*)&STARTFILE[9]=State->mFileSize;
			Socket->write(STARTFILE, 13);
			State->mState = PPatchState::PS_SENDFILE;
		} else
		{
			Console->Print("Patchserver: Requested file %s not available", State->mCurrentFile.c_str());
			*(uint16_t*)&FILEERROR[7]=State->mSerial;
			Socket->write(FILEERROR, 9);
			FinalizeClientDelayed(Client, State);
			State->mState=PPatchState::PS_UNKNOWN;
			return true;
		}
	} else
	// send patch data
	if(PacketSize==17 && *(uint16_t*)&Packet[3]==0x007d)
	{
		State->mSerial = *(uint16_t*)&Packet[7];
		State->mCurrentPatch = *(uint32_t*)&Packet[9];
		State->mPatchOffset = *(uint32_t*)&Packet[13];
		if(!SendPatchData(Client, State))
		{
			Console->Print("Patchserver: SendPatchData failed on client %i", Client->GetIndex());
			Console->Print("Patchserver: (probably due to garbage packets)");
			// state is undefined now, kill this client
			return false;
		}
	} else
	// send file data
	if(PacketSize > 13 && *(uint16_t*)&Packet[3]==0x00037)
	{
		State->mSerial = *(uint16_t*)&Packet[7];
		State->mFileOffset = *(uint32_t*)&Packet[9];
		if(!SendFileData(Client, State))
		{
			Console->Print("Patchserver: SendFileData failed on client %i", Client->GetIndex());
			Console->Print("Patchserver: (probably due to garbage packets)");
			// state is undefined now, kill this client
			return false;
		}
	} else
	{
		Console->Print("Patchserver protocol error (PS_GETPATCHORFILE): unknown packet");
		return false;
	}

	return true;
}

uint32_t PPatchServer::StartPatch(PPatchState *State)
{
	std::stringstream path;
	char patchname[13];
	snprintf(patchname, 13, "sp%06d.pat", State->mCurrentPatch);
	path << Config->GetOption("patches_path") << "/" << patchname << '\0';
	State->mPatchFile = std::fopen(path.str().c_str(), "rb");
	if(State->mPatchFile)
	{
		++mNumFileTransfers;
		fseek(State->mPatchFile, 0, SEEK_END);
		uint32_t size = ftell(State->mPatchFile);
		fseek(State->mPatchFile, 0, SEEK_SET);
		return size;
	}
	return 0;
}

bool PPatchServer::SendPatchData(PClient *Client, PPatchState *State) const
{
	if(!State->mPatchFile)
		return false;

	uint16_t size = Config->GetOptionInt("patch_packet_size");

	const int32_t BUFFERSIZE = 4082;

	size = std::min(BUFFERSIZE, std::max((int32_t)size, 64));
	static uint8_t Buffer[BUFFERSIZE+13];

	if(fseek(State->mPatchFile, State->mPatchOffset, SEEK_SET)!=0)
		return false;

	size = fread(&Buffer[13], 1, size, State->mPatchFile);
	Buffer[0]=0xfe;
	*(uint16_t*)&Buffer[1]=size+10;
	Buffer[3]=0x39;
	Buffer[4]=0x02;
	Buffer[5]=0x00;
	Buffer[6]=0x00;
	*(uint16_t*)&Buffer[7]=State->mSerial;
	*(uint32_t*)&Buffer[9]=size;
	return Client->getTCPConn()->write(Buffer, size+13)==size+13;
}

uint32_t PPatchServer::StartFile(PPatchState *State)
{
	// security checks: reject file paths containing ':', '..' or slashes/backslashes at the beginning
	if((State->mCurrentFile.find(':') != std::string::npos)
		|| (State->mCurrentFile.find("..") != std::string::npos)
		|| (State->mCurrentFile.find('/') == 0)
		|| (State->mCurrentFile.find('\\') == 0))
	{
		return 0;
	}

	//std::stringstream path;
	//path << Config->GetOption("file_path") << "/" << State->mCurrentFile << '\0';
	State->mSendFile = Filesystem->Open("", State->mCurrentFile.c_str(), Config->GetOption("file_path"));
	if(State->mSendFile)
	{
		++mNumFileTransfers;
		uint32_t size = State->mSendFile->GetSize();
		return size;
	}
	return 0;
}

bool PPatchServer::SendFileData(PClient *Client, PPatchState *State) const
{
	if(!State->mSendFile)
		return false;

	uint16_t size = Config->GetOptionInt("patch_packet_size");

	const int32_t BUFFERSIZE = 4082;

	size = std::min(BUFFERSIZE, std::max((int32_t)size, 1));
	static uint8_t Buffer[BUFFERSIZE+13];

	State->mSendFile->Seek(State->mFileOffset);

	size = State->mSendFile->Read(&Buffer[13], size);
	Buffer[0]=0xfe;
	*(uint16_t*)&Buffer[1]=size+10;
	Buffer[3]=0x3c;
	Buffer[4]=0x02;
	Buffer[5]=0x00;
	Buffer[6]=0x00;
	*(uint16_t*)&Buffer[7]=State->mSerial;
	*(uint32_t*)&Buffer[9]=size;
	return Client->getTCPConn()->write(Buffer, size+13)==size+13;
}

bool PPatchServer::ProcessClient(PClient *Client, PPatchState *State)
{
	static const uint8_t HANDSHAKE0A[6]={0xfe, 0x03, 0x00, 0x80, 0x03, 0x6b};

	if(!State)
	{
		PatchStateMap::iterator node = ClientStates.find(Client);
		if(node == ClientStates.end())
			return false;

		State = node->second;
	}

	ConnectionTCP *Socket = Client->getTCPConn();

	if(State->mWaitSend && Socket->getSendBufferSize()==0)
		return false;

	if(State->mState==PPatchState::PS_CONNECTED)
	{
	    Console->Print("Sending Handshake 0A");
		Socket->write(HANDSHAKE0A, sizeof(HANDSHAKE0A));
		//short unsigned int packet = (short unsigned int) HANDSHAKE0A;
		//Socket->write(htons(packet));
		State->mState = PPatchState::PS_HANDSHAKE0;
		Socket->flushSendBuffer();
	}

	int32_t PacketSize = 0;
	const uint8_t *Packet = Socket->read(&PacketSize);
	if(PacketSize > 0)
	{
		switch(State->mState)
		{
			case PPatchState::PS_HANDSHAKE1 :
			case PPatchState::PS_HANDSHAKE0 :
                Console->Print("Handling Handshake 0 and 1");
				return HandleHandshake(Client, State, Packet, PacketSize);

			case PPatchState::PS_VERSIONREQUEST :
                Console->Print("Handling Client Versionsrequest");
				return HandleVersionRequest(Client, State, Packet, PacketSize);

			case PPatchState::PS_GETPATCHORFILE :
			case PPatchState::PS_SENDPATCH :
			case PPatchState::PS_SENDFILE :
                Console->Print("Getpatchforfile, sendpatch, sendfile");
				return HandleFileRequests(Client, State, Packet, PacketSize);
			default:
				break;
		}
	}
	return true;
}
