#pragma once

#include <cstdint>

class ConnectionTCP;

enum PClientConnection
{
    PCC_NONE = 0,
    PCC_INFO = 1
};

class PClient {
private:
    ConnectionTCP* m_TCPConnection;
    int32_t mIndex;
    int32_t mConnection;
    uint32_t mAccountID;

public:
    PClient(int32_t Index);
    ~PClient();

    int32_t GetIndex() const;
    void setTCPConnection(ConnectionTCP *conn);
    ConnectionTCP *getTCPConn() const;
    int32_t GetConnection() const;
    const char *GetAddress() const;
    void setAccountID(uint32_t nAccountID);
    uint32_t getAccountID();
    void InfoDisconnect();
    void Update();
};
