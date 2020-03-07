#include "gameserver/decoder/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpItemManualReload ****/

PUdpItemManualReload::PUdpItemManualReload( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x18";
}

PUdpMsgAnalyser* PUdpItemManualReload::Analyse()
{
  mDecodeData->mName << "=Reloading item (manual)";

  //PMessage* nMsg = mDecodeData->mMessage;
  mUnknown = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 9 );
  // 0e: reload
  // 01 / slotid 0xNNNN : use item from backpack
  if ( gDevDebug )
    mDecodeData->mMessage->Dump();
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemManualReload::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();

  if ( gDevDebug )
    Console->Print( "%s Manual reload start", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );

  uint8_t activeSlot = tChar->GetQuickBeltActiveSlot();
  if (( activeSlot != INV_WORN_QB_NONE ) && ( activeSlot != INV_WORN_QB_HAND ) )
  {
    uint8_t newAmmoCount = GetMaxLoadableAmmos( activeSlot );
    if ( newAmmoCount )
    {
      PMessage* tmpMsg = MsgBuilder->BuildStartWeaponReloadMsg( nClient );
      nClient->SendUDPMessage( tmpMsg );

      tmpMsg = new PMessage( 32 );
      nClient->IncreaseUDP_ID();

      *tmpMsg << ( uint8_t )0x13;
      *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
      *tmpMsg << ( uint16_t )nClient->GetSessionID();

      *tmpMsg << ( uint8_t )0x00; // Message length
      *tmpMsg << ( uint8_t )0x03;
      *tmpMsg << ( uint16_t )nClient->GetUDP_ID();
      *tmpMsg << ( uint8_t )0x1f;
      *tmpMsg << ( uint16_t )nClient->GetLocalID();
      *tmpMsg << ( uint8_t )0x25;
      *tmpMsg << ( uint8_t )0x13;

      nClient->IncreaseTransactionID();
      *tmpMsg << ( uint16_t )nClient->GetTransactionID();
      *tmpMsg << ( uint8_t )0x05; // cmd=Ammo count update
      *tmpMsg << ( uint8_t )INV_LOC_WORN;
      *tmpMsg << ( uint8_t )(INV_WORN_QB_START + activeSlot); // X
      *tmpMsg << ( uint8_t )0x00; //(Y)
      *tmpMsg << ( uint8_t )newAmmoCount; // Ammo count

      ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

      nClient->SendUDPMessage( tmpMsg );
    }
  }
  //else
    //Console->Print( "%s no reloadable weapon on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), activeSlot );


  /*  PClient* nClient = mDecodeData->mClient;
    //PChar* tChar = nClient->GetChar();
    PMessage* tmpMsg = new PMessage(32);

    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << (uint8_t)0x13;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint16_t)nClient->GetSessionID();

    *tmpMsg << (uint8_t)0x00; // Message length
    *tmpMsg << (uint8_t)0x03;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint8_t)0x1f;
    *tmpMsg << (uint16_t)nClient->GetLocalID();
    *tmpMsg << (uint8_t)0x25;
    *tmpMsg << (uint8_t)0x13;
    *tmpMsg << (uint16_t)nClient->GetTransactionID();
    *tmpMsg << (uint8_t)0x0a; // cmd=Ammo type update (?)
    *tmpMsg << (uint8_t)0x03;
    *tmpMsg << (uint8_t)0x02;
    *tmpMsg << (uint8_t)0x00;
    *tmpMsg << (uint8_t)0x00;
    *tmpMsg << (uint8_t)0x00;
    nClient->IncreaseTransactionID();
    *tmpMsg << (uint16_t)nClient->GetTransactionID();
    *tmpMsg << (uint8_t)0x05; // cmd=Ammo count update
    *tmpMsg << (uint8_t)INV_LOC_BACKPACK;
    *tmpMsg << (uint8_t)0x0e; // X
    *tmpMsg << (uint8_t)0x00; //(Y)
    *tmpMsg << (uint8_t)0x03; // Ammo count
    nClient->IncreaseTransactionID();
    *tmpMsg << (uint16_t)nClient->GetTransactionID();
    *tmpMsg << (uint8_t)0x05; // cmd=Ammo count update
    *tmpMsg << (uint8_t)INV_LOC_WORN;
    *tmpMsg << (uint8_t)0x03; // X
    *tmpMsg << (uint8_t)0x00; //(Y)
    *tmpMsg << (uint8_t)0x02; // Ammo count

    (*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);
  tmpMsg->Dump();
    nClient->SendUDPMessage(tmpMsg);
  */
  /* Resp:
  03:33:00:1f:01:00:25:13
  c2:01:0a:00:02:00:00:00 ?? set ammo type ?
  c3:01:05:03:00:00:12 Update ammo left
  c4:01:05:02:00:00:0c Update ammo left

  tt:tt:02:loc:x:y delete item
  */

  /* manual reload
    srv resp : 13:58:00:d6:d4:08:03:58:00:1f:01:00:25:16 (do start reload anim - even if no relaod needed or no more ammo avail)
    cli resp: 13:3f:00:bd:d4:08:03:3f:00:1f:01:00:25:25 (reload anim done , also when reaload not possible because mission ammo)
    srv resp: 13:59:00:d7:d4:16:03:59:00:1f:01:00:25:13:8d:22:05:03:11:00:16:8e:22:05:02:00:00:0c : update inv if needed

    if(mUnknown != 0x0e)
   Console->Print(YELLOW, BLACK, "PUdpManualReloadItem: additionnal byte differs from 0x0e : %2x", mUnknown);
  */
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

uint8_t PUdpItemManualReload::GetMaxLoadableAmmos( uint8_t nBeltSlotId )
{
  PItem* activeItem = mDecodeData->mClient->GetChar()->GetInventory()->GetContainer( INV_DB_LOC_WORN )->GetItem( INV_WORN_QB_START + nBeltSlotId );
  if ( activeItem && ( activeItem->GetType() == ITEM_TYPE_WEAPON ) )
  {
    const PDefWeapon* activeWeapon = GameDefs->Weapons()->GetDef( activeItem->GetValue1() );
    //Console->Print( "%s Using weapon %d, type %d on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), activeItem->GetValue1(), activeItem->GetType(), nBeltSlotId );
    if ( activeWeapon )
    {
      int ammoId = activeWeapon->GetAmmoType(0); // not really the loaded ammo for now
      if ( ammoId > 0 ) // weapon really uses ammos
      {
        const PDefAmmo* activeAmmo = GameDefs->Ammos()->GetDef( ammoId );
        if ( activeAmmo )
        {
          return activeAmmo->GetMagSize();
        }
        //else
          //Console->Print( "%s no valid ammo (%d) on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), ammoId, nBeltSlotId );
      }
      //else
        //Console->Print( "%s no usable ammo (%d) on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), ammoId, nBeltSlotId );
    }
    //else
      //Console->Print( "%s no valid weapon (%d, type %d) on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), activeItem->GetValue1(), activeItem->GetType(), nBeltSlotId );
  }
  //else
    //Console->Print( "%s no valid item on slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nBeltSlotId );
  return 0;
}

/**** PUdpReloadAnimStart ****/

PUdpReloadAnimStart::PUdpReloadAnimStart( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x15";
}

PUdpMsgAnalyser* PUdpReloadAnimStart::Analyse()
{
  mDecodeData->mName << "=Reload animation starting";

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpReloadAnimStart::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  //PChar* tChar = nClient->GetChar();

  PMessage* tmpMsg = MsgBuilder->BuildStartWeaponReloadAnimMsg( nClient );
      ClientManager->UDPBroadcast( tmpMsg, nClient, 5000, true );
  if ( gDevDebug )
    Console->Print( "%s Reload animation starting", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
