#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PWorldIDInfoReq ****/

PWorldIDInfoReq::PWorldIDInfoReq(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x27";
}

PUdpMsgAnalyser* PWorldIDInfoReq::Analyse()
{
    PMessage* tmpMsg = mDecodeData->mMessage;
    mDecodeData->mName << "=WorldID Info Req.";

    tmpMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 5);
    (*tmpMsg) >> mInfoId;

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PWorldIDInfoReq::DoAction()
{
    PNPC* targetNPC = nullptr;

    PNPCWorld* currentNPCWorld = NPCManager->GetWorld( mDecodeData->mClient->GetChar()->GetLocation() );
    if ( currentNPCWorld )
    {
        targetNPC = currentNPCWorld->GetNPC( mInfoId );
        if(!targetNPC)
        {
            // Search for DEF version of NPC (remember, def IDs are on 255 offset!
            // Note to myself: This is UGLY!!!! and BAD!!! but it works for now. CHANGE THIS!
            targetNPC = currentNPCWorld->GetNPC( mInfoId - 255 );
        }
    }
    if (!targetNPC)
    {
        // No NPC, skipping
        return true;
    }

    currentNPCWorld->SendSingleNPCInfo(mDecodeData->mClient, targetNPC);
    // TODO: Handle client request here!
    // Note: It *seems* that the client sends this "eek, giev info about ID xxx"
    // if the NPC/WorldID is "not in the world", means the coods are pointing
    // the NPC somewhere below ground or inside a wall. However, not 100% sure about that
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
