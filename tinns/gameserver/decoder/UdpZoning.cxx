#include <cstring>
#include "gameserver/decoder/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpZoning1 ****/

PUdpZoning1::PUdpZoning1( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x0d";
}

PUdpMsgAnalyser* PUdpZoning1::Analyse()
{
  mDecodeData->mName << "=Zoning phase 1";

  PMessage* cMsg = mDecodeData->mMessage;
  uint8_t dumb8;
  uint16_t dumb16;
  //mUnknown = cMsg->U16Data(mDecodeData->Sub0x13Start+7);
  //mNewLocation = cMsg->U32Data(mDecodeData->Sub0x13Start+11);
  //mNewEntity = cMsg->U16Data(mDecodeData->Sub0x13Start+15);

  cMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 7 );
  *cMsg >> dumb8; // uint8_t = 0x01 in NC1, other in NC2.2 ?
  *cMsg >> mUnknown; //uint8_t
  *cMsg >> dumb16; //uint16_t unkown use
  *cMsg >> mNewLocation; //uint32_t
  *cMsg >> mNewEntity; //uint16_t
  //*cMsg >> dumb16; //uint16_t 0x0000
//Console->Print("Zoning Stage 1: New location: %d, Entity %d, Unknown %d", mNewLocation, mNewEntity, (uint16_t)mUnknown);
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpZoning1::DoAction()
{
  mDecodeData->mClient->ChangeCharLocation( mNewLocation );

//Console->Print("Zoning Stage 1: New location: %d", mNewLocation);

  mDecodeData->mClient->SetZoning();

  PMessage* tmpMsg = MsgBuilder->BuildZoning1Msg( mDecodeData->mClient, mNewEntity, mUnknown );
  mDecodeData->mClient->SendUDPMessage( tmpMsg );

  //Temp
  /*if ( mDecodeData->mClient->GetDebugMode( DBG_LOCATION ) )
  {
    char DbgMessage[128];
    PCharCoordinates &C = (mDecodeData->mClient->GetChar()->Coords);
    snprintf( DbgMessage, 128, "pos min/max y:%d/%d z:%d/%d x:%d/%d", C.minPos[0], C.maxPos[0], C.minPos[1], C.maxPos[1], C.minPos[2], C.maxPos[2] );
    Chat->send( mDecodeData->mClient, CHAT_GM, "Debug", DbgMessage );

    for(int i=0; i<3; i++)
      { C.minPos[i] = 0xffff; C.maxPos[i] = 0; }
  }*/

//Console->Print("Zoning Stage 1: packet sent");
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpZoning2 ****/

PUdpZoning2::PUdpZoning2( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x03";
}

PUdpMsgAnalyser* PUdpZoning2::Analyse()
{
  mDecodeData->mName << "=Zoning phase 2";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpZoning2::DoAction()
{
//Console->Print("Zoning Stage 2: Sending Ready packet");
  PMessage* tmpMsg = MsgBuilder->BuildZoningTCPReadyMsg(); // Not always sent (dongeon door 20)
  mDecodeData->mClient->SendTCPMessage( tmpMsg );

//Console->Print("Zoning Stage 2: Sending Zone information");
  PUdpSync0::GetToSync1( mDecodeData );

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpGenrepZoning ****/

PUdpGenrepZoning::PUdpGenrepZoning( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x04";
}

PUdpMsgAnalyser* PUdpGenrepZoning::Analyse()
{
  mDecodeData->mName << "=Genrep Zoning";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  PMessage* cMsg = mDecodeData->mMessage;
  cMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 12 );
  *cMsg >> mNewLocation; // uint32_t
  *cMsg >> mNewEntity; //uint16_t

  return this;
}

bool PUdpGenrepZoning::DoAction()
{
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return PUdpGenrepZoning::DoEffectiveZoning( mDecodeData->mClient, mNewLocation, mNewEntity );
}

bool PUdpGenrepZoning::DoEffectiveZoning( PClient* nClient, uint32_t  nNewLocation, uint16_t nNewEntity )
{
  PMessage* tmpMsg = MsgBuilder->BuildGenrepZoningMsg( nClient, nNewLocation, nNewEntity );
  nClient->SendUDPMessage( tmpMsg );

  //Client_Sockets[ClientNum].CharInfo.Flags = PFLAG_ZONING; //Player started zoning
  nClient->ChangeCharLocation( nNewEntity );

  tmpMsg = MsgBuilder->BuildZoning1Msg( nClient, nNewEntity );
  nClient->SendUDPMessage( tmpMsg );

  if ( gDevDebug ) Console->Print( "Client[%d]: Genrep Zoning to zone %d entity %d", nClient->GetID(), nNewLocation, nNewEntity );

  return true;
}

/**** PUdpAptGRZoning ****/

PUdpAptGRZoning::PUdpAptGRZoning( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x03";
}

PUdpMsgAnalyser* PUdpAptGRZoning::Analyse()
{
  mDecodeData->mName << "=Genrep Apt Zoning";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpAptGRZoning::DoAction()
{
  /*
    //uint16_t newEntity = cMsg->U16Data(mDecodeData->Sub0x13Start+12); // always 0x0047 ? Not a location/entity anyway...

    uint32_t newLocation = PWorlds::mAptBaseWorldId + nClient->GetChar()->GetBaseApartment();
    uint16_t newEntity = 0;
  */

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return PUdpAptGRZoning::DoEffectiveZoning( mDecodeData->mClient );
}

bool PUdpAptGRZoning::DoEffectiveZoning( PClient* nClient )
{
  uint32_t newLocation = PWorlds::mAptBaseWorldId + nClient->GetChar()->GetBaseApartment();
  uint16_t newEntity = 0;

  PMessage* tmpMsg = MsgBuilder->BuildGenrepZoningMsg( nClient, newLocation, newEntity );
  nClient->SendUDPMessage( tmpMsg );

  if ( ! nClient->ChangeCharLocation( newLocation ) )
    Console->Print( "Client[%d]: Bad Apartment location %d", nClient->GetID(), newLocation );

  //Client_Sockets[ClientNum].CharInfo.Flags = PFLAG_ZONING; //Player started zoning

  tmpMsg = MsgBuilder->BuildZoning1Msg( nClient, newEntity );
  nClient->SendUDPMessage( tmpMsg );

  if ( gDevDebug )
    Console->Print( "Client[%d]: Genrep Zoning to Base Apartment (zone %d - entity %d)", nClient->GetID(), newLocation, newEntity );

  return true;
}

/**** PUdpAddGenrepToList ****/

PUdpAddGenrepToList::PUdpAddGenrepToList( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x02";
}

PUdpMsgAnalyser* PUdpAddGenrepToList::Analyse()
{
  mDecodeData->mName << "=Add Genrep to list";

  PMessage* nMsg = mDecodeData->mMessage;
  // LocalID @offset 10
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 12 );
  *nMsg >> mLocation;
  *nMsg >> mEntity;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpAddGenrepToList::DoAction()
{
  PClient* nClient = mDecodeData->mClient;

//Console->Print("Client[%d]: Adding Genrep (location %d - entity %d)", nClient->GetID(), mLocation, mEntity);

  if ( mLocation == 1086 && mEntity == 1111 )
  {
    PMessage* tmpMsg = MsgBuilder->BuildTextIniMsg( nClient, 6, 160 );
    nClient->SendUDPMessage( tmpMsg );
  }
  else
  {
    nClient->GetChar()->AddGenrep( mLocation, mEntity );
    PMessage* tmpMsg = MsgBuilder->BuildGenrepAddToListMsg( nClient, mLocation, mEntity );
    nClient->SendUDPMessage( tmpMsg );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpAppartmentAccess ****/

PUdpAppartmentAccess::PUdpAppartmentAccess( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x38";
}

PUdpMsgAnalyser* PUdpAppartmentAccess::Analyse()
{
  mDecodeData->mName << "=Try acces appartment";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 9 );
  mAppartmentPlace = nMsg->U16Data( mDecodeData->Sub0x13Start + 9 );
  mPassword = ( char* )nMsg->GetMessageData() + mDecodeData->Sub0x13Start + 14;
  // NO SIZE ????? DO SIZE CHECK !

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpAppartmentAccess::DoAction()
{
  PMessage* tmpMsg;
  uint32_t AppLoc, Location, Entity;
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();

  Location = nChar->GetLocation();
  if ( gDevDebug ) Console->Print( "Client[%d]: Apt Access I/F (place %d - password %s)", nClient->GetID(), mAppartmentPlace, mPassword );
  if (( Location > PWorlds::mAptBaseWorldId ) && ( !strcmp( "Exit", mPassword ) ) )
  {
    AppLoc = Appartements->GetAptLocation( Location );
    const PDefAppPlace* nAppPlace = ( AppLoc ? GameDefs->AppPlaces()->GetDef( AppLoc ) : 0 );
    if ( nAppPlace )
    {
      Location = nAppPlace->GetExitWorldID();
      Entity = nAppPlace->GetExitWorldEntity();
    }
    else
    {
      Location = 1; //PLAZA 1
      Entity = 100; //TYPHERRA MEMORIAL GR 1
    }
    tmpMsg = MsgBuilder->BuildChangeLocationMsg( nClient, Location, Entity );
  }
  else
  {
    uint32_t Location = Appartements->GetAptID( mAppartmentPlace, ( uint8_t* )mPassword );
    if ( Location > 1 )
    {
      tmpMsg = MsgBuilder->BuildAptLiftUseMsg( nClient, Location, 1 );
      nClient->ChangeCharLocation( Location );
    }
    else if ( Location < 1 )
    {
      tmpMsg = MsgBuilder->BuildAptLiftFailedMsg( nClient );
    }
    else
      tmpMsg = nullptr;

    mDecodeData->mTraceDump = true;
  }

  if ( tmpMsg )
    nClient->SendUDPMessage( tmpMsg );

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpEndOfZoning ****/

PUdpEndOfZoning::PUdpEndOfZoning( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x08";
}

PUdpMsgAnalyser* PUdpEndOfZoning::Analyse()
{
  mDecodeData->mName << "=End of zoning";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpEndOfZoning::DoAction()
{
// Nothing implemented yet
  mDecodeData->mClient->SetZoning( false );
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
