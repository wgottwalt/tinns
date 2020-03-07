#pragma once

#include <cstdint>
#include <string>

class PDefItems;

// TODO: Add CreatorID (for "named" item), CurrentMunitionID, CurrentMunitionNb (for weapons & charge-items: muns,
//        medkit, etc.) to DB, equipped/free slots, & corresponding code

#define MAX_ITEMSTACK_SIZE 250

//Type
#define ITEM_TYPE_VARIOUS 0
#define ITEM_TYPE_WEAPON 1
#define ITEM_TYPE_AMMO 2
#define ITEM_TYPE_HEALTH 3
#define ITEM_TYPE_IMPLANT 4
#define ITEM_TYPE_DRUG 5
#define ITEM_TYPE_MOD 6
#define ITEM_TYPE_GFXMOD 7
#define ITEM_TYPE_BLUEPRINT 8
#define ITEM_TYPE_ARMOR 9
#define ITEM_TYPE_PSIMOD 10
#define ITEM_TYPE_PSIMODREADY 11
#define ITEM_TYPE_REPAIR 12
#define ITEM_TYPE_RECYCLER 13
#define ITEM_TYPE_DATACUBE 14
#define ITEM_TYPE_CONSTRUCTOR 15
#define ITEM_TYPE_RESEARCHER 16
#define ITEM_TYPE_IMPLANTER 17
#define ITEM_TYPE_APARTMENTKEY 18
#define ITEM_TYPE_CLANKEY 19
#define ITEM_TYPE_CASHCUBE 20
#define ITEM_TYPE_AUTOWEAPON 21
#define ITEM_TYPE_VHCKEY 22
#define ITEM_TYPE_UNIDENTPART 24
#define ITEM_TYPE_WRECKEDPART 25
#define ITEM_TYPE_SALVAGE 26
#define ITEM_TYPE_VHCCOMPONENT 27
#define ITEM_TYPE_RECORDABLE 28

// gfxmodflags
#define ITEM_MOD_FLASHLIGHT 1
#define ITEM_MOD_SCOP 2
#define ITEM_MOD_SILENCER 4
#define ITEM_MOD_LASERPOINTER 8

// itemflags:
#define ITEM_FLAG_RESEARCHABLE 1
#define ITEM_FLAG_NO_DROP 2
#define ITEM_FLAG_NO_MAX_REPAIRE_DECAY 4
#define ITEM_FLAG_AMMO 8 // for loadable ammo
// not sure for ITEM_FLAG_AMMO

class PItem {
  friend class PContainerEntry;
  friend class PMsgBuilder;

  private:
    uint32_t mItemID;
    const PDefItems* mDefItem;

    bool mStackable;
    uint8_t mStackSize;

    uint32_t mLoadedAmmoId;
    uint8_t mLoadedAmmoNb;

    uint8_t mPropertiesFlags;

    uint8_t mCurDuration;
    uint8_t mMaxDuration;
    uint8_t mDamages;
    uint8_t mFrequency;
    uint8_t mHandling;
    uint8_t mRange;

    uint8_t mUsedSlots;
    uint8_t mMaxSlots;
    uint8_t mSlot[5];
    uint8_t mModificators;

    uint32_t mConstructorId;

  public:
    PItem(uint32_t ItemID, uint8_t nStackSize = 1, uint8_t CurDur = 0, uint8_t MaxDur = 0, uint8_t Dmg = 0, uint8_t Freq = 0, uint8_t Hand = 0, uint8_t Rng = 0);
    //~PItem();
    void MakeItemStandard(uint8_t GlobalQualityMin = 120, uint8_t GlobalQualityMax = 180);

    uint32_t GetItemID();
    int GetType();
    uint8_t GetItemflags();
    const std::string &GetName() const;
    uint8_t GetSizeX();
    uint8_t GetSizeY();
    float GetWeight();
    float GetSingleUnitWeight();
    float GetFillWeight();
    uint32_t GetBasePrice();
    uint16_t GetTechlevel();
    int GetValue1();
    int GetValue2();
    int GetValue3();
    int GetQualifier();
    bool IsStackable();
    uint8_t GetStackSize();
    uint8_t AddToStack(uint8_t ItemNb); // return the nb of items NOT added
    uint8_t TakeFromStack(uint8_t ItemNb); // return the nb of retreived items

    //mItemGroupID = def->GetItemGroupID();
};
