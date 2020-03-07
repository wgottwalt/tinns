#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include "gameserver/Inventory.hxx" // FIXME: class design fault

#define MAXCASH 1000000000

class PGenrepList;
class PSkillHandler;
class PVhcAccessRequestList;

enum PSeatType {
  seat_none = 0,
  seat_chair,
  seat_subway,
  seat_vhc
};

class PCharCoordinates {
  public:
    uint16_t mY;     // Y-Position in world // Y increases when going East
    uint16_t mZ;     // Z-Position in world // Z increases when going up
    uint16_t mX;     // X-Position in world // X increases when going South
    uint8_t mUD;     // Up - Mid - Down (d6 - 80 - 2a)
    uint8_t mLR;     // Compass direction (S..E..N..W..S [0-45-90-135-179])
    uint8_t mAct;    // Last user action state
    uint8_t mUnknown;// sometime sent by client with value != 0 (usual case)
    // mAct:
    // 0x00 NC has no focus (player alt+tab'ed out)
    // 0x04 Char on ground/dead   00000100
    // 0x20 Char does nothing     00100000
    // 0x22 kneeing               00100010
    // 0x28 left step             00101000
    // 0x30 right step            00110000
    // 0x40 walking (not running) 01000000 // Seems to mean Running ? - to be verfied, with default walk/run mode !!!
    // 0x60 forward               01100000
    // 0xA0 backward              10100000
    // bits:                      BFWRL.K.

    uint8_t mJumpingState;

    //inline PCharCoordinates() { mX = mY = mZ = mUD = mLR = mAct = mUnknown = mJumpingState = 0;}
    void SetPosition( uint16_t nY, uint16_t nZ, uint16_t nX, uint8_t nUD = 0x80, uint8_t nLR = 0 );
    void SetInterpolate( PCharCoordinates& Pos1, PCharCoordinates& Pos2, float nCoef );

    //Temp
    uint16_t minPos[3];
    uint16_t maxPos[3];
    inline PCharCoordinates() { mX = mY = mZ = mUD = mLR = mAct = mUnknown = mJumpingState = 0; for(int i=0; i<3; ++i) { minPos[i] = 0xffff; maxPos[i] = 0; } }
};

class PChar
{
  private :
    // static members
    static RegEx* mCharnameRegexFilter;

    // instance members
    uint32_t mID;
    uint32_t mAccount;
    uint8_t mSlot;
    std::string mName;
    uint32_t mGender;
    uint32_t mClass;
    //uint32_t mType; // Removed that and only keep GetType()
    uint32_t mProfession;
    uint32_t mFaction;
    uint32_t mRealHead;  // Base Skin elements, in complement of (computed) mType
    uint32_t mRealTorso; // " Head shouldn't be changeable, except in case of surgery !!!
    uint32_t mRealLegs;  // "
    uint32_t mSkin;  // Current Skin elements. *** Not saved in DB atm ***
    uint32_t mHead;  // "
    uint32_t mTorso; // "
    uint32_t mLegs;  // "
    uint8_t mHeadColor;  // "
    uint8_t mTorsoColor; // "
    uint8_t mLegsColor;  // "
    uint8_t mHeadDarkness;  // " // 0=Bright, 255=dark
    uint8_t mTorsoDarkness; // "
    uint8_t mLegsDarkness;  // "
    // Skin scale factor setting remain to discover, provided they are somewhere for player chars ...

    bool mLocationLeased; // temp until char on-demand load/unload
    uint32_t mLocation;
    uint32_t mCash;
    uint32_t mStartApt; // set same as PrimaryApt atm
    uint32_t mPrimaryApt;

    // not saved in DB atm
    PSeatType mSeatInUseType;
    uint32_t mSeatInUseObjectId;
    uint8_t mSeatInUseSeatId;
    PVhcAccessRequestList* mVhcAccessRequestList;

    PContainer* mContainerInExclusiveUse;

    uint16_t mHealth;
    uint16_t mMana;
    uint16_t mStamina;

    int8_t mSoullight;
    uint8_t mCombatRank; // *** Not got/saved from DB atm ***
    uint8_t mSynaptic; // *** Not got/saved from DB atm ***
    bool mIsDead; // *** Not got/saved from DB atm ***

    // Only one body effect supported atm. Should be extended later to multiple effects
    uint8_t mBodyEffect; // *** Not got/saved from DB atm ***
    uint8_t mBodyEffectDensity; // *** Not got/saved from DB atm ***

    uint8_t mSpeedOverride; // a hack to control move speed. Not saved in DB

    uint32_t mDirectCharID; // for Direct Chat // *** Not got/saved from DB atm ***
    PBuddyList* mBuddyList; // For Buddy list Chat
    uint32_t mActiveChatChannels; // Active chat channels flags // *** Not got/saved from DB atm ***

    PGenrepList* mGenrepList; // Character's GR list

    uint8_t mQuickBeltActiveSlot; // QB SlotID of item "in hand", or INV_WORN_QB_HAND or INV_WORN_QB_NONE

    uint16_t mLookingAt;  // Zone charID of currently targeted player
    time_t mLookAtTimestamp; // Lifetimer of lookat var
    uint32_t mLastUsedWorldObjectId; // Last world object clicked on

    uint8_t mClanLevel; // 1-15
    uint16_t mClanID;

    bool mIsOnline;
    bool mDirtyFlag;

    bool mShunned;
    bool mJailed;

    uint32_t mDialogNPC; // NPCID the player talks to
    uint16_t mCurrentDialogNode; // Node in .lua file we're at right now

    class PInventory mInventory;

  protected :
    friend class PChars;
    inline void SetID( uint32_t ID ) { mID = ID; }
    inline void SetAccount( uint32_t Account ) { mAccount = Account; }
    inline void SetCharSlot( uint8_t Slot ) { if ( Slot < 4 ) mSlot = Slot;} // TODO: set max slot according to server config
    inline void SetName( const std::string &Name ) { mName = Name; }
    inline void SetGender( uint32_t Gender ) { mGender = Gender; }
    void SetProfession( uint32_t Profession );
    //inline void SetClass(uint32_t nClass) { mClass = nClass; } // mClass is defined by setting Profession
    //inline void SetType(uint32_t Type) { mType = Type; } // Removed. Type is computed from Gender & Profession (??? is it not Gender + Class ???)
    inline void SetFaction( uint32_t Faction ) { mFaction = Faction; }
    //inline void SetModel(uint32_t Model) { mModel = Model; } // Inhibited for the moment. Base model is deduced from from Gender & Class (Profession)
    void SetRealLook( uint32_t nHead, uint32_t nTorso, uint32_t nLegs );
    void SetBaseSkills();
    void SetBaseSubskills( uint8_t NZSNb, const char* NonZeroSubskills );
    void SetBaseInventory();

    bool SQLCreate();

  public :
    PChar();
    ~PChar();

    static bool SetCharnameRegexFilter( const char* RegexStr );
    static bool IsCharnameWellFormed( const char *Username );

    PSkillHandler *Skill;
    PCharCoordinates Coords;

    void SetCurrentLook( uint32_t nSkin, uint32_t nHead = 0, uint32_t nTorso = 0, uint32_t nLegs = 0 );
    void SetCurrentLookFromCharType( uint32_t nType );
    void ResetCurrentLook();

    void SetCurrentBodyColor( uint8_t nHeadColor, uint8_t nTorsoColor, uint8_t nLegsColor, uint8_t nHeadDarkness = 0, uint8_t nTorsoDarkness = 0, uint8_t nLegsDarkness = 0 );
    inline void SetBodyEffect( uint8_t nEffect, uint8_t nDensity = 0 ) { mBodyEffect = nEffect; mBodyEffectDensity = nDensity; }
    inline void SetSpeedOverride( uint8_t nSpeed = 255 ) { mSpeedOverride = nSpeed; }

    void SetLookingAt( uint16_t nLocalCharID );
    uint16_t GetLookingAt( uint16_t nMaxDelaySec = 1 );
    inline void SetLastUsedObject ( uint32_t nRawItemId ) { mLastUsedWorldObjectId = nRawItemId; }
    inline uint32_t GetLastUsedObject () const { return mLastUsedWorldObjectId; }

    inline PInventory* GetInventory() { return &mInventory; }
    inline uint32_t GetID() const { return mID; }
    inline uint32_t GetAccount() const { return mAccount; }
    inline const std::string &GetName() const { return mName; }
    inline uint32_t GetGender() const { return mGender; }
    inline uint32_t GetClass() const { return mClass; }
    inline uint32_t GetType() const { return 2 * mClass + mGender; }
    uint32_t GetSkinFromCharType( uint32_t nType );
    void GetRealLook( uint32_t &nSkin, uint32_t &nHead, uint32_t &nTorso, uint32_t &nLegs );
    void GetCurrentLook( uint32_t &nSkin, uint32_t &nHead, uint32_t &nTorso, uint32_t &nLegs );
    inline void GetBodyEffect( uint8_t &nEffect, uint8_t &nDensity ) { nEffect = mBodyEffect; nDensity = mBodyEffectDensity; }

    inline uint8_t GetQuickBeltActiveSlot() { return mQuickBeltActiveSlot; }
    bool SetQuickBeltActiveSlot( uint8_t nSlotID );

    void GetCurrentBodyColor( uint8_t &nHeadColor, uint8_t &nTorsoColor, uint8_t &nLegsColor, uint8_t &nHeadDarkness, uint8_t &nTorsoDarkness, uint8_t &nLegsDarkness );
    inline uint8_t GetSpeedOverride() { return mSpeedOverride; }
    inline uint32_t GetBaseModel();
    inline uint32_t GetProfession() const { return mProfession; }
    inline uint16_t GetMaxHealth() { return mHealth; }
    inline uint16_t GetMaxMana() { return mMana; }
    inline uint16_t GetMaxStamina() { return mStamina; }
    inline uint16_t GetHealth() { return mHealth; }
    inline uint16_t GetMana() { return mMana; }
    inline uint16_t GetStamina() { return mStamina; }
    inline uint32_t GetFaction() const { return mFaction; }
    inline uint32_t GetLocation() const { return mLocation; }

    inline uint32_t GetCash() const { return mCash; }
    uint32_t SetCash( uint32_t nCash );  // Does return the new cashvalue, NO udpmessage is sent out!!
    uint32_t AddCash( uint32_t nAmount );
    uint32_t TakeCash( uint32_t nAmount );

    inline uint32_t GetBaseApartment() const { return mPrimaryApt; }

    inline void SetJail( bool val ) { mJailed = val; };
    inline void SetShun( bool val ) { mShunned = val; };

    inline bool IsJailed() { return mJailed; };
    inline bool IsShunned() { return mShunned; };

    inline void SetDialogNPC( uint32_t nNPC ) { mDialogNPC = nNPC; };
    inline uint32_t GetDialogNPC() const { return mDialogNPC; };

    inline void SetDialogNode( uint16_t nNode ) { mCurrentDialogNode = nNode; };
    inline uint16_t GetDialogNode() const { return mCurrentDialogNode; };

    inline uint8_t GetClanLevel() const { return mClanLevel; };
    inline uint16_t GetClan() const { return mClanID; };
    void LoadClanLevel();

    inline int8_t GetSoullight() const { return mSoullight; }
    uint8_t GetMainRank();
    uint8_t GetCombatRank();
    inline uint8_t GetSynaptic() const { return mSynaptic; }
    inline bool IsDead() const { return mIsDead; }

    inline bool SetDirectChat( uint32_t nBuddyCharID ) { mDirectCharID = nBuddyCharID; return true; }
    inline uint32_t GetDirectChat() { return mDirectCharID; }
    inline void SetActiveChannels( uint32_t nChannels ) { mActiveChatChannels = nChannels; }
    inline uint32_t GetActiveChannels() { return mActiveChatChannels; }

    inline bool AddBuddy( uint32_t nBuddyCharID ) { return mBuddyList->AddChar( nBuddyCharID ); }
    inline bool RemoveBuddy( uint32_t nBuddyCharID ) { return mBuddyList->RemoveChar( nBuddyCharID ); }
    inline uint16_t GetBuddyListDataSize() { return mBuddyList->GetListDataSize(); }
    inline const void* GetBuddyListData() { return mBuddyList->GetListData(); }
    inline uint8_t GetBuddyCount() { return mBuddyList->Count(); }
    inline bool IsBuddy( uint32_t CharID ) { return mBuddyList->IsInBuddy( CharID ); };

    bool AddGenrep(uint16_t nWorldID, uint16_t nStationID);
    uint16_t GetGenrepListDataSize();
    const void *GetGenrepListData();
    uint8_t GetGenrepCount();

    inline bool IsDirty() const { return mDirtyFlag; }
    inline bool IsAnyDirty() const { return mDirtyFlag || mInventory.IsDirty(); }
    inline bool IsOnline() { return mIsOnline; }
    void SetOnlineStatus( bool IsOnline );

    bool CreateNewChar( uint32_t Account, const std::string &Name, uint32_t Gender, uint32_t Profession, uint32_t Faction,
                        uint32_t Head, uint32_t Torso, uint32_t Legs, uint8_t NZSNb, const char *NonZeroSubskills, uint32_t Slot );
    bool SQLLoad( int CharID );
    bool SQLSave();
    inline bool SQLSaveFull() { return SQLSave() && mInventory.SQLSave(); }
    bool SQLDelete(); // not implemented yet

    inline void SetLocation( uint32_t Location ) { mLocation = Location; }
    inline void SetDirtyFlag( bool Dirty = true ) { mDirtyFlag = Dirty; }

    // temp until char on-demand load/unload
    inline void SetLocationLeased( bool nState = true ) { mLocationLeased = nState; }
    inline bool GetLocationLeased() { return mLocationLeased; }

    PSeatType GetSeatInUse(uint32_t* nObjectId = nullptr, uint8_t* nSeatId = nullptr);
    void SetSeatInUse(PSeatType nSeatType, uint32_t nObjectId = 0, uint8_t nSeatId = 0);

    PVhcAccessRequestList* GetVhcAccessRequestList();

    inline PContainer* GetContainerInExclusiveUse() { return mContainerInExclusiveUse; }
    inline void SetContainerInExclusiveUse( PContainer* nContainer ) { mContainerInExclusiveUse = nContainer; }
};

struct PCharProfile
{
  uint32_t CharID;
  uint16_t Type;
  uint16_t Color0;
  uint16_t Unknown1;
  uint8_t Head;
  uint8_t Torso;
  uint8_t Legs;
  uint32_t Location;
  uint8_t NameLen;
  uint8_t Unknown3;
  uint8_t Unknown4;
  uint8_t Unknown5;
  uint8_t Unknown6;
  uint8_t Unknown7;
  uint8_t Unknown8;
  uint8_t Unknown9;
  uint8_t Unknown10;
  uint8_t Unknown11;
  uint8_t Unknown12;
  std::string Name;
  bool in_use;
};

class PChars
{
  private :
    typedef std::map<uint32_t, PChar*> CharMap;
    CharMap mChars;
    uint32_t mLastID;

    time_t mAutoSavePeriod;
    time_t mLastSave;

  public :
    PChars();
    ~PChars();

    bool LoadChar( uint32_t CharID );
    bool AddChar( PChar* nChar );
    PChar* RemoveChar( uint32_t CharID );

    PChar* GetChar( uint32_t CharID ) const;
    PChar* GetChar( const std::string &Name ) const;
    bool CharExist( const std::string &Name ) const;

    void SQLSave();
    void Update();

    int GetCharProfiles( const uint32_t AccountID, PCharProfile* CharSlotsArray, const uint8_t ArraySize ); // return effective entries nb
};
