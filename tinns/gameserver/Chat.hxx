#pragma once

#include <cstdint>

#define LOCALCHAT_MAXDISTANCE 1433

class PClient;

class PChat {
public:
    PChat();
    ~PChat();

    // INCOMING CHAT
    bool HandleGameChat(PClient *Client, const uint8_t *Packet);
    bool cmpr(const uint8_t *Array1, const uint8_t *Array2);

    // this function is called by the other more specific functions and simply sends the data to the receiver-client
    bool send(PClient* receiver, const uint8_t* Channel, const char* AuthorNickName, const char* text, bool debugOut=false);

    // this is for debugging and sends the current connected playerlist to the receiver-client
    void sendConnectedList(PClient* receiver, bool debugOut=false);

    // specific channel functions:
    void sendBuddy(PClient* author, const char* text, bool debugOut=false);
    void sendLocal(PClient* author, const char* text, bool debugOut=false);
    void sendClan(PClient* author, const char* text, bool debugOut=false);
    void sendTeam(PClient* author, const char* text, bool debugOut=false);
    void sendDirect(PClient* author, PClient* receiver, const char* text, bool debugOut=false);
    void sendZone(PClient* author, const char* text, bool debugOut=false);
    void sendFrak(PClient* author, const char* text, bool debugOut=false);
    void sendTradeCS(PClient* author, const char* text, bool debugOut=false);
    void sendTradeMB(PClient* author, const char* text, bool debugOut=false);
    void sendTradeNC(PClient* author, const char* text, bool debugOut=false);
    void sendTradeTH(PClient* author, const char* text, bool debugOut=false);
    void sendTradeWL(PClient* author, const char* text, bool debugOut=false);
    void sendOOC(PClient* author, const char* text, bool debugOut=false);
    void sendHelp(PClient* author, const char* text, bool debugOut=false);
    void sendClanSearch(PClient* author, const char* text, bool debugOut=false);
    void sendServicesCS(PClient* author, const char* text, bool debugOut=false);
    void sendServicesMB(PClient* author, const char* text, bool debugOut=false);
    void sendServicesNC(PClient* author, const char* text, bool debugOut=false);
    void sendServicesTH(PClient* author, const char* text, bool debugOut=false);
    void sendServicesWL(PClient* author, const char* text, bool debugOut=false);
    void sendTeam10(PClient* author, const char* text, bool debugOut=false);
    void sendTeam30(PClient* author, const char* text, bool debugOut=false);
    void sendTeam50(PClient* author, const char* text, bool debugOut=false);
    void sendTeam70(PClient* author, const char* text, bool debugOut=false);
    void sendAdmin(PClient* author, const char* text, bool debugOut=false);
    void sendGM(PClient* author, const char* text, bool debugOut=false);
    void sendGMAdmin(PClient* author, const char* text, bool debugOut=false);

    void sendBroadcast(const char* text, bool debugOut=false);
    void sendOOCBroadcast(const char* text, bool debugOut=false);
    void sendPlayerDirect(PClient* author, const char* text, uint32_t destination, bool debugOut=false);
    void sendLocalchat(PClient* receiver, PClient* author, const char* text, bool debugOut=false);
    bool chanEnabled(PClient* Client, uint32_t channel);
};

/** A WARNING: DO >N O T< (!!!) CHANGE >ANY< OF THE FOLLOWING VARIABLES UNLESS YOU KNOW EXACT WHAT YOU'RE DOING! **/
/** You can easily mess up the entire chat subsystem. If you're unsure, ask Namikon first! **/
/*
####################
   CHANNEL-CODES:
     (INCOMING)
####################
*/
static const uint32_t CHANNEL_BUDDY = 0x00000000;                // Buddy
static const uint32_t CHANNEL_CLAN = 0x00000002;                 // Clan
static const uint32_t CHANNEL_TEAM = 0x00000003;                 // Team
static const uint32_t CHANNEL_DIRECT = 0xFFFFFFFF;               // Direct UNKNOWN YET
static const uint32_t CHANNEL_CUS_ZONE = 0x00000105;             // Custom -> Zone
static const uint32_t CHANNEL_CUS_FRAKTION = 0x00000205;         // Custom -> Fraktion
static const uint32_t CHANNEL_CUS_TRADE_CANYON = 0x00002005;     // Custom -> Trade Canyon
static const uint32_t CHANNEL_CUS_TRADE_MB = 0x00000805;         // Custom -> Trade MB
static const uint32_t CHANNEL_CUS_TRADE_NC = 0x00000405;         // Custom -> Trade NC
static const uint32_t CHANNEL_CUS_TRADE_TH = 0x00001005;         // Custom -> Trade TH
static const uint32_t CHANNEL_CUS_TRADE_WASTE = 0x00004005;      // Custom -> Trade Wastelands
static const uint32_t CHANNEL_CUS_OOC = 0x04000005;              // Custom -> OOC
static const uint32_t CHANNEL_CUS_PLAYERHELP = 0x02000005;       // Custom -> Player 2 Player help
static const uint32_t CHANNEL_CUS_CLANSEARCH = 0x01000005;       // Custom -> Searching Clan
static const uint32_t CHANNEL_CUS_SERVICES_CANYON = 0x00040005;  // Custom -> Runner Services Canyon
static const uint32_t CHANNEL_CUS_SERVICES_MB = 0x00010005;      // Custom -> Runner Services MB
static const uint32_t CHANNEL_CUS_SERVICES_NC = 0x00008005;      // Custom -> Runner Services NC
static const uint32_t CHANNEL_CUS_SERVICES_TH = 0x00020005;      // Custom -> Runner Services TH
static const uint32_t CHANNEL_CUS_SERVICES_WASTE = 0x00080005;   // Custom -> Runner Services Wastelands
static const uint32_t CHANNEL_CUS_TEAM_10 = 0x00100005;          // Custom -> Searching Team ~10
static const uint32_t CHANNEL_CUS_TEAM_30 = 0x00200005;          // Custom -> Searching Team ~30
static const uint32_t CHANNEL_CUS_TEAM_50 = 0x00400005;          // Custom -> Searching Team ~50
static const uint32_t CHANNEL_CUS_TEAM_70 = 0x00800005;          // Custom -> Searching Team ~70
static const uint32_t CHANNEL_ADMIN = 0x000000FF;                // Admin chat
static const uint32_t CHANNEL_GMADMIN = 0x000000FE;                // Admin chat
static const uint32_t CHANNEL_GMCHAT = 0x000000FD;               // GameMaster chat
/*
####################
   CHANNEL-CODES:
     (OUTGOING)
####################
*/
static const uint8_t CHAT_BUDDY[] = {0x00, 0x10};
static const uint8_t CHAT_LOCAL[] =  {0x01, 0x10};
static const uint8_t CHAT_CLAN[] = {0x02, 0x10};
static const uint8_t CHAT_TEAM[] = {0x03, 0x10};
static const uint8_t CHAT_DIRECT[] = {0x04, 0x10};
static const uint8_t CHAT_ZONE[] = {0x05, 0x00};
static const uint8_t CHAT_FRAK[] = {0x05, 0x01};
static const uint8_t CHAT_TRADECS[] = {0x05, 0x05};
static const uint8_t CHAT_TRADEMB[] = {0x05, 0x03};
static const uint8_t CHAT_TRADENC[] = {0x05, 0x02};
static const uint8_t CHAT_TRADETH[] = {0x05, 0x04};
static const uint8_t CHAT_TRADEWL[] = {0x05, 0x06};
static const uint8_t CHAT_OOC[] = {0x05, 0x12};
static const uint8_t CHAT_HELP[] = {0x05, 0x11};
static const uint8_t CHAT_CLANSEARCH[] = {0x05, 0x10};
static const uint8_t CHAT_SERVICECS[] =  {0x05, 0x0A};
static const uint8_t CHAT_SERVICESMB[] = {0x05, 0x08};
static const uint8_t CHAT_SERVICESNC[] = {0x05, 0x07};
static const uint8_t CHAT_SERVICESTH[] = {0x05, 0x09};
static const uint8_t CHAT_SERVICESWL[] = {0x05, 0x0B};
static const uint8_t CHAT_TEAM10[] = {0x05, 0x0C};
static const uint8_t CHAT_TEAM30[] = {0x05, 0x0D};
static const uint8_t CHAT_TEAM50[] = {0x05, 0x0E};
static const uint8_t CHAT_TEAM70[] = {0x05, 0x0F};
static const uint8_t CHAT_ADMIN[] = {0xFF, 0x10};
static const uint8_t CHAT_GMADMIN[] = {0xFE, 0x10};
static const uint8_t CHAT_GM[] = {0xFD, 0x10};

/*
####################
   CHANNEL-CODES:
(EN/DISABLE-ABLE CHANNELS)
####################
*/
static const uint32_t C_ZONE = 1;
static const uint32_t C_FRAK = 2;
static const uint32_t C_TRADENC = 4;
static const uint32_t C_TRADEMB = 8;
static const uint32_t C_TRADETH = 16;
static const uint32_t C_TRADECS = 32;
static const uint32_t C_TRADEWL = 64;
static const uint32_t C_SERVICENC = 128;
static const uint32_t C_SERVICEMB = 256;
static const uint32_t C_SERVICETH = 512;
static const uint32_t C_SERVICECS = 1024;
static const uint32_t C_SERVICEWL = 2048;
static const uint32_t C_TEAM10 = 4096;
static const uint32_t C_TEAM30 = 8192;
static const uint32_t C_TEAM50 = 16384;
static const uint32_t C_TEAM70 = 32768;
static const uint32_t C_CLANSEARCH = 65536;
static const uint32_t C_HELP = 131072;
static const uint32_t C_OOC = 262144;
