#include "gameserver/decoder/Includes.hxx"

/**** PPvPTrade ****/

PUdpPvPTrade::PUdpPvPTrade(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x3e";
}

PUdpMsgAnalyser* PUdpPvPTrade::Analyse()
{
    mDecodeData->mName << "=Switch PvP Trading";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpPvPTrade::DoAction()
{
    // TODO: Handle client PvP trade switch on/off
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
