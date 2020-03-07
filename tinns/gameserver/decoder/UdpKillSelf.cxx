#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"

PUdpKillSelf::PUdpKillSelf( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x10";
}

PUdpMsgAnalyser* PUdpKillSelf::Analyse()
{
  mDecodeData->mName << "=Char Kill self";

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpKillSelf::DoAction()
{
  PClient* nClient = mDecodeData->mClient;

  PMessage* tmpMsg = MsgBuilder->BuildCharDeathMsg( nClient, 0 );
  ClientManager->UDPBroadcast( tmpMsg, nClient );

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;

  /* Server resp:
  13:a8:00:a8:e2:
  16: 03:a5:00:1b:01:10:00:80:24:ea:7c:cb:80:25:77:80:86:80:62:00:01:00: // Spwan belt object (?)
  09: 03:a6:00:1f:01:00:25:23:1c: // "UndefinedUseMsg" with arg 0x1c
  0b: 03:a7:00:1f:01:00:16:00:00:00:00: // Char Dead status
  0e: 03:a8:00:1f:01:00:25:13:10:19:02:02:0d:00 // Inventory delete item (=item dropped to belt)
  */
}
