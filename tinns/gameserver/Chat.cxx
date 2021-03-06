#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

//  TODO:- Add "hooks" to process chatevents everywhere in the source <-- kind of done. you can use the global PChat Instance.
//         Check:
//              Chat->send(PClient* receiver, const uint8_t* Channel, const char* AuthorNickName, char* text, bool debugOut=false);
//         Example:
//              Chat->send(receiverClient, CHAT_DIRECT, Chars->GetChar(authorClient->GetCharID())->GetName().c_str(), text);

PChat::PChat()
{
}

PChat::~PChat()
{
}

/*
DONE    void sendBuddy(PClient* author, char* text, bool debugOut=false);
DONE    void sendLocal(PClient* author, char* text, bool debugOut=false);
NEED CLANDATA    void sendClan(PClient* author, char* text, bool debugOut=false);
NEED TEAMDATA    void sendTeam(PClient* author, char* text, bool debugOut=false);
DONE    void sendDirect(PClient* author, PClient* receiver, char* text, bool debugOut=false);
DONE    void sendZone(PClient* author, char* text, bool debugOut=false);
DONE    void sendFrak(PClient* author, char* text, bool debugOut=false);
DONE    void sendTradeCS(PClient* author, char* text, bool debugOut=false);
DONE    void sendTradeMB(PClient* author, char* text, bool debugOut=false);
DONE    void sendTradeNC(PClient* author, char* text, bool debugOut=false);
DONE    void sendTradeTH(PClient* author, char* text, bool debugOut=false);
DONE    void sendTradeWL(PClient* author, char* text, bool debugOut=false);
DONE    void sendOOC(PClient* author, char* text, bool debugOut=false);
DONE    void sendHelp(PClient* author, char* text, bool debugOut=false);
DONE    void sendClanSearch(PClient* author, char* text, bool debugOut=false);
DONE    void sendServicesCS(PClient* author, char* text, bool debugOut=false);
DONE    void sendServicesMB(PClient* author, char* text, bool debugOut=false);
DONE    void sendServicesNC(PClient* author, char* text, bool debugOut=false);
DONE    void sendServicesTH(PClient* author, char* text, bool debugOut=false);
DONE    void sendServicesWL(PClient* author, char* text, bool debugOut=false);
DONE    void sendTeam10(PClient* author, char* text, bool debugOut=false);
DONE    void sendTeam30(PClient* author, char* text, bool debugOut=false);
DONE    void sendTeam50(PClient* author, char* text, bool debugOut=false);
DONE    void sendTeam70(PClient* author, char* text, bool debugOut=false);
DONE    void sendAdmin(PClient* author, char* text, bool debugOut=false);
DONE    void sendGM(PClient* author, char* text, bool debugOut=false);
*/


void PChat::sendBuddy(PClient* author, const char* text, bool debugOut)
{
    PChar* authorChar = Chars->GetChar(author->GetCharID());
    // send the message to all Buddys in list
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second && authorChar->IsBuddy(it->second->GetCharID()) == true)
        {
            PClient* receiver = it->second;
            //Console->Print("DEBUG: Buddychat - Sending msg to %s", Chars->GetChar(receiver->GetCharID())->GetName().c_str());
            send(receiver, CHAT_BUDDY, authorChar->GetName().c_str(), text, debugOut);
        }
    }
}

void PChat::sendConnectedList(PClient* receiver, bool debugOut)
{
    const char* text = "Connected Players are:]";
    send(receiver, CHAT_DIRECT, "[System", text, debugOut);

    int counter = 1;

    // send the list of currently connected players to receiver
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        PChar* receiverChar = Chars->GetChar(it->second->GetCharID());
        send(receiver, CHAT_DIRECT, (receiverChar ? receiverChar->GetName().c_str() : "*"), std::to_string(counter).c_str(), debugOut);

        counter++;
    }
}


void PChat::sendFrak(PClient* author, const char* text, bool debugOut)
{
    // send the message to all clients that have same FactionID
    PChar* authorChar = Chars->GetChar(author->GetCharID());
    uint32_t FID = authorChar->GetFaction(); // get LocationID of author

    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            PClient* receiver = it->second;
            PChar* receiverChar = Chars->GetChar(receiver->GetCharID());
            if(receiverChar && (receiverChar->GetFaction() == FID))
            {
                if(chanEnabled(receiver, C_FRAK) == true)
                    send(receiver, CHAT_FRAK, authorChar->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendZone(PClient* author, const char* text, bool debugOut)
{
    // send the message to all clients that have same ZoneID
    PChar* authorChar = Chars->GetChar(author->GetCharID());
    uint32_t ZID = authorChar->GetLocation(); // get LocationID of author

    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            PClient* receiver = it->second;
            PChar* receiverChar = Chars->GetChar(receiver->GetCharID());
            if(receiverChar && (receiverChar->GetLocation() == ZID))
            {
                if(chanEnabled(receiver, C_ZONE) == true)
                    send(receiver, CHAT_ZONE, authorChar->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendLocal(PClient* author, const char* text, bool debugOut)
{
    PChar* authorChar = Chars->GetChar(author->GetCharID());
    uint32_t ZID = authorChar->GetLocation(); // get LocationID of author

    // send the message to all clients that are in Area (Radius = X (needs to be defined somewhere!))
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            PClient* receiver = it->second;
            PChar* receiverChar = Chars->GetChar(receiver->GetCharID());
            if(receiverChar && (receiverChar->GetLocation() == ZID))
            {
                uint16_t distance = Misc::Math::distanceApprox((authorChar->Coords).mX, (authorChar->Coords).mY, (authorChar->Coords).mZ, (receiverChar->Coords).mX, (receiverChar->Coords).mY, (receiverChar->Coords).mZ);
                if(distance < LOCALCHAT_MAXDISTANCE)
                {
                    //sendLocalchat(receiver, author, text, debugOut); // Doesnt work!
                    send(receiver, CHAT_LOCAL, authorChar->GetName().c_str(), text, debugOut);
                }
            }
        }
    }
}

void PChat::sendGM(PClient* author, const char* text, bool debugOut)
{
    if(author->GetAccountLevel() >= PAL_GM) // Only send GM> chat when user is an Gamemaster or higher
    {
        // send the message to all GameMasters.
        for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
        {
            if(author != it->second) // if its not the client, that send the message to the server
            {
                if(it->second) // only send if the client is existing!
                {
                    PClient* receiver = it->second;
                    if(receiver->GetAccountLevel() >= PAL_GM) // Only send GM chat if RECEIVER is GM or higher
                        send(receiver, CHAT_GM, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
                }
            }
        }
    }
}

void PChat::sendGMAdmin(PClient* author, const char* text, bool debugOut)
{
    if(author->GetAccountLevel() >= PAL_GM) // Only send GM> chat when user is an Gamemaster or higher
    {
        // send the message to ALL users online
        for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
        {
            if(author != it->second) // if its not the client, that send the message to the server
            {
                if(it->second) // only send if the client is existing!
                {
                    PClient* receiver = it->second;
                    send(receiver, CHAT_GMADMIN, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
                }
            }
        }
    }
}

void PChat::sendAdmin(PClient* author, const char* text, bool debugOut)
{
    if(author->GetAccountLevel() >= PAL_ADMIN) // Only send ADMIN> chat when user is an serveradmin
    {
        // send the message to ALL users online
        for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
        {
            if(author != it->second) // if its not the client, that send the message to the server
            {
                if(it->second) // only send if the client is existing!
                {
                    PClient* receiver = it->second;
                    send(receiver, CHAT_ADMIN, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
                }
            }
        }
    }
}

void PChat::sendBroadcast(const char* text, bool debugOut)
{
    // send the message to ALL users online
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(it->second) // only send if the client is existing!
        {
            PClient* receiver = it->second;
            send(receiver, CHAT_ADMIN, "Server", text, debugOut);
        }
    }
}

void PChat::sendOOCBroadcast(const char* text, bool debugOut)
{
    // send the message to ALL users online
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(it->second) // only send if the client is existing!
        {
            PClient* receiver = it->second;
            if(chanEnabled(receiver, C_OOC) == true)
                send(receiver, CHAT_OOC, "Server", text, debugOut);
        }
    }
}

void PChat::sendClan(PClient* author, const char* text, bool debugOut)
{
    /**
        NOT ABLE TO IMPLEMENT THIS CHATTYPE YET, ITS SUPERGLOBAL TILL THEN
    **/
    // send the message to all clients that have same ClanID
    PChar* authorChar = Chars->GetChar(author->GetCharID());

//    int ClanID = authorChar->getClanID(); // get clanID of author

    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second)
        {
            PClient* receiver = it->second;
            PChar* receiverChar = Chars->GetChar(receiver->GetCharID());
            if(receiverChar /*&& (receiverChar->getClanID() == ClanID)*/)
            {
                send(receiver, CHAT_CLAN, authorChar->GetName().c_str(), text, debugOut);
            }
        }
    }

}

void PChat::sendTeam(PClient* author, const char* text, bool debugOut)
{
    /**
        NOT ABLE TO IMPLEMENT THIS CHATTYPE YET, ITS SUPERGLOBAL TILL THEN
    **/
    // send the message to all clients that have same TeamID

    PChar* authorChar = Chars->GetChar(author->GetCharID());

    //int TeamID = authorChar->getTeamID(); // get TeamID of author

    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second)
        {
            PClient* receiver = it->second;
            PChar* receiverChar = Chars->GetChar(receiver->GetCharID());
            if(receiverChar /*&& (receiverChar->getTeamID() == TeamID)*/)
            {
                send(receiver, CHAT_TEAM, authorChar->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendPlayerDirect(PClient* author, const char* text, uint32_t destination, bool debugOut)
{
    bool tmpTargetOnline = false;
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        PClient* receiver = it->second;
        PChar* receiverChar = Chars->GetChar(receiver->GetCharID());
        if(receiverChar && (receiver->GetCharID() == destination))       
        {
            tmpTargetOnline = true;
            sendDirect(author, receiver, text, debugOut);
        }
    }
    if(tmpTargetOnline == false)
    {
        ConnectionTCP *Socket = author->getTCPConn();
        uint8_t DirectTargetNotOnline[] = {0xFE, 0x07, 0x00, 0x83, 0x18, 0x01, 0x81, 0x54, 0x00, 0x00};

        Socket->write(DirectTargetNotOnline, sizeof(DirectTargetNotOnline));
        Socket->flushSendBuffer();
    }

    if(debugOut == true)
    {
        if(tmpTargetOnline == false)
            Console->Print("[DEBUG] Requested target CharID %d is not online", destination);
        else
            Console->Print("[DEBUG] CharID %d found and message transmitted!", destination);
    }
}

void PChat::sendDirect(PClient* author, PClient* receiver, const char* text, bool debugOut)
{
    PChar* authorChar = Chars->GetChar(author->GetCharID());

    char *DChatPacket;
    unsigned int packetsize = 0, c;
    int LenText, LenNick, fpp, d, e, loopout;

    ConnectionTCP *Socket = receiver->getTCPConn();

    uint8_t BasicDirectPacket[] = {0xFE, 0x20, 0x00, 0x83, 0x17, 0xB7, 0x5F, 0x00, 0x00, 0x0C, 0x04, 0x00};

    LenText = LenNick = fpp = 0;

    do {
      LenText++;
    } while(text[LenText] != '\0');

    do {
      LenNick++;
    } while(authorChar->GetName().c_str()[LenNick] != '\0');

    loopout = 0;

    packetsize = sizeof(BasicDirectPacket) + LenText + LenNick;

    if(debugOut == true) {
       Console->Print("Sizeof(TextToSend): %d || Sizeof(Nick): %d", LenText, LenNick);
       Console->Print("Whole size: %d", packetsize);
    }

    DChatPacket = new char [packetsize];

    // Copy basic packet into final packet
    for(c=0;c<sizeof(BasicDirectPacket);c++) {
       DChatPacket[fpp] = BasicDirectPacket[c];
       fpp++;
    }

    // Copy Nickname into final packet
    for(d=0;d<LenNick;d++) {
       DChatPacket[fpp] = authorChar->GetName().c_str()[d];
       fpp++;
    }

    // Copy Text into final packet
    for(e=0;e<LenText;e++) {
       DChatPacket[fpp] = text[e];
       fpp++;
    }

    // Change Lenght bytes in final packet
    DChatPacket[1] = packetsize - 3;
    DChatPacket[9] = LenNick;

    if(debugOut == true) {
        unsigned int debugout;
        for(debugout=0;debugout<packetsize;debugout++) {
           Console->Print("Byte %d: %#x", debugout, DChatPacket[debugout]);
        }
    }

    // Sending direct chat packet and removing dynamic array
    Socket->write(DChatPacket, packetsize);
    Socket->flushSendBuffer();
    delete[] DChatPacket;
}

void PChat::sendLocalchat(PClient* receiver, PClient* author, const char* text, bool debugOut)
{
    return; // IncreaseUDP could cause OOO here. Since this function is doing nothing, we disabled it
    char *LocalChatPacket;
    int overallsize = 0, LenText = 0;
    uint8_t BasicLocal[] = { 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x1B };

    // Get size of Text to send
    do {
      LenText++;
    } while(text[LenText] != '\0');

    // Calculate packetsize
    overallsize = sizeof(BasicLocal) + LenText + 1;

    if(debugOut == true)
       Console->Print("Whole size: %d", overallsize);

    LocalChatPacket = new char [overallsize];

    // Copy basic packet into final packet
    int fpp = 0;
    for(unsigned int c = 0; c < sizeof(BasicLocal); c++) {
       LocalChatPacket[fpp] = BasicLocal[c];
       fpp++;
    }

    // Copy Text into final packet
    for(int e = 0; e < LenText; e++) {
       LocalChatPacket[fpp] = text[e];
       fpp++;
    }

    // Terminate string
    LocalChatPacket[fpp] = 0x00;

    // Add UdpID, SessionID, lenght and local charid
    receiver->IncreaseUDP_ID();
    *(uint16_t*)&LocalChatPacket[1] = receiver->GetUDP_ID();     // UDP
    *(uint16_t*)&LocalChatPacket[3] = receiver->GetSessionID();  // Session
    *(uint8_t*)&LocalChatPacket[5] = overallsize - 6;            // Packetlen
    *(uint16_t*)&LocalChatPacket[7] = receiver->GetUDP_ID();     // 2nd UDP
    *(uint16_t*)&LocalChatPacket[10] = author->GetLocalID();     // Local ID

    // Sending local chat packet and removing dynamic array
    ConnectionUDP *Socket = receiver->getUDPConn();
    Socket->write(LocalChatPacket, overallsize);
    Socket->flushSendBuffer();
    delete[] LocalChatPacket;
}

void PChat::sendTradeCS(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TRADECS) == true)
                    send(receiver, CHAT_TRADECS, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTradeMB(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TRADEMB) == true)
                    send(receiver, CHAT_TRADEMB, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTradeNC(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TRADENC) == true)
                    send(receiver, CHAT_TRADENC, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTradeTH(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TRADETH) == true)
                    send(receiver, CHAT_TRADETH, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTradeWL(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TRADEWL) == true)
                    send(receiver, CHAT_TRADEWL, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendOOC(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_OOC) == true)
                    send(receiver, CHAT_OOC, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendHelp(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_HELP) == true)
                    send(receiver, CHAT_HELP, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendClanSearch(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_CLANSEARCH) == true)
                    send(receiver, CHAT_CLANSEARCH, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendServicesCS(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_SERVICECS) == true)
                    send(receiver, CHAT_SERVICECS, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendServicesMB(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_SERVICEMB) == true)
                    send(receiver, CHAT_SERVICESMB, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendServicesNC(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_SERVICENC) == true)
                    send(receiver, CHAT_SERVICESNC, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendServicesTH(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_SERVICETH) == true)
                    send(receiver, CHAT_SERVICESTH, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendServicesWL(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_SERVICEWL) == true)
                    send(receiver, CHAT_SERVICESWL, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTeam10(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TEAM10) == true)
                    send(receiver, CHAT_TEAM10, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTeam30(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TEAM30) == true)
                    send(receiver, CHAT_TEAM30, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTeam50(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TEAM50) == true)
                    send(receiver, CHAT_TEAM50, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

void PChat::sendTeam70(PClient* author, const char* text, bool debugOut)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(author != it->second) // if its not the client, that send the message to the server
        {
            if(it->second) // only send if the client is existing!
            {
                PClient* receiver = it->second;
                if(chanEnabled(receiver, C_TEAM70) == true)
                    send(receiver, CHAT_TEAM70, Chars->GetChar(author->GetCharID())->GetName().c_str(), text, debugOut);
            }
        }
    }
}

bool PChat::chanEnabled(PClient* Client, uint32_t channel)
{
    // Check if player has target channel enabled or disabled
    PChar* TargetChar = Chars->GetChar(Client->GetCharID());
    if(!TargetChar)
      return false;
      
    uint32_t actChans = TargetChar->GetActiveChannels();
    uint32_t check = actChans & channel;

    if(check == channel)
        return true;
    else
        return false;
}

bool PChat::send(PClient* receiver, const uint8_t* Channel, const char* AuthorNickName, const char* text, bool debugOut)
{
    char *ChatPacket;
    unsigned int packetsize = 0, c;
    int LenText, LenNick, fpp, d, e, loopout;
    uint8_t TargetChannel[2];

//Console->Print("1: %#x", Channel[0]);
//Console->Print("2: %#x", CHAT_BUDDY[0]);


      if(cmpr(Channel, CHAT_BUDDY) == true) {
           TargetChannel[0] = CHAT_BUDDY[0];
           TargetChannel[1] = CHAT_BUDDY[1];
      } else if(cmpr(Channel, CHAT_LOCAL) == true) {
           TargetChannel[0] = CHAT_LOCAL[0];
           TargetChannel[1] = CHAT_LOCAL[1];
      } else if(cmpr(Channel, CHAT_CLAN) == true) {
           TargetChannel[0] = CHAT_CLAN[0];
           TargetChannel[1] = CHAT_CLAN[1];
      } else if(cmpr(Channel, CHAT_TEAM) == true) {
           TargetChannel[0] = CHAT_TEAM[0];
           TargetChannel[1] = CHAT_TEAM[1];
      } else if(cmpr(Channel, CHAT_DIRECT) == true) {
           TargetChannel[0] = CHAT_DIRECT[0];
           TargetChannel[1] = CHAT_DIRECT[1];
      } else if(cmpr(Channel, CHAT_ZONE) == true) {
           TargetChannel[0] = CHAT_ZONE[0];
           TargetChannel[1] = CHAT_ZONE[1];
      } else if(cmpr(Channel, CHAT_FRAK) == true) {
           TargetChannel[0] = CHAT_FRAK[0];
           TargetChannel[1] = CHAT_FRAK[1];
      } else if(cmpr(Channel, CHAT_TRADECS) == true) {
           TargetChannel[0] = CHAT_TRADECS[0];
           TargetChannel[1] = CHAT_TRADECS[1];
      } else if(cmpr(Channel, CHAT_TRADEMB) == true) {
           TargetChannel[0] = CHAT_TRADEMB[0];
           TargetChannel[1] = CHAT_TRADEMB[1];
      } else if(cmpr(Channel, CHAT_TRADENC) == true) {
           TargetChannel[0] = CHAT_TRADENC[0];
           TargetChannel[1] = CHAT_TRADENC[1];
      } else if(cmpr(Channel, CHAT_TRADETH) == true) {
           TargetChannel[0] = CHAT_TRADETH[0];
           TargetChannel[1] = CHAT_TRADETH[1];
      } else if(cmpr(Channel, CHAT_TRADEWL) == true) {
           TargetChannel[0] = CHAT_TRADEWL[0];
           TargetChannel[1] = CHAT_TRADEWL[1];
      } else if(cmpr(Channel, CHAT_OOC) == true) {
           TargetChannel[0] = CHAT_OOC[0];
           TargetChannel[1] = CHAT_OOC[1];
      } else if(cmpr(Channel, CHAT_HELP) == true) {
           TargetChannel[0] = CHAT_HELP[0];
           TargetChannel[1] = CHAT_HELP[1];
      } else if(cmpr(Channel, CHAT_CLANSEARCH) == true) {
           TargetChannel[0] = CHAT_CLANSEARCH[0];
           TargetChannel[1] = CHAT_CLANSEARCH[1];
      } else if(cmpr(Channel, CHAT_SERVICECS) == true) {
           TargetChannel[0] = CHAT_SERVICECS[0];
           TargetChannel[1] = CHAT_SERVICECS[1];
      } else if(cmpr(Channel, CHAT_SERVICESMB) == true) {
           TargetChannel[0] = CHAT_SERVICESMB[0];
           TargetChannel[1] = CHAT_SERVICESMB[1];
      } else if(cmpr(Channel, CHAT_SERVICESNC) == true) {
           TargetChannel[0] = CHAT_SERVICESNC[0];
           TargetChannel[1] = CHAT_SERVICESNC[1];
      } else if(cmpr(Channel, CHAT_SERVICESTH) == true) {
           TargetChannel[0] = CHAT_SERVICESTH[0];
           TargetChannel[1] = CHAT_SERVICESTH[1];
      } else if(cmpr(Channel, CHAT_SERVICESWL) == true) {
           TargetChannel[0] = CHAT_SERVICESWL[0];
           TargetChannel[1] = CHAT_SERVICESWL[1];
      } else if(cmpr(Channel, CHAT_TEAM10) == true) {
           TargetChannel[0] = CHAT_TEAM10[0];
           TargetChannel[1] = CHAT_TEAM10[1];
      } else if(cmpr(Channel, CHAT_TEAM30) == true) {
           TargetChannel[0] = CHAT_TEAM30[0];
           TargetChannel[1] = CHAT_TEAM30[1];
      } else if(cmpr(Channel, CHAT_TEAM50) == true) {
           TargetChannel[0] = CHAT_TEAM50[0];
           TargetChannel[1] = CHAT_TEAM50[1];
      } else if(cmpr(Channel, CHAT_TEAM70) == true) {
           TargetChannel[0] = CHAT_TEAM70[0];
           TargetChannel[1] = CHAT_TEAM70[1];
      } else if(cmpr(Channel, CHAT_ADMIN) == true) {
           TargetChannel[0] = CHAT_ADMIN[0];
           TargetChannel[1] = CHAT_ADMIN[1];
      } else if(cmpr(Channel, CHAT_GMADMIN) == true) {
           TargetChannel[0] = CHAT_GMADMIN[0];
           TargetChannel[1] = CHAT_GMADMIN[1];
      } else if(cmpr(Channel, CHAT_GM) == true) {
           TargetChannel[0] = CHAT_GM[0];
           TargetChannel[1] = CHAT_GM[1];
      } else {
         Console->Print("SendChat error: Channel %#x unknown", Channel);
         return false;
      }

    ConnectionTCP *Socket = receiver->getTCPConn();

    uint8_t BasicChatPacket[] = {0xFE, 0x15, 0x00, 0x83, 0x17, 0x90, 0x03, 0x00, 0x00, 0x07, 0x05, 0x0F};

    LenText = LenNick = fpp = 0;

    do {
      LenText++;
    } while(text[LenText] != '\0');

    do {
      LenNick++;
    } while(AuthorNickName[LenNick] != '\0');

    if(LenText == 0 || LenNick == 0) {
       Console->Print("Error in SendChat, nickname or text is missing");
       return false;
    }

    loopout = 0;

    packetsize = sizeof(BasicChatPacket) + LenText + LenNick;

    if(debugOut == true) {
       Console->Print("Sizeof(TextToSend): %d || Sizeof(Nick): %d", LenText, LenNick);
       Console->Print("Whole size: %d", packetsize);
    }

    ChatPacket = new char [packetsize];

    // Copy basic packet into final packet
    for(c=0;c<sizeof(BasicChatPacket);c++) {
       ChatPacket[fpp] = BasicChatPacket[c];
       fpp++;
    }

    // Copy Nickname into final packet
    for(d=0;d<LenNick;d++) {
       ChatPacket[fpp] = AuthorNickName[d];
       fpp++;
    }

    // Copy Text into final packet
    for(e=0;e<LenText;e++) {
       ChatPacket[fpp] = text[e];
       fpp++;
    }

    // Change Lenght bytes in final packet
    ChatPacket[1] = packetsize - 3;
    ChatPacket[9] = LenNick;
    ChatPacket[10] = TargetChannel[0];
    ChatPacket[11] = TargetChannel[1];

    if(debugOut == true) {
        unsigned int debugout;
        for(debugout=0;debugout<packetsize;debugout++) {
           Console->Print("Byte %d: %#x", debugout, ChatPacket[debugout]);
        }
    }

    // Sending direct chat packet and removing dynamic array
    Socket->write(ChatPacket, packetsize);
    Socket->flushSendBuffer();
    delete[] ChatPacket;

    return true;
}



bool PChat::HandleGameChat(PClient *Client, const uint8_t *Packet)
{
    // if player is shunned, ignore all incomming chat and game commands.
    // ServerAdmins are not affected by any shuns. (Should never happen anyways...)
    if((Client->GetChar()->IsShunned() == true) && (Client->GetAccountLevel() < PAL_ADMIN)) return true;

    int i, j, k;

     uint8_t chattype = *(uint8_t*)&Packet[7];
// -----------------------------------------------
    if(chattype == 0x1B) {
      // Local chat
      i = 8;
      j = 0;

      char ChatText[255];

      do {
         ChatText[j] = Packet[i];
         i++;
         j++;
      } while (ChatText[j-1] != 0x00);

      ChatText[j] = '\0';
      if(ChatText[0] == '@' && sizeof(ChatText) > 2) {
            GameCommands->HandleGameCommand(ChatText, Client);
      } else {
          // We know its working, so we dont need console output anymore
            //Console->Print("Local Chat: %s", ChatText);
            //Console->Print("Client CharName is: %s", Chars->GetChar(Client->GetCharID())->GetName().c_str());
            sendLocal(Client, ChatText, false);
      }

// -----------------------------------------------
   } else if (chattype == 0x3B) {
      // Non-Local chat
      i = 13;
      j = 0;
      k = 0;

      char ChatText[255];

      do {
         ChatText[j] = Packet[i];
         i++;
         j++;
      } while (ChatText[j-1] != 0x00);


      ChatText[j] = '\0';
      if(ChatText[0] == '@' && sizeof(ChatText) > 2) {
            GameCommands->HandleGameCommand(ChatText, Client);
      } else {
        //      Console->Print("CHATLINE: %s HEX: %#X", ChatText, Packet[i-1]);
        char Channel[4];

        for(k = 0; k <= 3; k ++) {
           Channel[k] = Packet[k+8];
        }
  //    Console->Print("Channel no %#x %#x %#x %#x", Channel[0], Channel[1], Channel[2], Channel[3]);

        // First, check if packet is a direct-chat-packet
        if(*(uint8_t*)&Packet[8] == 0x04) {
             //Console->Print("Direct Chat: %s", ChatText);
             sendPlayerDirect(Client, ChatText, *(uint32_t*)&Packet[9], false);
             //sendDirect(Client, ChatText, false);
             // "DIRECT> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_BUDDY) {
             //Console->Print("Buddy Chat: %s", ChatText);
             sendBuddy(Client, ChatText, false);
             // "BUDDY> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CLAN) {
             //Console->Print("Clan Chat: %s", ChatText);
             sendClan(Client, ChatText, false);
             // "CLAN> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_TEAM) {
             //Console->Print("Team Chat: %s", ChatText);
             sendTeam(Client, ChatText, false);
             // "TEAM> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_ZONE) {
             //Console->Print("Custom - Zone Chat: %s", ChatText);
             sendZone(Client, ChatText, false);
             // "ZONE> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_FRAKTION) {
             //Console->Print("Custom - Fraktion Chat: %s", ChatText);
             sendFrak(Client, ChatText, false);
             // "FRACTION> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TRADE_CANYON) {
             //Console->Print("Custom - Trade_Canyon Chat: %s", ChatText);
             sendTradeCS(Client, ChatText, false);
             // "TRADE - CS> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TRADE_MB) {
             //Console->Print("Custom - Trade_MB Chat: %s", ChatText);
             sendTradeMB(Client, ChatText, false);
             // "TRADE - MB> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TRADE_NC) {
             //Console->Print("Custom - Trade_NC Chat: %s", ChatText);
             sendTradeNC(Client, ChatText, false);
             // "TRADE - NC> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TRADE_TH) {
             //Console->Print("Custom - Trade_TH Chat: %s", ChatText);
             sendTradeTH(Client, ChatText, false);
             // "TRADE - TH> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TRADE_WASTE) {
             //Console->Print("Custom - Trade_Waste Chat: %s", ChatText);
             sendTradeWL(Client, ChatText, false);
             // "TRADE - WL> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_OOC) {
             //Console->Print("Custom - OOC Chat: %s", ChatText);
             sendOOC(Client, ChatText, false);
             // "OOC> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_PLAYERHELP) {
             //Console->Print("Custom - PlayerToPlayerhelp Chat: %s", ChatText);
             sendHelp(Client, ChatText, false);
             // "HELP> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_CLANSEARCH) {
             //Console->Print("Custom - Clansearch Chat: %s", ChatText);
             sendClanSearch(Client, ChatText, false);
             // "CLANSEARCH> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_SERVICES_CANYON) {
             //Console->Print("Custom - Services_Canyon Chat: %s", ChatText);
             sendServicesCS(Client, ChatText, false);
             // "SKILL - CS> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_SERVICES_MB) {
             //Console->Print("Custom - Services_MB Chat: %s", ChatText);
             sendServicesMB(Client, ChatText, false);
             // "SKILL - MB> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_SERVICES_NC) {
             //Console->Print("Custom - Services_NC Chat: %s", ChatText);
             sendServicesNC(Client, ChatText, false);
             // "SKILL - NC> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_SERVICES_TH) {
             //Console->Print("Custom - Services_TH Chat: %s", ChatText);
             sendServicesTH(Client, ChatText, false);
             // "SKILL - TH> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_SERVICES_WASTE) {
             //Console->Print("Custom - Services_Waste Chat: %s", ChatText);
             sendServicesWL(Client, ChatText, false);
             // "SKILL - WL> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TEAM_10) {
             //Console->Print("Custom - Team10 Chat: %s", ChatText);
             sendTeam10(Client, ChatText, false);
             // "TEAMSEARCH 10> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TEAM_30) {
             //Console->Print("Custom - Team30 Chat: %s", ChatText);
             sendTeam30(Client, ChatText, false);
             // "EAMSEARCH 30> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TEAM_50) {
             //Console->Print("Custom - Team50 Chat: %s", ChatText);
             sendTeam50(Client, ChatText, false);
             // "EAMSEARCH 50> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_CUS_TEAM_70) {
             //Console->Print("Custom - Team70 Chat: %s", ChatText);
             sendTeam70(Client, ChatText, false);
             // "EAMSEARCH 70> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_ADMIN) {
             //Console->Print("Admin Chat: %s", ChatText);
             sendAdmin(Client, ChatText, false);
             // "ADMIN> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_GMADMIN) {
             //Console->Print("Admin Chat: %s", ChatText);
             sendGMAdmin(Client, ChatText, false);
             // "ADMIN> %s: %s", PlayerName, ChatText
        }
        else if(*(uint32_t*)Channel == CHANNEL_GMCHAT) {
             //Console->Print("GameMaster Chat: %s", ChatText);
             sendGM(Client, ChatText, false);
             // "GM> %s: %s", PlayerName, ChatText
        }
        else {
             Console->Print("Unknown Chat-Channel: %#x", *(uint32_t*)Channel);
        };
  }
}
return (true);
}

bool PChat::cmpr(const uint8_t *Array1, const uint8_t *Array2) {
   if(Array1[0] == Array2[0] && Array1[1] == Array2[1]) {
       return true;
   } else {
       return false;
   }
}
