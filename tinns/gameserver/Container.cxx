#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

/* --- PContainerEntry class --- */

PContainerEntry::PContainerEntry( PItem* nItem, uint8_t X, uint8_t Y, uint32_t nInvID, bool SetDirty )
{
  mItem = nItem;
  mPosX = X;
  mPosY = Y;
  mInvID = nInvID;
  mDirtyFlag = SetDirty;
}

PContainerEntry::PContainerEntry( MYSQL_ROW row )
{
  mItem = nullptr;
  mInvID = atoi( row[i_invid] );
  mPosX = atoi( row[i_x] );
  mPosY = atoi( row[i_y] );
  mDirtyFlag = false;

  uint32_t nItemID = atoi( row[i_itemid] );
  uint8_t nStackSize = atoi( row[i_qty] );
  //     = std::atoi(row[i_type]);
  uint8_t CurDur = atoi( row[i_curdur] );
  uint8_t Dmg = atoi( row[i_dmg] );
  uint8_t Freq = atoi( row[i_freq] );
  uint8_t Hand = atoi( row[i_hand] );
  uint8_t Rng = atoi( row[i_rng] );
  uint8_t Dur = atoi( row[i_maxdur] );

  mItem = new PItem( nItemID, nStackSize, CurDur, Dur, Dmg, Freq, Hand, Rng );
}

PContainerEntry::~PContainerEntry()
{
  delete mItem;
}

bool PContainerEntry::SQLSave( uint32_t CharID, uint32_t InvLoc )
{
    const std::string queryv = Misc::String::create(" inv_charid='", CharID, "',inv_loc='", InvLoc,
                               "',inv_x='", mPosX, "',inv_y='", mPosY, "',inv_itemid='",
                               mItem->mItemID, "',inv_qty='", mItem->mStackSize, "',inv_type='", 0,
                               "',inv_cdur='", mItem->mCurDuration, "',inv_dmg='", mItem->mDamages,
                               "',inv_frq='", mItem->mFrequency, "',inv_hnd='", mItem->mHandling,
                               "',inv_rng='", mItem->mRange, "'inv_mdur='", mItem->mMaxDuration,
                               "'");
    std::string query;


    if (mInvID)
        query = "UPDATE inventory SET " + queryv + " WHERE inv_id='" + std::to_string(mInvID) +
                "' LIMIT 1;";
    else
        query = "INSERT INTO inventory SET " + queryv + ";";

    if (MySQL->GameQuery(query.c_str()))
    {
        Console->Print(RED, BLACK, "PContainerEntry::SQLSave could not add/update some inventory item in the database");
        Console->Print("Query was:");
        Console->Print("%s", query.c_str());
        MySQL->ShowGameSQLError();
        return false;
    }
    else
    {
        if (!mInvID)
        {
            mInvID = MySQL->GetLastGameInsertId();
            if (gDevDebug)
                Console->Print(GREEN, BLACK, "New item %d added to inventory DB", mInvID);
        }
        //Console->Print(YELLOW, BLACK, "PContainerEntry::SQLSave - Query was:");
        //Console->Print(YELLOW, BLACK, "%s", query.c_str());
        mDirtyFlag = false;
        return true;
    }
}

bool PContainerEntry::SQLDelete()
{
    std::string query;

    if ( mInvID )
    {
        query = "DELETE FROM inventory WHERE inv_id='" + std::to_string(mInvID) +  "' LIMIT 1;";

        if (MySQL->GameQuery(query.c_str()))
        {
            Console->Print(RED, BLACK, "PContainerEntry::SQLDelete could not delete some container item from the database");
            Console->Print("Query was:");
            Console->Print("%s", query.c_str());
            MySQL->ShowGameSQLError();
            return false;
        }
        else
        {
            if (gDevDebug)
                Console->Print(GREEN, BLACK, "Item %d deleted from container DB", mInvID);
            mInvID = 0;
            delete mItem;
            mItem = nullptr;

            mDirtyFlag = false;
            return true;
        }
    }
    else
    {
        Console->Print(YELLOW, BLACK, "PContainerEntry::SQLDelete: Item was not in inventory DB");
        return true;
    }
}

/* --- PContainer class --- */

PContainer::PContainer( uint8_t nMaxSlots )
{
  mMaxSlots = ( nMaxSlots > CONTAINER_MAX_SIZE ? CONTAINER_MAX_SIZE : nMaxSlots ) ;
  if ( mMaxSlots )
    mContContent = new std::vector<PContainerEntry *>(mMaxSlots, nullptr);
  else
    mContContent = new std::vector<PContainerEntry *>;
  mCharID = 0;
  mInvLoc = 0;
  mExclusiveUseCharID = 0;
  mDirtyFlag = false;
}

PContainer::~PContainer()
{
  for ( uint8_t i = 0; i < mContContent->size(); ++i )
    delete mContContent->at( i );

  if ( mExclusiveUseCharID )
    Console->Print( RED, BLACK, "[ERROR] PContainer::~PContainer: Char %d still has exclusive access to container !!! Bad Pointer error will happen.", mExclusiveUseCharID );
}

bool PContainer::AddEntry( PContainerEntry* NewEntry, uint8_t nSlotId )
{
  if ( IsSlotAllowed( nSlotId ) )
  {
    for ( uint8_t i = mContContent->size(); i <= nSlotId; ++i ) // Extend as needed
      mContContent->push_back( static_cast<PContainerEntry *>(nullptr));
    if ( mContContent->at( nSlotId ) )
    {
      Console->Print( RED, BLACK, "[Warning] PContainer::AddEntry: Target entry already %d in use !!!", nSlotId );
      return false;
    }
    mContContent->at( nSlotId ) = NewEntry;
    mDirtyFlag = true;
    this->SetEntryPosXY( NewEntry, nSlotId, NewEntry->mPosX, NewEntry->mPosY );
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::AddEntry: Target entry %d not allowed (max = %d) !!!", nSlotId, mMaxSlots );
    return false;
  }
  /* --- degug only ---
  Console->Print(GREEN, BLACK, "PContainer::AddEntry: added item %d at pos (%d,%d)", NewEntry->mItem->GetItemID(), NewEntry->mPosX, NewEntry->mPosY);

  Dump();
  ---            --- */
}

void PContainer::SetEntryPosXY( PContainerEntry* nEntry, uint8_t nSlotId, uint8_t nPosX, uint8_t nPosY )
{
  nPosX = nPosY;
  nEntry->Set2DPos( nSlotId, 0 );
  mDirtyFlag = mDirtyFlag || nEntry->mDirtyFlag;
}

PContainerEntry* PContainer::RemoveEntry( uint8_t nSlotId )
{
  PContainerEntry *tEntry = nullptr;

  if ( nSlotId < mContContent->size() )
  {
    tEntry = mContContent->at( nSlotId );
    mContContent->at(nSlotId) = nullptr;
  }
  return tEntry;
}

bool PContainer::GetFreeSlot( uint8_t* nSlotId )
{
  return IsSlotFree( *nSlotId );
}

void PContainer::Compact( uint8_t startSlotId )
{
  uint8_t i, j;
  PContainerEntry* tEntry;

  for ( i = j = startSlotId; i < mContContent->size(); ++i )
  {
    if ( mContContent->at( i ) )
    {
      if ( j < i )
      {
        mContContent->at( j ) = tEntry = mContContent->at( i );
        this->SetEntryPosXY( tEntry, j, tEntry->mPosX, tEntry->mPosX );
        mContContent->at(i) = nullptr;
        mDirtyFlag = true;
      }
      ++j;
    }
  }

  if ( !mMaxSlots ) // reduce size only if not fixed-size
  {
    for ( ; j < i; ++j )
      mContContent->pop_back();
  }
}

bool PContainer::StartUse( uint32_t nExclusiveUseCharID )
{
  if ( mExclusiveUseCharID )
    return false;
  else
  {
    mExclusiveUseCharID = nExclusiveUseCharID;
    return true;
  }
}

bool PContainer::EndUse( uint32_t nExclusiveUseCharID )
{
  if ( nExclusiveUseCharID == mExclusiveUseCharID )
  {
    mExclusiveUseCharID = 0;
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::EndUse called with CharID %d when CharID %d had exclusive use", nExclusiveUseCharID, mExclusiveUseCharID );
    return false;
  }
}

bool PContainer::SQLLoad()
{
  if ( !mCharID )
  {
    return false;
  }

  MYSQL_RES *result;
  MYSQL_ROW row;
  char query[1024];
  PContainerEntry* NewEntry;
  bool SaveDirtyFlag;
  uint8_t nSlotId;
  //uint8_t nPosX, nPosY, nSizeX, nSizeY;

  /*if (!mContContent.empty())
  {
    Console->Print(RED, BLACK, "PContainer::SQLLoad: We don't want to load inventory on non-empty inventory for char %d", mCharID);
    return false;
  }*/

  snprintf( query, 1024, "SELECT * FROM inventory WHERE inv_charid='%d' AND inv_loc='%d' ORDER BY inv_x ASC", mCharID, mInvLoc );
  result = MySQL->GameResQuery( query );
  if (result == nullptr)
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::SQLLoad could not load inventory from the database" );
    Console->Print( "Query was:" );
    Console->Print( "%s", query );
    MySQL->ShowGameSQLError();
    return false;
  }
  while (( row = mysql_fetch_row( result ) ) )
  {
    NewEntry = new PContainerEntry( row );
    if ( NewEntry->mItem->GetItemID() )
    {
      nSlotId = NewEntry->mPosX;
      SaveDirtyFlag = mDirtyFlag;

      if ( ! this->GetFreeSlot( &nSlotId ) || ! this->AddEntry( NewEntry, nSlotId ) )
      {
        Console->Print( YELLOW, BLACK, "[Warning] PContainer::SQLLoad: Can't add item %d in slot %d of char %d inventory", NewEntry->mItem->GetItemID(), nSlotId, mCharID );
        delete NewEntry;
        mDirtyFlag = SaveDirtyFlag;
      }
      else
      {
        mDirtyFlag  = SaveDirtyFlag || ( nSlotId != NewEntry->mPosX );
      }
    }
    else
    {
      Console->Print( YELLOW, BLACK, "[Notice] Invalid item in DB (inv_id = %d) - Ignored", NewEntry->mInvID );
      delete NewEntry;
    }
  }
  MySQL->FreeGameSQLResult( result );
  return true;
}

bool PContainer::SQLSave()
{
  if ( !mCharID )
    return false;

  PContainerEntry* ContEntry;
  bool SavedOK = true;

  for ( uint8_t i = 0; i < mContContent->size(); ++i )
  {
    if (( ContEntry = mContContent->at( i ) ) )
    {
      if (( ContEntry->mDirtyFlag ) )
      {
        SavedOK = SavedOK && ContEntry->SQLSave( mCharID, mInvLoc );
      }
    }
  }

  if ( SavedOK )
  {
    mDirtyFlag = false;
  }
  return SavedOK;
}

bool PContainer::IsSlotFree( uint8_t nSlotId )
{
  if ( !IsSlotAllowed( nSlotId ) )
    return false;

  if ( nSlotId >= mContContent->size() )
    return true;
  else
    return !( mContContent->at( nSlotId ) );
}

bool PContainer::AddItem( PItem* NewItem, uint32_t nInvID, uint8_t nPosX, uint8_t nPosY, bool SetDirty )
{
  /* --- auto stacking not implemented yet
  1 - if stackable, check if same item(S) exists
  2 - - if exist
  3 - - - add to stack
  4 - - - if no new stack remains, update LastPutXY & return true
  5 - find an empty space
  6 -
  x - put in new space, update LastPutXY & return true

  */

  if ( NewItem->GetItemID() == 0 )
  {
    Console->Print( RED, BLACK, "[Warning] PContainer::AddItem: invalid item : ID 0" );
    return false;
  }

  PContainerEntry* NewEntry = new PContainerEntry( NewItem, nPosX, nPosY, nInvID, SetDirty );

  if ( this->GetFreeSlot( &nPosX ) && this->AddEntry( NewEntry, nPosX ) ) // !!!! nPosX not good
    return true;
  else
  {
    delete NewEntry;
    return false;
  }
}

bool PContainer::MoveItem( uint8_t srcSlotId, uint8_t nCount, PContainer* dstContainer, uint8_t dstSlotId, uint8_t nPosX, uint8_t nPosY )
{
  if ( dstContainer == this )
    return this->MoveItem( srcSlotId, nCount, dstSlotId );
  else if ( dstContainer->GetFreeSlot( &dstSlotId ) )
  {
    uint8_t savePosX, savePosY;
    PContainerEntry* tEntry = this->GetEntry( srcSlotId );
    if ( tEntry )
    {
      tEntry->Get2DPos( &savePosX, &savePosY );
      tEntry->Set2DPos( nPosX, nPosY );
      if ( !dstContainer->AddEntry( tEntry, dstSlotId ) )
      {
        tEntry->Set2DPos( savePosX, savePosY );
        Console->Print( RED, BLACK, "[Warning] PContainer::MoveItem - Couldn't add entry !" );
        return false;
      }
      tEntry->Get2DPos( &nPosX, &nPosY );
      tEntry->Set2DPos( savePosX, savePosY );
      RemoveEntry( srcSlotId );
      tEntry->Set2DPos( nPosX, nPosY );

      if ( dstContainer->mCharID != this->mCharID ) // save at once if owner changes
      {
        if ( !tEntry->SQLSave( dstContainer->mCharID, dstContainer->mInvLoc ) ) // TODO: improve to manage non-persistent containers
        {
          Console->Print( RED, BLACK, "[Warning] PContainer::MoveItem - Moved entry not saved" );
        }
        //else
        //{
        //  Console->Print(GREEN, BLACK, "[Succes] PContainer::MoveItem - Moved entry saved");
        //}
      }
      return true;
    }
    else
    {
      Console->Print( RED, BLACK, "[Warning] trying to move invalid entry %d", srcSlotId );
      return false;
    }
  }
  return false;
}

// !!! Not partial move yet !!!
bool PContainer::MoveItem( uint8_t srcSlotId, uint8_t nCount, uint8_t dstSlotId )
{
  if ( !( IsSlotAllowed( srcSlotId ) && IsSlotAllowed( dstSlotId ) ) )
    return false;

  if ( srcSlotId == dstSlotId )
    return true;

  if (( mContContent->at( srcSlotId ) )
      && ( mContContent->at( srcSlotId )->mItem->GetStackSize() >= nCount )
      && IsSlotFree( dstSlotId ) )
  {
    PContainerEntry* tEntry = mContContent->at( dstSlotId ) = mContContent->at( srcSlotId );
    tEntry->mPosX = dstSlotId;
    tEntry->mPosY = 0;
    tEntry->mDirtyFlag = true;
    mContContent->at(srcSlotId) = nullptr;
    mDirtyFlag = true;
    return true;
  }
  else
    return false;
}

std::vector< PContainerEntry* >* PContainer::GetEntries()
{
  std::vector< PContainerEntry* >* Entries = new std::vector< PContainerEntry* >;

  for ( uint8_t i = 0; i < mContContent->size(); ++i )
  {
    if ( mContContent->at( i ) )
      Entries->push_back( mContContent->at( i ) );
  }

  return Entries;
}

PContainerEntry* PContainer::GetEntry( uint8_t nSlotId )
{
  if ( nSlotId >= mContContent->size() )
    return nullptr;

  return mContContent->at( nSlotId );
}

PItem* PContainer::GetItem( uint8_t nSlotId )
{
  PContainerEntry* tEntry = this->GetEntry( nSlotId );
  return (tEntry ? tEntry->mItem : nullptr);
}

uint8_t PContainer::RandomFill( uint8_t nItemCount, int nItemContainerDefIndex )
{
  PItem* nItem = nullptr;
  const PDefItems* nItemDef;
  uint32_t nItemSeqId;
  uint8_t CreatedCount = 0;

  if ( !nItemCount )
    nItemCount = mMaxSlots;
  if ( !nItemCount )
    nItemCount = CONTAINER_MAX_SIZE;

  if ( nItemContainerDefIndex >= 0 )
  {
    int newItemIdx, newItemIndex;
    uint8_t newItemQuality;

    const PDefItemContainer* containerDef = GameDefs->ItemContainers()->GetDef( nItemContainerDefIndex );
    if ( containerDef )
    {
      for ( uint8_t i = 0; i < nItemCount; ++i )
      {
        newItemIdx = containerDef->GetRandomItemIdx();
        if ( newItemIdx >= 0 )
        {
          newItemIndex = containerDef->GetItemId( newItemIdx );
          newItemQuality = ( uint8_t )( 255 & int( 255.0 * containerDef->GetQuality( newItemIdx ) ) );
          if ( newItemIndex <= 0 ) // Item group
          {
            newItemIndex = GameDefs->Items()->GetRandomItemIdFromGroup( -newItemIndex );
          }
          nItem = new PItem( newItemIndex, 1, 255, newItemQuality, newItemQuality, newItemQuality, newItemQuality, 255 );
          if ( nItem->GetItemID() )
          {
            if ( this->AddItem( nItem ) )
            {
              ++CreatedCount;
            }
            else
            {
              Console->Print( YELLOW, BLACK, "[Warning] Couldn't add item %d at pos %d", nItem->GetItemID(), i );
              delete nItem;
            }
          }
        }
      }
    }
    else
      Console->Print( YELLOW, BLACK, "[Warning] Container def not found: %d", nItemContainerDefIndex );
  }
  else
  {
    int NumItemsDefs = GameDefs->Items()->GetNumDefs();

    for ( uint8_t i = 0; i < nItemCount; ++i )
    {
      nItemSeqId = Misc::Random::get(NumItemsDefs, 1);

      if (( nItemDef = GameDefs->Items()->GetDefBySeqIndex( nItemSeqId ) ) )
      {
        nItem = new PItem( nItemDef->GetIndex(), 1, 255, 255, 255, 255, 255, 255 );
        if ( nItem->GetItemID() )
        {
          if ( this->AddItem( nItem ) )
          {
            ++CreatedCount;
          }
          else
          {
            //Console->Print(GREEN, BLACK, "Couldn't add item % d at pos % d", nItem->GetItemID(), i);
            delete nItem;
          }
        }
      }
      else
        Console->Print( RED, BLACK, "[Warning] PContainer::RandomFill Couldn't find item for SEQ %d", nItemSeqId );
    }
  }
  return CreatedCount;
}

void PContainer::Dump()
{
  PContainerEntry* tmpEntry;
  PItem* tmpItem;
  uint8_t i;
  //uint8_t y, x;

  for ( i = 0; i < mContContent->size(); ++i )
  {
    if (( tmpEntry = mContContent->at( i ) ) )
    {
      tmpItem = tmpEntry->mItem;
      Console->Print( GREEN, BLACK, "%d: (%d,%d) (%d x %d) Item %d (InvID %d) %s %s", i, tmpEntry->mPosX, tmpEntry->mPosY, tmpItem->GetSizeX(), tmpItem->GetSizeY(), tmpItem->GetItemID(), tmpEntry->mInvID, tmpItem->GetName().c_str(), tmpEntry->mDirtyFlag ? "[*]" : "" );
    }
  }

  if ( mDirtyFlag )
    Console->Print( GREEN, BLACK, "Dirty" );
  Console->Print( GREEN, BLACK, "------------------------------------" );
}

/* --- PContainerWithHoles class --- */


/* --- PContainerAutoCompact --- */
PContainerEntry* PContainerAutoCompact::RemoveEntry( uint8_t nSlotId )
{
  PContainerEntry* tEntry = nullptr;

  if ( nSlotId < mContContent->size() )
  {
    tEntry = mContContent->at( nSlotId );
    mContContent->at( nSlotId ) = nullptr;
    if ( nSlotId == ( mContContent->size() - 1 ) )
      mContContent->pop_back();
    else
      Compact( nSlotId );
  }
  return tEntry;
}

bool PContainerAutoCompact::GetFreeSlot( uint8_t* nSlotId ) // not optimal. A "first free from end" would be better at PContainer level
{
  bool Found = false;

  for ( *nSlotId = 0; *nSlotId < mContContent->size(); ++*nSlotId )
    if ( ! mContContent->at( *nSlotId ) )
    {
      Found = true;
      break;
    }
  return ( Found || IsSlotAllowed( *nSlotId ) );
}

bool PContainerAutoCompact::MoveItem( uint8_t srcSlotId, uint8_t nCount, uint8_t dstSlotId )
{
  srcSlotId = nCount = dstSlotId;
  return false;
}

/* --- PContainer2D class --- */

void PContainer2D::SetEntryPosXY( PContainerEntry* nEntry, uint8_t /*nSlotId*/, uint8_t nPosX, uint8_t nPosY )
{
  //nSlotId = nSlotId;
  if (( nPosX >= INV_BACKPACK_COLS ) || ( nPosY == 255 ) )
  {
    nPosX = nPosY = 0;
  }
  nEntry->Set2DPos( nPosX, nPosY );
  mDirtyFlag = mDirtyFlag || nEntry->mDirtyFlag;
}

/* --- PContainer2DWorkaround --- */
/////
PContainer2DWorkaround::PContainer2DWorkaround( uint8_t nMaxSlots ) : PContainerWithHoles( nMaxSlots )
{
  mNextFreeSlot = 0;
  mMaxCols = 254;
  mMaxRows = 254;
  mRows = 0;
  AddRow();
}

PContainer2DWorkaround::~PContainer2DWorkaround()
{
  for ( int i = 0; i < mRows; i++ )
    delete mContSpace[i];
}

bool PContainer2DWorkaround::AddEntry( PContainerEntry* tEntry, uint8_t nSlotId )
{
  if ( IsSlotAllowed( nSlotId ) )
  {
    if ( FindValid2DPos( tEntry ) )
    {
      for ( uint8_t i = mContContent->size(); i <= nSlotId; ++i ) // Extend as needed
        mContContent->push_back( static_cast<PContainerEntry *>(nullptr));
      if ( mContContent->at( nSlotId ) )
      {
        Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::AddEntry: Target entry already %d in use !!!", nSlotId );
        return false;
      }
      mContContent->at( nSlotId ) = tEntry;
      mDirtyFlag = true;
      //this->SetEntryPosXY(tEntry, nSlotId, tEntry->mPosX, tEntry->mPosY);
      mDirtyFlag = mDirtyFlag || tEntry->mDirtyFlag;
      SetUsed( tEntry );
      return true;
    }
    else
    {
      Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::AddEntry: No 2D space left !!!", nSlotId, mMaxSlots );
      return false;
    }
  }
  else
  {
    Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::AddEntry: Target entry %d not allowed (max = %d) !!!", nSlotId, mMaxSlots );
    return false;
  }

}

PContainerEntry* PContainer2DWorkaround::RemoveEntry( uint8_t nSlotId )
{
  PContainerEntry* tEntry = nullptr;

  if ( nSlotId < mContContent->size() )
  {
    tEntry = mContContent->at( nSlotId );
    mContContent->at( nSlotId ) = nullptr;
    //Console->Print(YELLOW, BLACK, "Cleaning (%d,%d) (%d x %d)", tEntry->mPosX, tEntry->mPosY, tEntry->mItem->GetSizeX(), tEntry->mItem->GetSizeY());
    SetUsed( tEntry, false );
  }
  return tEntry;
}

bool PContainer2DWorkaround::GetFreeSlot( uint8_t* nSlotId )
{
  if ( IsSlotAllowed( mNextFreeSlot ) )
  {
    *nSlotId = mNextFreeSlot++;
    return true;
  }
  else
    return false;
}

void PContainer2DWorkaround::SetEntryPosXY( PContainerEntry* nEntry, uint8_t /*nSlotId*/, uint8_t nPosX, uint8_t nPosY )
{
  //nSlotId = nSlotId;
  if (( nPosX == 255 ) && ( nPosY == 255 ) )
  {
    nEntry->Set2DPos( 255, 255 );
  }
  else
  {
    if (( nPosX >= mMaxCols ) || ( nPosY >= mMaxRows ) )
    {
      nPosX = nPosY = 0;
    }
    nEntry->Set2DPos( nPosX, nPosY );

    if ( !FindValid2DPos( nEntry ) )
    {
      nEntry->Set2DPos( 255, 255 );
      Console->Print( RED, BLACK, "[Warning] PContainer2DWorkaround::SetEntryPosXY - Space position already used" );
    }
  }
  mDirtyFlag = mDirtyFlag || nEntry->mDirtyFlag;
}

bool PContainer2DWorkaround::MoveItem( uint8_t srcSlotId, uint8_t nCount, uint8_t dstSlotId )
{
  srcSlotId = nCount = dstSlotId;
  return false;
}

void PContainer2DWorkaround::AddRow()
{
  if ( mRows + 1 < mMaxRows )
  {
    std::vector<bool>* NewRow = new std::vector<bool>( mMaxCols, 0 );
    mContSpace.push_back( NewRow );
    ++mRows;
  }
}

bool PContainer2DWorkaround::Is2DFree( uint8_t PosX, uint8_t PosY, uint8_t SizeX, uint8_t SizeY )
{
  if (( PosX == 255 ) && ( PosY == 255 ) )
  {
    return true;
  }
  if ( !Is2DPosAllowed( PosX, PosY, SizeX, SizeY ) )
    return false;

  uint8_t h, v;
  for ( v = 0; ( v < SizeY ) && ( PosY + v < mRows ) ; v++ ) // what is over existing rows is free
  {
    for ( h = 0; h < SizeX; h++ )
    {
      if ( PosX + h >= mMaxCols ) // what is over max col is not free
        return false;

      if (( *mContSpace[PosY+v] )[PosX+h] )
        return false;
    }
  }
  return ( PosY + SizeY <= mMaxRows );
}

void PContainer2DWorkaround::SetUsed( PContainerEntry* nEntry, bool Value )
{
  uint8_t PosX = nEntry->mPosX;;
  uint8_t PosY = nEntry->mPosY;
  uint8_t SizeX = nEntry->mItem->GetSizeX();
  uint8_t SizeY = nEntry->mItem->GetSizeY();

  if ( !Is2DPosAllowed( PosX, PosY, SizeX, SizeY ) )
    return;

  while ( PosY + SizeY > mRows ) // add new rows when needed
    AddRow();

  uint8_t h, v;
  for ( v = 0; ( v < SizeY ) && ( PosY + v < mRows ) ; v++ )
  {
    for ( h = 0; ( h < SizeX ) && ( PosX + h < mMaxCols ); h++ )
      ( *mContSpace[PosY+v] )[PosX+h] = Value;
  }
}

bool PContainer2DWorkaround::FindValid2DPos( PContainerEntry* nEntry )
{
  bool Found = false;
  uint8_t SizeX = nEntry->mItem->GetSizeX();
  uint8_t SizeY = nEntry->mItem->GetSizeY();
  uint8_t dPosX, dPosY;

  if ( Is2DFree( nEntry->mPosX, nEntry->mPosY, SizeX, SizeY ) )
    Found = true;
  else
  {
    dPosX = nEntry->mPosX;
    dPosY = nEntry->mPosY;
    if ( !Is2DPosAllowed( dPosX, dPosY, SizeX, SizeY ) )
    {
      dPosX = dPosY = 0;
      //Console->Print(YELLOW, BLACK, "Init pos RESET");
    }

    while ( !Found && Is2DPosAllowed( dPosX, dPosY, SizeX, SizeY ) )
    {
      //Console->Print(YELLOW, BLACK, "Searching line %d", dPosY);
      for ( ; dPosX <= ( mMaxCols - SizeX ); ++dPosX )
      {
        if (( Found = Is2DFree( dPosX, dPosY, SizeX, SizeY ) ) )
          break;
      }
      if ( Found )
      {
        nEntry->mPosX = dPosX;
        nEntry->mPosY = dPosY;
        nEntry->mDirtyFlag = true;
        //Console->Print(YELLOW, BLACK, "Success: Found new space position : (%d,%d)", dPosX, dPosY);
        break;
      }
      ++dPosY;
      dPosX = 0;
    }
  }
  return Found;
}

void PContainer2DWorkaround::Dump()
{
  PContainer::Dump();

  std::string tmpS;
  uint8_t y, x;
  for ( y = 0 ; y < mRows; y++ )
  {
    tmpS = "";
    for ( x = 0 ; x < mMaxCols; x++ )
    {
      tmpS += (( *mContSpace[y] )[x] ) ? "X" : ".";
    }
    Console->Print( "%s", tmpS.c_str() );
  }
  Console->Print( GREEN, BLACK, "------------------------------------" );
}

/* --- PContainerAutoFindFree --- */
bool PContainerAutoFindFree::GetFreeSlot( uint8_t* nSlotId )
{
  bool Found = false;

  for ( *nSlotId = 0; *nSlotId < mContContent->size(); ++*nSlotId )
    if ( ! mContContent->at( *nSlotId ) )
    {
      Found = true;
      break;
    }

  return ( Found || IsSlotAllowed( *nSlotId ) );
}

/* --- PContainerAutoCompactOnClose --- */
bool PContainerAutoCompactOnClose::EndUse( uint32_t nExclusiveUseCharID )
{
  if ( nExclusiveUseCharID == mExclusiveUseCharID )
  {
    mExclusiveUseCharID = 0;
    Compact();
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[ERROR] PContainerAutoCompactOnClose::EndUse called with CharID %d when CharID %d had exclusive use", nExclusiveUseCharID, mExclusiveUseCharID );
    return false;
  }
}
