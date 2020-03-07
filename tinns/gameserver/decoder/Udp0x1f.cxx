#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdp0x1f ****/

PUdp0x1f::PUdp0x1f( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdp0x1f::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = nullptr;
  mDecodeData->mState = DECODE_MORE;
  uint8_t MsgType = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 7 );
  uint8_t MsgSubType = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 8 );

  switch ( MsgType )
  {
    case 0x00:
    {
      nextAnalyser = new PUdpHeldItemBasicAction( mDecodeData );
      break;
    }
    case 0x01:
    {
      nextAnalyser = new PUdpHeldItemAimedAction( mDecodeData );
      break;
    }
    case 0x02:
    {
      nextAnalyser = new PUdpCharJump( mDecodeData );
      break;
    }
    case 0x17:
    {
      nextAnalyser = new PUdpUseObject( mDecodeData );
      break;
    }
    case 0x19: // NPC Dialog closed
    {
      nextAnalyser = new PUdpNPCDialogClose( mDecodeData );
      break;
    }
    case 0x1a: // NPC Dialog action/reply
    {
      nextAnalyser = new PUdpNPCDialogAction( mDecodeData );
      break;
    }
    case 0x1b:
    {
      nextAnalyser = new PUdpChatLocal( mDecodeData );
      break;
    }
    case 0x1e:  // item move QB<>INV<>GND
    {
      nextAnalyser = new PUdpItemMove( mDecodeData );
      break;
    }
    case 0x1f: // Slot use
    {
      nextAnalyser = new PUdpItemSlotUse( mDecodeData );
      break;
    }
    case 0x20: // Use item for hacking, launcher, "launcher" spell
    {
      nextAnalyser = new PUdpHeldItemLaunchingAction( mDecodeData );
      break;
    }
    case 0x22:
    {
      nextAnalyser = new PUdpCharExitChair( mDecodeData );
      break;
    }
    case 0x25:
    {
      mDecodeData->mName << "/0x25";
      switch ( MsgSubType )
      {
        case 0x04: // Hack announcement?
        {
          nextAnalyser = new PUdpSubskillInc( mDecodeData );
          break;
        }
        case 0x0c: // addons activation
        {
          nextAnalyser = new PUdpItemAddonActivation( mDecodeData );
          break;
        }
        case 0x14: // Hack announcement?
        {
          nextAnalyser = new PUdpItemMoveBP( mDecodeData );
          break;
        }
        case 0x16: // ???? confirm reload anim start ???
        {
          mDecodeData->mUnknownType = MsgSubType;
          mDecodeData->mTraceUnknownMsg = true;
          break;
        }
        case 0x17: // Item drop on item
        {
          nextAnalyser = new PUdpItemDropOnItem( mDecodeData );
          break;
        }
        case 0x18:
        {
          mDecodeData->mName << "/0x18";
          switch ( mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 9 ) )
          {
            case 0x01: // Use inventory item
            {
              nextAnalyser = new PUdpItemUse( mDecodeData );
              break;
            }
            case 0x0e: // Request manual reload
            {
              nextAnalyser = new PUdpItemManualReload( mDecodeData );
              break;
            }
            default:
            {
              mDecodeData->mUnknownType = MsgSubType;
              mDecodeData->mTraceUnknownMsg = true;
              break;
            }
          }
          break;
        }

        case 0x1d: // ? before Packet0: 0b 03 01 00 1f 00 00 25 1d 00 00 01
        case 0x1e: // ? before Packet0: 0d 03 02 00 1f 00 00 25 1e ff ff ff ff 00
        default:
        {
          mDecodeData->mUnknownType = MsgSubType;
          mDecodeData->mTraceUnknownMsg = true;
          break;
        }
      }
      break;
    }
    case 0x27:
    {
      nextAnalyser = new PUdpCloseItemContainer( mDecodeData );
      break;
    }
    case 0x29:
    {
      nextAnalyser = new PUdpHackSuccess( mDecodeData );
      break;
    }
    case 0x2c:
    {
      nextAnalyser = new PUdpHackFail( mDecodeData );
      break;
    }
    case 0x2e:
    {
      nextAnalyser = new PUdpOutfitter( mDecodeData );
      break;
    }
    case 0x2f:
    {
      nextAnalyser = new PUdpDeathRespawn( mDecodeData );
      break;
    }
    case 0x33:
    {
      nextAnalyser = new PUdpChatListAdd( mDecodeData );
      break;
    }
    case 0x38:
    {
      nextAnalyser = new PUdpAppartmentAccess( mDecodeData );
      break;
    }
    case 0x39:
    {
      nextAnalyser = new PUdpChatListRemove( mDecodeData );
      break;
    }
    case 0x3b:
    {
      nextAnalyser = new PUdpChatGlobal( mDecodeData );
      break;
    }
    case 0x3d:
    {
      mDecodeData->mName << "/0x3d";
      switch ( MsgSubType ) // In fact MsgSubType is U32, but only lower byte is used
      {
        case 0x02:
        {
          nextAnalyser = new PUdpAddGenrepToList( mDecodeData );
          break;
        }
        case 0x03:
        {
          nextAnalyser = new PUdpAptGRZoning( mDecodeData );
          break;
        }
        case 0x04:
        {
          nextAnalyser = new PUdpGenrepZoning( mDecodeData );
          break;
        }
        case 0x09:
        {
          nextAnalyser = new PUdpPopupResponse( mDecodeData );
          break;
        }
        case 0x0a:
        {
          nextAnalyser = new PUdpAptLocInfo( mDecodeData );
          break;
        }
        case 0x0f:
        {
          nextAnalyser = new PUdpVhcUse( mDecodeData );
          break;
        }
        case 0x10:
        {
          nextAnalyser = new PUdpKillSelf( mDecodeData );
          break;
        }
        default:
        {
          mDecodeData->mUnknownType = MsgSubType;
          break;
        }
      }
      break;
    }
    case 0x3e:
    {
        nextAnalyser = new PUdpPvPTrade( mDecodeData );
        break;
    }
    case 0x4c:
    {
      nextAnalyser = new PUdpChatChannels( mDecodeData );
      break;
    }
    default:
    {
      mDecodeData->mUnknownType = MsgType;
      mDecodeData->mTraceUnknownMsg = true;
      break;
    }
  }

  if ( ! nextAnalyser )
  {
    nextAnalyser = new PUdpMsgUnknown( mDecodeData );
  }

  return nextAnalyser;
}
