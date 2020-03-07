#pragma once

#include <chrono>
#include <cstdint>
#include <map>

struct PGameState
{
  struct TCP
  {
    enum State
    {
      GS_UNKNOWN,
      GS_CONNECTED,
      GS_HANDSHAKE0,
      GS_AUTHENTICATE,
      GS_GAMEDATA,
      GS_REQUESTCHARS,
      GS_CHARLIST,
      GS_GETSTATUS,
      GS_GAMEINFO,
      GS_INGAME
    } mState;

    bool mWaitSend; // wait-for-completition flag
  } TCP;

  struct UDP
  {
    enum State
    {
      GUS_UNKNOWN,
      GUS_SYNC0,
      GUS_SYNC1,
      GUS_SYNC2,
      GUS_SYNC3
    } mState;

    bool mSynced;
    uint16_t mServerPacketNum;
    uint16_t mSequence;
    //uint16_t mClientPacketNum;
  } UDP;

  PGameState()
  {
    TCP.mState = TCP::GS_UNKNOWN;
    TCP.mWaitSend = false;

    UDP.mState = UDP::GUS_UNKNOWN;
    UDP.mSynced = false;
    UDP.mServerPacketNum = 0x9c9f;
    UDP.mSequence = 2;
    //UDP.mClientPacketNum = 0;
  };
};

class PUdpMsgDecoder;

class PGameServer
{
  private :
    time_t mServerStartupTime;
    int mNumClients;
    uint32_t mBaseGameTime;
    struct timespec mStartTime;

    typedef std::map<PClient*, struct PGameState*> GameStateMap;
    GameStateMap ClientStates;
    PUdpMsgDecoder* MsgDecoder;

  protected :
    bool ProcessClient( PClient *Client, PGameState *State = 0 );
    void FinalizeClient( PClient *Client, PGameState *State );
    void FinalizeClientDelayed( PClient *Client, PGameState *State );

//  bool HandleHandshake(PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize);
    bool HandleHandshake( PGameState *State, const uint8_t *Packet, int PacketSize );

    bool HandleAuthenticate( PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize );
//  bool HandleGameData(PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize);
    bool HandleGameData( PClient *Client, PGameState *State, const uint8_t *Packet );

//  bool HandleRequestChars(PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize);
    bool HandleRequestChars( PClient *Client, PGameState *State, const uint8_t *Packet );

    bool HandleCharList( PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize );
//  bool HandleGetStatus(PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize);
    bool HandleGetStatus( PClient *Client, PGameState *State, const uint8_t *Packet );

//  bool HandleGameInfo(PClient *Client, PGameState *State, const uint8_t *Packet, int PacketSize);
    bool HandleGameInfo( PClient *Client, PGameState *State, const uint8_t *Packet );

    bool HandleGame( PClient *Client, PGameState *State );

  public :
    PGameServer();
    ~PGameServer();

    void Start();
    void Update();
    void ClientDisconnected( PClient *Client );
    void UDPStreamClosed( PClient *Client );
    void SetGameTime( uint32_t newtime );
    uint32_t GetGameTime();

    inline time_t GetStartTime() const { return mServerStartupTime; };

    PGameState* GetClientState( PClient* nClient );
};
