#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpOutfitter ****/

PUdpOutfitter::PUdpOutfitter(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x2e";
}

PUdpMsgAnalyser* PUdpOutfitter::Analyse()
{
    mDecodeData->mName << "=Outfitter validation";

    uint16_t tUnknown;
    PMessage* nMsg = mDecodeData->mMessage;
    nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 8);

    (*nMsg) >> mSkin;
    (*nMsg) >> tUnknown;
    (*nMsg) >> mHead;
    (*nMsg) >> mTorso;
    (*nMsg) >> mLegs;
    (*nMsg) >> mHeadColor;
    (*nMsg) >> mTorsoColor;
    (*nMsg) >> mLegsColor;
    (*nMsg) >> mHeadDarkness;
    (*nMsg) >> mTorsoDarkness;
    (*nMsg) >> mLegsDarkness;

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpOutfitter::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();

    /* Some validity checks against char model and equipment can be done here */
    /* as well as payment */

    tChar->SetCurrentLook(mSkin, mHead, mTorso, mLegs);
    tChar->SetCurrentBodyColor(mHeadColor, mTorsoColor, mLegsColor, mHeadDarkness, mTorsoDarkness, mLegsDarkness);

    PMessage* tmpMsg = MsgBuilder->BuildCharHelloMsg(nClient);
    ClientManager->UDPBroadcast(tmpMsg, nClient);


    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
