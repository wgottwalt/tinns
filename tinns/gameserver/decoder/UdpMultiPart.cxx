#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpMultiPart ****/

PUdpMultiPart::PUdpMultiPart( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x07";
}

PUdpMsgAnalyser* PUdpMultiPart::Analyse()
{
    mDecodeData->mName << "=Multipart Message";

    PMessage* TmpMsg = mDecodeData->mMessage;

    uint8_t tChunkSize = 0;

    ( *TmpMsg ) >> tChunkSize;
    TmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 5 );
    ( *TmpMsg ) >> mChunkNr;
    ( *TmpMsg ) >> mChunkTotal;
    ( *TmpMsg ) >> mSequence;

    tChunkSize -= 9;
    int tCurPos = mDecodeData->Sub0x13Start + 10;

    mChunk = new PMessage(tChunkSize);
    mChunk = TmpMsg->GetChunk(tCurPos, tChunkSize);

    //if (gDevDebug) Console->Print("Received MULTIPART fragment. ChunkNr: %d of %d, SequenceNr: %d ChunkSize: %d", mChunkNr, mChunkTotal, mSequence, mChunk->GetSize());
    //if (gDevDebug) mChunk->Dump();

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpMultiPart::DoAction()
{
    if ( mDecodeData->mState & DECODE_ACTION_READY )
    {
        MultiPartHandler->AddMultiPartChunk(mDecodeData->mClient, mChunk, mChunkNr, mChunkTotal, mSequence);
        return true;
    }
    else
        return false;
}
