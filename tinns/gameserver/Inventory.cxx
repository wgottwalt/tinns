#include "gameserver/Includes.hxx"

/* --- PInventory class --- */

PInventory::PInventory ()
{
  mWorn = new PContainerWithHoles(INV_WORN_MAXSLOTS);
  //mBackpack = new PContainer2D();
  mBackpack = new PContainer2DWorkaround(); // until Inside-Backpack item moving issues are solved
  mBackpack->Set2DPosMax(INV_BACKPACK_COLS);
  mGogo = new PContainerAutoFindFree(INV_GOGO_MAXSLOTS);
}

PInventory::~PInventory ()
{
  delete mWorn;
  delete mBackpack;
  delete mGogo;
}

void PInventory::SetCharId(uint32_t CharID)
{
  mWorn->SetInfo(CharID, INV_DB_LOC_WORN);
  mBackpack->SetInfo(CharID, INV_DB_LOC_BACKPACK);
  mGogo->SetInfo(CharID, INV_DB_LOC_GOGO);
}

bool PInventory::SQLLoad()
{
  return ( mWorn->SQLLoad() && mBackpack->SQLLoad() && mGogo->SQLLoad() );

/*bool ret = mWorn->SQLLoad() && mBackpack->SQLLoad() && mGogo->SQLLoad();
Console->Print(YELLOW, BLACK, "--- Worn Inventory ---");
mWorn->Dump();
Console->Print(YELLOW, BLACK, "--- Backpack Inventory ---");
mBackpack->Dump();
Console->Print(YELLOW, BLACK, "--- Gogo Inventory ---");
mGogo->Dump();
Console->Print(YELLOW, BLACK, "--- End Inventory ---");
return ret;*/
}

bool PInventory::SQLSave()
{
  return mWorn->SQLSave() && mBackpack->SQLSave() && mGogo->SQLSave();
}

bool PInventory::IsDirty() const
{
  return (mWorn && mWorn->IsDirty()) || (mBackpack && mBackpack->IsDirty()) || (mGogo && mGogo->IsDirty());
}

PContainer* PInventory::GetContainer(uint8_t nInvLoc)
{
  PContainer* tContainer;
  switch(nInvLoc)
  {
    case INV_LOC_WORN:
      tContainer = mWorn;
      break;
    case INV_LOC_BACKPACK:
      tContainer = mBackpack;
      break;
    case INV_LOC_GOGO:
      tContainer = mGogo;
      break;
    default:
      tContainer = nullptr;
  }
  return tContainer;
}

bool PInventory::AddItem(PItem *NewItem, uint8_t nInvLoc, uint32_t nInvID, uint8_t nPosX, uint8_t nPosY, bool SetDirty)
{
  PContainer* destContainer = GetContainer(nInvLoc);
  return(destContainer ? destContainer->AddItem(NewItem, nInvID, nPosX, nPosY, SetDirty) : false);
}

/*
bool PInventory::QB_IsFree(uint8_t nSlot)
{
    if(nSlot > 9)
        return false;

    //    if (gDevDebug) Console->Print("Checking QBSlot %d. Content: %d", nSlot, mQuickAccessBelt[nSlot]);
    if(mQuickAccessBelt[nSlot] == 0)
        return true;
    else
        return false;
}

void PInventory::QB_SetSlot(uint8_t nSlot, uint16_t nItemID)
{
    if(nSlot > 9)
        return;
    //    if (gDevDebug) Console->Print("Setting QBSlot %d. Newcontent: %d Oldcontent: %d", nSlot, nItemID, mQuickAccessBelt[nSlot]);
    mQuickAccessBelt[nSlot] = nItemID;
}

uint16_t PInventory::QB_GetSlot(uint8_t nSlot)
{
    if(nSlot > 9)
        return 0;
    //    if (gDevDebug) Console->Print("Getting QBSlot %d. Content: %d", nSlot, mQuickAccessBelt[nSlot]);
    return mQuickAccessBelt[nSlot];
}

void PInventory::QB_Move(uint8_t nSlotSRC, uint8_t nSlotDST)
{
    if(nSlotSRC > 9 || nSlotDST > 9)
        return;

    //    if (gDevDebug) Console->Print("Moving QBSlot %d [%d] to %d [%d]", nSlotSRC, mQuickAccessBelt[nSlotSRC], nSlotDST, mQuickAccessBelt[nSlotDST]);
    mQuickAccessBelt[nSlotDST] = mQuickAccessBelt[nSlotSRC];
    mQuickAccessBelt[nSlotSRC] = 0;
    //    if (gDevDebug) Console->Print("Moving done. %d [%d] %d [%d]", nSlotSRC, mQuickAccessBelt[nSlotSRC], nSlotDST, mQuickAccessBelt[nSlotDST]);
}
*/
