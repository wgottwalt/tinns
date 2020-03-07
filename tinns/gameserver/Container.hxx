#pragma once

#include <cstdint>
#include <vector>
#ifdef MYSQL_INC_DIR
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

#define CONTAINER_MAX_SIZE 254

class PItem;

class PContainerEntry {
  friend class PContainer;
  friend class PContainer2D;
  friend class PMsgBuilder;
  friend class PContainer2DWorkaround;

private:
    enum {
      i_invid = 0,
      i_charid,
      i_invloc,
      i_x,
      i_y,
      i_itemid,
      //i_type,
      i_flag,
      i_qty,
      i_sqty,
      i_curdur,
      i_dmg,
      i_freq,
      i_hand,
      i_rng,
      i_maxdur,
      i_slots,
      i_slt1,
      i_slt2,
      i_slt3,
      i_slt4,
      i_slt5,
      i_atype,
      i_conatin
    };

    PItem* mItem;
    uint8_t  mPosX;
    uint8_t  mPosY;
    uint32_t mInvID;
    bool mDirtyFlag;

    PContainerEntry(PItem* nItem, uint8_t X, uint8_t Y, uint32_t nInvID = 0, bool SetDirty = true);
    PContainerEntry(MYSQL_ROW row);

    bool SQLSave(uint32_t CharID, uint32_t InvLoc);
    bool SQLDelete();

    inline void Set2DPos(uint8_t nPosX, uint8_t nPosY) { mDirtyFlag = mDirtyFlag || (mPosX != nPosX) || (mPosY != nPosY) ; mPosX = nPosX; mPosY = nPosY; }

public:
    ~PContainerEntry();

    inline void Get2DPos(uint8_t* oPosX, uint8_t* oPosY) { *oPosX = mPosX; *oPosY = mPosY; }
};



class PContainer // Holes allowed, no autofind free slots
{
    protected:
    uint8_t mMaxSlots;
    std::vector< PContainerEntry* >* mContContent;
    uint32_t mCharID;
    uint32_t mInvLoc;
    uint32_t mExclusiveUseCharID;
    bool mDirtyFlag;

    inline bool IsSlotAllowed(uint8_t nSlotId) { return ((nSlotId < CONTAINER_MAX_SIZE) && (!mMaxSlots || (nSlotId < mMaxSlots))); }
    virtual bool AddEntry(PContainerEntry* NewEntry, uint8_t nSlotId = 0);
    virtual PContainerEntry* RemoveEntry(uint8_t nSlotId);
    virtual bool GetFreeSlot(uint8_t* nSlotId);
        void Compact(uint8_t startSlotId = 0);

  public:
    PContainer(uint8_t nMaxSlots = 0);
    virtual ~PContainer();

    inline void SetInfo(uint32_t CharID, uint32_t InvLoc) { mCharID = CharID; mInvLoc = InvLoc; }
    inline uint32_t GetOwnerId() { return mCharID; }
    inline bool IsDirty() { return mDirtyFlag; }
    inline void SetDirty() { mDirtyFlag = true; }
    bool StartUse(uint32_t nExclusiveUseCharID = 0);
    virtual bool EndUse(uint32_t nExclusiveUseCharID = 0);
    bool SQLLoad();
    bool SQLSave();
    bool IsSlotFree(uint8_t nSlotId);
    virtual bool AddItem(PItem* NewItem, uint32_t nInvID = 0, uint8_t nPosX = 0, uint8_t nPosY = 0, bool SetDirty = true);
    virtual bool MoveItem(uint8_t srcSlotId, uint8_t nCount, uint8_t dstSlotId);
    bool MoveItem(uint8_t srcSlotId, uint8_t nCount, PContainer* dstContainer, uint8_t dstSlotId = 0, uint8_t nPosX = 0, uint8_t nPosY = 0);
      virtual void SetEntryPosXY(PContainerEntry* nEntry, uint8_t nSlotId, uint8_t nPosX = 0, uint8_t nPosY = 0);

    virtual uint8_t RandomFill(uint8_t nItemCount = 0, int nItemContainerDefIndex = -1);
    PContainerEntry* GetEntry(uint8_t nSlotId);
    std::vector< PContainerEntry* >* GetEntries();
    PItem* GetItem(uint8_t nSlotId);
    virtual void Dump();
};


class PContainerWithHoles : public PContainer // Holes allowed, no autofind free slots
{
  public:
    PContainerWithHoles(uint8_t nMaxSlots = 0) : PContainer(nMaxSlots) {}
    virtual ~PContainerWithHoles() {}
};


class PContainerAutoCompact : public PContainer // No holes allowed, automatic add to end slot (no control on insertion slot)
{
protected:
    virtual PContainerEntry* RemoveEntry(uint8_t nSlotId);
    virtual bool GetFreeSlot(uint8_t* nSlotId);

public:
    PContainerAutoCompact(uint8_t nMaxSlots = 0) : PContainer(nMaxSlots) {}
    virtual ~PContainerAutoCompact() {}
    virtual bool MoveItem(uint8_t srcSlotId, uint8_t nCount, uint8_t dstSlotId);
};


class PContainer2D : public PContainerAutoCompact // + slotId not used, non-significant XY used (no XY check yet)
{
  public:
    PContainer2D(uint8_t nMaxSlots = 0) : PContainerAutoCompact(nMaxSlots) {}
    virtual ~PContainer2D() {}

        virtual void SetEntryPosXY(PContainerEntry* nEntry, uint8_t nSlotId, uint8_t nPosX = 0, uint8_t nPosY = 0);
};

class PContainer2DWorkaround : public PContainerWithHoles // Holes allowed, autofind free slot (always increasing id)
{
private:
    uint8_t mNextFreeSlot;
    std::vector< std::vector<bool>* > mContSpace;
    uint8_t mMaxCols;
    uint8_t mMaxRows;
    uint8_t mRows;

    void AddRow();
    inline bool Is2DPosAllowed(uint8_t PosX, uint8_t PosY, uint8_t SizeX, uint8_t SizeY)
    {
      return ((PosX < mMaxCols-SizeX+1) && (PosY < mMaxRows-SizeY+1));
    }
    bool Is2DFree(uint8_t PosX, uint8_t PosY, uint8_t SizeX, uint8_t SizeY);
    bool FindValid2DPos(PContainerEntry* nEntry);

protected:
    bool AddEntry(PContainerEntry* NewEntry, uint8_t nSlotId = 0);
    PContainerEntry* RemoveEntry(uint8_t nSlotId);
    bool GetFreeSlot(uint8_t* nSlotId);

  public:
    PContainer2DWorkaround(uint8_t nMaxSlots = 0);
    ~PContainer2DWorkaround();

    bool MoveItem(uint8_t srcSlotId, uint8_t nCount, uint8_t dstSlotId);

    void Set2DPosMax(uint8_t MaxPosX, uint8_t MaxPosY = 254) { mMaxCols = MaxPosX; mMaxRows = MaxPosY; }
    void SetEntryPosXY(PContainerEntry* nEntry, uint8_t nSlotId, uint8_t nPosX = 0, uint8_t nPosY = 0);
    void SetUsed(PContainerEntry* nEntry, bool Value = true);
    void Dump();
};

class PContainerAutoFindFree : public PContainerWithHoles // No holes kept after EndUse, automatic find first free slots (no control on insertion slot)
{
  protected:
    virtual bool GetFreeSlot(uint8_t* nSlotId);

  public:
    PContainerAutoFindFree(uint8_t nMaxSlots = 0) : PContainerWithHoles(nMaxSlots) {}
    virtual ~PContainerAutoFindFree() {}
};

class PContainerAutoCompactOnClose : public PContainerAutoFindFree // No holes kept after EndUse, automatic find first free slots (no control on insertion slot)
{
  public:
    PContainerAutoCompactOnClose(uint8_t nMaxSlots = 0) : PContainerAutoFindFree(nMaxSlots) {}
    virtual ~PContainerAutoCompactOnClose() {}

    virtual bool EndUse(uint32_t nExclusiveUseCharID = 0);
};
