#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdp0x13 ****/

PUdp0x13::PUdp0x13( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
    nDecodeData->mName << "/0x13";
}

PUdpMsgAnalyser* PUdp0x13::Analyse()
{
    PMessage* TmpMsg = mDecodeData->mMessage;
    PUdpMsgAnalyser* nextAnalyser = nullptr;

    if ( ! mDecodeData->mHandling0x13Sub ) // First decoding pass
    {
    //Console->Print(" --- New 0x13 msg");
    //TmpMsg->Dump();
        ( *TmpMsg ) >> ( mDecodeData->mClientState->UDP.mSequence ); // "LastPacket"
        ( *TmpMsg ) >> ( mDecodeData->mClientState->UDP.mServerPacketNum ); //PID
        mDecodeData->Sub0x13StartNext = 0;
    }

    if ( TmpMsg->EOM() )
    {
        Console->Print( RED, BLACK, "PUdp0x13::Analyse(): Emptied 0x13 msg handling !!!" );
        mDecodeData->mState = DECODE_FINISHED;
        mDecodeData->mHandling0x13Sub = false;
        nextAnalyser = this;
        mDecodeData->Sub0x13Start = mDecodeData->Sub0x13StartNext = 0;
    }
    else
    {
        if ( mDecodeData->mHandling0x13Sub && mDecodeData->Sub0x13StartNext )
        {
            TmpMsg->SetNextByteOffset( mDecodeData->Sub0x13StartNext );
//Console->Print("Multi 0x13 msg continuing at %d", TmpMsg->GetNextByteOffset());
        }
        mDecodeData->Sub0x13Start = TmpMsg->GetNextByteOffset();
        uint8_t PSize = TmpMsg->U8Data( mDecodeData->Sub0x13Start );
        uint16_t EndOffset = mDecodeData->Sub0x13StartNext = mDecodeData->Sub0x13Start + 1 + PSize;

        if ( EndOffset >= TmpMsg->GetSize() )
        {
            mDecodeData->mHandling0x13Sub = false;
            //mDecodeData->Sub0x13Start = mDecodeData->Sub0x13StartNext = 0;
//Console->Print("Simple 0x13 msg");
            if ( EndOffset > TmpMsg->GetSize() )
            {
                Console->Print( RED, BLACK, "PUdp0x13::Analyse(): Size error in 0x13 msg handling !!!" );
                mDecodeData->mState = DECODE_ERROR;
                mDecodeData->mErrorDetail = "Submessage too long.";
                return this;
            }
        }
        else
        {
//Console->Print("Multi 0x13 msg. End at %d for total length %d", EndOffset, TmpMsg->GetSize());
            mDecodeData->mHandling0x13Sub = true;
        }

        mDecodeData->mState = DECODE_MORE;
        uint8_t MsgType = TmpMsg->U8Data( mDecodeData->Sub0x13Start + 1 );
        /*uint16_t PSeq = 0; // This will have to be handled to detected missing granted-delivery messages
        if (MsgType == 0x03)
        PSeq = *(uint16_t*)&Buf[Offset+2];*/
//Console->Print("0x13 Type: %d", MsgType);
        switch ( MsgType )
        {
        case 0x03:
        {
            mDecodeData->mName << "/0x03";
            uint8_t MsgSubType = TmpMsg->U8Data( TmpMsg->GetNextByteOffset() + 4 );
            switch ( MsgSubType )
            {
            case 0x01: // Out of order
            {
                nextAnalyser = new PUdpOOO( mDecodeData );
                break;
            }
            case 0x07: // Fragmented message
            {
                nextAnalyser = new PUdpMultiPart( mDecodeData );
                break;
            }
            case 0x08: // Client zoning completed (!!! does not happen on login)
            {
                nextAnalyser = new PUdpEndOfZoning( mDecodeData );
                break;
            }
            case 0x1f:
            {
                nextAnalyser = new PUdp0x1f( mDecodeData );
                break;
            }
            case 0x22:
            {
                nextAnalyser = new PUdp0x22( mDecodeData );
                break;
            }
            case 0x2b: // Citycom (General Terminal?)
            {
                nextAnalyser = new PUdp0x2b( mDecodeData );
                break;
            }
            case 0x27: // Request for more information about WorldID
            {
                nextAnalyser = new PWorldIDInfoReq( mDecodeData );
                break;
            }

            case 0x24: // ? 06 03 03 00 24 01 00
            default:
            {
                mDecodeData->mUnknownType = MsgSubType;
                break;
            }
            }
            break;
        }

        case 0x0b: // Ping
        {
            nextAnalyser = new PUdpPing( mDecodeData );
            break;
        }

        case 0x0c: // Baseline
        {
            nextAnalyser = new PUdpSync2( mDecodeData );
            break;
        }

        case 0x1f:
        {
            mDecodeData->mName << "/0x1f";
            uint8_t MsgSubType = TmpMsg->U8Data( TmpMsg->GetNextByteOffset() + 4 );
            if ( MsgSubType == 0x15 ) // Weapon reload animation start
            {
                nextAnalyser = new PUdpReloadAnimStart( mDecodeData );
            }
            else
            {
                mDecodeData->mUnknownType = MsgType;
                mDecodeData->mTraceUnknownMsg = true;
            }
            break;
        }

        case 0x20: // Char move
        {
            nextAnalyser = new PUdpCharPosUpdate( mDecodeData );
             break;
        }

        case 0x27: // Sitting object info request
        {
            nextAnalyser = new PUdpRequestVhcInfo( mDecodeData );
            break;
        }

        case 0x2a: // "Packet0"
        {
            nextAnalyser = new PUdpPacket0( mDecodeData );
            break;
        }

        case 0x2d: // sent when targeting another char
        {
            nextAnalyser = new PUdpCharTargeting( mDecodeData );
            break;
        }

        case 0x32: // Vhc move
        {
            mDecodeData->mName << "/0x32";
            uint8_t MsgSubType = TmpMsg->U8Data( TmpMsg->GetNextByteOffset() + 4 );
            switch ( MsgSubType )
            {
            case 0x00: // Subway position update
            {
                nextAnalyser = new PUdpSubwayUpdate( mDecodeData );
                break;
            }
            case 0x03: // Vhc position update
            {
                nextAnalyser = new PUdpVhcMove( mDecodeData );
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

        default:
        {
            mDecodeData->mUnknownType = MsgType;
            break;
        }
        }

        if ( ! nextAnalyser )
        {
            nextAnalyser = new PUdpMsgUnknown( mDecodeData );
        }

    }
    return nextAnalyser;
}
