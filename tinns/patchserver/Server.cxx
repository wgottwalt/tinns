#include "patchserver/Includes.hxx"
#include "common/Includes.hxx"

PServer::PServer()
{
    mNumClients = 0;
    mMaxClients = Config->GetOptionInt("maxclients");
    mGMSlots = Config->GetOptionInt("gm_slots");

    if (mMaxClients==0)
        mMaxClients=1;
    if (mGMSlots==0)
        mGMSlots=1;
    mClients.reserve(mMaxClients + mGMSlots);
    for (int32_t i=0; i<mMaxClients+mGMSlots; i++)
        mClients[i]=0;
}

PServer::~PServer()
{
    for (int32_t i=0; i<mMaxClients+mGMSlots; i++)
        delete mClients[i];
}

int32_t PServer::GetMaxClients() const
{
    return mMaxClients;
}

int32_t PServer::GetGMSlots() const
{
    return mGMSlots;
}

int32_t PServer::GetNumClients() const
{
    return mNumClients;
}

int PServer::NewClient()
{
//Console->Print("%d %d", mMaxClients, mGMSlots);
    if (mNumClients==mMaxClients+mGMSlots)
        return -1;
    for (int32_t i=0; i<mMaxClients+mGMSlots; i++)
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
    if (Client < 0 || Client >= mMaxClients+mGMSlots)
        return 0;

    return mClients[Client];
}

void PServer::Update()
{
    for (int32_t i=0; i<mMaxClients+mGMSlots; i++)
    {
        if (mClients[i])
        {
            mClients[i]->Update();
            if (mClients[i]->GetConnection()==PCC_NONE && mClients[i]->getTCPConn() == 0)
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
    Console->Print("Shutting down Patchserver...");
    for (int32_t i=0; i<mMaxClients+mGMSlots; i++)
    {
        if(mClients[i])
        {
            delete mClients[i];
            mClients[i]=0;
        }
    }
}
