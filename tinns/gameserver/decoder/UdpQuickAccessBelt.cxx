#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpItemSlotUse ****/

PUdpItemSlotUse::PUdpItemSlotUse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdpItemSlotUse::Analyse()
{
  mDecodeData->mName << "=Active QuickBelt Slot";

  mTargetSlot = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 8 );
  // TODO : Check on mTargetSlot value + put set to INV_WORN_QB_HAND for hand
  if ( mTargetSlot == 255 ) // H "slot 0" Hand
    mTargetSlot = INV_WORN_QB_HAND;

  //if(mTargetSlot == 11) //  ALT-H ?

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemSlotUse::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  PItem* targetItem = nullptr;
  bool tUsable = false;
  bool tUsableInHand = false;
  uint16_t nWeaponId = 0;

  if( gDevDebug )
    Console->Print( "%s Client trying to activate item in slot %d.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTargetSlot );

  uint8_t currentActiveSlot = tChar->GetQuickBeltActiveSlot();
  if ( mTargetSlot == INV_WORN_QB_HAND )
  {
    if( gDevDebug )
      Console->Print( "%s Want to use Hand", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    tUsable = true;
    tUsableInHand = true;
  }
  else
  {
    targetItem = tChar->GetInventory()->GetContainer( INV_LOC_WORN )->GetItem( INV_WORN_QB_START + mTargetSlot );
    if ( targetItem )
    {
      // TODO : do the real check;
      if( gDevDebug )
        Console->Print( "%s Want to use existing item", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      tUsable = true;
      tUsableInHand = true;
      nWeaponId = targetItem->GetValue1();
    }
  }

//TODO:
// if tUsableInHand
//  must check if weapon and allowed in zone
//  and change tUsableInHand to false if needed

  if ( tUsable && !tUsableInHand )
  {
    // active item in belt, but don't take in hand
    if ( gDevDebug )
      Console->Print( "%s activation of QB item slot %d (%s)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTargetSlot, targetItem->GetName().c_str() );
  }
  else
  {
    if ( !tUsable )
    {
      if ( gDevDebug )
        Console->Print( "%s Want to use non-usable item", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      mTargetSlot = INV_WORN_QB_NONE; // if not usable, unequip active one
    }

    if ( mTargetSlot == currentActiveSlot ) // if same as active, unequip active one
    {
      if ( gDevDebug )
        Console->Print( "%s Want to use active slot", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      mTargetSlot = INV_WORN_QB_NONE;
    }

    if ( mTargetSlot != currentActiveSlot ) // now do somthing only if not same as active
    {
      if ( tChar->SetQuickBeltActiveSlot( mTargetSlot ) )
      {
        PMessage* tmpMsg;
        tmpMsg = MsgBuilder->BuildCharHelloMsg( nClient );
        ClientManager->UDPBroadcast( tmpMsg, nClient );

        tmpMsg = MsgBuilder->BuildUndefineduseMsg( nClient, 59 );
        nClient->SendUDPMessage( tmpMsg );

        if ( nWeaponId > 0 )
        {
          tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg2( nClient );
          nClient->SendUDPMessage( tmpMsg );
        }
        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg3( nClient, mTargetSlot );
        nClient->SendUDPMessage( tmpMsg );

        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg4( nClient, nWeaponId );
        nClient->SendUDPMessage( tmpMsg );

        if ( gDevDebug )
          Console->Print("%s activation of QB item slot %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTargetSlot );
      }
      else Console->Print( "%s activation of QB slot %d refused by PChar", Console->ColorText( YELLOW, BLACK, "[WARNING]" ), mTargetSlot );
    }
    else
    {
      Console->Print( "%s Same slot %d as active - should not happen", Console->ColorText( YELLOW, BLACK, "[WARNING]" ), mTargetSlot );
    }
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
