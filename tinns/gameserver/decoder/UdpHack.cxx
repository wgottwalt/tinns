#include "gameserver/decoder/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpHackSuccess ****/

PUdpHackSuccess::PUdpHackSuccess(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x29";
}

PUdpMsgAnalyser* PUdpHackSuccess::Analyse()
{
    mDecodeData->mName << "=Hackgame was successfull";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackSuccess::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();
    PWorld* CurrentWorld = Worlds->GetWorld(tChar->GetLocation());
    const PFurnitureItemTemplate* tFurnitureTemplate = nullptr;
    const PDefWorldModel* tFurnitureModel = nullptr;

    uint32_t mRawItemID = mDecodeData->mMessage->U32Data(mDecodeData->Sub0x13Start+8);
    if(gDevDebug) Console->Print("DEBUG: Client %d successfully hacked object %d", mDecodeData->mClient->GetID(), mRawItemID);

    bool tHandleDynamicActor = false;
    // First try to find out if we have an dynamic worldactor
    if(WorldActors->IsDynamicActor(mRawItemID) == true)
    {
        // Now get the get the function value: (What kind of hackable object)
        int tFunctionVal = WorldActors->GetWorldActorFunctionID(mRawItemID);

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc. Here: Hack difficult
        tFurnitureModel = GameDefs->WorldModels()->GetDef(tFunctionVal);
        tHandleDynamicActor = true;
    }
    else
    {
        // Dat files have smaller IDs
        uint32_t ItemID = mRawItemID/1024 -1;

        // Now grab the template from .dat file
        tFurnitureTemplate = CurrentWorld->GetFurnitureItemTemplate(ItemID);

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc
        tFurnitureModel = CurrentWorld->GetFurnitureItemModel(ItemID);
    }


    if(tHandleDynamicActor == false && tFurnitureTemplate != nullptr)
    {
        if (tFurnitureTemplate->GetLinkedObjectID())
        {
            PMessage* tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor(tFurnitureTemplate->GetLinkedObjectID())->IsDoubleDoor());
            ClientManager->UDPBroadcast(tmpMsg, nClient);
        }
        else
        {
            PMessage* tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg(nClient, mRawItemID);
            nClient->SendUDPMessage(tmpMsg);
        }
    }
    else
    {
        if (WorldActors->GetLinkedObjectID(mRawItemID))
        {
            PMessage* tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+WorldActors->GetLinkedObjectID(mRawItemID), CurrentWorld->GetDoor(WorldActors->GetLinkedObjectID(mRawItemID))->IsDoubleDoor());
            ClientManager->UDPBroadcast(tmpMsg, nClient);
        }
        else
        {
            PMessage* tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg(nClient, mRawItemID);
            nClient->SendUDPMessage(tmpMsg);
        }
    }

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}


/**** PUdpHackFail ****/

PUdpHackFail::PUdpHackFail(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x2C";
}

PUdpMsgAnalyser* PUdpHackFail::Analyse()
{
    mDecodeData->mName << "=Hackgame failed";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackFail::DoAction()
{
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
