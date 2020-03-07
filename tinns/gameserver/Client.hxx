#pragma once

#include <cstdint>

class ConnectionTCP;
class ConnectionUDP;
class PMessage;

//  TODO: - check that SetUDP_ID, and the mSessionID(UDP_ID_HIGH) real use,
//          and if UDP_ID and mSessionID must be synced (like in NeoX) or not

enum PClientConnection
{
  PCC_NONE = 0,
  PCC_GAME = 1
};

// AccountLevel handling is part of accounts.cpp
/*
enum PClientLevel
{
 PCL_BANNED = -1,
 PCL_UNREGPLAYER = 0,
 PCL_REGPLAYER = 1,
 PCL_VOLUNTEER = 30,
 PCL_GM = 50,
 PCL_ADMIN = 100
};
*/
#define DEBUG_MODES 3
enum PDebugMode
{
  DBG_LOCATION = 0,
  DBG_ITEMID = 1,
  DBG_SUBWAY = 2,
  DBG_ALL = DEBUG_MODES // must always be last, with DEBUG_MODES updated as needed
};


class PClient
{
  private :
    ConnectionTCP* m_TCPConnection;
    ConnectionUDP* m_UDPConnection;

    uint32_t mAccountID;
    int mAccountLevel;
    uint32_t mIndex;
    uint32_t mCharID;

//  uint16_t mUDP_ID;
//  uint16_t mSessionID;
//  uint16_t mTransactionID;

    // AccountLevel handling is part of accounts.cpp
    //PClientLevel mLevel;
    int mConnection;
    int mRemotePort;

    bool mDebugMode[DEBUG_MODES];
    // new multiuser-chat implementation //
    int m_ZoneID;
    //int[4] m_IP;

    //*******
    bool mActorRemoveMode;
    //*******
    bool mAwaitingWarpto;
    uint16_t mTargetX;
    uint16_t mTargetY;
    uint16_t mTargetZ;
    //*******
    bool mAcceptNPCUpdates;
    bool mZoning;
    bool mVhcZoning;

  public :
    PClient( int Index );
    ~PClient();

    bool GetDebugMode(PDebugMode nDebugID);
    void SetDebugMode(PDebugMode nDebugID, bool nVal = true);

    bool IsAcceptingNPCUpdates();
    void SetAcceptNPCUpdates(bool nVal);
    bool IsZoning();
    void SetZoning(bool nVal = true);
    bool IsVhcZoning();
    void SetVhcZoning(bool nVal = true);

    uint32_t GetIndex() const; // better use GetID()
    uint32_t GetID() const; // for better coherency with other classes
    uint32_t GetLocalID() const;
    uint32_t GetCharID() const;
    PChar* GetChar() const;
    bool ChangeCharLocation(uint32_t nLocation, bool DoForce = false);

    int GetRemoteUDPPort() const; // Temp solution

    bool IsInRemoveActorMode();
    void SetRemoveActorMode(bool nNewValue);

    void SetRemoteUDPPort(int port); // Temp solution
    void SetCharID(int id);//NEW added

    /*
        inline uint16_t GetUDP_ID() const { return mUDP_ID; }
        inline uint16_t GetSessionID() const { return SESSION_UDP_OFFSET + mUDP_ID ; }
        inline uint16_t GetTransactionID() {return mTransactionID; }
        void SetUDP_ID(int id);
        inline void IncreaseUDP_ID() { SetUDP_ID(mUDP_ID + 1); }
        inline void ResetTransactionID() { mTransactionID = 10170; }

        inline void IncreaseTransactionID(uint8_t nInc = 1) { mTransactionID += nInc; }
    */

    // All outgoing ID's and stuff is now part of the ConnectionUDP class itself!
    //    (which is not so good.... comment from Hammag)
    // However, we still have full access to it through these functions
    uint16_t GetUDP_ID();
    void SetUDP_ID( int id );
    void IncreaseUDP_ID();

    uint16_t GetSessionID();

    uint16_t GetTransactionID();
    void ResetTransactionID();
    void IncreaseTransactionID( uint8_t nInc = 1 );

    void FillInUDP_ID( PMessage* nMessage );

// ************************************************************************ //
    // AccountLevel handling is part of accounts.cpp
    //inline PClientLevel GetLevel() const { return mLevel; }

    void setTCPConnection(ConnectionTCP *conn);
    void setUDPConnection(ConnectionUDP *conn);

    ConnectionTCP* getTCPConn();
    ConnectionUDP* getUDPConn();

    void SendTCPMessage(PMessage *nMessage);
    void FragmentAndSendUDPMessage( PMessage* nMessage, uint8_t nType );
    void SendUDPMessage(PMessage *nMessage, bool nVIP = false);

    int GetConnection() const;
    const char *GetAddress() const;
    uint32_t GetAccountID() const;
    int GetAccountLevel() const;

    void GameDisconnect();

    void RefreshAccountInfo(PAccount *Account);
    void LoggedIn(PAccount *Account);
    void Update();

    // new multiuser-chat implementation //
    int getZoneID() const;
    //inline int* getIP() const { return (int*) m_IP; }

    void SetAwaitingWarpto(bool yesno, uint16_t NewX, uint16_t NewY, uint16_t NewZ);
    bool GetCharAwaitingWarpto(uint16_t* PosX = nullptr, uint16_t* PosY = nullptr, uint16_t* PosZ = nullptr);

    // Char broadcasted effects
    void InitWarpCircle();
    void InitCharVanish();

    // used for dynamic ingame testing
    uint8_t testval8;
    uint16_t testval16;
    uint32_t testval32;
};
