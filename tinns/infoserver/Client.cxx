#include "infoserver/Includes.hxx"
#include "common/Includes.hxx"

PClient::PClient(int32_t Index)
{
    mIndex = Index;
    mConnection = PCC_NONE;
    mAccountID = 0;
}

PClient::~PClient()
{
    if(m_TCPConnection)
    {
        delete m_TCPConnection;
    }
}

int32_t PClient::GetIndex() const
{
    return mIndex;
}

void PClient::setTCPConnection(ConnectionTCP *conn)
{
    m_TCPConnection = conn; mConnection = PCC_INFO;
}

ConnectionTCP *PClient::getTCPConn() const
{
    return m_TCPConnection;
}

int32_t PClient::GetConnection() const
{
    return mConnection;
}

const char *PClient::GetAddress() const
{
    return m_TCPConnection->getRemoteAddress();
}

void PClient::setAccountID(uint32_t nAccountID)
{
    mAccountID = nAccountID;
}

uint32_t PClient::getAccountID()
{
    return mAccountID;
}

void PClient::InfoDisconnect()
{
    if(m_TCPConnection)
    {
        delete m_TCPConnection;
    }
    m_TCPConnection = 0;

    //mConnection &= ~PCC_INFO;
    mConnection = PCC_NONE;
    mAccountID = 0;
}

void PClient::Update()
{
    if(m_TCPConnection)
    {
        if(m_TCPConnection->timeOut())
        {
                Console->Print("InfoSocket: Client %i: timeout", mIndex);
                InfoServer->ClientDisconnected(this);
        }
        else
        {
            if(!m_TCPConnection->update())
            {
                InfoServer->ClientDisconnected(this);
            }
        }
    }
}
