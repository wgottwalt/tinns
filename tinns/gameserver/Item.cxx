#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"

PItem::PItem(uint32_t ItemID, uint8_t nStackSize, uint8_t CurDur, uint8_t MaxDur, uint8_t Dmg, uint8_t Freq, uint8_t Hand, uint8_t Rng)
{
  mDefItem = GameDefs->Items()->GetDef(ItemID);
  if (mDefItem) {
    mItemID = ItemID;
    mStackable = mDefItem->IsStackable();
    mStackSize = mStackable ? nStackSize : 1;

    mLoadedAmmoId = 0;
    mLoadedAmmoNb = 0;

    mPropertiesFlags = 0;

    mUsedSlots = mMaxSlots = 0;
    mModificators = 0;

    mConstructorId = 0;
    //mType = mDefItem->GetType();
    //mValue1 = mDefItem->GetValue1();
    //mValue2 = mDefItem->GetValue2();
    //mValue3 = mDefItem->GetValue3();
    //mSizeX = mDefItem->GetSizeX();
    //mSizeY = mDefItem->GetSizeY();
    //mWeight = mDefItem->GetWeight();
    //mFillWeight = mDefItem->GetFillWeight();
    //mQualifier = mDefItem->GetQualifier();
    //mItemGroupID = mDefItem->GetItemGroupID();
    //mBasePrice = mDefItem->GetBasePrice();
    //mTechlevel = mDefItem->GetTechlevel();
    //mItemflags = mDefItem->GetItemflags();

    //if (!mStackable) // only non-stackable items can have quality stats (?)
    //{
    //  mStackSize = 1;
      mCurDuration = CurDur;
      mMaxDuration = MaxDur;
      mDamages = Dmg;
      mFrequency = Freq;
      mHandling = Hand;
      mRange = Rng;
    //}
  }
  else
  {
    mItemID = 0;
    Console->Print(YELLOW, BLACK, "PItem::PItem: Invalid item ID %d", ItemID);
  }
}

void PItem::MakeItemStandard(uint8_t GlobalQualityMin, uint8_t GlobalQualityMax)
{
  if(GlobalQualityMin > GlobalQualityMax) GlobalQualityMin = GlobalQualityMax;

  mCurDuration = 255;
  mMaxDuration = 255;
  if(GlobalQualityMin == GlobalQualityMax)
  {
    mDamages = GlobalQualityMin;
    mFrequency = GlobalQualityMin;
    mHandling = GlobalQualityMin;
    mRange = GlobalQualityMin;
  }
  else
  {
    mDamages = (uint8_t) GetRandom(GlobalQualityMax, GlobalQualityMin);
    mFrequency = (uint8_t) GetRandom(GlobalQualityMax, GlobalQualityMin);
    mHandling = (uint8_t) GetRandom(GlobalQualityMax, GlobalQualityMin);
    mRange = (uint8_t) GetRandom(GlobalQualityMax, GlobalQualityMin);
  }
}

uint32_t PItem::GetItemID()
{
    return mItemID;
}

int PItem::GetType()
{
    return mDefItem->GetType();
}

uint8_t PItem::GetItemflags()
{
    return mDefItem->GetItemflags();
}

const std::string &PItem::GetName() const
{
    return mDefItem->GetName();
}

uint8_t PItem::GetSizeX()
{
    return mDefItem->GetSizeX();
}

uint8_t PItem::GetSizeY()
{
    return mDefItem->GetSizeY();
}

float PItem::GetWeight()
{
    return mStackSize * mDefItem->GetWeight();
}

float PItem::GetSingleUnitWeight()
{
    return mDefItem->GetWeight();
}

float PItem::GetFillWeight()
{
    return mDefItem->GetFillWeight();
}

uint32_t PItem::GetBasePrice()
{
    return mDefItem->GetBasePrice();
}

uint16_t PItem::GetTechlevel()
{
    return mDefItem->GetTechlevel();
}

int PItem::GetValue1()
{
    return mDefItem->GetValue1();
}

int PItem::GetValue2()
{
    return mDefItem->GetValue2();
}

int PItem::GetValue3()
{
    return mDefItem->GetValue3();
}

int PItem::GetQualifier()
{
    return mDefItem->GetQualifier();
}

bool PItem::IsStackable()
{
    return mDefItem->IsStackable();
}

uint8_t PItem::GetStackSize()
{
    return mStackSize;
}

uint8_t PItem::AddToStack(uint8_t ItemNb) // returns nb of items not added
{
  uint8_t addedItems = 0;
  if (mStackable)
  {
    addedItems = (ItemNb <= (MAX_ITEMSTACK_SIZE - mStackSize)) ? ItemNb : (MAX_ITEMSTACK_SIZE - mStackSize);
    mStackSize += addedItems;
  }
  return (ItemNb - addedItems);
}

uint8_t PItem::TakeFromStack(uint8_t ItemNb)
{
  uint8_t retreivedItems = 0;
  if (mStackable)
  {
    retreivedItems = std::min(mStackSize, ItemNb);
    mStackSize -= retreivedItems;
  }
  return retreivedItems;
}
