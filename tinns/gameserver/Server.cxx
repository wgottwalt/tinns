#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PServer::PServer()
{
    mNumClients = 0;
    mMaxClients = Config->GetOptionInt("maxclients");
    mGMSlots = Config->GetOptionInt("gm_slots");
    if(mMaxClients==0)
        mMaxClients=1;
    if(mGMSlots==0)
        mGMSlots=1;
    Console->Print("Max clients: %i / GM slots: %i", mMaxClients, mGMSlots);
    mClients.reserve(mMaxClients + mGMSlots);
    for(int i=0; i<mMaxClients+mGMSlots; i++)
        mClients[i]=0;
}

PServer::~PServer()
{
    for(int i=0; i<mMaxClients+mGMSlots; i++)
        delete mClients[i];
}

int PServer::NewClient()
{
    if(mNumClients==mMaxClients+mGMSlots)
        return -1;

    for(int i=0; i<mMaxClients+mGMSlots; i++)
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

PClient *PServer::GetClient(int Client) const
{
    if(Client < 0 || Client >= mMaxClients+mGMSlots)
        return 0;

    return mClients[Client];
}

void PServer::Update()
{
    for(int i=0; i<mMaxClients+mGMSlots; i++)
    {
        if(mClients[i])
        {
            mClients[i]->Update();
            if(mClients[i]->GetConnection()==PCC_NONE && mClients[i]->getTCPConn() == 0)
            {
                Console->Print("Removing client %i...", i);
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
    Console->Print("Shutting down Gameserver...");
    for(int i=0; i<mMaxClients+mGMSlots; i++)
    {
        if(mClients[i])
        {
            delete mClients[i];
            mClients[i]=0;
        }
    }
}

//SELECT `s_name` , NOW( ) , `s_lastupdate` , NOW( ) - `s_lastupdate` FROM `server_list` 
