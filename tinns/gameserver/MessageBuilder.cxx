#include <cstring>
#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

PMessage* PMsgBuilder::BuildOutpostClanInfoMsg( PClient* nClient, uint32_t nClanID, uint8_t nFaction )
{
    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();

    *tmpMsg << (uint8_t)0x13;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint16_t)nClient->GetSessionID();
    *tmpMsg << (uint8_t)0x13; // Message length
    *tmpMsg << (uint8_t)0x03;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint8_t)0x23;

    //*tmpMsg << (uint16_t)GetArgInt(2);
    *tmpMsg << (uint16_t)14;
    *tmpMsg << (uint8_t)0x00;
    *tmpMsg << (float)nClanID; // ClanID float
    *tmpMsg << (uint8_t)nFaction;  // Faction
    // The next 3 bytes are unknown.
    // However, with this combination, the OP and its bonuses are
    // set correctly; eg Own clan full bonus, friend clan 75%, etc
    *tmpMsg << (uint8_t)0x06; // 6? 0x06
    *tmpMsg << (uint8_t)0x14; // 0? 0x14
    *tmpMsg << (uint8_t)0x0b; // 11? 0x0b
    *tmpMsg << (uint32_t)nClanID; // ClanID uint32_t

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharHelloMsg( PClient* nClient )
{
    PChar *nChar = nClient->GetChar();
    uint32_t nSkin, nHead, nTorso, nLegs;
    uint8_t nHeadColor, nTorsoColor, nLegsColor, nHeadDarkness, nTorsoDarkness, nLegsDarkness;

    nChar->GetCurrentLook( nSkin, nHead, nTorso, nLegs );
    nChar->GetCurrentBodyColor( nHeadColor, nTorsoColor, nLegsColor, nHeadDarkness, nTorsoDarkness, nLegsDarkness );

    PMessage* tmpMsg = new PMessage( );

    uint8_t currentActiveSlot = nChar->GetQuickBeltActiveSlot();
    uint16_t weaponId = 0;
    switch ( currentActiveSlot )
    {
    case INV_WORN_QB_NONE:
        //weaponId = 0;
        break;
    case INV_WORN_QB_HAND:
        weaponId = 5; // <= TODO: set it somewhere
        break;
    default:
        PItem* currentItem = nChar->GetInventory()->GetContainer( INV_LOC_WORN )->GetItem( INV_WORN_QB_START + currentActiveSlot );
        //Todo : item addons & effects
        //ItemInHandID = currentItem->GetItemID():
        if ( currentItem )
            weaponId = currentItem->GetValue1();
        //else
        //ItemInHandID = 0;
        break;
    }

    //nClient->IncreaseUDP_ID(); // This must be done outside

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x00; // size placeholder, set later in the function
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x25;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint32_t )nChar->GetID();

    *tmpMsg << ( uint8_t )0x60; // 0x40 if current faction epic done (master), | 0x80 to display [afk] | 0x20 if LE in
    *tmpMsg << ( uint8_t )(( nChar->GetSpeedOverride() == 255 ) ? 10 : nChar->GetSpeedOverride() ); // move speed, reset by client (and for him only) when getting fall damage
    *tmpMsg << ( uint8_t )0x08; // ??? something to do with speed ?
    *tmpMsg << ( uint16_t )weaponId; // WeaponID of the weapon in hand
    *tmpMsg << ( uint8_t )0x01; // ???
    *tmpMsg << ( uint8_t )0x01; // ???
    *tmpMsg << ( uint8_t )( 128 + nChar->GetSoullight() );

    *tmpMsg << ( uint8_t )nChar->GetMainRank(); // in fact, Ranks are of type int8_t, but it doesn't matter much
    *tmpMsg << ( uint8_t )nChar->GetCombatRank();
    *tmpMsg << ( uint8_t )nChar->GetFaction();

    *tmpMsg << ( uint8_t )0x00; // This was 0x21 for an GM, with faction 0x00. However, no visible change when set...
    *tmpMsg << ( uint8_t )0x0f; // size of the next bloc (skin + ?clan?)
    // Current skin
    *tmpMsg << ( uint16_t )nSkin;
    *tmpMsg << ( uint8_t )nHead;
    *tmpMsg << ( uint8_t )nTorso;
    *tmpMsg << ( uint8_t )nLegs;
    // Skin colors
    *tmpMsg << ( uint8_t )nHeadColor;
    *tmpMsg << ( uint8_t )nTorsoColor;
    *tmpMsg << ( uint8_t )nLegsColor;
    *tmpMsg << ( uint8_t )nHeadDarkness; // (0=bright 255=dark)
    *tmpMsg << ( uint8_t )nTorsoDarkness;
    *tmpMsg << ( uint8_t )nLegsDarkness;
    *tmpMsg << ( uint8_t )0x00; // ??? << not sure at all // eg: 0x3e
    *tmpMsg << ( uint8_t )0x00; // eg: 0x03
    *tmpMsg << ( uint8_t )0x00; // eg: 0xa3
    *tmpMsg << ( uint8_t )0x00; // eg: 0x03

    //Name
    *tmpMsg << ( uint8_t )(( nChar->GetName() ).length() + 1 );
    *tmpMsg << ( nChar->GetName() ).c_str();
    //Body effects
    uint8_t cBodyEffect, cEffectDensity;
    nChar->GetBodyEffect( cBodyEffect, cEffectDensity );
    if ( cBodyEffect )
    {
        *tmpMsg << ( uint8_t )0x06; // size of effect list : 6 bytes/effect. Only one supported atm
        *tmpMsg << ( uint8_t )cBodyEffect; // effect type (0=none, effecive values 1 - 17)
        *tmpMsg << ( uint8_t )cEffectDensity; // density: 0=max, 0xff=min (for some effects only)
        *tmpMsg << ( uint8_t )0x00; // ???
        *tmpMsg << ( uint8_t )0x00; // ???
        *tmpMsg << ( uint8_t )0x00; // ???
        *tmpMsg << ( uint8_t )0x00; // ???
    }
    else
    {
        *tmpMsg << ( uint8_t )0x00; // size of empty effect list
    }

    // Clans working, yeah :D
    uint16_t tClanVal = nChar->GetClan();
    if(tClanVal > 0)
    {
        uint8_t tClanLevel = nChar->GetClanLevel();
        uint32_t tmpVal;
        tmpVal = tClanVal << 4;
        tmpVal |= tClanLevel;
        *tmpMsg << ( uint8_t )0x04;
        *tmpMsg << ( uint32_t )tmpVal;
    }
    else
        *tmpMsg << ( uint8_t )0x00; // ending null
    // alternate interpretation to this "ending null"/optional bloc:
    /* *tmpMsg << (uint8_t)0x04; // size of unknown bloc ... 0x00 when empty (aka the "ending null")
     *tmpMsg << (uint8_t)0x0b; // vary ... ??? 0b, eb, ee, ...
     *tmpMsg << (uint8_t)0x44; // vary ... ???
     *tmpMsg << (uint8_t)0x00; // these two seem always null
     *tmpMsg << (uint8_t)0x00; */

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildReqNPCScriptAnswerMsg( uint32_t nInfoId, std::string *nNPCScript )
 {
    PMessage* tmpMsg;
 
    tmpMsg = new PMessage();
 
    *tmpMsg << ( uint8_t )0x19;
    *tmpMsg << ( uint16_t )0x0006; // InfoQuery
    *tmpMsg << ( uint16_t )0x0003; // NPC Script
    *tmpMsg << ( uint32_t )nInfoId;
    *tmpMsg << nNPCScript->c_str();
 
    return tmpMsg;
 
 }
 
PMessage* PMsgBuilder::BuildYouGotEmailsMsg( PClient* nClient, uint8_t nMailCount )
 {
    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();
 
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0c;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3d;
    *tmpMsg << ( uint8_t )0x0c;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )nMailCount;
 
    return tmpMsg;
 }
 
PMessage* PMsgBuilder::BuildReceiveDBAnswerMsg( PClient* nClient, PMessage* nResultBuffer, std::string *nCommandName, uint16_t nNumRows, uint16_t nNumFields)
 {
    PMessage* tmpMsg = new PMessage();
/*    nClient->IncreaseUDP_ID();
 
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x2b;
    *tmpMsg << ( uint8_t )0x1a;
    if(nCommandName->length() > 0)
        *tmpMsg << ( uint8_t )0x01;
    else
        *tmpMsg << ( uint8_t )0x00;
 
     *tmpMsg << ( uint8_t )0x00;
     *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << *nCommandName;
    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
 
    // 2nd message
    *tmpMsg << ( uint16_t )(13 + nCommandName->length() + nResultBuffer->GetSize()); // ??
 */
    nClient->IncreaseUDP_ID();
 
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x2b;
    *tmpMsg << ( uint8_t )0x17;
    *tmpMsg << ( uint16_t )(nCommandName->length()+1);
    *tmpMsg << ( uint16_t )nNumRows;
    *tmpMsg << ( uint16_t )nNumFields;
    *tmpMsg << *nCommandName;
    *tmpMsg << *nResultBuffer;
 
    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
 
 
    return tmpMsg;
            //len = (unsigned int)strlen(DB);
            //SendBuffer[0] = 0x13;
            //SendBuffer[5] = 11 + len;
            //SendBuffer[6] = 0x03;
            //Network_IncrementUDP (ClientNum);
            //*(unsigned short*)&SendBuffer[7] = Client_Sockets[ClientNum].UDP_ID;
    //      SendBuffer[9] = 0x2b;
    //      SendBuffer[10] = 0x1a;
    //      *(unsigned short*)&SendBuffer[11] = len;
    //      if (num == 0)
    //          SendBuffer[13] = 0x00;
    //      else
    //          SendBuffer[13] = 0x01;
    //      SendBuffer[14] = 0x00;
    //      SendBuffer[15] = 0x00;
    //      strcpy (SendBuffer+16, DB);
    //      plen = 17+len;
 
    //      SendBuffer[plen] = 13+len+slen;
    //      SendBuffer[plen+1] = 0x03;
    //      Network_IncrementUDP (ClientNum);
    //      *(unsigned short*)&SendBuffer[plen+2] = Client_Sockets[ClientNum].UDP_ID;
    //      *(unsigned short*)&SendBuffer[1] = Client_Sockets[ClientNum].UDP_ID;
    //      *(unsigned short*)&SendBuffer[3] = Client_Sockets[ClientNum].UDP_ID_HIGH;
    //      SendBuffer[plen+4] = 0x2b;
    //      SendBuffer[plen+5] = 0x17;
    //      *(unsigned short*)&SendBuffer[plen+6] = len+1;
    //      *(unsigned short*)&SendBuffer[plen+8] = num;
    //      *(unsigned short*)&SendBuffer[plen+10] = Fields;
    //      //Fieldnum is defined in each DB below
    //      strcpy (SendBuffer+plen+12, DB);
    //      plen += 13+len;
 
    //      for (i=0;i<slen;i++)
    //          SendBuffer[plen+i] = TempBuffer[i];
 
 
 }
 
 
PMessage* PMsgBuilder::BuildTryAccessAnswerMsg(PClient* nClient, char *nArea, bool nAllowed)
 {
    PMessage* tmpMsg = new PMessage();
    //uint8_t i = (uint8_t)strlen(nArea);
 
    nClient->IncreaseUDP_ID();
 
    *tmpMsg << (uint8_t)0x13;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint16_t)nClient->GetSessionID();
    *tmpMsg << (uint8_t)0x00;
    *tmpMsg << (uint8_t)0x03;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint8_t)0x2b;
    *tmpMsg << (uint8_t)0x1a;
    *tmpMsg << (uint16_t)(strlen(nArea)+1);
 
    if(nAllowed)
        *tmpMsg << (uint8_t)0x01;
    else
        *tmpMsg << (uint8_t)0x00;
 
    *tmpMsg << (uint8_t)0x00;
    *tmpMsg << (uint8_t)0x00;
    *tmpMsg << nArea;
 
    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
    return tmpMsg;
 }

PMessage* PMsgBuilder::BuildReqInfoAnswerMsg( PClient* nClient, uint16_t nReqType, uint32_t nInfoId, void* nResponse, uint16_t nResponseLength )
{
    PMessage* tmpMsg;

    tmpMsg = new PMessage( 18 + nResponseLength );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x23;
    *tmpMsg << ( uint16_t )0x0006; // cmd
    *tmpMsg << ( uint16_t )nReqType; // wrong size here (uint32_t) for buffer size uint16_t in NeoX
    *tmpMsg << ( uint32_t )nInfoId;
    tmpMsg->Write( nResponse, nResponseLength );

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharHealthUpdateMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 14 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x30;
    *tmpMsg << ( uint8_t )0x64; //Head Heath =Head HP/(3 *0.45)(with max Head HP = 45% of total)
    *tmpMsg << ( uint8_t )0x64; //Body Heath =Body HP/(3 *0.35)(for max 35% of total)
    *tmpMsg << ( uint8_t )0x64; //Feet Heath =Feet HP/(3 *0.20)(for max 20% of total)
    *tmpMsg << ( uint8_t )0x01; // Sta/Mana ?

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharDeathMsg( PClient* nClient, uint32_t nKillerCharId )
{
    PMessage* tmpMsg = new PMessage( 17 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x16;
    *tmpMsg << ( uint32_t )nKillerCharId;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharPosUpdateMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 32 );
    PChar* nChar = nClient->GetChar();

    uint32_t cSeatObjectId;
    uint8_t cSeatId;
    PSeatType cSeatType = nChar->GetSeatInUse( &cSeatObjectId, &cSeatId );
    if ( cSeatType == seat_chair )   // temp ! Must migrate to RAW
    {
        cSeatObjectId = ( cSeatObjectId + 1 ) * 1024;
    }

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x1b;
    *tmpMsg << ( uint32_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x03;
    if ( cSeatType )
    {
        *tmpMsg << ( uint32_t )cSeatObjectId;
        *tmpMsg << ( uint16_t )cSeatId; // testing... else 0x0000
    }
    else
    {
        *tmpMsg << ( uint16_t )(( nChar->Coords ).mY );
        *tmpMsg << ( uint16_t )(( nChar->Coords ).mZ );
        *tmpMsg << ( uint16_t )(( nChar->Coords ).mX );
    }
    *tmpMsg << ( uint16_t )( 31910 + ( nChar->Coords ).mUD - 50 );  // Up - Mid - Down  mUD=(d6 - 80 - 2a) NeoX original offset: 31910
    *tmpMsg << ( uint16_t )( 31820 + ( nChar->Coords ).mLR*2 - 179 ); // Compass direction mLR=(S..E..N..W..S [0-45-90-135-179]) There still is a small buggy movement when slowly crossing the South axis from the right
    if ( cSeatType )
    {
        *tmpMsg << ( uint8_t )0x00;
        *tmpMsg << ( uint8_t )0x10;
    }
    else
    {
        *tmpMsg << ( uint8_t )(( nChar->Coords ).mAct );
        *tmpMsg << ( uint8_t )0x00;
    }

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharPosUpdate2Msg( PClient* nClient, uint8_t InfoBitfield )
{
    PMessage* tmpMsg = new PMessage( 32 );
    PChar* nChar = nClient->GetChar();

    if ( InfoBitfield == 0x80 )
    {
        Console->Print( RED, BLACK, "[ERROR] PMsgBuilder::BuildCharPosUpdate2Msg : using InfoBitfield=0x80 forbidden. Using 0x7f instead." );
        InfoBitfield = 0x7f;
    }

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x20;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )InfoBitfield;

    if ( InfoBitfield & 0x01 )
    {
        *tmpMsg << ( uint16_t )(( nChar->Coords ).mY );
    }
    if ( InfoBitfield & 0x02 )
    {
        *tmpMsg << ( uint16_t )(( nChar->Coords ).mZ );
    }
    if ( InfoBitfield & 0x04 )
    {
        *tmpMsg << ( uint16_t )(( nChar->Coords ).mX );
    }
    if ( InfoBitfield & 0x08 )
    {
        *tmpMsg << ( uint8_t )(( nChar->Coords ).mUD );
    }
    if ( InfoBitfield & 0x10 )
    {
        *tmpMsg << ( uint8_t )(( nChar->Coords ).mLR );
    }
    if ( InfoBitfield & 0x20 )
    {
        *tmpMsg << ( uint8_t )(( nChar->Coords ).mAct );
    }
    /*if(InfoBitfield & 0x40) // Not used (?)
    {
      *tmpMsg << (uint8_t)((nChar->Coords).mUnknown);
    }*/
    /*if(InfoBitfield & 0x80) // mRoll ????
    {
      *tmpMsg << (uint8_t)((nChar->Coords).mUnknown);
    }*/

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

/*
PMessage* PMsgBuilder::BuildCharSittingMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 24 );
  PChar* nChar = nClient->GetChar();

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
  *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
  *tmpMsg << ( uint8_t )0x32;
  *tmpMsg << ( uint16_t )nClient->GetLocalID();
  *tmpMsg << ( uint8_t )0x00; // Type = chair/subway ???
  *tmpMsg << ( uint8_t )0x00;
  *tmpMsg << ( uint8_t )0x03; // Type = chair ???
  *tmpMsg << ( uint16_t )(( nChar->Coords ).mY );
  *tmpMsg << ( uint16_t )(( nChar->Coords ).mZ );
  *tmpMsg << ( uint16_t )(( nChar->Coords ).mX );
  *tmpMsg << ( uint16_t )( 31910 + ( nChar->Coords ).mUD - 50 );  // Up - Mid - Down  mUD=(d6 - 80 - 2a) NeoX original offset: 31910
  *tmpMsg << ( uint16_t )( 31820 + ( nChar->Coords ).mLR*2 - 179 ); // Compass direction mLR=(S..E..N..W..S [0-45-90-135-179]) There still is a small buggy movement when slowly crossing the South axis from the right
  *tmpMsg << ( uint8_t )(( nChar->Coords ).mAct );
  *tmpMsg << ( uint8_t )0x00;

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}
*/

PMessage* PMsgBuilder::BuildCharUseSeatMsg( PClient* nClient, uint32_t nRawObjectId, uint8_t nSeatId )
{
    PMessage* tmpMsg = new PMessage( 18 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // nClient->GetUDP_ID() placeholder
    *tmpMsg << ( uint16_t )0x0000; // nClient->GetSessionID()placeholder

    *tmpMsg << ( uint8_t )0x0c; // Sub message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // ++ nClient->GetUDP_ID() placeholder
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x21;
    *tmpMsg << ( uint32_t )nRawObjectId;
    *tmpMsg << ( uint8_t )nSeatId; // 0x00 for real chair, 1+ for subway cab

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharExitSeatMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 22 );
    PChar* nChar = nClient->GetChar();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // nClient->GetUDP_ID() placeholder
    *tmpMsg << ( uint16_t )0x0000; // nClient->GetSessionID()placeholder
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // nClient->GetUDP_ID() placeholder
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x22;
    *tmpMsg << ( uint16_t )(( nChar->Coords ).mY + 768 ) ;
    *tmpMsg << ( uint16_t )(( nChar->Coords ).mZ + 768 ) ;
    *tmpMsg << ( uint16_t )(( nChar->Coords ).mX + 768 ) ;
    *tmpMsg << ( uint8_t )( nChar->Coords ).mUD;
    *tmpMsg << ( uint8_t )( nChar->Coords ).mLR;
    *tmpMsg << ( uint8_t )( nChar->Coords ).mAct;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildPacket0Msg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 70 );
    PChar* nChar = nClient->GetChar();
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x2c;
    *tmpMsg << ( uint8_t )0x01; // ??
    *tmpMsg << ( uint8_t )0x01; // ??
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( float )(( nChar->Coords ).mY - 32000 );
    *tmpMsg << ( float )(( nChar->Coords ).mZ - 32000 );
    *tmpMsg << ( float )(( nChar->Coords ).mX - 32000 );
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint8_t )0x01; // ????
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint8_t )0x07;
    *tmpMsg << ( uint8_t )0x02;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint32_t )0x00000000;


    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
    return tmpMsg;
}

PMessage* PMsgBuilder::BuildPingMsg( PClient* nClient, uint32_t nClientTime )
{
    PMessage* tmpMsg = new PMessage( 15 );
    uint32_t LocalTime = GameServer->GetGameTime();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x0b;
    *tmpMsg << ( uint32_t )LocalTime;
    *tmpMsg << ( uint32_t )nClientTime;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildBaselineMsg( PClient* nClient )
{
    PMessage* BaselineMsg = new PMessage( 512 );
    PMessage SectionMsg( 256 );

    PChar *nChar = nClient->GetChar();
    const PDefCharKind *def = GameDefs->CharKinds()->GetDef( nChar->GetProfession() );
    PSkillHandler *Skill = nChar->Skill;

    nClient->IncreaseTransactionID( 3 );

    // Head
    *BaselineMsg << ( uint8_t )0x22;
    *BaselineMsg << ( uint8_t )0x02; // ?
    *BaselineMsg << ( uint8_t )0x01; // ?

    // ---- Section 1 ----
    *BaselineMsg << ( uint8_t )0x01; // section id

    SectionMsg << ( uint8_t )0xfa; // ?  // // section content at offset 3
    SectionMsg << ( uint8_t )nChar->GetProfession();
    SectionMsg << ( uint16_t )nClient->GetTransactionID(); // Transaction ID ? 0x8aa0
    SectionMsg << ( uint32_t )nChar->GetID();
    SectionMsg << ( uint8_t )0x0e; // ? 0x0e in NeoX, 0x10 in Tinns ... doesn't seem to matter
    SectionMsg << ( uint8_t )0x00; // ?

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 2 ----
    *BaselineMsg << ( uint8_t )0x02; // section id
    SectionMsg << ( uint8_t )0x04; // ?  // section content at offset 3
    SectionMsg << ( uint8_t )0x50; // ?
    SectionMsg << ( uint16_t )500; // nChar->GetHealth();
    SectionMsg << ( uint16_t )500; // nChar->GetMaxHealth();
    SectionMsg << ( uint16_t )500; // nChar->GetMana();
    SectionMsg << ( uint16_t )500; // nChar->GetMaxMana();
    SectionMsg << ( uint16_t )500; // nChar->GetStamina();
    SectionMsg << ( uint16_t )500; // nChar->GetMaxStamina();
    SectionMsg << ( uint16_t )0x00ff; // ?
    SectionMsg << ( uint16_t )0x00ff; // ?
    SectionMsg << ( uint16_t )0x00e1; // (nChar->GetHealth() + 1); // ? Probably Head Health (45% of total) 0x0065
    SectionMsg << ( uint16_t )0x0147; // (nChar->GetHealth() + 1); // ? Torso Health (35% of total)
    SectionMsg << ( uint16_t )0x0147; // (nChar->GetHealth() + 1); // ? Legs Health (20% of total)
    SectionMsg << ( uint8_t )100; // 100 - SI
    SectionMsg << ( uint8_t )0x80; // The lower this value is, the more your char has a "drug effect" on it 0x00 = unmoveable
    SectionMsg << ( uint16_t )0x0000;

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 3 ----
    *BaselineMsg << ( uint8_t )0x03; // section id

    SectionMsg << ( uint8_t )0x06; // ?  // section content at offset 3
    SectionMsg << ( uint8_t )0x09; // ?
    SectionMsg << ( uint32_t )0x00000000; // ?
    SectionMsg << ( uint32_t )0x00000000; // ?
    SectionMsg << ( uint8_t )0x01; // ?

    SectionMsg << ( uint8_t )Skill->GetMainSkill( MS_STR );
    SectionMsg << ( uint16_t )Skill->GetSP( MS_STR );
    SectionMsg << ( uint32_t )Skill->GetXP( MS_STR );
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_STR ).mGrow;
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_STR ).mMax;

    SectionMsg << ( uint8_t )Skill->GetMainSkill( MS_DEX );
    SectionMsg << ( uint16_t )Skill->GetSP( MS_DEX );
    SectionMsg << ( uint32_t )Skill->GetXP( MS_DEX );
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_DEX ).mGrow;
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_DEX ).mMax;

    SectionMsg << ( uint8_t )Skill->GetMainSkill( MS_CON );
    SectionMsg << ( uint16_t )Skill->GetSP( MS_CON );
    SectionMsg << ( uint32_t )Skill->GetXP( MS_CON );
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_CON ).mGrow;
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_CON ).mMax;

    SectionMsg << ( uint8_t )Skill->GetMainSkill( MS_INT );
    SectionMsg << ( uint16_t )Skill->GetSP( MS_INT );
    SectionMsg << ( uint32_t )Skill->GetXP( MS_INT );
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_INT ).mGrow;
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_INT ).mMax;

    SectionMsg << ( uint8_t )Skill->GetMainSkill( MS_PSI );
    SectionMsg << ( uint16_t )Skill->GetSP( MS_PSI );
    SectionMsg << ( uint32_t )Skill->GetXP( MS_PSI );
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_PSI ).mGrow;
    SectionMsg << ( uint8_t )def->GetSkillInfo( MS_PSI ).mMax;

    SectionMsg << ( uint16_t )0x0000; // ?
    SectionMsg << ( uint8_t )0x00; // ?
    SectionMsg << ( uint8_t )0x08; // ?

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 4 ----
    *BaselineMsg << ( uint8_t )0x04; // section id

    SectionMsg << ( uint8_t )0x2e; // ?  // section content at offset 3
    SectionMsg << ( uint8_t )0x02; // ?
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_MC );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_MC );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_HC );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_HC );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_TRA );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_TRA );
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_PC );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_PC );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_RC );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_RC );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_TC );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_TC );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_VHC );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_VHC );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_AGL );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_AGL );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_REP );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_REP );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_REC );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_REC );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_RCL );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_RCL );
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_ATL );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_ATL );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_END );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_END );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_FOR );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_FOR );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_FIR );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_FIR );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_ENR );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_ENR );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_XRR );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_XRR );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_POR );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_POR );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_HLT );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_HLT );
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_HCK );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_HCK );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_BRT );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_BRT );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_PSU );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_PSU );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_WEP );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_WEP );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_CST );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_CST );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_RES );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_RES );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_IMP );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_IMP );
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )0x00; // spare
    SectionMsg << ( uint8_t )0x01; // cost
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_PPU );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_PPU );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_APU );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_APU );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_MST );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_MST );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_PPW );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_PPW );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_PSR );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_PSR );
    SectionMsg << ( uint8_t )Skill->GetSubSkill( SK_WPW );
    SectionMsg << ( uint8_t )Skill->GetSKPCost( SK_WPW );

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 5 ----
    *BaselineMsg << ( uint8_t )0x05; // section id

    PMessage* ContentList = BuildContainerContentList( nChar->GetInventory()->GetContainer( INV_LOC_BACKPACK ), INV_LOC_BACKPACK );
    SectionMsg << *ContentList;
    delete ContentList;

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 6 ----
    *BaselineMsg << ( uint8_t )0x06; // section id

    ContentList = BuildContainerContentList( nChar->GetInventory()->GetContainer( INV_LOC_WORN ), INV_LOC_WORN );
    SectionMsg << *ContentList;
    delete ContentList;

    /*    SectionMsg << (uint8_t)0x04; // QB/Armor/Implants items nb  // section content at offset 3

        // THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== BEGIN
        SectionMsg << (uint16_t)0x06;     // Size of item
        SectionMsg << (uint16_t)0x00;     // Location: Quickbelt slot 0
        SectionMsg << (uint16_t)0x0051;   // ItemID: 81, Flashlight
        SectionMsg << (uint8_t)0x01;      // Datatype
        SectionMsg << (uint8_t)0x00;      // Data
    */

    /****
    SectionMsg << (uint8_t)0x04; // QB/Armor/Implants items nb  // section content at offset 3

    // THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== BEGIN
    SectionMsg << (uint16_t)0x001b;     // Size of item
    SectionMsg << (uint8_t)0x00;     // Location: Quickbelt slot 0
    SectionMsg << (uint8_t)0x00; // nop (Y)
    SectionMsg << (uint16_t)0x0003;   // ItemID: 3, assault riffle
    SectionMsg << (uint8_t)(0x01|0x02|0x10|0x40);      // Datatype

    //SectionMsg << (uint8_t)0x01; // for 0x80. Use ???

    SectionMsg << (uint8_t)0x00; // Qty / remaining ammos

    SectionMsg << (uint8_t)0x06; // Qual entries
    SectionMsg << (uint8_t)0x40; // current qual
    SectionMsg << (uint8_t)0x80; // dmg
    SectionMsg << (uint8_t)0xc0; // freq
    SectionMsg << (uint8_t)0xa0; // handl
    SectionMsg << (uint8_t)0xb0; // range
    SectionMsg << (uint8_t)0xff; // max qual <= always last

    SectionMsg << (uint8_t)0x07; // addons bitflag: flashlight=1, scope, silencer, laserpointer

    SectionMsg << (uint8_t)0x02; // used slots
    SectionMsg << (uint8_t)0x05; // max slots
    SectionMsg << (uint16_t)1526; // slots / explo ammo
    SectionMsg << (uint16_t)21; // riffle-barrel
    SectionMsg << (uint16_t)0x0000;
    SectionMsg << (uint16_t)0x0000;
    SectionMsg << (uint16_t)0x0000;
    *****/
    /*
    SectionMsg << (uint16_t)0x06;     // Size of item
    SectionMsg << (uint8_t)0x01;     // Location: Quickbelt slot 1
    SectionMsg << (uint8_t)0x00; // nop (Y)
    SectionMsg << (uint16_t)0x0023;   // ItemID: 35, Med Kit
    SectionMsg << (uint8_t)0x01;      // Data=ammo count
    SectionMsg << (uint8_t)0x03;      // Data
    */
    /*
        SectionMsg << (uint16_t)0x06;     // Size of item
        SectionMsg << (uint16_t)0x01;     // Location: Quickbelt slot 0
        SectionMsg << (uint16_t)0x0055;   // ItemID: 81, Flashlight
        SectionMsg << (uint8_t)0x01;      // Datatype
        SectionMsg << (uint8_t)0x00;      // Data

        SectionMsg << (uint16_t)0x06;     // Size of item
        SectionMsg << (uint16_t)0x02;     // Location: Quickbelt slot 0
        SectionMsg << (uint16_t)0x176F;   // ItemID: 81, Flashlight
        SectionMsg << (uint8_t)0x01;      // Datatype
        SectionMsg << (uint8_t)0x00;      // Data


        SectionMsg << (uint16_t)0x08;      // Size of item
        SectionMsg << (uint16_t)0x1a;      // Location: Brain #1
        SectionMsg << (uint16_t)0x08fc;    // ItemID: Law enforcer
        SectionMsg << (uint8_t)0x02;       // Datatype. 02: Item Duration information follows
        SectionMsg << (uint8_t)0x02;       // SubDatatype02: Full itemdetails follow
        SectionMsg << (uint8_t)0x2a;       // Current duration
        SectionMsg << (uint8_t)0x2a;       // Max duration
    */
//    nChar->GetInventory()->QB_SetSlot(0, 81); // Add Flashlight to QB slot 1
//    nChar->GetInventory()->QB_SetSlot(1, 85); // Add Flashlight to QB slot 1
//    nChar->GetInventory()->QB_SetSlot(2, 5999); // Add Flashlight to QB slot 1
    // THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== END
    /*
     StatsBuffer[len+3] = 0; //Number of items
     plen = 4;
     for (i=0;i<MAX_INVENTORY;i++)
     {
      for (t=0;t<128;t++)
      {
       if (CurrentChar.QuickBelt[t]-1 != i)
        continue;
       *(unsigned short*)&StatsBuffer[len+plen] = 6;    //Data size of item
       *(unsigned short*)&StatsBuffer[len+plen+2] = t;  //X position in Inventory
       *(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.ItemList[CurrentChar.QuickBelt[t]-1].ItemID; //Item ID
       *(unsigned short*)&StatsBuffer[len+plen+6] = CurrentChar.ItemList[CurrentChar.QuickBelt[t]-1].Qty;   //Quantity
       plen += 8;
       *(unsigned short*)&StatsBuffer[len+3] += 1;      //Add to item
       break;
      }
    */
    /*if (CurrentChar.Inventory[i].ItemID == 0)     //Last Item
     break;
    if (CurrentChar.Inventory[i].Location != 2)     //Not Quickbelt, skip it
     continue;
    if (Def_GetItemType(CurrentChar.Inventory[i].ItemID) == 1)
    {
     //18 00 01 00 08 00 63 00 06 ff c8 c8 c8 c8 ff 00 01 00 28 04 00 01 04 ff 01
     //12 00 11    5f 07 23 00 06 eb a4 99 a3 a5 ff 04 00 01 04 ff 01
     //13 00 05 00 03 02 23 00 06 6d c4 c4 c4 c4 ff 04 00 01 04 ff 00
     //17 00 05    9d 01 73 28 06 13 c1 c1 c1 c1 ff 00 01 01 f1 05 04
     //|Size|Slot|ItemID|???  |? |Du|    Stats  |MD|?????????????????|
     *(unsigned short*)&StatsBuffer[len+plen] = 19; //Size
     *(unsigned short*)&StatsBuffer[len+plen+2] = CurrentChar.Inventory[i].LocX; //Slot
     *(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.Inventory[i].ItemID;//Item ID
     StatsBuffer[len+plen+6] = 0x23;            //0x73 = Ranged, 0x63 = close/spell
     StatsBuffer[len+plen+7] = CurrentChar.Inventory[i].Qty;      //Quantity
     StatsBuffer[len+plen+8] = 0x06;            //0x06
     StatsBuffer[len+plen+9] = CurrentChar.Inventory[i].CurDur;     //Current Durability
     StatsBuffer[len+plen+10] = CurrentChar.Inventory[i].Damage;     //Stats
     StatsBuffer[len+plen+11] = CurrentChar.Inventory[i].Freq;     //Stats
     StatsBuffer[len+plen+12] = CurrentChar.Inventory[i].Hand;     //Stats
     StatsBuffer[len+plen+13] = CurrentChar.Inventory[i].Rng;     //Stats
     StatsBuffer[len+plen+14] = CurrentChar.Inventory[i].MaxDur;     //Max Durability
     StatsBuffer[len+plen+15] = 0x04;           //Size or part
     StatsBuffer[len+plen+16] = 0x00;           //follow or ?
     StatsBuffer[len+plen+17] = 0x01;           //Ammo loaded?
     StatsBuffer[len+plen+18] = 0x04;           //0x04 if not named, 0x0a if named
     StatsBuffer[len+plen+19] = 0x05;           //0x06 if named, 0xff for no ammo needed, no idea otherwise
     StatsBuffer[len+plen+20] = 0x0f;           //Binary representation of ammo allowed 0x01 = normal
     plen+=21;
    }
    else
    {
     *(unsigned short*)&StatsBuffer[len+plen] = 6;         //Size of Item Data
     *(unsigned short*)&StatsBuffer[len+plen+2] = CurrentChar.Inventory[i].LocX;  //Slot
     *(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.Inventory[i].ItemID; //Item ID
     *(unsigned short*)&StatsBuffer[len+plen+6] = CurrentChar.Inventory[i].Qty;  //Quantity
     plen+= 8;
    }
    StatsBuffer[len+3] += 1;*/

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 7 ----
    *BaselineMsg << ( uint8_t )0x07; // section id

    SectionMsg << ( uint8_t )0x00; // ?? // section content at offset 3

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 0c ----
    *BaselineMsg << ( uint8_t )0x0c; // section id

    ContentList = BuildContainerContentList( nChar->GetInventory()->GetContainer( INV_LOC_GOGO ), INV_LOC_GOGO );
    SectionMsg << *ContentList;

    delete ContentList;

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 8 ----
    *BaselineMsg << ( uint8_t )0x08; // section id

    uint32_t nSkin, nHead, nTorso, nLegs;
    nChar->GetRealLook( nSkin, nHead, nTorso, nLegs );

    SectionMsg << ( uint8_t )0x0a; // ? // section content at offset 3
    SectionMsg << ( uint32_t )nChar->GetCash();

    // Genrep list
    SectionMsg << ( uint16_t )nChar->GetGenrepCount(); //Number of GR tagged (Tinns value on uint8_t only)
    SectionMsg << ( uint8_t )0x04;
    if ( nChar->GetGenrepCount() ) // For each entry : (uint16_t)genrep.g_worldid, (uint16_t)genrep.g_stationid
    {
        SectionMsg.Write( nChar->GetGenrepListData(), nChar->GetGenrepListDataSize() );
    }

    SectionMsg << ( uint8_t )0x04;
    SectionMsg << ( uint8_t )0x04;
    SectionMsg << ( uint8_t )0x00;

    // Direct chat
    if ( nChar->GetDirectChat() )
    {
        SectionMsg << ( uint8_t )1;
        SectionMsg << ( uint32_t )nChar->GetDirectChat();
    }
    else
    {
        SectionMsg << ( uint8_t )0;
    }

    // Buddy Chat
    SectionMsg << ( uint8_t )nChar->GetBuddyCount(); //Number of Buddies
    if ( nChar->GetBuddyCount() ) // For each buddy (uint32_t)buddy CharID
    {
        SectionMsg.Write( nChar->GetBuddyListData(), nChar->GetBuddyListDataSize() );
    }
    SectionMsg << ( uint8_t )0x00;

    SectionMsg << ( uint16_t )nClient->GetTransactionID(); // ??
    //SectionMsg << (uint32_t)0x00000000;
    SectionMsg << ( uint32_t )0x00000000;
    SectionMsg << ( uint32_t )0x00000000; // Epic status ?
    SectionMsg << ( uint16_t )nSkin;
    SectionMsg << ( uint8_t )nHead;
    SectionMsg << ( uint8_t )nTorso;
    SectionMsg << ( uint8_t )nLegs;
    SectionMsg << ( uint8_t )0x00; // Rank
    SectionMsg << ( uint32_t )( nChar->GetBaseApartment() + PWorlds::mAptBaseWorldId ); // 0x22, 0x00, 0x00, 0x00, //Primary Apartment (GR activated) ???
    SectionMsg << ( uint8_t )0x01; // ?
    SectionMsg << ( uint8_t )0x00; // ?
    SectionMsg << ( uint8_t )0x00; // ?
    SectionMsg << ( uint8_t )0x00; // ?
    SectionMsg << ( uint8_t )0x00; // ?

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 9 ----
    *BaselineMsg << ( uint8_t )0x09; // section id

    SectionMsg << ( uint16_t )0x15; // Nb of factions // section content at offset 3
    SectionMsg << ( uint16_t )nChar->GetFaction();
    SectionMsg << ( uint8_t )0x04; // ?
    SectionMsg << ( float ) 1000.0f*1000.0f; // 96.0f*9000.0f; // SL
    // Faction Sympathies Points effective sympathie is Square root of these points, abs value rounded down (beware of the sign ! :p)
    SectionMsg << ( float ) 1000.0f*1250.0f; // City Admin => 111
    SectionMsg << ( float ) 1000.0f*1000.0f; // Diamond => 100
    SectionMsg << ( float ) 1000.0f* -750.0f; // Next => -86
    SectionMsg << ( float ) 1000.0f*500.0f; // Tangent => 70
    SectionMsg << ( float ) 1000.0f* -250.0f; // Biotech => -50
    SectionMsg << ( float ) 1000.0f*0.0f; // ProtoPharm => 0
    SectionMsg << ( float ) 1000.0f*1000.0f; // Trader's Union
    SectionMsg << ( float ) 1000.0f*1000.0f; // Tsunami
    SectionMsg << ( float ) 1000.0f*1000.0f; // Black Dragons
    SectionMsg << ( float ) 1000.0f*1000.0f; // City Mercs
    SectionMsg << ( float ) 1000.0f*1000.0f; // Crahn Sect
    SectionMsg << ( float ) 1000.0f*1000.0f; // Dome Of York
    SectionMsg << ( float ) 1000.0f* -1000.0f; // Anarchy Breed
    SectionMsg << ( float ) 1000.0f*1000.0f; // Fallen Angels
    SectionMsg << ( float ) 1000.0f*1000.0f; // Twilight Guardian
    SectionMsg << ( float ) 1000.0f* -1000.0f; // Regeant's Legacy
    SectionMsg << ( float ) 1000.0f* -1000.0f; // Regeant's Mutants
    SectionMsg << ( float ) 1000.0f* -1000.0f; // Insects
    SectionMsg << ( float ) 1000.0f* -1000.0f; // Monsters
    SectionMsg << ( float ) 1000.0f*1000.0f; // Unknown
    SectionMsg << ( float ) 1000.0f*1000.0f; // Highest SL?

    SectionMsg << ( uint32_t )0x00000000; // Epic done Data : bit 2^FactionID set <=> Epic done
    SectionMsg << ( uint8_t )nChar->GetFaction(); // Faction ??? wrong size ...

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 0a ----
    *BaselineMsg << ( uint8_t )0x0a; // section id

    // Clan data ?
    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 0b ----
    *BaselineMsg << ( uint8_t )0x0b; // section id

    SectionMsg << ( uint8_t )0x00; // ?? // section content at offset 3

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    // ---- Section 0d ----
    *BaselineMsg << ( uint8_t )0x0d; // section id

    SectionMsg << ( uint8_t )0xfa; // ?? // section content at offset 3
    SectionMsg << ( uint8_t )nChar->GetProfession();
    SectionMsg << ( uint16_t )nClient->GetTransactionID(); // ?? TransactionID ? 0x8aa0
    SectionMsg << ( uint32_t )nChar->GetID();

    *BaselineMsg << ( uint16_t )SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Reset();

    return BaselineMsg;
}

PMessage* PMsgBuilder::BuildAliveRepMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 7 );

    // uint8_t up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
    *tmpMsg << ( uint8_t )0x04;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0xe3; // ? not always...
    *tmpMsg << ( uint8_t )0x6b; // ? not always...
    *tmpMsg << ( uint16_t )( nClient->getUDPConn()->getPort() ); // really ?

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoning1Msg( PClient* nClient, uint16_t nEntity, uint8_t nUnknown )
{
    PMessage* tmpMsg = new PMessage( 42 );

    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID(); // from NeoX

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0c; // Message length place;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID(); // from NeoX
    *tmpMsg << ( uint8_t )0x25; // ??
    *tmpMsg << ( uint8_t )0x13; // ??
    *tmpMsg << ( uint16_t )nClient->GetTransactionID(); // from NeoX / ?? right ???
    // In case of apt GR or NC2.2 ?
    // *tmpMsg << (uint8_t)0x04; // len ?
    // *tmpMsg << (uint32_t)AptWorldID; // len ?
    // nClient->IncreaseTransactionID();
    // *tmpMsg << (uint16_t)nClient->GetTransactionID();
    *tmpMsg << ( uint8_t )0x0e; // cmd => but not for zoning, because used in non-zoning situation
    *tmpMsg << ( uint8_t )0x02; // ?? from NeoX
    //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x17; // Message length place;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x23;
    *tmpMsg << ( uint16_t )0x0004; // cmd
    *tmpMsg << ( uint32_t )0x00000000; // from NeoX
    *tmpMsg << ( uint32_t )0x00000000; // from NeoX
    *tmpMsg << ( uint8_t )nUnknown;
    *tmpMsg << ( uint16_t )nEntity;
    *tmpMsg << ( uint16_t )0x0000; // from NeoX
    *tmpMsg << ( uint16_t )nClient->GetTransactionID(); // from NeoX
    *tmpMsg << ( uint16_t )0x0000; // from NeoX

    // We DO need this! This is a multiframe packet, and the first UDP/SessionID set *HAS* to be
    // as high as the last UDP/SessionID set in the frame!
    tmpMsg->U16Data( 1 ) = nClient->GetUDP_ID();
    tmpMsg->U16Data( 3 ) = nClient->GetSessionID();

    //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoningTCPReadyMsg()
{
    PMessage* tmpMsg = new PMessage( 7 );

    //static const uint8_t READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
    *tmpMsg << ( uint8_t )0xfe;
    *tmpMsg << ( uint16_t )0x0004; //length
    *tmpMsg << ( uint8_t )0x83; //cmd
    *tmpMsg << ( uint8_t )0x0d; // sub-cmd
    *tmpMsg << ( uint16_t )0x0000;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildSendZoneTCPMsg( uint32_t nLocation, std::string *nWorldName )
{
    PMessage* tmpMsg = new PMessage( 14 + nWorldName->size() );

    *tmpMsg << ( uint8_t )0xfe;
    *tmpMsg << ( uint16_t )0x0000; // size placeholder
    *tmpMsg << ( uint8_t )0x83;
    *tmpMsg << ( uint8_t )0x0c;
    *tmpMsg << ( uint32_t )nLocation;
    *tmpMsg << ( uint32_t )0x00000000;
    tmpMsg->Write( nWorldName->c_str(), nWorldName->size() + 1 );

    tmpMsg->U16Data( 1 ) = ( uint16_t )( tmpMsg->GetSize() - 3 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoning2Msg( PClient* nClient, uint32_t nClientTime )
{
    PMessage* tmpMsg = new PMessage( 22 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x0d;
    *tmpMsg << ( uint32_t )GameServer->GetGameTime();

    *tmpMsg << ( uint32_t )nClientTime;

    *tmpMsg << ( uint8_t )0xe5; // ??? varies
    *tmpMsg << ( uint8_t )0x0a; // ??? varies
    *tmpMsg << ( uint8_t )0xbb; // ??? varies
    *tmpMsg << ( uint8_t )0x00; // ??? usually 0

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildGenrepZoningMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity )
{
    PMessage* tmpMsg = new PMessage( 50 );

    nClient->IncreaseUDP_ID();

    tmpMsg->Fill( 0 );
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x23;
    *tmpMsg << ( uint16_t )0x000c; // cmd
    tmpMsg->SetNextByteOffset( 38 );
    *tmpMsg << ( uint32_t )0xffffffff;
    *tmpMsg << ( uint32_t )nLocation;
    *tmpMsg << ( uint16_t )nEntity;
    *tmpMsg << ( uint16_t )0x0000;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildGenrepAddToListMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity )
{
    PMessage* tmpMsg = new PMessage( 23 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3d;
    *tmpMsg << ( uint32_t )0x00000002;
    *tmpMsg << ( uint32_t )nLocation;
    *tmpMsg << ( uint16_t )nEntity;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildAptLiftUseMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity, uint8_t nEntityType )
{
    PMessage* tmpMsg = new PMessage( 43 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x0f; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x38;
    *tmpMsg << ( uint8_t )0x04; // Accepted (?)
    *tmpMsg << ( uint8_t )nEntityType; // "Sewer Level"
    *tmpMsg << ( uint32_t )nLocation;
    *tmpMsg << ( uint16_t )nEntity;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildAptLiftFailedMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 14 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x08; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x38;
    *tmpMsg << ( uint8_t )0x03; // Refused

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildChangeLocationMsg( PClient* nClient, uint32_t nLocation, uint16_t nEntity, uint8_t nEntityType, uint32_t nRawItemID )
{
    PMessage* tmpMsg = new PMessage( 28 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    if ( nRawItemID )
    {
        *tmpMsg << ( uint8_t )0x06; // Sub message length;
        *tmpMsg << ( uint8_t )0x2d; // Item use response;
        *tmpMsg << ( uint32_t )nRawItemID;
        *tmpMsg << ( uint8_t )0x0a; // Use allowed
    }

    *tmpMsg << ( uint8_t )0x0f; // Sub message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x38;
    *tmpMsg << ( uint8_t )0x04; // Accepted (?)
    *tmpMsg << ( uint8_t )nEntityType;
    *tmpMsg << ( uint32_t )nLocation;
    *tmpMsg << ( uint16_t )nEntity;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildEntityPositionMsg( PClient* nClient, uint16_t pX, uint16_t pY, uint16_t pZ )
{
    PMessage* tmpMsg = new PMessage( 18 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x23;
    *tmpMsg << ( uint16_t )0x000a;
    *tmpMsg << ( uint16_t )( pY + 768 );
    *tmpMsg << ( uint16_t )( pZ + 768 );
    *tmpMsg << ( uint16_t )( pX + 768 );

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharAptLocInfoMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 21 );
    nClient->IncreaseUDP_ID();

    int BaseAppId = nClient->GetChar()->GetBaseApartment();
    uint32_t AptLocation = ( uint32_t )Appartements->GetAptLocation( BaseAppId );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0f;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3d;
    *tmpMsg << ( uint32_t )0x0000000b;
    *tmpMsg << ( uint32_t )AptLocation;

    return tmpMsg;
}

// OLD FUNCTION, REWRITTEN BELOW
/*
PMessage* PMsgBuilder::BuildSubskillIncMsg( PClient* nClient, uint8_t nSubskill, uint16_t nSkillPoints )
{
  PMessage* tmpMsg = new PMessage( 33 );

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint16_t )nClient->GetSessionID();

  nClient->IncreaseUDP_ID();
  *tmpMsg << ( uint8_t )0x09; // SubMessage length;
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nClient->GetLocalID();
  *tmpMsg << ( uint8_t )0x25;
  *tmpMsg << ( uint8_t )0x23;
  *tmpMsg << ( uint8_t )0x41; // 0x28 ?? // 0x 18 // 0x2c

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID(); // testing ...
  *tmpMsg << ( uint8_t )0x11; // SubMessage length;
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nClient->GetLocalID();
  *tmpMsg << ( uint8_t )0x25;
  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )nClient->GetTransactionID(); // testing / 0x0000 ????
  *tmpMsg << ( uint8_t )0x09; // ?
  *tmpMsg << ( uint16_t )nSubskill;
  *tmpMsg << ( uint16_t )nClient->GetChar()->Skill->GetSubSkill( nSubskill ); // nSubskill ?
  *tmpMsg << ( uint16_t )nSkillPoints;

  //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}
 */
// NPC Dialog. Start dialog with NPC
PMessage* PMsgBuilder::BuildNPCStartDialogMsg( PClient* nClient, uint32_t nNPCWorldID, std::string *nDialogScript  )
 {
    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t ) 0x0000; // UDP Placeholder
    *tmpMsg << ( uint16_t ) 0x0000; // UDP Placeholder
    *tmpMsg << ( uint8_t )0x00; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x18;
    *tmpMsg << ( uint32_t ) nNPCWorldID;

    // Todo: is this correct? random uint32_t value??
    *tmpMsg << Misc::Random::get(65535u, 4369u);
    *tmpMsg << Misc::Random::get(65535u, 4369u);
    *tmpMsg << ( uint32_t ) 0x0000;
    *tmpMsg << nDialogScript->c_str();
    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x0a;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x1a;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x00;

    tmpMsg->U16Data( 1 ) = nClient->GetUDP_ID();
    tmpMsg->U16Data( 3 ) = nClient->GetSessionID();

    return tmpMsg;
 }
// NPC Dialog. Send next node number in lua script to client
PMessage* PMsgBuilder::BuildNPCDialogReplyMsg( PClient* nClient, uint16_t nNextNode, std::vector<int>*nResultBuffer)
 {
    PMessage* tmpMsg = new PMessage();

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();;
    *tmpMsg << ( uint16_t )nClient->GetSessionID();;
    *tmpMsg << ( uint8_t )0x00; // SubMessage length;

    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();;
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x1a;
    *tmpMsg << ( uint16_t )nNextNode;
    //*tmpMsg << ( uint8_t )nNumResults;
    *tmpMsg << ( uint8_t )nResultBuffer->size();

    std::vector<int>::const_iterator it;

    for(it = nResultBuffer->begin(); it != nResultBuffer->end(); it++)
     {
        *tmpMsg << ( float )*(it);
     }

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
 }

PMessage* PMsgBuilder::BuildNPCBeginAllBuyerTradeMsg( PClient* nClient, int nWorldID )
 {
    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();
 
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x26;
    *tmpMsg << ( uint32_t ) nWorldID;
    *tmpMsg << ( uint8_t )0x01; // Traders inventory
    *tmpMsg << ( uint16_t )0xFFFF; // Traders inventory
 
    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
 
    return tmpMsg;
 }
 
PMessage* PMsgBuilder::BuildNPCShoppingListMsg( PClient* nClient, PMessage* nContentList, int nWorldID, uint8_t nItemQuality)
 {
    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();
 
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x26;
    *tmpMsg << ( uint32_t ) nWorldID;
    *tmpMsg << ( uint8_t )0x01; // Traders inventory
    *tmpMsg << ( uint16_t )( nContentList->GetSize() / 6 ); // List entries
    *tmpMsg << ( uint8_t )nItemQuality; // Items quality
    *tmpMsg << *nContentList;
 
    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
 
    return tmpMsg;
 }
 
// ==========================
PMessage* PMsgBuilder::BuildNPCSingleInfoMsg( PClient* nClient, uint32_t nWorldID, uint16_t nTypeID, uint16_t nClothing,
uint16_t nNameID, uint16_t nPosY, uint16_t nPosZ, uint16_t nPosX, uint16_t nUnknown,
uint16_t nTraderID, std::string *nAngleStr, std::string *nNpcName, std::string *nCustomName)
// Initial NPC Packet that defines how the NPC look, etc
 {
//    uint8_t tMsgLen = 29 + nNpcName->size() + nAngleStr->size() + nCustomName->size();
 
    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();
 
    *tmpMsg << ( uint8_t )0x13; // Begin UDP message
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00;
     *tmpMsg << ( uint8_t )0x03;
     *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
     *tmpMsg << ( uint8_t )0x28;
     *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x01;
    *tmpMsg << ( uint32_t )nWorldID;
    *tmpMsg << ( uint16_t )nTypeID;
    *tmpMsg << ( uint16_t )nClothing;
    *tmpMsg << ( uint16_t )nNameID;
    *tmpMsg << ( uint16_t )nPosY;
    *tmpMsg << ( uint16_t )nPosZ;
    *tmpMsg << ( uint16_t )nPosX;
     *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint16_t )nUnknown;
    *tmpMsg << ( uint16_t )nTraderID;
    *tmpMsg << nNpcName->c_str();
    *tmpMsg << nAngleStr->c_str();
    if(nCustomName->length() > 1)
        *tmpMsg << nCustomName->c_str();
 
    (*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);
    return tmpMsg;
 }
 
PMessage* PMsgBuilder::BuildNPCMassInfoMsg( uint32_t nWorldID, uint16_t nTypeID, uint16_t nClothing,
uint16_t nNameID, uint16_t nPosY, uint16_t nPosZ, uint16_t nPosX, uint16_t nHealth,
uint16_t nTraderID, std::string *nAngleStr, std::string *nNpcName, std::string *nCustomName)
// Initial NPC Packet that defines how the NPC look, etc
 {
//    uint8_t tMsgLen = 29 + nNpcName->size() + nAngleStr->size() + nCustomName->size();
 
    PMessage* tmpMsg = new PMessage();
 
    *tmpMsg << ( uint8_t )0x13; // Begin UDP message
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x28;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x01;
    *tmpMsg << ( uint32_t )nWorldID;
    *tmpMsg << ( uint16_t )nTypeID;
    *tmpMsg << ( uint16_t )nClothing;
    *tmpMsg << ( uint16_t )nNameID;
    *tmpMsg << ( uint16_t )nPosY;
    *tmpMsg << ( uint16_t )nPosZ;
    *tmpMsg << ( uint16_t )nPosX;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint16_t )nHealth;
    *tmpMsg << ( uint16_t )nTraderID;
    *tmpMsg << nNpcName->c_str();
    *tmpMsg << nAngleStr->c_str();
    if(nCustomName->length() > 1)
        *tmpMsg << nCustomName->c_str();
 
    (*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);
     return tmpMsg;
 }
 
// **************
PMessage* PMsgBuilder::BuildNPCUpdateMsg(uint32_t nWorldID, uint16_t nPosY, uint16_t nPosZ, uint16_t nPosX, uint8_t nActionBM, uint16_t nHealth, uint8_t nWeaponState, uint8_t nUnknown, uint32_t nTargetID)
 {
    PMessage* tmpMsg = new PMessage();
 
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x00;      // len
    *tmpMsg << ( uint8_t )0x1b;      // NPC Update
    *tmpMsg << ( uint32_t )nWorldID; // NPCs world ID
    *tmpMsg << ( uint8_t )0x1f;      // Parameters
    *tmpMsg << ( uint16_t )nPosY;    // Position Y
    *tmpMsg << ( uint16_t )nPosZ;    // Position Z
    *tmpMsg << ( uint16_t )nPosX;    // Position X
    *tmpMsg << ( uint8_t )nActionBM; // NPCs current action-bitmask
    *tmpMsg << ( uint16_t )nHealth;   // Health value
    if(nTargetID > 0)
        *tmpMsg << ( uint32_t )nTargetID; // WorldID of NPCs target (if any)
    *tmpMsg << ( uint8_t )nUnknown;
    *tmpMsg << ( uint8_t )nWeaponState;
 
    (*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);
 
    return tmpMsg;
 }
// **************
 
PMessage* PMsgBuilder::BuildNPCSingleAliveMsg( PClient* nClient, uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint8_t nAction )
 {
    PMessage* tmpMsg = new PMessage();
 
    *tmpMsg << ( uint8_t )0x13; // Begin UDP message
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x11;
    *tmpMsg << ( uint8_t )0x1B;
    *tmpMsg << ( uint32_t )nWorldID;
    *tmpMsg << ( uint8_t )0x1F;
    *tmpMsg << ( uint16_t )nY;
    *tmpMsg << ( uint16_t )nZ;
    *tmpMsg << ( uint16_t )nX;
    *tmpMsg << ( uint8_t )nActionStatus;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )nHealth;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )nAction;
 
    return tmpMsg;
 }
 
PMessage* PMsgBuilder::BuildNPCMassAliveMsg( uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint8_t nAction )
 {
    PMessage* tmpMsg = new PMessage();
 
    *tmpMsg << ( uint8_t )0x13; // Begin UDP message
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x11;
    *tmpMsg << ( uint8_t )0x1B;
    *tmpMsg << ( uint32_t )nWorldID;
    *tmpMsg << ( uint8_t )0x1F;
    *tmpMsg << ( uint16_t )nY;
    *tmpMsg << ( uint16_t )nZ;
    *tmpMsg << ( uint16_t )nX;
    *tmpMsg << ( uint8_t )nActionStatus;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )nHealth;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )nAction;
 
    return tmpMsg;
 }
 
PMessage* PMsgBuilder::BuildNPCMassUpdateMsg( uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint16_t nTarget, uint8_t nAction )
 {
    PMessage* tmpMsg = new PMessage();
 
    *tmpMsg << ( uint8_t )0x13; // Begin UDP message
     *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x15; // Message length
    *tmpMsg << ( uint8_t )0x1b;
    *tmpMsg << ( uint32_t )nWorldID;
    *tmpMsg << ( uint8_t )0x1F;
    *tmpMsg << ( uint16_t )nY;
    *tmpMsg << ( uint16_t )nZ;
    *tmpMsg << ( uint16_t )nX;
    *tmpMsg << ( uint8_t )nActionStatus;
    *tmpMsg << ( uint8_t )0x77; // ?
    *tmpMsg << ( uint8_t )nHealth;
    *tmpMsg << ( uint16_t )nTarget;
    *tmpMsg << ( uint8_t )0x00; // ?
    *tmpMsg << ( uint8_t )0x00; // ?
    *tmpMsg << ( uint8_t )0x00; // ?
    *tmpMsg << ( uint8_t )nAction;
 
    return tmpMsg;
 }
 
// ==========================
 
PMessage* PMsgBuilder::BuildNPCSingleUpdateMsg( PClient* nClient, uint32_t nWorldID, uint16_t nX, uint16_t nY, uint16_t nZ, uint8_t nActionStatus, uint8_t nHealth, uint16_t nTarget, uint8_t nAction )
 {
    PMessage* tmpMsg = new PMessage();
 
    *tmpMsg << ( uint8_t )0x13; // Begin UDP message
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x15; // Message length
    *tmpMsg << ( uint8_t )0x1b;
    *tmpMsg << ( uint32_t )nWorldID;
    *tmpMsg << ( uint8_t )0x1F;
    *tmpMsg << ( uint16_t )nY;
    *tmpMsg << ( uint16_t )nZ;
    *tmpMsg << ( uint16_t )nX;
    *tmpMsg << ( uint8_t )nActionStatus;
    *tmpMsg << ( uint8_t )0x77; // ?
    *tmpMsg << ( uint8_t )nHealth;
    *tmpMsg << ( uint16_t )nTarget;
    *tmpMsg << ( uint8_t )0x00; // ?
    *tmpMsg << ( uint8_t )0x00; // ?
    *tmpMsg << ( uint8_t )0x00; // ?
    *tmpMsg << ( uint8_t )nAction;
 
    return tmpMsg;
 }
// ==========================
PMessage* PMsgBuilder::BuildSubskillIncMsg( PClient* nClient, uint8_t nSubskill, uint16_t nSkillPoints )
{
    PMessage* tmpMsg = new PMessage( 33 );

    nClient->IncreaseUDP_ID();
    uint16_t tFirstUDPID = nClient->GetUDP_ID();

    nClient->IncreaseUDP_ID();
    uint16_t tSecondUDPID = nClient->GetUDP_ID();
    uint16_t tSecondSessionID = nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )tSecondUDPID;
    *tmpMsg << ( uint16_t )tSecondSessionID;

    *tmpMsg << ( uint8_t )0x09; // SubMessage length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )tFirstUDPID;
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25;
    *tmpMsg << ( uint8_t )0x23;
    *tmpMsg << ( uint8_t )0x27;

    nClient->IncreaseTransactionID();
    *tmpMsg << ( uint8_t )0x11; // SubMessage length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )tSecondUDPID;
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25;
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetTransactionID(); // testing / 0x0000 ????
    *tmpMsg << ( uint8_t )0x09; // ?
    *tmpMsg << ( uint16_t )nSubskill;
    *tmpMsg << ( uint16_t )nClient->GetChar()->Skill->GetSubSkill( nSubskill ); // nSubskill ?
    *tmpMsg << ( uint16_t )nSkillPoints;

    //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildLevelUpMessage( PClient* nClient, uint8_t nMainSkill, uint8_t nNewLevel, uint16_t nFreeSkillPoints)
{
    PMessage* tmpMsg = new PMessage(21);
    nClient->IncreaseUDP_ID();

    *tmpMsg << (uint8_t)0x13;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint16_t)nClient->GetSessionID();
    *tmpMsg << (uint8_t)0x0F;
    *tmpMsg << (uint8_t)0x03;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint8_t)0x1F;
    *tmpMsg << (uint16_t)nClient->GetLocalID();
    *tmpMsg << (uint8_t)0x25;
    *tmpMsg << (uint8_t)0x0B;
    *tmpMsg << (uint16_t)nMainSkill;
    *tmpMsg << (uint8_t)nNewLevel;
    *tmpMsg << (uint16_t)nFreeSkillPoints;
    *tmpMsg << (uint8_t)0x00;
    *tmpMsg << (uint8_t)0x00;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildChatAddMsg( PClient* nClient, uint32_t nAddedCharID, uint8_t nMode )
{
    PMessage* tmpMsg = new PMessage( 18 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x32;
    *tmpMsg << ( uint8_t )nMode;
    *tmpMsg << ( uint32_t )nAddedCharID;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildDoorOpenMsg( uint32_t nRawItemID, bool nDoubleDoor )
{
    //PMessage* tmpMsg = new PMessage(37);
    PMessage* tmpMsg = new PMessage( 21 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside

    *tmpMsg << ( uint8_t )0x0f; // Sub-message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; //++Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint8_t )0x1b;
    *tmpMsg << ( uint32_t )nRawItemID;
    *tmpMsg << ( uint8_t )0x20; //?
    if ( nDoubleDoor )
    {
        *tmpMsg << ( uint16_t )0x0005; //?
        *tmpMsg << ( uint16_t )0x0000; //?
        *tmpMsg << ( uint16_t )0x1500; //?
    }
    else
    {
        *tmpMsg << ( uint16_t )0x0000; //?
        *tmpMsg << ( uint16_t )0x00c8; //? or 0x64 ?
        *tmpMsg << ( uint16_t )0x10ff; //?
    }

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

// Message from text.ini, section [MISC], id = 100+nTxtMsgId
PMessage* PMsgBuilder::BuildText100Msg( PClient* nClient, uint8_t nTxtMsgId, uint32_t nRawObjectID )
{
    PMessage* tmpMsg = new PMessage( 17 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x0c; // Message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x31;
    *tmpMsg << ( uint8_t )nTxtMsgId;
    *tmpMsg << ( uint32_t )nRawObjectID;

    //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

    return tmpMsg;
}

// Same as BuildText100Msg, but here we can specify *ANY* text from text.ini, not limited to section [MISC]
// 1: [DIALOG]
// 2: [STATEMENT]
// 3: [GUI] + [playertextures]
// 4: [TERMINAL]
// 5: [MENU]
// 6: [MISC]
// 7: [ITEMDESC]
// 8: [HELPTEXT]
PMessage* PMsgBuilder::BuildTextIniMsg( PClient* nClient, uint8_t nTxtGroupID, uint16_t nTxtID )
{
    PMessage* tmpMsg = new PMessage( 20 );

    nClient->IncreaseUDP_ID();
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0e; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25; // ??
    *tmpMsg << ( uint8_t )0x15; // ??
    *tmpMsg << nTxtGroupID;
    *tmpMsg << nTxtID;
    *tmpMsg << ( uint8_t )0x00; // ??
    *tmpMsg << ( uint8_t )0x00; // ??
    *tmpMsg << ( uint8_t )0x00; // ??

    //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharInteractionMenuMsg( PClient* nClient, uint32_t nRawTargetID )
{
    PMessage* tmpMsg = new PMessage( 17 );

    nClient->IncreaseUDP_ID();
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0b; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x4d;
    *tmpMsg << ( uint32_t )nRawTargetID;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildFurnitureActivateMsg( PClient* nClient, uint32_t nRawObjectID, uint8_t nActionValue )
{
    PMessage* tmpMsg = new PMessage( 12 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x06; // SubMessage length;
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nRawObjectID;
    *tmpMsg << ( uint8_t )nActionValue; // known valid are 5 (ring), 9 (remove) and 10 (clic)

    //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseFurnitureMsg( PClient* nClient, uint32_t nRawObjectID )
{
    PMessage* tmpMsg = new PMessage( 24 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x06; // SubMessage length;
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nRawObjectID;
    *tmpMsg << ( uint8_t )0x0a;

    *tmpMsg << ( uint8_t )0x0b; // SubMessage length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x17;
    *tmpMsg << ( uint32_t )nRawObjectID;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseVhcTerminalMsg( PClient* nClient, uint32_t nRawObjectID )
{
    PMessage* tmpMsg = new PMessage( 24 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x06; // SubMessage length;
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nRawObjectID;
    *tmpMsg << ( uint8_t )0x0a;

    *tmpMsg << ( uint8_t )0x07; // SubMessage length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x4a;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseGogoMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 17 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x0b; // Message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3d;
    *tmpMsg << ( uint32_t )0x0000000d; // cmd

    //(*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseVentureWarpMsg( PClient* nClient, uint32_t nRawObjectID )
{
    PMessage* tmpMsg = new PMessage( 17 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x0b; // Message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3d;
    *tmpMsg << ( uint32_t )0x00000008; // cmd
    *tmpMsg << ( uint32_t )0x00000007; // cmd ?
    *tmpMsg << ( uint32_t )0x00000002; // ?
    *tmpMsg << ( uint16_t )0x0004; // ?
    *tmpMsg << nRawObjectID;
    // *tmpMsg << (uint8_t)0x13; // ? Seems we can do without...

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildVhcAccessRequestMsg( PClient* nClient, uint32_t nRequestId, uint32_t nRequesterCharId, uint32_t nRequesterLocalId, uint32_t nVhcRawObjectID )
{
    PMessage* tmpMsg = new PMessage( 40 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x22; // Message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3d;
    *tmpMsg << ( uint32_t )0x00000008; // cmd
    *tmpMsg << ( uint32_t )0x00000005; // cmd ?
    *tmpMsg << ( uint32_t )nRequestId;
    *tmpMsg << ( uint16_t )0x000c; // ? length ?
    *tmpMsg << nRequesterCharId; //uint32_t
    *tmpMsg << nRequesterLocalId; // ? uint32_t
    *tmpMsg << nVhcRawObjectID;
    *tmpMsg << ( uint8_t )0x08; // ?

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}
/* S->C
13:ff:00:72:d6:22:
03:ff:00:1f:03:00:
3d:
08:00:00:00:
05:00:00:00:
01:00:00:00:
0c:00:
2f:d8:01:00:
02:00:00:00:
5d:03:00:00:
08:
*/

/* C->S RESP: OK
13:79:00:ec:d5:17:
03:79:00:1f:03:00:
3d:
09:00:00:00:
06:00:00:00:
02:00:00:00:
01:00:
01:00
*/
/* C->S RESP: NOK
13:74:00:e7:d5:17:
03:74:00:1f:03:00:
3d:
09:00:00:00:
06:00:00:00:
01:00:00:00:
01:00:
00:00

*/

PMessage* PMsgBuilder::BuildCharUseGenrepMsg( PClient* nClient, uint32_t nRawObjectID, uint32_t nLocation, uint16_t nEntity )
{
    PMessage* tmpMsg = new PMessage( 24 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x06; // SubMessage length;
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nRawObjectID;
    *tmpMsg << ( uint8_t )0x0a;

    // this submessage is only needed to set to location/entity of the GR for a potential record in the char's GR list
    *tmpMsg << ( uint8_t )0x0d; // SubMessage length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nLocation;
    *tmpMsg << ( uint16_t )nEntity;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseLiftMsg( PClient* nClient, uint32_t nRawObjectID, uint16_t nAptPlace )
{
    PMessage* tmpMsg = new PMessage( 29 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x06; // SubMessage length;
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nRawObjectID;
    *tmpMsg << ( uint8_t )0x0a;

    *tmpMsg << ( uint8_t )0x11; // SubMessage length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x38;
    *tmpMsg << ( uint8_t )0x01;
    *tmpMsg << ( uint32_t )nRawObjectID;
    *tmpMsg << ( uint16_t )nAptPlace;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x00;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharShowGlowCircleMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 14 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // UDP ID placeholder
    *tmpMsg << ( uint16_t )0x0000; // SessionID placeholder
    *tmpMsg << ( uint8_t )0x08;  // Len (static, always 0x08
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // Sub UDP ID placeholder
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3c; // Command SHOW GLOWING CIRCLE (kinda ^^)
    *tmpMsg << ( uint8_t )0x01; // "on" ?

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharMoneyUpdateMsg( PClient* nClient, uint32_t nCredits )
{
    PMessage* tmpMsg = new PMessage( 21 );
    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0f; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25; // cmd
    *tmpMsg << ( uint8_t )0x13; // cmd
    *tmpMsg << ( uint16_t )nClient->GetTransactionID();
    *tmpMsg << ( uint8_t )0x04; // cmd
    *tmpMsg << nCredits;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildUndefineduseMsg( PClient* nClient, uint8_t nValue )
{
    PMessage* tmpMsg = new PMessage( 15 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x09; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25;
    *tmpMsg << ( uint8_t )0x23;
    *tmpMsg << nValue;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg2( PClient* nClient, uint16_t nV1, uint16_t nV2, uint16_t nV3, uint16_t nV4, uint16_t nV5, uint16_t nV6, uint16_t nV7 )
{
    // lol? Whats this?
    PMessage* tmpMsg = new PMessage( 28 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x16; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25; // cmd
    *tmpMsg << ( uint8_t )0x22; // cmd
    *tmpMsg << nV1;
    *tmpMsg << nV2;
    *tmpMsg << nV3;
    *tmpMsg << nV4;
    *tmpMsg << nV5;
    *tmpMsg << nV6;
    *tmpMsg << nV7;
    /* *tmpMsg << (uint8_t)0x64; // ??
     *tmpMsg << (uint8_t)0x00; // ??
     *tmpMsg << (uint8_t)0x64; // ??
     *tmpMsg << (uint8_t)0x00; // ??
     *tmpMsg << (uint8_t)0x64; // ??
     *tmpMsg << (uint8_t)0x00; // ??
     *tmpMsg << (uint8_t)0x64; // ??
     *tmpMsg << (uint8_t)0x00; // ??
     *tmpMsg << (uint8_t)0x64; // ??
     *tmpMsg << (uint8_t)0x00; // ??
     *tmpMsg << (uint8_t)0x64; // ??
     *tmpMsg << (uint8_t)0x00; // ??
     *tmpMsg << (uint8_t)0x00; // ??
     *tmpMsg << (uint8_t)0x00; // ??
    */

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg3( PClient* nClient, uint8_t nSlot )
{
    PMessage* tmpMsg = new PMessage( 19 );
    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0d; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25; // cmd
    *tmpMsg << ( uint8_t )0x13; // cmd
    *tmpMsg << ( uint16_t )nClient->GetTransactionID();
    *tmpMsg << ( uint8_t )0x0b; // cmd
    *tmpMsg << nSlot; // ??
    *tmpMsg << ( uint8_t )0x00; // ??

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg4( PClient* nClient, uint16_t nWeaponId )
{
    PMessage* tmpMsg = new PMessage( 16 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x0a; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x2f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x02; // ??
    *tmpMsg << ( uint8_t )0x02; // ??
    *tmpMsg << nWeaponId;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildContainerContentList( PContainer* nContainer, uint8_t nLocType )
{
    PMessage* tmpMsg = new PMessage( 256 );
    std::vector< PContainerEntry* >* Entries = nContainer->GetEntries();
    PContainerEntry* tEntry;
    PMessage* entryMsg;

//Console->Print(YELLOW, BLACK, "BuildContainerContentList for loc %d", nLocType);
    if ( nLocType != INV_LOC_BOX )
    {
        if ( nLocType == INV_LOC_BACKPACK )
            *tmpMsg << ( uint16_t )Entries->size(); // items nb
        else
            *tmpMsg << ( uint8_t )Entries->size(); // items nb
    }

    for ( uint16_t i = 0; i < Entries->size(); ++i )
    {
        tEntry = Entries->at( i );
        entryMsg = BuildContainerContentEntry( tEntry, nLocType );
//if(tEntry->mItem->mItemID == 390)
//{
//Console->Print(YELLOW, BLACK, "BuildContainerContentList entry %d - size %d", i, entryMsg->GetSize());
//entryMsg->Dump();
//}
        *tmpMsg << *entryMsg;
        delete entryMsg;
    }

    delete Entries;
    return tmpMsg;
}

PMessage* PMsgBuilder::BuildContainerContentEntry( PContainerEntry* nEntry, uint8_t nLocType )
{
    PMessage* tmpMsg = new PMessage( 16 );
    PItem* tItem;
    uint8_t dataFlags, Qualifier;

    tItem = nEntry->mItem;
    dataFlags = Qualifier = 0x00 ;

    if (( tItem->mItemID == 390 ) /* testing */ || tItem->mLoadedAmmoNb )
    {
        dataFlags |= 0x01;
    }
    if (( tItem->GetType() == ITEM_TYPE_WEAPON ) || ( tItem->GetType() == ITEM_TYPE_BLUEPRINT ) || ( tItem->GetType() == ITEM_TYPE_WRECKEDPART )/*|| (tItem->GetType() == ITEM_TYPE_APARTMENTKEY) || (tItem->GetType() == ITEM_TYPE_CLANKEY) || (tItem->GetType() == ITEM_TYPE_VHCKEY) */ ) // testing loaded ammo type & BP attributes
    {
        dataFlags |= 0x20;
    }

    switch ( tItem->GetType() )
    {
    case ITEM_TYPE_WEAPON:
    case ITEM_TYPE_AUTOWEAPON:
        Qualifier = 6;
        dataFlags |= 0x02;
        break;
    case ITEM_TYPE_IMPLANT:
    case ITEM_TYPE_ARMOR:
        Qualifier = 2;
        dataFlags |= 0x02;
        break;
    default:
        Qualifier = 0;
        break;
    }

    if ( tItem->IsStackable() && tItem->mStackSize )
    {
        dataFlags |= 0x04;
    }

    if (( tItem->mModificators ) || ( tItem->mItemID == 390 ) ) // TEST
        dataFlags |= 0x10;

    if ( tItem->mMaxSlots || ( tItem->mItemID == 390 ) ) // TEST
        dataFlags |= 0x40;



    if ( nLocType == INV_LOC_BOX )
        *tmpMsg << ( uint8_t )0x00;   // Size of item placeholder
    else
        *tmpMsg << ( uint16_t )0x0000;   // Size of item placeholder

    switch ( nLocType )
    {
    case INV_LOC_WORN:
        *tmpMsg << ( uint8_t )nEntry->mPosX; // X Location
        *tmpMsg << ( uint8_t )0x00; // just nothing
        break;
    case INV_LOC_BACKPACK:
        *tmpMsg << ( uint8_t )0x00; // just nothing again
        *tmpMsg << ( uint8_t )nEntry->mPosX; // X Location
        *tmpMsg << ( uint8_t )nEntry->mPosY; // Y Location
        break;
    case INV_LOC_GOGO:
        *tmpMsg << ( uint8_t )nEntry->mPosX;
        break;
    case INV_LOC_BOX:
    case INV_LOC_BOX2:
        break;
    default:
        break;
    }

    *tmpMsg << ( uint16_t )tItem->mItemID; // ItemID
    *tmpMsg << ( uint8_t )dataFlags; // (0x01|0x02|0x04|0x10|0x20|0x40|0x80); // Datatypes

    if ( dataFlags & 0x01 )
    {
        if ( tItem->GetType() == ITEM_TYPE_WEAPON ) // TESTING
            *tmpMsg << ( uint8_t )6; // Remaining ammos
        else
            *tmpMsg << ( uint8_t )tItem->mLoadedAmmoNb; // Remaining ammos
    }

    if ( dataFlags & 0x02 )
    {
        *tmpMsg << ( uint8_t )Qualifier; // Qual entries
        if ( Qualifier >= 2 )
        {
            *tmpMsg << ( uint8_t )tItem->mCurDuration; // current qual
            if ( Qualifier == 6 )
            {
                *tmpMsg << ( uint8_t )tItem->mDamages; // dmg
                *tmpMsg << ( uint8_t )tItem->mFrequency; // freq
                *tmpMsg << ( uint8_t )tItem->mHandling; // handl
                *tmpMsg << ( uint8_t )tItem->mRange; // range
            }
            *tmpMsg << ( uint8_t )tItem->mMaxDuration; // max qual
        }
    }

    if ( dataFlags & 0x10 )
    {
        if ( tItem->mItemID == 390 ) // test
            *tmpMsg << ( uint8_t )4;
        else
            *tmpMsg << ( uint8_t )tItem->mModificators; // addons bitflag: flashlight=1, scope, silencer, laserpointer
    }

    if ( dataFlags & 0x40 )
    {
        if ( tItem->mItemID == 390 ) // test
        {
            *tmpMsg << ( uint8_t )3;
            *tmpMsg << ( uint8_t )3;
            *tmpMsg << ( uint16_t )0x000b; // enlarged
            *tmpMsg << ( uint16_t )0x05de; // phosophore
            *tmpMsg << ( uint16_t )( -3 ); // silencer
        }
        else
        {

            *tmpMsg << ( uint8_t )tItem->mUsedSlots; // used slots
            *tmpMsg << ( uint8_t )tItem->mMaxSlots; // max slots
            for ( uint8_t j = 0; j < tItem->mMaxSlots; ++j )
                *tmpMsg << ( uint16_t )(( j < tItem->mUsedSlots ) ? tItem->mSlot[j] : 0 ); // mod in slot
        }
    }

    if ( dataFlags & 0x20 ) // loaded ammo type ????
    {
        uint16_t lengthFieldOffset = tmpMsg->GetNextByteOffset();
        *tmpMsg << ( uint16_t )0x0000; // length placeholder

        if ( tItem->GetType() == ITEM_TYPE_WEAPON )
        {
            *tmpMsg << ( uint8_t )0x01; // ammo info
            *tmpMsg << ( uint8_t )0x04; // total length ?
            *tmpMsg << ( uint8_t )0x00; // + baseammo => current ammoId. 0xff => undefined
            *tmpMsg << ( uint8_t )0xff; // supported ammos bitmap (all here)
        }

        if ( false && ( tItem->GetType() == ITEM_TYPE_APARTMENTKEY ) ) // activated Apartment key
        {
            *tmpMsg << ( uint8_t )0x02; // ammo info
            *tmpMsg << ( uint8_t )0x06; // total length
            *tmpMsg << ( uint32_t )123456; // apartmentObjectID ?
        }

        if ( false && ( tItem->GetType() == ITEM_TYPE_CLANKEY ) ) // activated ClanKey
        {
            *tmpMsg << ( uint8_t )0x04; // BP of... info
            *tmpMsg << ( uint8_t )0x0a; // total length
            *tmpMsg << ( uint32_t )1234; // ClanID ?
            *tmpMsg << ( uint32_t )123456; // apartmentObjectID ?
        }

        if ( tItem->GetType() == ITEM_TYPE_BLUEPRINT ) // BP
        {
            *tmpMsg << ( uint8_t )0x05; // BP of... info
            *tmpMsg << ( uint8_t )0x06; // total length
            *tmpMsg << ( uint32_t )486; // ItemID ("Tangent Sniper Rifle")
        }

        if ( false && ( tItem->GetType() == ITEM_TYPE_VHCKEY ) ) // activated VHC Key
        {
            *tmpMsg << ( uint8_t )0x08; // VHC Key
            *tmpMsg << ( uint8_t )0x0a; // total length
            *tmpMsg << ( uint32_t )654321; // vhcObjectID ?
            *tmpMsg << ( uint32_t )123456; // Owner CharID ?
        }

        if ( false && ( tItem->GetType() == ITEM_TYPE_WRECKEDPART ) ) // Identified rare part
        {
            *tmpMsg << ( uint8_t )0x09; // Rare part
            *tmpMsg << ( uint8_t )0x05; // total length
            *tmpMsg << ( uint16_t )453; // Rare Item ID ? REDEEMER
            *tmpMsg << ( uint8_t )0; // ??
        }

        if (( tItem->mConstructorId ) || ( tItem->mItemID == 390 ) ) // Named item /itemId 390: test
        {
            *tmpMsg << ( uint8_t )0x0a; // constructor info
            *tmpMsg << ( uint8_t )0x06; // total length
            //*tmpMsg << (uint32_t)tItem->mConstructorId; // charID
            *tmpMsg << ( uint32_t )2;
        }

        tmpMsg->U16Data( lengthFieldOffset ) = tmpMsg->GetNextByteOffset() - lengthFieldOffset - 2;
    }

    if ( dataFlags & 0x04 )
    {
        *tmpMsg << ( uint32_t )tItem->mStackSize;
    }


    if ( nLocType == INV_LOC_BOX )
        tmpMsg->U8Data( 0 ) = tmpMsg->GetSize() - 1;
    else
        tmpMsg->U16Data( 0 ) = tmpMsg->GetSize() - 2;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharOpenContainerMsg( PClient* nClient, uint32_t nContainerID, PContainer* nContainer )
{
    PMessage* ContentList = BuildContainerContentList( nContainer, INV_LOC_BOX );

    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x26;
    *tmpMsg << nContainerID;
    *tmpMsg << ( uint8_t )0x00; // Always the same on item containers? // 0x01 for Trader (NeoX gameclient 3608)
    *tmpMsg << ( uint8_t )0x64; // Always the same on item containers?
    *tmpMsg << ( uint8_t )0x00; // Always the same on item containers?
    *tmpMsg << ( uint8_t )0x08; // 0x08 when container is filled, 0x00 when not? At least it works..

    *tmpMsg << ( uint16_t )( ContentList->GetSize() );
    *tmpMsg << *ContentList;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    delete ContentList;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildItemMoveMsg( PClient* nClient, uint8_t nSource, uint8_t nSrcX, uint8_t nSrcY, uint8_t nDestination, uint8_t nDestX, uint8_t nDestY, uint8_t nItemCnt )
{
    PMessage* tmpMsg = new PMessage( 26 );
    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x14; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25;
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetTransactionID();
    *tmpMsg << ( uint8_t )0x14; // ItemMove Answer
    *tmpMsg << nSource;
    *tmpMsg << nSrcX;
    *tmpMsg << nSrcY;
    *tmpMsg << nDestination;
    *tmpMsg << nDestX;
    *tmpMsg << nDestY;
    *tmpMsg << nItemCnt;
    *tmpMsg << ( uint8_t )0x00; // ??
    *tmpMsg << ( uint8_t )0x00; // ??

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildBoxItemMoveMsg( PClient* nClient, PContainerEntry* nEntry, uint8_t nSrcX, uint8_t nSrcY, uint8_t nDestination, uint8_t nDestX, uint8_t nDestY, uint8_t nItemCnt )
{
    PMessage* tmpMsg = new PMessage( 64 );
    PMessage* entryMsg = BuildContainerContentEntry( nEntry, INV_LOC_BOX2 );

    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x09; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25;
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetTransactionID();
    *tmpMsg << ( uint8_t )0x17; // BoxItemMove Answer Src
    *tmpMsg << ( uint8_t )INV_LOC_BOX; // Src = Box
    *tmpMsg << nSrcX;
    *tmpMsg << nSrcY;
    *tmpMsg << nItemCnt;
    *tmpMsg << ( uint8_t )0x00; // Qty high
    *tmpMsg << ( uint8_t )0x18; // BoxItemMove Answer Dst
    *tmpMsg << nDestination;
    *tmpMsg << nDestX;
    *tmpMsg << nDestY;
    *tmpMsg << *entryMsg;
    *tmpMsg << ( uint8_t )0x12; // ? vary ...
    *tmpMsg << ( uint8_t )0x00;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    delete entryMsg;
    return tmpMsg;
}

/*PMessage* PMsgBuilder::BuildItemAmmoUpdateMsg (PClient* nClient, PContainerEntry* nEntry, uint8_t nSrcX, uint8_t nSrcY, uint8_t nDestination, uint8_t nDestX, uint8_t nDestY, uint8_t nItemCnt)
{
  PMessage* tmpMsg = new PMessage(64);
  PMessage* entryMsg = BuildContainerContentEntry(nEntry, INV_LOC_BOX2);

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID();

  *tmpMsg << (uint8_t)0x13;
  *tmpMsg << (uint16_t)nClient->GetUDP_ID();
  *tmpMsg << (uint16_t)nClient->GetSessionID();

  *tmpMsg << (uint8_t)0x09; // Message length
  *tmpMsg << (uint8_t)0x03;
  *tmpMsg << (uint16_t)nClient->GetUDP_ID();
  *tmpMsg << (uint8_t)0x1f;
  *tmpMsg << (uint16_t)nClient->GetLocalID();
  *tmpMsg << (uint8_t)0x25;
  *tmpMsg << (uint8_t)0x13;
  *tmpMsg << (uint16_t)nClient->GetTransactionID();
  *tmpMsg << (uint8_t)0x17; // BoxItemMove Answer Src
  *tmpMsg << (uint8_t)INV_LOC_BOX; // Src = Box
  *tmpMsg << nSrcX;
  *tmpMsg << nSrcY;
  *tmpMsg << nItemCnt;
  *tmpMsg << (uint8_t)0x00; // Qty high
  *tmpMsg << (uint8_t)0x18; // BoxItemMove Answer Dst
  *tmpMsg << nDestination;
  *tmpMsg << nDestX;
  *tmpMsg << nDestY;
  *tmpMsg << *entryMsg;
  *tmpMsg << (uint8_t)0x12; // ? vary ...
  *tmpMsg << (uint8_t)0x00;

  (*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);

  delete entryMsg;
  return tmpMsg;
}*/
/* Resp:
03:33:00:1f:01:00:25:13
c2:01:0a:00:02:00:00:00 ??
c3:01:05:03:00:00:12 Update ammo left
c4:01:05:02:00:00:0c Update ammo left
*/

PMessage* PMsgBuilder::BuildStartHackGameMsg( PClient* nClient, uint32_t nWorldObjID, uint8_t nHackDifficult )
{
    PMessage* tmpMsg = new PMessage( 22 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x10;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x28;
    *tmpMsg << nWorldObjID;
    *tmpMsg << nHackDifficult;
    *tmpMsg << ( uint8_t )0x28;
    *tmpMsg << ( uint8_t )0x5c;
    *tmpMsg << ( uint8_t )0xcf;
    *tmpMsg << ( uint8_t )0x3e;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildSubwaySpawnMsg( PClient* nClient, bool IsSecondMessage )
{
    PMessage* tmpMsg = new PMessage( 197 );
    uint16_t First = IsSecondMessage ? 6 : 0;
    uint16_t Last = IsSecondMessage ? 10 : 5;

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    for ( uint16_t i = First; ( i <= Last ); i++ )
    {
        nClient->IncreaseUDP_ID();
        *tmpMsg << ( uint8_t )0x11; //msg size
        *tmpMsg << ( uint8_t )0x03;
        *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
        *tmpMsg << ( uint8_t )0x28;
        *tmpMsg << ( uint16_t )0x0027;
        *tmpMsg << ( uint32_t )( PSubway::mCabsBaseId + i );
        *tmpMsg << ( uint16_t )0x0000;
        *tmpMsg << ( uint8_t )0x00;
        *tmpMsg << ( uint16_t )Subway->mSubways[i].mPosition;
        *tmpMsg << ( uint8_t )0x00;
        *tmpMsg << ( uint8_t )Subway->mSubways[i].mDoorOpened;;

        nClient->IncreaseUDP_ID(); // vhc health update
        *tmpMsg << ( uint8_t )0x0d; //msg size
        *tmpMsg << ( uint8_t )0x03;
        *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
        *tmpMsg << ( uint8_t )0x2d;
        *tmpMsg << ( uint32_t )( PSubway::mCabsBaseId + i );
        *tmpMsg << ( uint8_t )0x0a;
        *tmpMsg << ( uint32_t )PSubway::mCabsBaseHealth;
    }

    tmpMsg->U16Data( 1 ) = nClient->GetUDP_ID();
    tmpMsg->U16Data( 3 ) = nClient->GetSessionID();

    return tmpMsg;
}

/*
PMessage* PMsgBuilder::BuildSubwayFullUpdateMsg(PClient* nClient)
{
    PMessage* tmpMsg = new PMessage(148);
    *tmpMsg << (uint8_t)0x13;
    *tmpMsg << (uint16_t)0x0000; // placeholder for UDP_ID;
    *tmpMsg << (uint16_t)0x0000; // placeholder for SessionID();

    for(uint8_t i=0; i<PSubway::mCabsNumber; i++)
    {
      nClient->IncreaseUDP_ID();
      *tmpMsg << (uint8_t)0x0c; //msg size
      *tmpMsg << (uint8_t)0x03;
      *tmpMsg << (uint16_t)nClient->GetUDP_ID();
      *tmpMsg << (uint8_t)0x32;
      *tmpMsg << (uint32_t)(PSubway::mCabsBaseId + i);
      *tmpMsg << (uint8_t)0x00;
      *tmpMsg << (uint16_t)Subway->mSubways[i].mPosition;
      *tmpMsg << (uint8_t)Subway->mSubways[i].mDoorOpened;;
  }

    tmpMsg->U16Data(1) = nClient->GetUDP_ID();
    tmpMsg->U16Data(3) = nClient->GetSessionID();

    return tmpMsg;
}
*/

PMessage* PMsgBuilder::BuildSubwaySingleUpdateMsg( uint32_t nVehicleID, uint16_t nPosition, uint8_t nDoorOpened )
{
    PMessage* tmpMsg = new PMessage( 18 );
    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x0c; //msg size
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // ++UDP_ID placeholder
    *tmpMsg << ( uint8_t )0x32;
    *tmpMsg << ( uint32_t )nVehicleID;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint16_t )nPosition;
    *tmpMsg << ( uint8_t )nDoorOpened;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildSpawnWorldObjectMsg( uint16_t nModelID, uint16_t nFunctionID, uint32_t nWOID, uint16_t nPosX, uint16_t nPosY, uint16_t nPosZ, uint8_t nRotX, uint8_t nRotY, uint8_t nRotZ )
{
    PMessage* tmpMsg = new PMessage( 31 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // UDP placeholder
    *tmpMsg << ( uint16_t )0x0000; // Session placeholder
    *tmpMsg << ( uint8_t )0x16;  // Message length
    *tmpMsg << ( uint8_t )0x03;  // 0x03 commandset
    *tmpMsg << ( uint16_t )0x0000; // UDP placeholder
    *tmpMsg << ( uint8_t )0x1b;  // Subcommandset
    *tmpMsg << ( uint32_t )nWOID;  // WorldobjectID

    *tmpMsg << ( uint8_t )0x19;  // Positiondata follows
    *tmpMsg << ( uint16_t )nPosY;
    *tmpMsg << ( uint16_t )nPosZ;
    *tmpMsg << ( uint16_t )nPosX;
    *tmpMsg << ( uint8_t )nRotY;  // Rotation X
    *tmpMsg << ( uint8_t )nRotZ;  // Rotation Y
    *tmpMsg << ( uint8_t )nRotX;  // Rotation Z
    *tmpMsg << ( uint16_t )nModelID;
    *tmpMsg << ( uint16_t )nFunctionID;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildRemoveWorldObjectMsg( uint32_t nWOID )
{
    PMessage* tmpMsg = new PMessage( 14 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // UDP ID placeholder
    *tmpMsg << ( uint16_t )0x0000; // SessionID placeholder
    *tmpMsg << ( uint8_t )0x08;  // Len (static, always 0x08
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // Sub UDP ID placeholder
    *tmpMsg << ( uint8_t )0x26;  // Command FADE AWAY CHAR (kinda ^^)
    *tmpMsg << ( uint32_t )nWOID;  // WorldobjectID

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildDBRequestStatusMsg( PClient* nClient, std::string *nCommandName, uint8_t nStatus, uint16_t nErrCode )
{
    PMessage* tmpMsg = new PMessage( 32 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x14; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x2b;
    *tmpMsg << ( uint8_t )0x1a;
    *tmpMsg << ( uint16_t )( nCommandName->size() + 1 );
    *tmpMsg << ( uint8_t )nStatus;
    *tmpMsg << ( uint16_t )nErrCode;
    *tmpMsg << ( *nCommandName );

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildDBAnswerMsg( PClient* nClient, std::string *nCommandName, std::string *nAnswerData, uint16_t nRows, uint16_t nCols )
{
    uint8_t i, j, k;
    PMessage* tmpMsg = new PMessage( 32 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x14; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x2b;
    *tmpMsg << ( uint8_t )0x17;
    *tmpMsg << ( uint16_t )( nCommandName->size() + 1 );
    *tmpMsg << ( uint16_t )nRows;
    *tmpMsg << ( uint16_t )nCols;
    *tmpMsg << ( *nCommandName );

    for ( i = 0, k = 0; i < nRows; ++i )
    {
        for ( j = 0; j < nCols; ++j, ++k )
        {
            *tmpMsg << ( uint16_t )( nAnswerData[k].size() + 1 );
            *tmpMsg << nAnswerData[k];
        }
    }

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseVhcMsg( PClient* nClient, uint32_t nRawObjectID, uint16_t nVhcType, uint16_t nAvailableSeats )
{
    PMessage* tmpMsg = new PMessage( 24 );

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x13; // Message length;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x3d;
    *tmpMsg << ( uint32_t )0x0000000e; // cmd
    *tmpMsg << nRawObjectID;
    *tmpMsg << nVhcType;
    *tmpMsg << nAvailableSeats; // Bit flags

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildVhcInfoMsg( PClient* nClient, PSpawnedVehicle* nVehicle )
{
    PMessage* tmpMsg = new PMessage( 32 );
    PVhcCoordinates VhcPos = nVehicle->GetPosition();
    PVehicleInformation VhcInfo = nVehicle->GetInformation();

    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();

    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x28;
    *tmpMsg << ( uint16_t )0x0031;
    *tmpMsg << ( uint32_t )nVehicle->GetLocalId();
    *tmpMsg << ( uint8_t )0x02;
    *tmpMsg << ( uint16_t )( VhcPos.GetY() + 768 );
    *tmpMsg << ( uint16_t )( VhcPos.GetZ() + 768 );
    *tmpMsg << ( uint16_t )( VhcPos.GetX() + 768 );
    *tmpMsg << ( uint8_t )VhcPos.GetUD();
    *tmpMsg << ( uint16_t )VhcPos.GetLR();
    *tmpMsg << ( uint16_t )VhcPos.GetRoll();
    *tmpMsg << ( uint8_t )VhcInfo.GetVehicleType();
    *tmpMsg << ( uint8_t )0xff;
    *tmpMsg << ( uint32_t )0x00000000;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )0x01; // ? changes

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

/*PMessage* PMsgBuilder::BuildVhcInfoMsg( PClient* nClient, PSpawnedVehicle* nVehicle )
{
  PMessage* tmpMsg = new PMessage( 32 );
  PVhcCoordinates VhcPos = nVehicle->GetPosition();
  PVehicleInformation VhcInfo = nVehicle->GetInformation();

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint16_t )nClient->GetSessionID();

  *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint8_t )0x28;
  *tmpMsg << ( uint16_t )0x0031;
  *tmpMsg << ( uint32_t )nVehicle->GetLocalId();
  *tmpMsg << ( uint8_t )0x02;
  *tmpMsg << ( uint16_t )( VhcPos.GetY() + 768 );
  *tmpMsg << ( uint16_t )( VhcPos.GetZ() + 768 );
  *tmpMsg << ( uint16_t )( VhcPos.GetX() + 768 );
  *tmpMsg << ( uint8_t )VhcPos.GetUD();
  *tmpMsg << ( uint16_t )VhcPos.GetLR();
  *tmpMsg << ( uint16_t )VhcPos.GetRoll();
  *tmpMsg << ( uint8_t )VhcInfo.GetVehicleType();
  *tmpMsg << ( uint32_t )0x00000000;
  *tmpMsg << ( uint32_t )0x00000000;
  *tmpMsg << ( uint32_t )0x00000000;
  *tmpMsg << ( uint16_t )0x0000;
  uint32_t tCharId;
  for(uint8_t i = 0; i < 8; ++i)
  {
    if( (tCharId = nVehicle->GetSeatUser(i)) )
    {
      *tmpMsg << tCharId;
    }
    else
    {
      *tmpMsg << ( uint32_t )0xffffffff;
    }
    *tmpMsg << i;
    *tmpMsg << ( uint16_t )0x0000;
  }

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}*/
/* ????
13 f7 00 49 bf
5d
03 f7 00
28 31 00
c9 03 00 00 = Object ID
02
47 ff
00 cd
c3 c3
d7
d7 ec
00 00
29
2b 65 35 8b 8c 6c 7f 80 96
5f 26 00 80 00
ff ff ff ff 00 00 00
ff ff ff ff 01 00 00
ff ff ff ff 02 00 00
ff ff ff ff 03 00 00
ff ff ff ff 04 00 00
ff ff ff ff 05 00 00
ff ff ff ff 06 00 00
ff ff ff ff 07 00 00

*/

PMessage* PMsgBuilder::BuildVhcHealthUpdateMsg( PSpawnedVehicle* nVehicle )
{
    PMessage* tmpMsg = new PMessage( 19 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x0d; //msg size
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for ++UDP_ID
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nVehicle->GetLocalId();
    *tmpMsg << ( uint8_t )0x0a; // Health update
    *tmpMsg << ( float )( nVehicle->GetInformation().GetHealth() );

    return tmpMsg;
}

// NB: same as BuildCharSittingMsg. To be merged later when classes are adapted
PMessage* PMsgBuilder::BuildVhcPosUpdateMsg( PSpawnedVehicle* nVehicle )
{
    PMessage* tmpMsg = new PMessage( 33 );
    PVhcCoordinates VhcPos = nVehicle->GetPosition();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
    *tmpMsg << ( uint16_t )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside

    *tmpMsg << ( uint8_t )0x00; // Message length placeholder;
    *tmpMsg << ( uint8_t )0x32;
    *tmpMsg << ( uint16_t )( nVehicle->GetLocalId() & 0xffff );
    *tmpMsg << ( uint8_t )0x03; // We suppose we use move type 3, as in client message
    *tmpMsg << ( uint16_t )( VhcPos.GetY() + 768 ); // +768 or +0 ??? All Char/Vhc/NPC/Objet offset to clean up...
    *tmpMsg << ( uint16_t )( VhcPos.GetZ() + 768 );
    *tmpMsg << ( uint16_t )( VhcPos.GetX() + 768 );
    *tmpMsg << ( uint8_t )VhcPos.GetUD();
    *tmpMsg << ( uint16_t )VhcPos.GetLR();
    *tmpMsg << ( uint16_t )VhcPos.GetRoll();
    *tmpMsg << ( uint16_t )VhcPos.GetUnknown();
    *tmpMsg << ( uint8_t )0x00;
    *tmpMsg << ( uint8_t )VhcPos.GetAct();
    /* What is that for ????
    *tmpMsg << ( uint8_t )0x02; // <= these two uint8_t corresond to mUnknown... = additionnal data ?
    *tmpMsg << ( uint8_t )0x00; // maybe weapon related ?
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint16_t )0x0001;
    *tmpMsg << ( uint16_t )0x8000;
    *tmpMsg << ( uint16_t )0x8000;
    */

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildVhcPosUpdate2Msg( PSpawnedVehicle* nVehicle )
{
    PMessage* tmpMsg = new PMessage( 28 );
    PVhcCoordinates VhcPos = nVehicle->GetPosition();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x00; //msg size
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // ++UDP_ID placeholder
    *tmpMsg << ( uint8_t )0x32;
    *tmpMsg << ( uint16_t )( nVehicle->GetLocalId() & 0xffff );
    *tmpMsg << ( uint8_t )0x03; // 0x07 in nc2.2, and one more field (same a received msg)
    *tmpMsg << ( uint16_t )( VhcPos.GetY() + 768 ); // +768 or +0 ??? All Char/Vhc/NPC/Objet offset to clean up...
    *tmpMsg << ( uint16_t )( VhcPos.GetZ() + 768 );
    *tmpMsg << ( uint16_t )( VhcPos.GetX() + 768 );
    *tmpMsg << ( uint8_t )VhcPos.GetUD();
    *tmpMsg << ( uint16_t )VhcPos.GetLR();
    *tmpMsg << ( uint16_t )VhcPos.GetRoll();
    *tmpMsg << ( uint16_t )VhcPos.GetUnknown();
    *tmpMsg << ( uint8_t )VhcPos.GetFF();
    *tmpMsg << ( uint8_t )VhcPos.GetAct();

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

/*
ground vhc in NC1
C->S
[...] 32:fb:03:03:d4:8c:2f:84:a0:7b:7b:bf:c6:53:28:01:00:ff:05
S->C
[...] 32: fb:03: 03: d4:8c: 2f:84: a0:7b: 7b: bf:c6: 53:28: 01:00:ff:05

  *nMsg >> mVhcLocalId;
  *nMsg >> mMoveType; // 0 for subway, 3 for nc1 vhc, 7 for nc2 vhc // uint8_t
  *nMsg >> mNewY;
  *nMsg >> mNewZ;
  *nMsg >> mNewX;
  *nMsg >> mNewUD; // uint8_t
  *nMsg >> mNewLR;
  *nMsg >> mNewRoll;
  *nMsg >> mUnk1;
  *nMsg >> mFF; // uint8_t
  *nMsg >> mAction; // uint8_t

quad in NC2.2
C->S
[...] 32:c9:03:07:28:65:35:8b:8c:6c:7f:09:99:45:26:10:80:01:00:ff:05
S->C
[...] 32:c9:03:07:28:65:35:8b:8c:6c:7f:09:99:45:26:10:80:01:00:ff:05

glider in NC2.2
C->S
[...] 32:4b:03:07:13:57:a9:88:89:af:86:c1:89:ec:81:64:82:01:00:ff:00
S->C
[...] 32:4b:03:07:98:57:c8:88:59:b0:83:5c:88:13:81:64:82:01:00:ff:00:
[...] 32:4b:03:07:13:57:a9:88:89:af:86:c1:89:ec:81:64:82:01:00:ff:00:


*/
PMessage* PMsgBuilder::BuildTraderItemListMsg( PClient* nClient, uint32_t nTraderNpcID ) //, PContainer* nContainer)
{
//    PMessage* ContentList = BuildContainerContentList(nContainer, INV_LOC_BOX);
// Tmp manual content list:
    PMessage* ContentList = new PMessage();
    float PriceCoef = 1 / 1.379942;
    uint8_t Quality = 255; // Range 0 - 255

    //Item 1:
    *ContentList << ( uint16_t )0x05e6; // Item Id "Clan key for CityAdmin"
    *ContentList << ( uint32_t )304567; // Base (?) Item price = item.def price / 1.97
    // Displayed price is this price * 1.38 . Is this faction/barter factor ?
    //Item 2:
    *ContentList << ( uint16_t )0x05e7; // Item Id "Clan key for Diamond Real Estate"
    *ContentList << ( uint32_t )( 420285 * PriceCoef );
    //Item 3:
    *ContentList << ( uint16_t )0x05e8; // Item Id "Clan key for N.E.X.T."
    *ContentList << ( uint32_t )( 420285 * PriceCoef );
    //Item 4:
    *ContentList << ( uint16_t )0x060f; // Item Id "PLAZA - 2nd Lev. Apartment"
    *ContentList << ( uint32_t )( 245166 * PriceCoef );
    //Item 5:
    *ContentList << ( uint16_t )0x065c; // Item Id "Normal Viarosso Apartment Alamo Living"
    *ContentList << ( uint32_t )( 840571 * PriceCoef );
    //Item 6:
    *ContentList << ( uint16_t )0x065d; // Item Id "Luxus Viarosso Apartment Alamo Living"
    *ContentList << ( uint32_t )( 1260856 * PriceCoef );

    //Item 7:
    *ContentList << ( uint16_t )355; // Item Id "HEW �Liquid Fire� Rifle"
    *ContentList << ( uint32_t )( 1260856 * PriceCoef );

    PMessage* tmpMsg = new PMessage();
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x26;
    *tmpMsg << nTraderNpcID;
    *tmpMsg << ( uint8_t )0x01; // Traders inventory
    *tmpMsg << ( uint16_t )( ContentList->GetSize() / 6 ); // List entries
    *tmpMsg << ( uint8_t )Quality; // Items quality
    *tmpMsg << *ContentList;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    delete ContentList;

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildStartWeaponReloadMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 16 );
    nClient->IncreaseUDP_ID();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint16_t )nClient->GetSessionID();
    *tmpMsg << ( uint8_t )0x00; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x25; // cmd
    *tmpMsg << ( uint8_t )0x16; // cmd

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildStartWeaponReloadAnimMsg( PClient* nClient )
{
    PMessage* tmpMsg = new PMessage( 13 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x00;  // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint8_t )0x15; // cmd

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildHeldItemUseMsg( uint16_t nUserCharLocalId, uint16_t nWeaponId, uint32_t nTargetRawItemID, uint8_t nAiming, uint8_t nTargetedHeight, uint8_t nScore )
{
    PMessage* tmpMsg = new PMessage( 22 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x00;  // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )nUserCharLocalId;
    *tmpMsg << ( uint8_t )0x01; // cmd
    *tmpMsg << ( uint16_t )nWeaponId;
    *tmpMsg << ( uint32_t )nTargetRawItemID;
    *tmpMsg << ( uint8_t )nAiming;
    *tmpMsg << ( uint8_t )nTargetedHeight;
    *tmpMsg << ( uint8_t )nScore;

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildHeldItemUse2Msg( uint16_t nUserCharLocalId, uint32_t nTargetRawItemID )
{
  PMessage* tmpMsg = new PMessage( 18 );

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

  *tmpMsg << ( uint8_t )0x00;  // Message length placeholder;
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nUserCharLocalId;
  *tmpMsg << ( uint8_t )0x2c; // cmd
  *tmpMsg << ( uint8_t )0x09; // cmd
  *tmpMsg << ( uint32_t )nTargetRawItemID;

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildHeldItemUse3Msg(  PClient* nClient, uint16_t nUnknown1, uint16_t nUnknown2, uint16_t nUnknown3, uint16_t nUnknown4 )
{
  PMessage* tmpMsg = new PMessage( 22 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint16_t )nClient->GetSessionID();
  *tmpMsg << ( uint8_t )0x00; // Message length
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nClient->GetLocalID();
  *tmpMsg << ( uint8_t )0x25; // cmd
  *tmpMsg << ( uint8_t )0x1a; // cmd
  *tmpMsg << ( uint16_t )nUnknown1;
  *tmpMsg << ( uint16_t )nUnknown2;
  *tmpMsg << ( uint16_t )nUnknown3;
  *tmpMsg << ( uint16_t )nUnknown4;

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildHeldItemUse4Msg( uint16_t nUserCharLocalId, uint32_t nTargetRawItemID, uint16_t nUnknown1, uint16_t nUnknown2, uint8_t nTargetedHeight )
{
  PMessage* tmpMsg = new PMessage( 23 );

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

  *tmpMsg << ( uint8_t )0x00;  // Message length placeholder;
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nUserCharLocalId;
  *tmpMsg << ( uint8_t )0x2c; // cmd
  *tmpMsg << ( uint8_t )0x01; // cmd
  *tmpMsg << ( uint16_t )nUnknown1;
  *tmpMsg << ( uint16_t )nUnknown2;
  *tmpMsg << ( uint32_t )nTargetRawItemID;
  *tmpMsg << ( uint8_t )nTargetedHeight;

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildHeldItemAddonActivationMsg( PClient* nClient, uint8_t nState )
{
    PMessage* tmpMsg = new PMessage( 15 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x00;  // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint8_t )0x2f;
    *tmpMsg << ( uint16_t )nClient->GetLocalID();
    *tmpMsg << ( uint16_t )0x0001; // cmd ?
    *tmpMsg << ( uint8_t )( 0x60 | nState );

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildWeatherControlMsg( uint16_t nWeatherId )
{
    PMessage* tmpMsg = new PMessage( 13 );

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x00;  // Message length placeholder;
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint8_t )0x2e;
    *tmpMsg << ( uint8_t )0x01; // cmd
    *tmpMsg << ( uint16_t )nWeatherId; //see at bottom of weather.def

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseTimedDrugMsg( PClient* nClient, const PDefDrug* nDrugDef, uint16_t nItemId )
{
  PMessage* tmpMsg = new PMessage( 60 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint16_t )nClient->GetSessionID();
  *tmpMsg << ( uint8_t )0x00; // Message length
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nClient->GetLocalID();
  *tmpMsg << ( uint8_t )0x25; // cmd
  *tmpMsg << ( uint8_t )0x06; // cmd
  *tmpMsg << ( uint8_t )nDrugDef->GetChangeNum();
  *tmpMsg << ( uint8_t )0x01; // ??? not working if 0, no apparent change if > 1
  *tmpMsg << ( uint16_t )nDrugDef->GetDuration();
  *tmpMsg << ( uint16_t )nItemId;
  for( uint8_t i = 0; i < nDrugDef->GetChangeNum(); ++i )
  {
    *tmpMsg << ( uint8_t )nDrugDef->GetChangeType( i );
    *tmpMsg << ( uint16_t )( nDrugDef->GetChangeScale( i ) * 100 );
    *tmpMsg << ( uint16_t )nDrugDef->GetChangeTarget( i );
  }

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseInstantDrugMsg( PClient* nClient, const PDefDrug* nDrugDef )
{
  PMessage* tmpMsg = new PMessage( 60 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint16_t )nClient->GetSessionID();
  *tmpMsg << ( uint8_t )0x00; // Message length
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nClient->GetLocalID();
  *tmpMsg << ( uint8_t )0x25; // cmd
  *tmpMsg << ( uint8_t )0x07; // cmd
  *tmpMsg << ( uint8_t )nDrugDef->GetChangeNum();
  *tmpMsg << ( uint8_t )0x02; // ??? other values not tested
  for( uint8_t i = 0; i < nDrugDef->GetChangeNum(); ++i )
  {
    *tmpMsg << ( uint8_t )nDrugDef->GetChangeType( i );
    *tmpMsg << ( float )( nDrugDef->GetChangeScale( i ) * 100 ); // float in nc2.2 - uint16_t in nc1 ???
    *tmpMsg << ( uint16_t )nDrugDef->GetChangeTarget( i );
  }

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

  Console->Print( "%s PMsgBuilder::BuildCharUseInstantDrugMsg : data format not tested", Console->ColorText( YELLOW, BLACK, "[WARNING]" ) );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseRecreationUnitMsg( PClient* nClient, uint32_t nObjectId )
{
  PMessage* tmpMsg = new PMessage( 16 );
  nClient->IncreaseUDP_ID();
/*
  *tmpMsg << ( uint8_t )0x13;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint16_t )nClient->GetSessionID();
  *tmpMsg << ( uint8_t )0x00; // Message length
  *tmpMsg << ( uint8_t )0x03;
  *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
  *tmpMsg << ( uint8_t )0x1f;
  *tmpMsg << ( uint16_t )nClient->GetLocalID();
  *tmpMsg << ( uint8_t )0x25; // cmd
  *tmpMsg << ( uint8_t )0x16; // cmd

  ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
*/
  Console->Print( "%s PMsgBuilder::BuildCharUseRecreationUnitMsg : not implemented (target: 0x%08x)", Console->ColorText( YELLOW, BLACK,"[WARNING]" ), nObjectId );
  return tmpMsg;
}

// For testing - packet to be broadcasted to zone
PMessage* PMsgBuilder::BuildNpcDeathMsg( PClient* nClient, uint32_t nNpcId, uint8_t unknown1, uint8_t unknown2 )
{
    PMessage* tmpMsg = new PMessage( 19 );
    PChar *nChar = nClient->GetChar();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for UDP_ID;
    *tmpMsg << ( uint16_t )0x0000; // placeholder for SessionID();

    *tmpMsg << ( uint8_t )0x00;  // Message length placeholder;
    *tmpMsg << ( uint8_t )0x1b;
    *tmpMsg << ( uint32_t )nNpcId;
    *tmpMsg << ( uint8_t )0x1f;
    *tmpMsg << ( uint16_t )( nChar->Coords.mY + 768 + 30 ); //move Npc near to char :p
    *tmpMsg << ( uint16_t )( nChar->Coords.mZ + 768 + 0 );
    *tmpMsg << ( uint16_t )( nChar->Coords.mX + 768 + 30 );
    *tmpMsg << ( uint8_t )2; // ??? 0x01=look at target, 0x02=?, 0x10 = kneel, 0x80 = die
    *tmpMsg << ( uint8_t )1; //0=> dead on health 0 / else alive on health 0. Changes in caps
    *tmpMsg << ( uint8_t )96; // health => 0 alive if prec >0 1-127 alive, <0 dead (ie uint8_t 128-255 = neg signed values)
    *tmpMsg << ( uint16_t )259; // targetId (N)PC - Here: left copbot at NC entrance (zone 2008)
    *tmpMsg << ( uint8_t )0x00; // ? doesn't seem to change in caps
    *tmpMsg << ( uint8_t )0x00; // ? doesn't seem to change in caps
    *tmpMsg << ( uint8_t )0; // ? changes in caps
    *tmpMsg << ( uint8_t )0; // ? changes in caps // moving speed somewhere ?

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    unknown2 = unknown1; // so that gcc doesn't complain if these vars are not used
    return tmpMsg;
}

PMessage* PMsgBuilder::BuildNpcCleanupMsg( uint32_t nNpcId, uint8_t nCmd )
{
    PMessage* tmpMsg = new PMessage();

    *tmpMsg << ( uint8_t )0x13;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x00; // Message length
    *tmpMsg << ( uint8_t )0x03;
    *tmpMsg << ( uint16_t )0x0000;
    *tmpMsg << ( uint8_t )0x2d;
    *tmpMsg << ( uint32_t )nNpcId;
    *tmpMsg << ( uint8_t )nCmd; // 6: npc/vhc "cleanup", 1: kill npc + msg "no reward, too small"

    ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

    return tmpMsg;
 }
 

/*
void Cmd_GiveItem (int ItemId, int Amount, int ClientNum)
{
 unsigned char SendBuffer[256];

// Inventory_AddNewItem (&Client_Sockets[ClientNum].CharInfo, ItemId, Amount);
 SendBuffer[0] = 0x13;
 SendBuffer[5] = 0x1b;
 SendBuffer[6] = 0x03;
 Network_IncrementUDP (ClientNum);
 *(unsigned short*)&SendBuffer[7] = Client_Sockets[ClientNum].UDP_ID;
 SendBuffer[9] = 0x1f;
 *(unsigned short*)&SendBuffer[10] = Client_Sockets[ClientNum].CharInfo.MapID;
 SendBuffer[12] = 0x25;
 SendBuffer[13] = 0x13;
 Client_Sockets[ClientNum].TransactionID++;
 *(unsigned short*)&SendBuffer[14] = Client_Sockets[ClientNum].TransactionID;//Transaction ID
 SendBuffer[16] = 0x18;
 SendBuffer[17] = 0x03; //Location
 SendBuffer[18] = 0xff; // \/
 SendBuffer[19] = 0xff; //Anywhere
 SendBuffer[20] = 0x08;
 SendBuffer[21] = 0x00;
 *(unsigned short*)&SendBuffer[22] = ItemId; //Item Id
 SendBuffer[24] = 0x05;
 SendBuffer[25] = 0x01;
 *(unsigned short*)&SendBuffer[26] = Amount; //Quantity
 SendBuffer[28] = 0x00;
 SendBuffer[29] = 0x00;
 *(unsigned short*)&SendBuffer[30] = Client_Sockets[ClientNum].CharInfo.ItemTransactionID; //Id of Purchased Item (Client sends another packet for placement of new item)

 *(unsigned short*)&SendBuffer[1] = Client_Sockets[ClientNum].UDP_ID;
 *(unsigned short*)&SendBuffer[3] = Client_Sockets[ClientNum].UDP_ID_HIGH;

 Network_SendUDP (SendBuffer, 32, ClientNum);
}
*/

/* Unkown use packets (from nc2.2)
13:81:00:81:e2: 0c: 03:81:00:23: 12:00: 07:00:00:00:00:00 // weather related ?

13:56:00:56:e2: 40: 03:56:00:1f:01:00:25:13: f1:18:13:01:77:05:48:c7: f2:18:13:02:16:74:61:c7: f3:18:13:03:17:74:61:c7: f4:18:13:04:18:74:61:c7: f5:18:13:05:1f:2a:60:c7: f6:18:13:06:1f:2a:60:c7: f7:18:13:0b:3e:8f:6d:c7

13:5c:00:5c:e2: 0c: 03:5c:00:1f:01:00:25:13: f8:18:0e:02

13:xx:xx:xx:xx: 09: 03:68:00:2d: 6d:03:00:00: 06 // Action update ? other NPC update ? some vanish ?
*/
