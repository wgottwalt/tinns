#pragma once

#include <cstdint>

class ConnectionTCP;

enum PClientConnection {
    PCC_NONE = 0,
    PCC_PATCH = 1
};

class PClient {
private:
    ConnectionTCP *m_TCPConnection;
    int32_t mIndex;
    int32_t mConnection;

public:
    PClient(int32_t Index);
    ~PClient();

    int32_t GetIndex() const;
    int32_t GetConnection() const;
    const char *GetAddress() const;
    void setTCPConnection(ConnectionTCP *conn);
    ConnectionTCP *getTCPConn();
    void Update();
    void PatchDisconnect();
};
