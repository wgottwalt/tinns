#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <vector>
#include <string>

// Healthfactor for NPCs (see old npc.def)
#define NPC_HEALTHFACTOR 15

// Minimum time in seconds that has to pass before an NPC
// gets his heartbeat send
#define NPC_HEARTBEAT_MIN 5
// Maximum time in seconds that is allowed to pass without
// an NPC heartbeat
#define NPC_HEARTBEAT_MAX 20

// If no custom NPC is set in this Zone, what ID to start with?
#define NEW_NPC_ZONEID_START 1000

// How many seconds have to pass until the zone gets reset?
// Reset in: NPCs and PWorld object gets deleted and reloaded when
// next player syncs to that zone.
#define ZONE_RESET_AFTER 600

// How many seconds have to pass until the NPC is "revived"
#define NPC_RESPAWN_AFTER 300

// How many seconds have to pass until we need an NPC "keepalive" packet?
#define NPC_ALIVE_MSG 15

// How often a NPC should check if an enemy is nearby?
#define NPC_ENEMYCHECK 5

#define NPC_ACTIONSTATE_SITGND  0x00
#define NPC_ACTIONSTATE_ATTACK  0x01
//#define NPC_ACTIONSTATE_?       0x02
//#define NPC_ACTIONSTATE_?       0x04
//#define NPC_ACTIONSTATE_?       0x08
#define NPC_ACTIONSTATE_KNEEL   0x10
#define NPC_ACTIONSTATE_PASSIVE 0x20
#define NPC_ACTIONSTATE_IDLE    0x40
#define NPC_ACTIONSTATE_DEATH   0x80

#define NPC_SHOOT_IDLE      15
#define NPC_SHOOT_SINGLE    16
#define NPC_SHOOT_AUTO1     17
#define NPC_SHOOT_AUTO2     18

class PNPC;
class PNPCWorld;

typedef std::map<uint32_t, PNPC*> PNPCMap;
typedef std::map<uint32_t, PNPCWorld*> PNPCWorldMap;

typedef struct
{
    uint16_t ItemID;
    uint32_t Price;
} stShopListEntry;

class PNPC
{
private:
    // SQL layout
    enum
    {
        npc_id,
        npc_worldid,
        npc_nameid,
        npc_typeid,
        npc_name,
        npc_location,
        npc_x,
        npc_y,
        npc_z,
        npc_angle,
        npc_clothing,
        npc_loot,
        npc_unknown,
        npc_trader,  // trader.def entry, or clan/faction data!
        npc_customname,
        npc_customscript,
        npc_shop_quality,
        npc_scripting
    };

    time_t mNextUpdate;    // Timestamp for next heartbeat
    time_t mNextEnemyCheck; // Timestamp for next enemycheck
    void PushUpdateTimer();

    // SQL values
    uint32_t mID;
    uint32_t mWorldID;
    uint16_t mNameID;
    uint16_t mTypeID;
    uint16_t mClothing;
    uint16_t mPosX;
    uint16_t mPosY;
    uint16_t mPosZ;
    int8_t mAngle;
    uint16_t mLoot;
    uint16_t mTrader;
    uint8_t mItemQuality; // Used for Shopping stuff
    uint8_t mUnknown;

    std::string mDialogScript;
    std::string mLUAFile; // Load File; Preloaded uppon NPC creation

    std::vector<stShopListEntry> mVectItemsInShop;    // We need to keep track of the itemorder for shopping
    void AddToVectorList(uint16_t nItemID, uint32_t nPrice);
    inline const stShopListEntry* GetItemNum(uint32_t nIdx) const { if(nIdx > mVectItemsInShop.size()) { return nullptr; } else { return &mVectItemsInShop[nIdx]; }};

    bool mScripting;    // Manual override to disable scripting for an NPC TRUE: Scripts will be executed FALSE: Scripts will be ignored

    std::string mName;
    std::string mCustomName;
    std::string mCustomLua;

    time_t mRespawn;    // Respawn timer

    // Runtime values
    //bool mDeath;        // Death...
    uint8_t mFaction;        // NPC's faction

    uint16_t mHealth;         // NPC Current Health-Value
    uint16_t mMaxHealth;      // NPC Max Health value
    uint32_t mTarget;        // Current focused player
    bool mDirty;        // Needs update to clients

    // WorldID Fix 10.10.2009
    bool mFromDEF;      // to differ DEF NPCs from SQL NPCs
    bool mSuccess;      // NPC load successfull?


     uint8_t mAction;         // Current action
    inline uint8_t GetActionStatus() const { return mAction; };
    // 00000001 (  1) 0x01: Attack-Mode (Depends on WeaponStatus)
    // 00000010 (  2) 0x02: ?
    // 00000100 (  4) 0x04: ?
    // 00001000 (  8) 0x08: ?
    // 00010000 ( 16) 0x10: kneel
    // 00100000 ( 32) 0x20: Passive-Mode  (Depends on WeaponStatus. Difference between 0x01: NPC does NOT open fire)
    // 01000000 ( 64) 0x40: Idle
    // 10000000 (128) 0x80: Die

    uint8_t mWeaponStatus;
    inline uint8_t GetWeaponStatus() const { return mWeaponStatus; };
    // 00001111 (15) 0x0F: Follow given target with eyes / Put weapon away if pulled
    // 00010000 (16) 0x10: Pull weapon if not pulled / If pulled, attack
    // 00010001 (17) 0x11: Pull weapon and attack


    bool SQL_Load();
    bool DEF_Load(uint32_t nWorldID);

     PNPC( int nSQLID );
    PNPC( int nDEFID, uint32_t nWorldID );
     ~PNPC();

    void InitVars();
    void ContentListAddItem(PMessage* nContentList, uint16_t nItemID, uint32_t nBasePrice = 0, bool nAddToList = true);
    void ContentListAddItemGroup(PMessage* nContentList, uint32_t nItemGroupID);
    void StartDialog( PClient* nClient/*, string &nDialogscript */);

    bool DoSQLShoppingList( PClient* nClient, PMessage* nContentList );
    bool HasSQLShoppingList( PClient* nClient );
    bool IsAllbuyer( PClient* nClient );
    bool LoadLUAScript();

    inline uint32_t GetRealWorldID() { if(mFromDEF == true) return mWorldID+255; else return mWorldID; };

public:
    friend class PNPCWorld;

    inline void StopAttack() { mDirty = true; mAction = NPC_ACTIONSTATE_IDLE; mWeaponStatus = NPC_SHOOT_IDLE; };
    inline void Attack( PClient* nClient, uint8_t nType = NPC_SHOOT_SINGLE, uint8_t nUnknown = 90 ) { Attack(nClient->GetChar()->GetID(), nType, nUnknown); };
    void Attack( uint32_t nWorldID, uint8_t nType = NPC_SHOOT_SINGLE, uint8_t nUnknown = 90 );

    inline void Move( uint16_t nNewX, uint16_t nNewY, uint16_t nNewZ )
    {
        mPosX = nNewX;
        mPosY = nNewY;
        mPosZ = nNewZ;
        mDirty = true;
    }

    void Die(); // ... die?
    void Update(); // Check respawn timer
    void StartConversation( PClient* nClient );
    void DoConversation( PClient* nClient, uint8_t nAnswer ) ;

    // GameCommands
    bool ReloadLUAScript();
    bool ReloadShopList();
    bool SetShopQuality(uint8_t nNewVal);
    inline bool IsSQLNPC() const { return !mFromDEF; };
    inline int GetNPCID() const { return mWorldID; };
    inline int GetNPCSQLID() const { return mID; };
    inline void SetTrader( uint16_t nTraderDef ) { mTrader = nTraderDef; };
    inline void SetScripting(bool nVal) { mScripting = nVal; };
    inline uint8_t GetFaction() const { return mFaction; };
};

// *****************************************

class PNPCWorld {
private:
    time_t mCreation;  // Creation time. (Required to check zone-reset timer
    time_t mLastAliveMsg;  // Time of last "ping" message to keep NPCs in world

    PNPCMap mNPCs;
    PNPCMap::iterator GetNPCListBegin()
    {
        return mNPCs.begin();
    }
    PNPCMap::iterator GetNPCListEnd()
    {
        return mNPCs.end();
    }

    bool mSuccessfullInit;

    uint32_t mWorldID;

    PNPCWorld( uint32_t nWorldID );
    ~PNPCWorld();

    void Update();

    // Send all NPCs to one player (Initial zone setup)
    void MSG_SendNPCs( PClient* nClient );

    // Send "alive" message for all NPCs as zone broadcast to everyone
    //void MSG_SendAlive();

    bool LoadNPCfromSQL();
    bool LoadNPCfromDEF();

    void BroadcastNewNPC(PNPC* nNpc);
    void CheckForEnemies(PNPC* nNPC);

public:
    friend class PNPCManager;
     PNPC* GetNPC( uint32_t nNPCID );

    // Functions to add/remove an NPC while server is running
    void SendSingleNPCInfo( PClient* nClient, PNPC* nNpc ); // Send
    bool AddNPC(uint32_t nSQL_ID, uint32_t nRaw_ID); // Load single SQL NPC from given SQL ID
    void DelNPC(uint32_t nWorldID); // Remove given NPC from list. Works for *all* npcs
                                                     // but uppon zone reset they're back.
};

// *****************************************

class PNPCManager
{
private:
    PNPCWorldMap mWorlds;
    PNPCWorldMap::iterator GetWorldListBegin()
    {
        return mWorlds.begin();
    }
    PNPCWorldMap::iterator GetWorldListEnd()
    {
        return mWorlds.end();
    }

public:
    PNPCManager();
    ~PNPCManager();

    void Update();
    PNPCWorld* InitWorld( uint32_t nWorldID );

    PNPCWorld* GetWorld( uint32_t nWorldID );
    void InitPlayer( PClient* nClient ); // Player is entering zone
};
