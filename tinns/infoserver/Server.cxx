#include "infoserver/Includes.hxx"
#include "common/Includes.hxx"

PServer::PServer()
{
  mMaxClients = Config->GetOptionInt("maxclients");

    mClients.reserve(mMaxClients);
    mNumClients = 0;
    for (int32_t i=0; i<mMaxClients; i++)
        mClients[i]=0;
}

PServer::~PServer()
{
    for (int32_t i=0; i<mMaxClients; i++)
        delete mClients[i];
}

int32_t PServer::GetNumClients() const
{
    return mNumClients;
}

int PServer::NewClient()
{
    if(mNumClients==mMaxClients)
        return -1;

    for (int32_t i=0; i<mMaxClients; i++)
    {
        if(!mClients[i])
        {
            mClients[i]=new PClient(i);
            ++mNumClients;
            return i;
        }
    }
    return -1;
}

PClient *PServer::GetClient(int32_t Client) const
{
    if (Client < 0 || Client >= mMaxClients)
        return 0;

    return mClients[Client];
}

void PServer::Update()
{
    for (int32_t i=0; i<mMaxClients; i++)
    {
        if(mClients[i])
        {
            mClients[i]->Update();
            if(mClients[i]->GetConnection()==PCC_NONE && mClients[i]->getTCPConn() == 0)
            {
                Console->Print("Removing client ...");
                delete mClients[i];
                mClients[i]=0;
                --mNumClients;
            }
        }
    }
}

void PServer::Shutdown()
{
    Console->Print("======================");
    Console->Print("Shutting down Infoserver...");
    for (int32_t i=0; i<mMaxClients; i++)
    {
        if(mClients[i])
        {
            delete mClients[i];
            mClients[i]=0;
        }
    }
}
