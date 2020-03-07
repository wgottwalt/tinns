#include "patchserver/Includes.hxx"
#include "common/Includes.hxx"

PClient::PClient(int32_t Index)
{
	mIndex = Index;
	mConnection = PCC_NONE;
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

int32_t PClient::GetConnection() const
{
    return mConnection;
}

const char *PClient::GetAddress() const
{
    return m_TCPConnection->getRemoteAddress();
}

void PClient::setTCPConnection(ConnectionTCP *conn)
{
    m_TCPConnection = conn;
    mConnection = PCC_PATCH;
}

ConnectionTCP *PClient::getTCPConn()
{
    return m_TCPConnection;
}

void PClient::Update()
{
    if(m_TCPConnection)
    {
        if(m_TCPConnection->timeOut())
        {
            Console->Print("Patchsocket: Client %i: timeout", mIndex);
            PatchServer->ClientDisconnected(this);
        }
        else
        {
            if(!m_TCPConnection->update())
            {
                PatchServer->ClientDisconnected(this);
            }
        }
	}
}

void PClient::PatchDisconnect()
{
    if(m_TCPConnection)
    {
        delete m_TCPConnection;
    }
    mConnection = PCC_NONE;
    m_TCPConnection = 0;
}
