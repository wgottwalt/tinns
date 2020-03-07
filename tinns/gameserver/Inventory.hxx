#pragma once

#include <cstdint>

//NC containers message locations
#define INV_LOC_GROUND 1
#define INV_LOC_WORN 2
#define INV_LOC_BACKPACK 3
#define INV_LOC_BOX 4
#define INV_LOC_NPCTRADE 5
#define INV_LOC_GOGO 18
#define INV_LOC_BOX2 255
//PC-Trade window = ?

//Inventory containers info
#define INV_WORN_QB_START 0
#define INV_WORN_QB_END 9
#define INV_WORN_QB_NONE 99
#define INV_WORN_QB_HAND 11

#define INV_WORN_PROC_START 12
#define INV_WORN_PROC_END 24

#define INV_WORN_IMP_START 26
#define INV_WORN_IMP_END 38

#define INV_WORN_ARMOR_START 39
#define INV_WORN_ARMOR_END 43

#define INV_WORN_COLS 44
#define INV_WORN_MAXSLOTS 44


#define INV_BACKPACK_COLS 10

#define INV_GOGO_COLS 5
#define INV_GOGO_MAXSLOTS 50

#define INV_CABINET_COLS 5
#define INV_CABINET_MAXSLOTS 33

// inv_loc values in database
#define INV_DB_LOC_GOGO 1
#define INV_DB_LOC_WORN 2
#define INV_DB_LOC_BACKPACK 3

class PItem;
class PContainer;
class PContainerWithHoles;
class PContainer2DWorkaround;
class PContainerAutoFindFree;

class PInventory
{
  private:
    PContainerWithHoles* mWorn; // PContainerLinearSlots
    PContainer2DWorkaround* mBackpack; // PContainer2DAreas
    PContainerAutoFindFree* mGogo; // PContainerLinearSlots

  public:

    PInventory();
    ~PInventory();

    void SetCharId(uint32_t CharID);
    bool SQLLoad();
    bool SQLSave();
    PContainer* GetContainer(uint8_t nInvLoc);
    inline PContainer2DWorkaround* GetBackpackContainer() { return mBackpack; }

    bool IsDirty() const;

    bool AddItem(PItem* NewItem, uint8_t nInvLoc = INV_LOC_BACKPACK, uint32_t nInvID = 0, uint8_t nPosX = 0, uint8_t nPosY = 0, bool SetDirty = true);
    //bool CheckItem(uint32_t ItemID, uint8_t StackSize = 1);
    //PItem *GetItem(uint32_t ItemID, uint8_t StackSize = 1);
    //PItem *GetItemByPos(uint8_t nPosX, uint8_t nPosY, uint8_t StackSize = 1);
    //bool MoveItem(uint8_t oPosX, uint8_t oPosY, uint8_t dPosX, uint8_t dPosY);

    //bool QB_IsFree(uint8_t nSlot);
    //void QB_SetSlot(uint8_t nSlot, uint16_t nItemID);
    //uint16_t QB_GetSlot(uint8_t nSlot);
    //void QB_Move(uint8_t nSlotSRC, uint8_t nSlotDST);
};
