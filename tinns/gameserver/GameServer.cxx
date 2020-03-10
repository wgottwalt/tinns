#include <cstring>
#include "gameserver/Includes.hxx"
#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

// TODO:
// - Deny login if char is already online (More information about the login procedure is necessary to do that)
// - Take main loop timeout setting from config file
// - Add Regex control to new char name validation
// - Add Check of Char offline (and unloaded) before deleting (from the char choosing i/f) if multiple login
//   allowed for the same account
//  - Check if adding to Client to Client manager shouldn't be done only one UDP connection done ?
//    => Risk of sending UDP chat on non-socket ????

PGameServer::PGameServer()
{
  mServerStartupTime = Time::nowTimeT();
  mNumClients = 0;
  MsgDecoder = new PUdpMsgDecoder();
}

PGameServer::~PGameServer()
{
  ServerSock->closeServer();
  delete MsgDecoder;
}

void PGameServer::Start()
{
  uint16_t Port = Config->GetOptionInt( "gameserver_port" );
  Console->LPrint( "Starting gameserver on port %i...", Port );

  if ( ServerSock->open( Port ) )
  {
    Console->LPrint( GREEN, BLACK, "Success" );
  }
  else
  {
    Console->LPrint( RED, BLACK, "Failed" );
  }
  Console->LClose();
  ServerSock->settimeout( 0, 10000 );

  SetGameTime(( 1000 / 10 )*3600*12 ); //Set initial time to 12:00 on 7 Jul 2789

  // Init random generator
  struct timespec tmpTime;
  if ( !clock_gettime( CLOCK_REALTIME, &tmpTime ) )
  {
    srandom(( uint32_t )tmpTime.tv_sec );
    Misc::InitRandom( tmpTime.tv_sec );
//Console->Print("Initializing random generator. First value is %d", random());
  }

  if ( Config->GetOptionInt( "gameserver_udpport_max" ) - Config->GetOptionInt( "gameserver_udpport_min" ) + 1 < Config->GetOptionInt( "maxclients" ) )
  {
    Console->Print( "%s UDP port range setting doesn't allow for the simultaneous max clients set in config", Console->ColorText( YELLOW, BLACK, "[Warning]" ) );
  }

}

void PGameServer::SetGameTime( uint32_t newtime )
{
  if ( clock_gettime( CLOCK_REALTIME, &mStartTime ) )
  {
    Console->Print( RED, BLACK, "[ERROR] Clock unavailable !!!" );
    perror( "clock_gettime CLOCK_REALTIME" );
    mStartTime.tv_sec = 0;
    mStartTime.tv_nsec = 0;
  }
  mBaseGameTime = newtime;
}

uint32_t PGameServer::GetGameTime()
{
  //const uint32_t TimeFactor = 375;
  const uint32_t TimeFactor = 1000;
  const uint32_t nsTimeFactor = 1000000000 / TimeFactor;

  struct timespec tmpTime;

  if ( mStartTime.tv_sec )
  {
    clock_gettime( CLOCK_REALTIME, &tmpTime );
    return ( mBaseGameTime + (( tmpTime.tv_sec - mStartTime.tv_sec ) * TimeFactor ) + ( tmpTime.tv_nsec / nsTimeFactor ) - ( mStartTime.tv_nsec / nsTimeFactor ) );
  }
  else
    return 0;
}

void PGameServer::Update()
{
  if ( ServerSock->newConnection() )
  {
    int clid = Server->NewClient();
    if ( clid != -1 )
    {
      Console->Print( GREEN, BLACK, "[Info] Gameserver: client [%i] connected", clid );
      PClient *Client = Server->GetClient( clid );
      if ( Client->getTCPConn() ) Console->Print( RED, BLACK, "WARNING: Client %d : TCP Socket NOT NULL before allocation.", Client->GetID() );
      ConnectionTCP* tcpConn = ServerSock->getTCPConnection();
      Client->setTCPConnection( tcpConn );

      //ConnectionUDP* udpConn = ServerSock->getUDPConnection();
      //Client->setUDPConnection(udpConn);

      Console->Print( "Client address: %s", Client->GetAddress() );
      ++mNumClients;

      PGameState *state = new PGameState();
      ClientStates.insert( std::make_pair( Client, state ) );
      state->TCP.mState = PGameState::TCP::GS_CONNECTED;

      // add the new connected client to the global clientmanager for further use in chat, etc...
      ClientManager->addClientToList( Client );
    }
    else
    {
      Console->Print( YELLOW, BLACK, "[Notice] Gameserver: Client connection refused (server full?)" );
    }
  }

  /*** temp check ***/
  for ( PClientMap::iterator it = ClientManager->getClientListBegin(); it != ClientManager->getClientListEnd(); it++ )
  {
    if ( !it->second )
    {
      Console->Print( RED, BLACK, "PANIC: NULL Client found in ClientManager Clients Map." );
    }
  }
  /*** end temp check ***/

  for ( GameStateMap::iterator i = ClientStates.begin(); i != ClientStates.end(); )
  {
    PClient *Client = i->first;
    PGameState *State = i->second;
    // node gets erased in FinalizeClient, increment iterator now
    ++i;
    if ( !ProcessClient( Client, State ) )
      FinalizeClient( Client, State );
  }
}

bool PGameServer::HandleHandshake( PGameState *State, const uint8_t *Packet, int PacketSize )
{
  //static const uint8_t HANDSHAKE1A[6]={0xfe, 0x03, 0x00, 0x80, 0x03, 0x68};
  //PGameSocket *Socket = Client->GetGameSocket();

  switch ( State->TCP.mState )
  {
    case PGameState::TCP::GS_HANDSHAKE0 :
    {
      if ( PacketSize == 6 && *( uint16_t* )&Packet[3] == 0x0080 && Packet[5] == 0x78 )
      {
        //FIXME: this packet seems to be unnecessary,
        // although it appears in traffic dumps
        // (causes clientside "Wrong protocol" errors)

        //Socket->Write(HANDSHAKE1A, 6);
        State->TCP.mState = PGameState::TCP::GS_AUTHENTICATE;
      }
      else
      {
        Console->Print( YELLOW, BLACK, "[Notice] Gameserver protocol error (GS_HANDSHAKE0): invalid packet [%04x]", *( uint16_t* )&Packet[3] );
        return ( false );
      }

      break;
    }
    default:
    {
      break;
    }
  }

  return true;
}

bool PGameServer::HandleAuthenticate( PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize )
{
  ConnectionTCP *Socket = Client->getTCPConn();

  if ( PacketSize > 20 && *( uint16_t* )&Packet[3] == 0x8084 )
  {
    // authentication method #1
    const uint8_t *Key = &Packet[5];  // password key
    uint16_t ULen = *( uint16_t* ) & Packet[16];  // username length
    uint16_t PLen = *( uint16_t* ) & Packet[18];  // password length
    char *UserName = ( char* ) & Packet[20]; // account name
    const uint8_t *PW = &Packet[20+ULen]; // encoded password

    // Safety controls
    if ( 15 + ULen + PLen > PacketSize )
    {
      Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_AUTHENTICATE): Exessive internal size fields" );
      return false;
    }
    if ( strnlen( UserName, ULen ) == ULen )
    {
      Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_AUTHENTICATE): non-terminated username field" );
      return false;
    }

    bool Failed = false;

    PAccount Account( UserName );
    if ( Account.GetID() == 0 )
    {
      Console->Print( YELLOW, BLACK, "[Info] Gameserver: Unknown user %s", UserName );
      Failed = true;
    }
    else if ( !Account.Authenticate( PW, PLen, Key ) )
    {
      Console->Print( YELLOW, BLACK, "[Info] Gameserver: User '%s': authentication failed", UserName );
      Failed = true;
    }

    if ( !Failed )
    {
      Console->Print( GREEN, BLACK, "[Info] Gameserver: User '%s' authentication successful", UserName );
      if ( Account.GetLevel() == PAL_BANNED )
      {
        Console->Print( YELLOW, BLACK, "[Info] User %s is banned, connection refused", UserName );
        // TODO: ban ip for an adjustable time span?
        Failed = true; // player is banned
      }

      if ( Account.GetLevel() == PAL_UNREGPLAYER || Account.GetLevel() == PAL_REGPLAYER )
      {
        if ( Server->GetNumClients() > Server->GetMaxClients() )
        {
          Console->Print( YELLOW, BLACK, "[Info] Server full, refusing connection from user '%s'", UserName );
          Failed = true; // server full
        }
      }
    }

    if ( Failed )
    {
      // TODO: is this packet correct here?
      uint8_t AUTHFAILED[15] = {0xfe, 0x0c, 0x00, 0x83, 0x86, 0x05, 0x00, 0x06, 0x00, 'E', 'R',
                           'R', 'O', 'R', 0
                          };
      // TODO: send actual reason instead of ERROR
      Socket->write( AUTHFAILED, 15 );
      FinalizeClientDelayed( Client, State );
      State->TCP.mState = PGameState::TCP::GS_UNKNOWN;
      Console->Print( YELLOW, BLACK, "[Info] Gameserver: User '%s' login refused", UserName );
    }
    else
    {
      Client->LoggedIn( &Account );
      uint8_t AUTHOK[28] = {0xfe, 0x19, 0x00, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                      };
      *( uint32_t* )&AUTHOK[5] = Account.GetID();
      Socket->write( AUTHOK, 28 );
      State->TCP.mState = PGameState::TCP::GS_GAMEDATA;
      Console->Print( GREEN, BLACK, "[Info] Gameserver: User '%s' logged in", UserName );
    }
  }
  else if ( PacketSize > 29 && *( uint16_t* )&Packet[3] == 0x0183 )
  {
    // authentication method #2, sent when game starts
    const uint8_t *Key = &Packet[13];  // password key
    uint16_t PLen = *( uint16_t* ) & Packet[25];  // password length
    uint16_t ULen = *( uint16_t* ) & Packet[27];  // username length
    char *UserName = ( char* ) & Packet[29]; // account name
    const uint8_t *PW = &Packet[29+ULen]; // encoded password

    // Safety controls
    if ( 24 + ULen + PLen > PacketSize )
    {
      Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_AUTHENTICATE): Exessive internal size fields" );
      return false;
    }
    if ( strnlen( UserName, ULen ) == ULen )
    {
      Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_AUTHENTICATE): non-terminated username field" );
      return false;
    }

    bool Failed = false;

    PAccount Account( UserName );
    if ( Account.GetID() == 0 )
    {
      Console->Print( YELLOW, BLACK, "[Info] Gameserver: Unknown user %s", UserName );
      Failed = true;
    }
    else if ( !Account.Authenticate( PW, PLen, Key ) )
    {
      Console->Print( YELLOW, BLACK, "[Info] Gameserver: User '%s': authentication failed", UserName );
      Failed = true;
    }

    if ( !Failed )
    {
      if ( Account.GetLevel() == PAL_BANNED )
      {
        Console->Print( YELLOW, BLACK, "[Info] User %s is banned, connection refused", UserName );
        // TODO: ban ip for an adjustable time span?
        Failed = true; // player is banned
      }

      if ( Account.GetLevel() == PAL_UNREGPLAYER || Account.GetLevel() == PAL_REGPLAYER )
      {
        if ( Server->GetNumClients() > ( Server->GetMaxClients() - Server->GetGMSlots() ) )
        {
          Console->Print( YELLOW, BLACK, "[Info] Server full, refusing connection from regular user '%s'", UserName );
          Failed = true; // server full for non-GM users
        }
      }
      else if ( Config->GetOptionInt( "require_validation" ) == 1 && Account.GetLevel() == PAL_UNREGPLAYER )
      {
        Console->Print( YELLOW, BLACK, "[Info] Rejecting connection from regular user '%s', account not activated yet", UserName );
        Failed = true;
      }
      else if ( Config->GetOptionInt( "minlevel" ) > Account.GetLevel() )
      {
        Console->Print( YELLOW, BLACK, "[Info] Rejecting connection from regular user '%s', insufficient level %d vs %d required", UserName, Account.GetLevel(), Config->GetOptionInt( "minlevel" ) );
        Failed = true;
      }
      else if ( Server->GetNumClients() > Server->GetMaxClients() )
      {
        Console->Print( YELLOW, BLACK, "[Info] Server full, refusing connection from privileged user '%s'", UserName );
        Failed = true; // server full even for GM users
      }
    }


    if ( !Failed )
    {
      int value = *( uint32_t* ) & Packet[21];//i think here we must read uint32_t instead of uint8_t
      uint32_t CharID = Account.GetCharIdBySlot( value );

      if ( Chars->LoadChar( CharID ) )
      {
        Client->SetCharID( CharID );
      }
      else
      {
        Failed = true;
      }
    }

    if ( Failed ) // something strange happened
      FinalizeClientDelayed( Client, State );
    else
    {
      Client->LoggedIn( &Account );
      /*uint8_t AUTHOK[28]={0xfe, 0x19, 0x00, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      *(uint32_t*)&AUTHOK[5]=Account->GetID();
      Socket->Write(AUTHOK, 28);*/
      //Client->SetRemoteUDPAddr(*(uint32_t*)&Packet[5], *(uint16_t*)&Packet[9]);
      State->TCP.mState = PGameState::TCP::GS_GETSTATUS;
      Console->Print( "Gameserver: User '%s' entered game (%08x:%04x)", UserName, *( uint32_t* )&Packet[5], *( uint16_t* )&Packet[9] );
      Client->SetRemoteUDPPort( *( int* )&Packet[9] );
    }
  }
  else
  {
    Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_AUTHENTICATE): invalid packet [%04x]", *( uint16_t* )&Packet[3] );
    return ( false );
  }

  return ( true );
}

bool PGameServer::HandleGameData( PClient *Client, PGameState *State, const uint8_t *Packet )
{
  static const uint8_t GAMEDATA[5] = {0xfe, 0x02, 0x00, 0x87, 0x3a};
  ConnectionTCP *Socket = Client->getTCPConn();

  if ( *( uint16_t* )&Packet[3] == 0x3787 )
  {
    Socket->write( GAMEDATA, 5 );
    State->TCP.mState = PGameState::TCP::GS_REQUESTCHARS;
  }
  else
  {
    Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_GAMEDATA): invalid packet [%04x]", *( uint16_t* )&Packet[3] );
    return ( false );
  }

  return ( true );
}

bool PGameServer::HandleRequestChars( PClient *Client, PGameState *State, const uint8_t *Packet )
{
  PAccount Account( Client->GetAccountID() );

  if ( !Account.GetID() )
    return false;

  ConnectionTCP *Socket = Client->getTCPConn();

  struct PCharList
  {
    uint16_t Unknown1;
    uint16_t NumSlots;
    uint16_t Unknown2;
  } CharList;

  PCharProfile CharEntry[MAX_CHARS_PER_ACCOUNT];

  const int CHARBASESIZE = 28;

  if ( *( uint16_t* )&Packet[3] == 0x8284 )
  {
    CharList.NumSlots = MAX_CHARS_PER_ACCOUNT;
    int NameLengths = 0;

    for ( int i = 0; i < MAX_CHARS_PER_ACCOUNT; i++ )
    {
      CharEntry[i].CharID = 0;
      CharEntry[i].Type = 0;
      CharEntry[i].Color0 = 0;
      CharEntry[i].Location = 1;
      CharEntry[i].Unknown1 = 0;
      CharEntry[i].Head = 0;
      CharEntry[i].Torso = 0;
      CharEntry[i].Legs = 0;
      CharEntry[i].Unknown3 = 1;
      CharEntry[i].Unknown4 = 1;
      CharEntry[i].Unknown5 = 1;
      CharEntry[i].Unknown6 = 1;
      CharEntry[i].Unknown7 = 1;
      CharEntry[i].Unknown8 = 0;
      CharEntry[i].Unknown9 = 0;
      CharEntry[i].Unknown10 = 0;
      CharEntry[i].Unknown11 = 0;
      CharEntry[i].Unknown12 = 0;

      CharEntry[i].in_use = false;
    }

    Chars->GetCharProfiles( Account.GetID(), CharEntry, MAX_CHARS_PER_ACCOUNT );

    for ( int i = 0; i < MAX_CHARS_PER_ACCOUNT; i++ )
    {
      if ( CharEntry[i].in_use )
      {
        NameLengths += CharEntry[i].NameLen;
      }
      else
      {
        CharEntry[i].CharID = 0xffffffff;
        CharEntry[i].NameLen = 0;
      }
    }

    uint8_t PacketHeader[5] = {0xfe, 0x00, 0x00, 0x83, 0x85};
    *( uint16_t* )&PacketHeader[1] = sizeof( uint16_t ) * 3 + ( MAX_CHARS_PER_ACCOUNT * CHARBASESIZE ) + NameLengths + 2;
    Socket->write( PacketHeader, 5 );

    CharList.Unknown1 = 0x0000;
    CharList.Unknown2 = CHARBASESIZE;
    Socket->write( CharList.Unknown1 );
    Socket->write( CharList.NumSlots );
    Socket->write( CharList.Unknown2 );

    for ( int i = 0; i < MAX_CHARS_PER_ACCOUNT; i++ )
    {
      Socket->write( CharEntry[i].CharID );
      Socket->write( CharEntry[i].Type );
      Socket->write( CharEntry[i].Color0 );
      Socket->write( CharEntry[i].Unknown1 );
      Socket->write( CharEntry[i].Head );
      Socket->write( CharEntry[i].Torso );
      Socket->write( CharEntry[i].Legs );
      Socket->write( CharEntry[i].Location );
      Socket->write( CharEntry[i].NameLen );
      Socket->write( CharEntry[i].Unknown3 );
      Socket->write( CharEntry[i].Unknown4 );
      Socket->write( CharEntry[i].Unknown5 );
      Socket->write( CharEntry[i].Unknown6 );
      Socket->write( CharEntry[i].Unknown7 );
      Socket->write( CharEntry[i].Unknown8 );
      Socket->write( CharEntry[i].Unknown9 );
      Socket->write( CharEntry[i].Unknown10 );
      Socket->write( CharEntry[i].Unknown11 );
      Socket->write( CharEntry[i].Unknown12 );

      if ( CharEntry[i].Name.length() > 0 )
      {
        Socket->write( CharEntry[i].Name.c_str() );
        Socket->write(( uint8_t )0 );
      }
    }

    State->TCP.mState = PGameState::TCP::GS_CHARLIST;
  }
  else
  {
    Console->Print( RED, BLACK, "Gameserver protocol error (GS_REQUESTCHARS): invalid packet [%04x]", *( uint16_t* )&Packet[3] );
    return ( false );
  }

  return ( true );
}

bool PGameServer::HandleCharList( PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize )
{
  static uint8_t Answer[10] = {0xfe, 0x07, 0x00, 0x83, 0x86, 0, 0, 0, 0, 0};
  ConnectionTCP *Socket = Client->getTCPConn();

  if ( PacketSize > 9 && *( uint16_t* )&Packet[3] == 0x8284 )
  {
    uint8_t Mode = Packet[9];

    switch ( Mode )
    {
      case 6: // request list
        return ( HandleRequestChars( Client, State, Packet ) );

      case 5: // validate name
      {
        if ( PacketSize < 31 )
          return ( false );

        // check for valid name string
        bool ValidString = false;

        for ( int i = 30; i < PacketSize; i++ )
          if ( Packet[i] == 0 )
          {
            ValidString = true;
            break;
          }

        const char *Name = ( char* ) & Packet[30];
        if ( ValidString )
        {
          ValidString = PChar::IsCharnameWellFormed( Name );
        }

        if ( ValidString )
        {
          if ( Chars->CharExist( std::string( Name ) ) )
          {
            ValidString = false;
          }
        }

        if ( ValidString )
          Answer[5] = 1; // ok
        else
          Answer[5] = 2; // 2..6 => 'char name already in use!'

        // Answer[5] = 0; // => 'unknown error'
        Socket->write( Answer, 10 );
        return ( true );
      }

      case 3: // delete char
      {
        PAccount Acc( Client->GetAccountID() );
        uint8_t Num = Packet[PacketSize-1];

        if ( Acc.GetID() )
        {
          uint32_t CharID = Acc.GetCharIdBySlot( Num );

          // Also check that char is out of game
          if ((CharID != 0) && (Chars->GetChar(CharID) == nullptr))
          {
            char query[100];
            snprintf( query, 100, "DELETE FROM characters WHERE c_id = %d LIMIT 1", CharID );
            if ( MySQL->GameQuery( query ) )
              Console->Print( RED, BLACK, "[Notice] Char %d not deleted!", CharID );
            else
            {
              Console->Print( GREEN, BLACK, "[Info] Char %d deleted!", CharID );

              snprintf( query, 100, "DELETE FROM buddy_list WHERE bud_charid = %d", CharID );
              if ( MySQL->GameQuery( query ) )
                Console->Print( YELLOW, BLACK, "[Notice] Char %d's buddy list not removed!", CharID );

              snprintf( query, 100, "DELETE FROM genrep WHERE g_charid = %d", CharID );
              if ( MySQL->GameQuery( query ) )
                Console->Print( YELLOW, BLACK, "[Notice] Char %d's genrep list not removed!", CharID );

              snprintf( query, 100, "DELETE FROM inventory WHERE inv_charid = %d", CharID );
              if ( MySQL->GameQuery( query ) )
                Console->Print( YELLOW, BLACK, "[Notice] Char %d's inventory not removed!", CharID );

              Appartements->DeleteCharAppartements( CharID );
            }
          }
          else
            return false;
        }
        return ( true );
      }

      case 7: // create char
      {
        if ( PacketSize < 64 )
          return ( false );

        uint32_t Slot = * ( uint32_t* ) & Packet[30];
        //uint32_t nClass =* (uint32_t*)&Packet[34]; // Not used - indirectly redundant with Profession
        uint32_t Profession = * ( uint32_t* ) & Packet[38];
        uint32_t Gender = * ( uint32_t* ) & Packet[42];
        uint32_t Head = * ( uint32_t* ) & Packet[46];
        uint32_t Torso = * ( uint32_t* ) & Packet[50];
        uint32_t Legs = * ( uint32_t* ) & Packet[54];
        uint32_t Faction = * ( uint32_t* ) & Packet[58];
        uint8_t NameLen = Packet[62];
        uint8_t NZSNb = Packet[63];

        char TempName[256];
        std::strncpy( TempName, ( const char* )&Packet[64], NameLen );
        TempName[NameLen] = 0;

        Answer[5] = 2; // return error if char creation fails

        if ( PChar::IsCharnameWellFormed( TempName ) )
        {
          // check for already used char name - should not happen though
          if ( ! Chars->CharExist( std::string( TempName ) ) )
          {
            PAccount Acc( Client->GetAccountID() );
            PChar* nChar = new PChar();

            if ( nChar->CreateNewChar( Acc.GetID(), TempName, Gender, Profession, Faction,
                                       Head, Torso, Legs, NZSNb, ( const char* )&Packet[64+NameLen], Slot ) )
            {
              Answer[5] = 1; // return success
            }
            delete nChar;
          }
        }

        Socket->write( Answer, 10 );
        return ( true );
      }
    }
  }
  else
  {
    Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_CHARLIST): invalid packet [%04x]", *( uint16_t* )&Packet[3] );

    return ( false );
  }

  return true;
}

bool PGameServer::HandleGetStatus( PClient *Client, PGameState *State, const uint8_t *Packet )
{
  ConnectionTCP *Socket = Client->getTCPConn();

  if ( *( uint16_t* )&Packet[3] == 0x3787 )
  {
    static const uint8_t STATUS[9] = {0xfe, 0x06, 0x00, 0x87, 0x3a, 0x11, 0x00, 0x00, 0x00};

    Socket->write( STATUS, 9 );
    State->TCP.mState = PGameState::TCP::GS_GAMEINFO;
  }
  else
  {
    Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_GETSTATUS): invalid packet [%04x]", *( uint16_t* )&Packet[3] );

    return ( false );
  }

  return ( true );
}

bool PGameServer::HandleGameInfo( PClient *Client, PGameState *State, const uint8_t *Packet )
{
// Console->Print("Inside HandleGameInfo");//NEW added

  static uint8_t GameInfo[31] = {0xfe, 0x1c, 0x00, 0x83, 0x05, // header
                            0x00, 0x00, 0x00, 0x00, // account id
                            0x00, 0x00, 0x00, 0x00, // char id
                            0x00, 0x00, 0x00, 0x00, // udp ip
                            0x00, 0x00,   // udp port
                            0x00, 0x00, 0x00, 0x00, // unknown
                            0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, // session key
                           };

  ConnectionTCP *Socket = Client->getTCPConn();

  if ( *( uint16_t* )&Packet[3] == 0x3c87 )
  {
    //int PortFix = Config->GetOptionInt("debug_mode");

    ConnectionUDP* udpConn = ServerSock->getUDPConnection(Misc::Net::ip4StringToUint32(Client->GetAddress()), Client->GetRemoteUDPPort());
    Client->setUDPConnection( udpConn );
    if ( !udpConn )
    {
      Console->Print( RED, BLACK, "Client %d: UDP port setup failed", Client->GetID() );
      ClientDisconnected( Client );
    }

    uint16_t Port = Client->getUDPConn()->getPort();

    if ( Port == 0 )
      Console->Print( RED, BLACK, "Client->OpenUDP() failed" );


    /* if(PortFix == 1) // removed, no more use
    {
     Port = Config->GetOptionInt("useudpport");
     Console->Print(YELLOW, BLACK, "UDP Port set to non-standard for debugging!");
    }; */

    uint32_t IP;
    std::string IPServerString;
// use [server_nat_ip] for server if client is NOT on [no_nat_net] (and [no_nat_net]!=0)
    if ( strcmp( Config->GetOption( "no_nat_net" ).c_str(), "0" ) && strncmp( Client->GetAddress(), Config->GetOption( "no_nat_net" ).c_str(), strlen( Config->GetOption( "no_nat_net" ).c_str() ) ) )
    {
      IPServerString = Config->GetOption( "server_nat_ip" );
    }
    else // else client is "local" so use [server_ip]
    {
      IPServerString = Config->GetOption( "server_ip" );
    }
    IP = Misc::Net::ip4StringToUint32(IPServerString);
//Console->Print("IP-1 %d", IP);
    if ( IP == 0 )
      IP = 0x0100007f;
//Console->Print("IP-2 %d", IP);
    *( uint32_t* )&GameInfo[13] = IP;
    *( uint16_t* )&GameInfo[17] = Port;
    Console->Print( GREEN, BLACK, "[Info] Using UDP %s:%d on server", IPServerString.c_str(), Port );

    *( uint32_t* )&GameInfo[5] = Client->GetAccountID();
    *( uint32_t* )&GameInfo[9] = Client->GetCharID();
    Console->Print( GREEN, BLACK, "[Info] Serving char id :%d", Client->GetCharID() );

    Socket->write( GameInfo, 31 );
    Socket->flushSendBuffer();

    static const uint8_t READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
    Socket->write( READY, 7 );

    State->TCP.mState = PGameState::TCP::GS_INGAME;
    State->UDP.mState = PGameState::UDP::GUS_SYNC0;
//Console->Print("Sync Reset");
    Client->ResetTransactionID();

    // Mark char as Online
    PChar *Char = Client->GetChar();
    Char->SetOnlineStatus( true ); //Also using this info to check if Char may have to be saved at client disconnect
    Client->ChangeCharLocation( Char->GetLocation(), true );

    // hello-message from server..
    /*
    std::string serverName = Config->GetOption("server_name");
    std::string helloMessage = "Welcome to " + serverName + " - A TinNS Neocron Server.";
    char* message = (char*) helloMessage.c_str();
    Chat->send(Client, CHAT_DIRECT, "System", message, false);
    */

    bool SendBC = false;
    if ( Config->GetOptionInt( "broadcast_new" ) == 1 )
    {
      if ( Config->GetOptionInt( "broadcast_new_hidestaff" ) == 1 )
      {
        if ( Client->GetAccountLevel() > PAL_REGPLAYER )
          SendBC = false;
        else
          SendBC = true;
      }
      else
      {
        SendBC = true;
      }
    }
    if ( SendBC == true )
    {
      std::string playerName = Chars->GetChar( Client->GetCharID() )->GetName();
      std::string serverName = Config->GetOption( "server_name" );
      std::string helloMessage = "Hello " + playerName + "! Welcome to " + serverName + " - A TinNS Neocron Server.";
      char* message = ( char* ) helloMessage.c_str();

      Chat->sendOOCBroadcast( message );
    }
    //Console->Print("UDP Setup: %s", nlGetErrorStr(nlGetError()));
  }
  else
  {
    Console->Print( RED, BLACK, "[Notice] Gameserver protocol error (GS_GAMEINFO): invalid packet [%04x]", *( uint16_t* )&Packet[3] );
    return ( false );
  }

  return ( true );
}

bool PGameServer::HandleGame( PClient *Client, PGameState *State )
{
  //PGameSocket *Socket = Client->GetGameSocket();
  ConnectionUDP *UDPSocket = Client->getUDPConn();
  //Console->Print("Clientadress %s", Client->GetAddress());

  //int Size = 0;
  PMessage* NewMsg = UDPSocket->GetMessage();
  if ( NewMsg && NewMsg->GetSize() )
  {

    MsgDecoder->Init( NewMsg, Client, State ); // State used temporarily
    do
    {
      if ( MsgDecoder->Analyse() )
      {
        if ( gDevDebug && MsgDecoder->IsTraceKnownMsg() )
          Console->Print( "%s Client[%d] msg: %s", Console->ColorText( GREEN, BLACK, "[Debug]" ), Client->GetID(), MsgDecoder->GetName().c_str() );

        if ( MsgDecoder->IsActionReady() )
        {
          MsgDecoder->DoAction();
        }
      }
      else if ( MsgDecoder->IsError() )
      {
        Console->Print( YELLOW, BLACK, "[Info] Client[%d] Decoding error: %s", Client->GetID(), MsgDecoder->GetError().c_str() );
      }
//      else if (MsgDecoder->GetState() == DECODE_UNKNOWN)
      else if ( gDevDebug && ( MsgDecoder->GetState() == DECODE_UNKNOWN ) && MsgDecoder->IsTraceUnknownMsg() )
      {
        Console->Print( "%s Client[%d] Unknown msg: %s", Console->ColorText( YELLOW, BLACK, "[Info]" ), Client->GetID(), MsgDecoder->GetName().c_str() );
      }

//      if (MsgDecoder->IsTraceDump())
      if ( gDevDebug && MsgDecoder->IsTraceDump() )
      {
        MsgDecoder->DumpMsg();
      }

    }
    while ( MsgDecoder->MoreSubMsg() );
  }

  if ( NewMsg )
  {
    delete NewMsg;
  }

  return ( true );
}

bool PGameServer::ProcessClient( PClient *Client, PGameState *State )
{
  static const uint8_t HANDSHAKE0A[6] = {0xfe, 0x03, 0x00, 0x80, 0x01, 0x66};

  if ( !State )
  {
    GameStateMap::iterator node = ClientStates.find( Client );
    if ( node == ClientStates.end() )
      return ( false );

    State = node->second;
  }

  ConnectionTCP *Socket = Client->getTCPConn();
  if ( !Socket ) Console->Print( RED, BLACK, "PANIC: Client %d : TCP Socket is NULL !!!", Client->GetID() );
  if ( State->TCP.mWaitSend && Socket->getSendBufferSize() == 0 )
    return ( false );

  if ( State->TCP.mState == PGameState::TCP::GS_CONNECTED )
  {
    Socket->write( HANDSHAKE0A, 6 );
    State->TCP.mState = PGameState::TCP::GS_HANDSHAKE0;
  }

  if ( State->TCP.mState == PGameState::TCP::GS_INGAME )
  {
    return ( HandleGame( Client, State ) );
  }
  else
  {
    int PacketSize = 0;
    const uint8_t *Packet = Socket->read( &PacketSize );

    if ( PacketSize > 0 )
    {
      switch ( State->TCP.mState )
      {
        case PGameState::TCP::GS_HANDSHAKE0:
          return ( HandleHandshake( State, Packet, PacketSize ) );

        case PGameState::TCP::GS_AUTHENTICATE:
          return ( HandleAuthenticate( Client, State, Packet, PacketSize ) );

        case PGameState::TCP::GS_GAMEDATA:
          return ( HandleGameData( Client, State, Packet ) );

        case PGameState::TCP::GS_REQUESTCHARS:
          return ( HandleRequestChars( Client, State, Packet ) );

        case PGameState::TCP::GS_CHARLIST:
          return ( HandleCharList( Client, State, Packet, PacketSize ) );

        case PGameState::TCP::GS_GETSTATUS:
          return ( HandleGetStatus( Client, State, Packet ) );

        case PGameState::TCP::GS_GAMEINFO:
          return ( HandleGameInfo( Client, State, Packet ) );
        default:
          break;
      }
    }
  }

  return ( true );
}

void PGameServer::ClientDisconnected( PClient *Client )
{
  GameStateMap::iterator node = ClientStates.find( Client );

  if ( node == ClientStates.end() )
    return;

  PGameState *State = node->second;
  FinalizeClient( Client, State );
}

void PGameServer::UDPStreamClosed( PClient *Client )
{
  GameStateMap::iterator node = ClientStates.find( Client );

  if ( node == ClientStates.end() )
    return;

  PGameState *State = node->second;
  FinalizeClient( Client, State );
}

void PGameServer::FinalizeClient( PClient *Client, PGameState *State )
{
  Console->Print( GREEN, BLACK, "[Info] Gameserver: client %s disconnected", Client->GetAddress() );

  // Isnt working somehow. I assume that mID and mAccount is deleted already.
  // Mark char as Offline
  //PChar *Char = Chars->GetChar(Client->GetCharID());
  //Char->SetOnlineStatus(false);

  // delete client from clientmanager list => Do it before we remove network access
  ClientManager->deleteClientFromList( Client->GetLocalID() );

  Client->GameDisconnect();
  ClientStates.erase( Client );
  delete State;
  --mNumClients;

}

void PGameServer::FinalizeClientDelayed( PClient *Client, PGameState *State )
{
  Console->Print( GREEN, BLACK, "[Info] Gameserver: client %s is about to be disconnected", Client->GetAddress() );
  State->TCP.mWaitSend = true;
}

PGameState* PGameServer::GetClientState( PClient* nClient )
{
  GameStateMap::iterator node = ClientStates.find( nClient );

  if ( node == ClientStates.end() )
    return nullptr;
  else
    return node->second;
}
