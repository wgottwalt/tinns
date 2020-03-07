#pragma once

#include <cstdint>
#include <map>

class PClient;
struct PInfoState;

#define MAX_SERVER_NAME_LENGTH 45
struct GameServers {
    char  mName[MAX_SERVER_NAME_LENGTH];
    uint32_t mLanIp;
    uint32_t mWanIp;
    int16_t mPort;
    int32_t mPlayers;
    bool mOnline;
    bool mUpdated;
    time_t mLasttimestamp;
/* Prepared for future addon Servers by Accesslevel */
//    int             mMinLv;
/* ------------------------------------------------ */
};

class PInfoServer {
private:
    // SQL layout
    enum {
        s_id,
        s_name,
        s_wanaddr,
        s_port,
        s_players,
        s_lastupdate,
        s_lanaddr,
        s_timecheck // computed field, not in table !
    };
    //int mNumClients;
    typedef std::map<PClient *, struct PInfoState *> InfoStateMap;
    InfoStateMap ClientStates;

    typedef std::map<int32_t, GameServers> ServerMap;
    ServerMap Serverlist;

    //time_t mLastLivecheck;
    int mLivecheckInterval;
    void GSLiveCheck();

protected:
    bool ProcessClient(PClient *Client, PInfoState *State = nullptr);
    void FinalizeClient(PClient *Client, PInfoState *State);
    void FinalizeClientDelayed(PClient *Client, PInfoState *State);

    bool HandleHandshake(PInfoState *State, const uint8_t *Packet, int32_t PacketSize);
    bool HandleAuthenticate(PClient *Client, PInfoState *State, const uint8_t *Packet, int32_t PacketSize);
    bool HandleServerList(PClient *Client, const uint8_t *Packet, int32_t PacketSize);

public:
    PInfoServer();
    ~PInfoServer();

    void Start();
    void Update();
    void ClientDisconnected(PClient *Client);
};
