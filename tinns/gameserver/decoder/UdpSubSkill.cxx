#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpSubskillInc ****/

PUdpSubskillInc::PUdpSubskillInc(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x04";
}

PUdpMsgAnalyser* PUdpSubskillInc::Analyse()
{
  mDecodeData->mName << "=Subskill increase request";
  SubskillID = mDecodeData->mMessage->U16Data(mDecodeData->Sub0x13Start+9);

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpSubskillInc::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* nChar = nClient->GetChar();

    // Function tries to increase given subskill. If it fails, returnvalue is -1.
    // If success, returnvalue is amount of skillpoints left
    int tRemainingSkillPoints = 0;
    tRemainingSkillPoints = nChar->Skill->IncreaseSubSkill(SubskillID);

    if ( tRemainingSkillPoints > -1 )
    {
        PMessage* tmpMsg = MsgBuilder->BuildSubskillIncMsg(nClient, SubskillID, tRemainingSkillPoints); // last is remaining skillpoints
        nClient->SendUDPMessage(tmpMsg);
    }
    else
    {
        // Send NACK, or just ignore..
    }
    /*if(nChar->Skill->IncSubSkillPossible(SubskillID) == true)
    {
        int tRemainingSkillPoints = 0;
        nChar->Skill->IncreaseSubSkill(SubskillID); // SubskillID
        PMessage* tmpMsg = MsgBuilder->BuildSubskillIncMsg(nClient, SubskillID, tRemainingSkillPoints); // last is remaining skillpoints
        nClient->SendUDPMessage(tmpMsg);
    }*/

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
