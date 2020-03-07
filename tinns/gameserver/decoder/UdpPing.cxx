#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpPing ****/

PUdpPing::PUdpPing( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x0b";
}

PUdpMsgAnalyser* PUdpPing::Analyse()
{
  mDecodeData->mName << "=Ping";

  mClientTime = mDecodeData->mMessage->U32Data( mDecodeData->Sub0x13Start + 2 );

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpPing::DoAction()
{
  if ( mDecodeData->mState & DECODE_ACTION_READY )
  {
    // if(gDevDebug)
    //  Console->Print( "%s PUdpPing: Client timestamp %d (0x%08x)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mClientTime, mClientTime );

    PMessage* tmpMsg = MsgBuilder->BuildPingMsg( mDecodeData->mClient, mClientTime );
    mDecodeData->mClient->SendUDPMessage( tmpMsg );

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false;
}
