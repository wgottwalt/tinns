#pragma once

#include <cstdint>
#include <map>
#include "gameserver/definitions/Definition.hxx"

class PDefItems : public PDef {
private:
    //int32_t mIndex;//1
    std::string mName;//2
    int32_t mModel; // used for IG display
    int32_t mType;
    int32_t mValue1;
    int32_t mValue2;
    int32_t mValue3;
    // int32_t mBmNum; // used IG for inventory display
    // int32_t mmBmNumIndex; // used IG for inventory display
    int32_t mSizeX;
    int32_t mSizeY;
    // int32_t mSmallbmnum; // used IG for inventory display
    float mWeight;
    int32_t mStackable;
    float mFillWeight;
    int32_t mQualifier;
    int32_t mGfxMods;
    int32_t mItemGroupID;
    int32_t mTextDescID;
    int32_t mBasePrice;
    int32_t mTechlevel;
    int32_t mItemflags;
    // std::mShortname; // used IG for display on inventories Icons

public:
    PDefItems();
    //~PDefItems();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetModel() const { return mModel; }
    inline int32_t GetType() const { return mType; }
    inline int32_t GetValue1() const { return mValue1; }
    inline int32_t GetValue2() const { return mValue2; }
    inline int32_t GetValue3() const { return mValue3; }
    inline int32_t GetSizeX() const { return mSizeX; }
    inline int32_t GetSizeY() const { return mSizeY; }
    inline float GetWeight() const { return mWeight; }
    inline bool IsStackable() const { return (mStackable == 1); }
    inline float GetFillWeight() const { return mFillWeight; }
    inline int32_t GetQualifier() const { return mQualifier; }
    inline int32_t GetGfxMods() const { return mGfxMods; }
    inline int32_t GetItemGroupID() const { return mItemGroupID; }
    inline int32_t GetTextDescID() const { return mTextDescID; }
    inline int32_t GetBasePrice() const { return mBasePrice; }
    inline int32_t GetTechlevel() const { return mTechlevel; }
    inline int32_t GetItemflags() const { return mItemflags; }
};

class PDefItemsMap : public PDefMap<PDefItems> {
private:
    std::map<int32_t, PDefItems*>::const_iterator* mMapItCache;
    int32_t mMapItCacheCount;
    std::map<int32_t, std::vector<int32_t> > mItemGroups;
    int32_t mMaxItemGroupId;
    void BuildMapItCache();
    void BuildItemGroups();

public:
    PDefItemsMap();
    ~PDefItemsMap();
    bool Load(const char* nName, const char* nFilename);
    const PDefItems* GetDefBySeqIndex( int32_t nSeqIndex ) const;
    int32_t GetRandomItemIdFromGroup( int32_t nGroupId ) const;

    inline std::map<int32_t, PDefItems*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int32_t, PDefItems*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};
