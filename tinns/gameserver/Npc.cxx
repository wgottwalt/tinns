#include <algorithm>
#include <cstring>
#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

void PNPC::PushUpdateTimer()
{
    mNextUpdate = Time::nowTimeT() + Misc::Random::get(NPC_HEARTBEAT_MAX, NPC_HEARTBEAT_MIN);
};

// Reload LUA script while running, in case we modified it and dont want to restart the entire server
bool PNPC::ReloadLUAScript()
{
    // Erase current LUA script
    mLUAFile = "";

    // Reload it
    return LoadLUAScript();
}

bool PNPC::DEF_Load(uint32_t nWorldID)
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] Now loading NPC data for NPC id %d from DEF", mWorldID );
    const PNPCTemplate* t_defNPC = Worlds->GetWorld(nWorldID)->GetNPCTemplate(mWorldID);

    mNameID = (uint16_t)t_defNPC->GetNPCTypeID(); // 16 or 32??
    const PDefNpc* t_NpcTypeDef = GameDefs->Npcs()->GetDef(mNameID);
    if(!t_NpcTypeDef)
    {
        Console->Print("%s [PNPC::DEF_Load()] Unknown NPC Type %d in .dat file found", Console->ColorText(RED,BLACK, "[Error]"), mNameID);
        return false;
    }

    // TODO: Find out what exactly these TypeID and ClothingID values do and where they are generated/read
    // Possible (working) solution: Random seed for name generation that happens clientside
    mTypeID = Misc::Random::get(32767, 1);
    mClothing = Misc::Random::get(32767, 1);
    // -------------

    mPosX = t_defNPC->GetPosX()+32768;
    mPosY = t_defNPC->GetPosY()+32768;
    mPosZ = t_defNPC->GetPosZ()+32768;
    mAngle = atoi( t_defNPC->GetAngle().c_str() );
    mLoot = t_NpcTypeDef->GetLoot();
    mHealth = t_NpcTypeDef->GetHealth() * NPC_HEALTHFACTOR;
    mMaxHealth = mHealth;
    mTrader = t_defNPC->GetTradeID();
    mFaction = t_NpcTypeDef->GetFaction();
    // WorldID Fix 10.10.2009
    mFromDEF = true;

    mName = t_defNPC->GetActorName();


    // Load dialogscript for this NPC right uppon startup
    mDialogScript = t_NpcTypeDef->GetDialogScript();
    Misc::String::cleanUpString(mDialogScript);

    // Try to load any lua scripts
    // Checks are done in target function
    LoadLUAScript();
    if ( gDevDebug ) Console->Print( "[DEBUG] NPC: WID:%d NID:%d TID:%d CL:%d PX:%d PY:%d PZ:%d ", mWorldID, mNameID, mTypeID, mClothing, mPosX, mPosY, mPosZ);
    if ( gDevDebug ) Console->Print( "ANG:%d HITPOINTS:%d TRADE:%d LOOT:%d NAME:%s CNAME:%s CUSTOMSCRIPT:%s", mAngle, mHealth, mTrader, mLoot, mName.c_str(), mCustomName.c_str(), mCustomLua.c_str() );
    if ( gDevDebug ) Console->Print( "DIALOGSCR:%s", mDialogScript.c_str() );
    return true;
 }

bool PNPC::SQL_Load()
{
    if ( gDevDebug ) Console->Print( "[DEBUG] Now loading NPC data for NPC id %d from SQL", mID );
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf( query, 100, "SELECT * FROM `npc_spawns` WHERE `npc_id` = %d", mID );
    if ( gDevDebug ) Console->Print( "[DEBUG] Executing query %s", query );
    result = MySQL->GameResQuery( query );
    if (result == nullptr)
    {
        Console->Print( RED, BLACK, "PNPC::SQL_Load could not load NPC definition" );
        Console->Print( "Query was:" );
        Console->Print( "%s", query );
        MySQL->ShowGameSQLError();
        return false;
    }
    if ( mysql_num_rows( result ) == 0 )
    {
        if ( gDevDebug ) Console->Print( "[DEBUG] No NPCs found, returning false" );
        MySQL->FreeGameSQLResult( result );
        return false;
    }
    else if ( mysql_num_rows( result ) > 1 )
    {
        Console->Print( RED, BLACK, "PNPC::SQL_Load Duplicate entry for NPC %d", mID );
        MySQL->FreeGameSQLResult( result );
        return false;
    }
    if ( gDevDebug ) Console->Print( "[DEBUG] One NPC found for my ID. Now grabbing data..." );
    row = mysql_fetch_row( result );

    mWorldID = atoi( row[npc_worldid] );
    mNameID = atoi( row[npc_nameid] );
    mTypeID = atoi( row[npc_typeid] );
    mClothing = atoi( row[npc_clothing] );
    mPosX = atoi( row[npc_x] );
    mPosY = atoi( row[npc_y] );
    mPosZ = atoi( row[npc_z] );
    mAngle = atoi( row[npc_angle] );
    mLoot = atoi( row[npc_loot] );
    mHealth = atoi( row[npc_unknown] );
    mMaxHealth = mHealth;
    mTrader = atoi( row[npc_trader] );
    mItemQuality = atoi( row[npc_shop_quality] );
    if(atoi(row[npc_scripting]) == 1)
        mScripting = true;
    else
        mScripting = false;

    if (row[npc_name] != nullptr)
        mName = row[npc_name];

    if (row[npc_customname] != nullptr)
        mCustomName = row[npc_customname];

    if (row[npc_customscript] != nullptr)
        mCustomLua = row[npc_customscript];

    // Now make sure we have an valid NPC here. Besides we need some more information
    const PDefNpc* t_npc = GameDefs->Npcs()->GetDef(mNameID);
    if(!t_npc)
    {
        Console->Print( RED, BLACK, "PNPC::SQL_Load Invalid NPC Type found; SQL ID: %d", mID );
        return false;
    }
    mFaction = t_npc->GetFaction();

    // Load dialogscript for this NPC right uppon startup
    // !-> Only if no custom lua script is attached <-!
    if(mCustomLua.length() < 1)
    {
        if(t_npc->GetDialogScript().length() > 3)
        {
            size_t tfound;
            std::string t_dialogscript = t_npc->GetDialogScript();
            std::string t_replacechr ("\"");

            tfound = t_dialogscript.find(t_replacechr);
            while(tfound != std::string::npos)
            {
                t_dialogscript.replace(tfound, 1, " ");
                tfound = t_dialogscript.find( t_replacechr, tfound +1 );
            }
            t_dialogscript = Misc::String::trim(t_dialogscript);
            if(t_dialogscript.length() > 1)
            {
                mDialogScript = t_dialogscript;
            }
        }
    }
    // Try to load any lua scripts
    // Checks are done in target function
    LoadLUAScript();

    if ( gDevDebug ) Console->Print( "[DEBUG] NPC: WID:%d NID:%d TID:%d CL:%d PX:%d PY:%d PZ:%d ", mWorldID, mNameID, mTypeID, mClothing, mPosX, mPosY, mPosZ);
    if ( gDevDebug ) Console->Print( "ANG:%d HITPTS:%d TRADE:%d LOOT:%d NAME:%s CNAME:%s CUSTOMSCRIPT:%s", mAngle, mHealth, mTrader, mLoot, mName.c_str(), mCustomName.c_str(), mCustomLua.c_str() );
    if ( gDevDebug ) Console->Print( "DIALOGSCR:%s", mDialogScript.c_str() );
    MySQL->FreeGameSQLResult( result );
    return true;
}

bool PNPC::LoadLUAScript()
{
    uint32_t tFileLen = 0;
    PFile* fLua = nullptr;
    std::string tLuaFile = "";
    std::string tHDRFile = "";

    // Load LUA script and include the correct header file
    // based in mDialogScript
    // if mDialogScript is empty, look at mCustomLua and load this file. Standart include header for custom
    // script files is dialogheader.lua since we dont know (yet) what standart header the NC Client uses
    // for custom loaded scripts.

    if(mDialogScript.length() > 1)
    {
        // Get LUA filename from defs
        const PDefScripts* tDefScripts = nullptr;
        std::map<int, PDefScripts*>::const_iterator itScrStart = GameDefs->Scripts()->ConstIteratorBegin();
        std::map<int, PDefScripts*>::const_iterator itScrEnd = GameDefs->Scripts()->ConstIteratorEnd();
        for ( std::map<int, PDefScripts*>::const_iterator i = itScrStart; i != itScrEnd; i++ )
        {
            tDefScripts = i->second;
    //        Console->Print("[DEBUG PNPC::LoadLUAScript] Identifier: [%s] LUA: [%s]", tDefScripts->GetIdentifier().c_str(), tDefScripts->GetLuaFile().c_str());

            if(tDefScripts->GetIdentifier().compare(mDialogScript) == 0)
            {
                break;
            }
        }
        // If we left the loop without an positive match.. false!
        if(tDefScripts->GetIdentifier().compare(mDialogScript) != 0)
        {
            return false;
        }

        // Assign our LUA file to load later
        tLuaFile = tDefScripts->GetLuaFile();
        // MaKe ThE sTrInG aLl lowercase! :)
        std::transform(tLuaFile.begin(), tLuaFile.end(), tLuaFile.begin(), (int(*)(int))std::tolower);


        // We only have 2 headerfiles, so this one is hardcoded
        if(tDefScripts->GetScriptHeader() == "DIALOGHEADER")
        {
            tHDRFile = "scripts/lua/dialogheader.lua";
        }
        else if(tDefScripts->GetScriptHeader() == "MISSIONHEADER")
        {
            tHDRFile = "scripts/lua/missionheader.lua";
        }
    }
    // Customlua is set?
    else if(mCustomLua.length() > 1)
    {
        // Assign lua file and header
        tLuaFile = mCustomLua;
        tHDRFile = "scripts/lua/dialogheader.lua";
    }
    else
    {
        // No LUA Scripts to load. Skipping
        return true;
    }

    // Load HEADER file
    fLua = Filesystem->Open( "", tHDRFile.c_str(), Config->GetOption( "nc_data_path" ) );
    if(fLua)
    {
        tFileLen = fLua->GetSize();
        char* t_content = new char[tFileLen+1];
        memset(t_content, '\0', tFileLen+1);

        fLua->Read( t_content, tFileLen );
        Filesystem->Close( fLua );
        mLUAFile = t_content;
        delete t_content;
        if (gDevDebug) Console->Print( "%s [PNPC::LoadLUAScript] Loaded LUA Header Script %s", Console->ColorText( GREEN, BLACK, "[SUCCESS]" ), tHDRFile.c_str() );
    }
    else
    {
        Console->Print( "%s [PNPC::LoadLUAScript] Unable to load LUA Header script %s NPC is now DISABLED for scripting", Console->ColorText( RED, BLACK, "[ERROR]" ), tLuaFile.c_str() );
        // We encountered an error while loading. Make sure this NPC will never act as Dialog NPC!
        mLUAFile = "";
        mCustomLua = "";
        mDialogScript = "";
        return false;
    }

    // Reset vars
    tFileLen = 0;
    fLua = nullptr;

    fLua = Filesystem->Open( "", tLuaFile.c_str(), Config->GetOption( "nc_data_path" ) );
    if(fLua)
    {
        tFileLen = fLua->GetSize();
        char* t_content = new char[tFileLen+1];
        memset(t_content, '\0', tFileLen+1);

        fLua->Read( t_content, tFileLen );
        Filesystem->Close( fLua );
        mLUAFile += t_content;  // APPEND the script to our existing lua headerfile
        delete t_content;
        if (gDevDebug) Console->Print( "%s [PNPC::LoadLUAScript] Loaded LUA Script %s", Console->ColorText( GREEN, BLACK, "[SUCCESS]" ), tLuaFile.c_str() );
        //Console->Print( "%s", mLUAFile.c_str() );
    }
    else
    {
        Console->Print( "%s [PNPC::LoadLUAScript] Unable to load LUA Script %s NPC is now DISABLED for scripting", Console->ColorText( RED, BLACK, "[ERROR]" ), tLuaFile.c_str() );
        // We encountered an error while loading. Make sure this NPC will never act as Dialog NPC!
        mLUAFile = "";
        mCustomLua = "";
        mDialogScript = "";
        return false;
    }
    // LUA file prepared. Check if LUA file is valid
    if(LuaEngine->CheckLUAFile(mLUAFile) == true)
    {
        // Everything is fine. NPC is ready for action
        return true;
    }
    else
    {
        // LUA file seems to be corrupt
        mLUAFile = "";
        mCustomLua = "";
        mDialogScript = "";
        return false;
    }
}

 void PNPC::Die()
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] NPC dying now" );
    mHealth = 0;
    mAction = NPC_ACTIONSTATE_DEATH;
    mRespawn = Time::nowTimeT() + NPC_RESPAWN_AFTER;
    mDirty = true;
 }

 void PNPC::Update()
{
    // Has to be changed for mobs later
    if (Time::nowTimeT() >= mRespawn && (mAction&NPC_ACTIONSTATE_DEATH))
    {
        if ( gDevDebug ) Console->Print( "[DEBUG] NPC Update: Respawn timer triggered! Setting NPC back to life" );
        mHealth = mMaxHealth;
        mAction = NPC_ACTIONSTATE_IDLE;
        mWeaponStatus = NPC_SHOOT_IDLE;
        mDirty = true;
    }
}

void PNPC::InitVars()
 {
    mID = 0;
    mWorldID = 0;
    mNameID = 0;
    mTypeID = 0;
    mClothing = 0;
    mPosX = 0;
    mPosY = 0;
    mPosZ = 0;
    mAngle = 0;
    mHealth = 0;
    mUnknown = 0;
    mTrader = 0;
    mLoot = 0;
    mDialogScript = "";
    mName = "";
    mCustomName = "";
    mCustomLua = "";
    mAction = NPC_ACTIONSTATE_IDLE;
    mWeaponStatus = NPC_SHOOT_IDLE;
    //mDeath = false;
    mTarget = 0;
    mDirty = false; // No need to send instand update
    // WorldID Fix 10.10.2009
    mFromDEF = false;
    mItemQuality = 50;
    mScripting = true;
    mFaction = 0;

    // Set next update timer for this NPC to 10 - 30 seconds
    // Note: this is for regular heartbeats only. If npc is dirty,
    // an update is sent anyway
    mNextUpdate = Time::nowTimeT() + Misc::Random::get(30, 10);
 }

void PNPC::Attack( uint32_t nWorldID, uint8_t nType, uint8_t nUnknown )
{
    mDirty = true;
    mTarget = nWorldID;
    mAction = NPC_ACTIONSTATE_ATTACK;
    mWeaponStatus = nType;
    mUnknown = nUnknown;
}

 PNPC::PNPC( int nSQLID )
 {
    InitVars();
//if(gDevDebug) Console->Print("[DEBUG] New NPC instance created. ID is %d", nSQLID);
    mID = nSQLID;
    if ( SQL_Load() == false )  // Try to load NPC contents
        mSuccess = false;
    else
        mSuccess = true;
}

PNPC::PNPC( int nDEFID, uint32_t nWorldID )
{
    InitVars();
//if(gDevDebug) Console->Print("[DEBUG] New NPC instance created. ID is %d", nSQLID);
    mWorldID = nDEFID;
    if ( DEF_Load(nWorldID) == false )  // Try to load NPC contents
        mSuccess = false;
    else
        mSuccess = true;
 }

 PNPC::~PNPC()
 {
//if(gDevDebug) Console->Print("[DEBUG] NPC ID %d terminated", mID);
 }
/*
 uint8_t PNPC::GetActionStatus()
 {
    if ( mDeath == true )
    {
        return 128; // 128 triggers "death" animation
    }
    else
    {
        return 3; // 3 is the value found in many packets. However, no idea what this does
    }
 }
*/
 ///***********************************************************************
 ///***********************************************************************

// Broadcast a single NPC
void PNPCWorld::BroadcastNewNPC(PNPC* nNpc)
{
    std::string tAngleStr = std::to_string(nNpc->mAngle);
    PMessage* tmpMsg = MsgBuilder->BuildNPCMassInfoMsg (nNpc->mWorldID, nNpc->mTypeID, nNpc->mClothing, nNpc->mNameID, nNpc->mPosY,
                                                        nNpc->mPosZ, nNpc->mPosX, nNpc->mHealth, nNpc->mTrader, &tAngleStr,
                                                        &nNpc->mName, &nNpc->mCustomName);

    ClientManager->UDPBroadcast(tmpMsg, mWorldID);
}


bool PNPCWorld::AddNPC(uint32_t nSQL_ID, uint32_t nRaw_ID)
{
    PNPC* tmpNpc = new PNPC( nSQL_ID );
    if(tmpNpc->mSuccess == true)
    {
        // Now broadcast the new NPC to all clients
        BroadcastNewNPC(tmpNpc);
        mNPCs.insert( std::make_pair( nRaw_ID, tmpNpc ) );
        tmpNpc = nullptr;
        if ( gDevDebug ) Console->Print( "[PNPCWorld::AddNPC] Custom NPC added" );
    }
    else
    {
        if ( gDevDebug ) Console->Print( "[PNPCWorld::AddNPC] Custom NPC not added due error" );
        delete tmpNpc;
        return false;
    }

    return true;
}

void PNPCWorld::DelNPC(uint32_t nWorldID)
{
    PNPCMap::iterator it = mNPCs.find( nWorldID );
    if ( it == mNPCs.end() )
        return;

    // Delete NPC from Map
    mNPCs.erase(it);

    // Send Vanish message to clients
    PMessage* tmpMsg = MsgBuilder->BuildRemoveWorldObjectMsg(nWorldID);
    ClientManager->UDPBroadcast( tmpMsg, mWorldID );

    return;
}

void PNPCWorld::SendSingleNPCInfo( PClient* nClient, PNPC* nNpc )
{
    std::string tAngleStr = std::to_string(nNpc->mAngle);
    PMessage* tmpMsg = MsgBuilder->BuildNPCSingleInfoMsg (nClient, nNpc->GetRealWorldID(), nNpc->mTypeID, nNpc->mClothing, nNpc->mNameID, nNpc->mPosY,
                                                        nNpc->mPosZ, nNpc->mPosX, nNpc->mHealth, nNpc->mTrader, &tAngleStr,
                                                        &nNpc->mName, &nNpc->mCustomName);

    nClient->SendUDPMessage(tmpMsg);
    return;
}

 void PNPCWorld::MSG_SendNPCs( PClient* nClient )
{
    PNPC* nNpc = nullptr;
    for ( PNPCMap::iterator it = mNPCs.begin(); it != mNPCs.end(); it++ )
    {
        nNpc = it->second;

        std::string tAngleStr = std::to_string(nNpc->mAngle);
        PMessage* tmpMsg = MsgBuilder->BuildNPCSingleInfoMsg (nClient, nNpc->GetRealWorldID(), nNpc->mTypeID, nNpc->mClothing, nNpc->mNameID, nNpc->mPosY,
                                                            nNpc->mPosZ, nNpc->mPosX, nNpc->mHealth, nNpc->mTrader, &tAngleStr,
                                                            &nNpc->mName, &nNpc->mCustomName);

        nClient->SendUDPMessage( tmpMsg );
    }

    return;
}

bool PNPCWorld::LoadNPCfromSQL()
 {
// Load NPC defs from MySQL
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf( query, 100, "SELECT * FROM `npc_spawns` WHERE `npc_location` = %d", mWorldID );
//if(gDevDebug) Console->Print("[DEBUG] Query is: %s", query);
    result = MySQL->GameResQuery( query );
    if (result == nullptr)
     {
        Console->Print( RED, BLACK, "PNPCWorld::PNPCWorld could not load NPC definition" );
        Console->Print( "Query was:" );
        Console->Print( "%s", query );
        MySQL->ShowGameSQLError();
        return false;
     }
    if ( mysql_num_rows( result ) == 0 ) // No NPCs found
     {
        MySQL->FreeGameSQLResult( result );
//if(gDevDebug) Console->Print("[NPC] No NPCs for this world found");
        return true;
     }
//if(gDevDebug) Console->Print("[DEBUG] Found NPCs, now adding!");
    PNPC* tmpNpc = nullptr;
    uint32_t tRawID = 0;
    int tSQLID = 0;
    while (( row = mysql_fetch_row( result ) ) )
     {
        tRawID = atoi( row[PNPC::npc_worldid] );
        tSQLID = atoi( row[PNPC::npc_id] );
        tmpNpc = new PNPC( tSQLID );
        if ( tmpNpc->mSuccess == true )
        {
            //if(gDevDebug) Console->Print("[DEBUG] NPC init successfull, adding to list");
            mNPCs.insert( std::make_pair( tRawID, tmpNpc ) );
            tmpNpc = nullptr;
        }
        else
        {
            //if(gDevDebug) Console->Print("[DEBUG] NPC init failed, removing link");
            delete tmpNpc;
        }
     }
    if ( gDevDebug ) Console->Print( "[DEBUG] NPC Load from MySQL done" );
    MySQL->FreeGameSQLResult( result );
    return true;
 }

bool PNPCWorld::LoadNPCfromDEF()
 {
    const PNPCsMap* tNPCmap = Worlds->GetWorld(mWorldID)->GetNPCMap(); // Get the NPC Map for this world

    PNPC* tmpNpc = nullptr;
    uint32_t tDEFID = 0;

    for ( PNPCsMap::const_iterator i = tNPCmap->begin(); i != tNPCmap->end(); i++ )
     {
// call PNPC with NPC ID and WorldID
        tDEFID = i->first;
        tmpNpc = new PNPC( tDEFID, mWorldID );
        if ( tmpNpc->mSuccess == true )
         {
            mNPCs.insert( std::make_pair( tDEFID, tmpNpc ) );
            tmpNpc = nullptr;
         }
         else
         {
            delete tmpNpc;
         }
     }
    if ( gDevDebug ) Console->Print( "[DEBUG] NPC Load from .def done" );
    return true;
 }

 PNPCWorld::PNPCWorld( uint32_t nWorldID )
 {
//if(gDevDebug) Console->Print("[DEBUG] New world got initialized! Now starting to add NPCs. (WorldID %d)", nWorldID);

// Assign WorldValues now
    mCreation = Time::nowTimeT();
    mWorldID = nWorldID;
    mLastAliveMsg = 0;
    LoadNPCfromSQL();
    LoadNPCfromDEF();
 }

 PNPCWorld::~PNPCWorld()
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] Erasing all NPCs" );
// Erase all NPCs
     for ( PNPCMap::iterator it = mNPCs.begin(); it != mNPCs.end(); it++ )
     {
        delete it->second;
        mNPCs.erase( it );
     }
}

void PNPCWorld::Update() // v2; New send function
{
    // Updates NPC in a World.
    // If NPC is dirty, send "large" update. Else
    // send small "i'm alive" message
    time_t tNow = Time::nowTimeT();
    PNPC* tNPC = nullptr;
    for ( PNPCMap::iterator it = mNPCs.begin(); it != mNPCs.end(); it++ )
    {
        if ( it->second )
        {
            tNPC = it->second;
            // Check for enemies nearby
            CheckForEnemies(tNPC);
            // Let NPC make themselfs "dirty"
            tNPC->Update();
            // Only update dirty npcs
            if ( tNPC->mDirty == true || tNPC->mNextUpdate <= tNow)
            {
                PMessage* tmpMsg = MsgBuilder->BuildNPCUpdateMsg(tNPC->GetRealWorldID(),
                                                                 tNPC->mPosY,
                                                                 tNPC->mPosZ,
                                                                 tNPC->mPosX,
                                                                 tNPC->GetActionStatus(),
                                                                 tNPC->mHealth,
                                                                 tNPC->GetWeaponStatus(),
                                                                 tNPC->mUnknown,
                                                                 tNPC->mTarget);

                ClientManager->UDPBroadcast( tmpMsg, mWorldID );
                tNPC->mDirty = false;
                tNPC->PushUpdateTimer();
            }
        }
    }

    return;
 }

 PNPC* PNPCWorld::GetNPC( uint32_t nNPCID )
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] Searching for NPC %d in list", nNPCID );
    PNPCMap::const_iterator it = mNPCs.find( nNPCID );
    if ( it == mNPCs.end() ) // only if client not found in list
        return nullptr;
    else
        return it->second;
 }

 ///***********************************************************************

 PNPCManager::PNPCManager()
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] NPCManager wakeup!" );
// Nothing yet
 }

 PNPCManager::~PNPCManager()
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] NPCManager shutdown" );
    for ( PNPCWorldMap::iterator it = mWorlds.begin(); it != mWorlds.end(); it++ )
    {
        delete it->second;
        mWorlds.erase( it );
    }
 }

 void PNPCManager::InitPlayer( PClient* nClient )
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] Player entered zone, doing init" );
// Ok, player entered zone. First, get zone!
    uint32_t nZone = nClient->GetChar()->GetLocation();
    if ( gDevDebug ) Console->Print( "[DEBUG] Now searching for zone %d in list", nZone );
// Search worldmanager for this zone
    PNPCWorld* tmpWorld = GetWorld( nZone );

    if (tmpWorld != nullptr)
     {
        if ( gDevDebug ) Console->Print( "[DEBUG] World found, poking MSG_SendNPCs" );
// World found? Fine. Then poke the class to send its content to the client
        tmpWorld->MSG_SendNPCs( nClient );
     }
    else
    {
        if ( gDevDebug ) Console->Print( "[DEBUG] World not found, creating...." );
// World not found! OMG! Create it!
        tmpWorld = InitWorld( nZone );
        if (tmpWorld == nullptr)
        {
            Console->Print( "%s Unable to init NPCs for world %d", Console->ColorText( RED, BLACK, "[ERROR]" ), nZone );
            return;
        }
        if ( gDevDebug ) Console->Print( "[DEBUG] Done. Poking MSG_SendNPCs" );
// now we have the world, poke it to send its content
        tmpWorld->MSG_SendNPCs( nClient );
        //tmpWorld->MSG_SendAlive( nClient ); // Force instand-update of NPCs for this client
    }
 }

 PNPCWorld* PNPCManager::InitWorld( uint32_t nWorldID )
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] InitWorld triggered: ID %d", nWorldID );
    PNPCWorld* tmpWorld = nullptr;
    tmpWorld = new PNPCWorld( nWorldID );
    if (tmpWorld != nullptr)
    {
        if ( gDevDebug ) Console->Print( "[DEBUG] World created. Adding to list..." );
        mWorlds.insert( std::make_pair( nWorldID, tmpWorld ) );
        if ( gDevDebug ) Console->Print( "[DEBUG] ... and returning instance" );
        return tmpWorld;
    }
    else
    {
        if ( gDevDebug ) Console->Print( "[DEBUG] Failed to init world. Returning NULL" );
        return nullptr;
    }
 }

 void PNPCManager::Update()
 {
    static time_t lastdebug = Time::nowTimeT();
// Loop all worlds
 //    if(lastdebug < Time::nowTimeT())
 //        if(gDevDebug) Console->Print("[DEBUG] WorldLoop still running...");

    for ( PNPCWorldMap::iterator it = mWorlds.begin(); it != mWorlds.end(); it++ )
     {
// Make sure target still exists
        if ( it->second )
        {
            // Getworld, and check if zone is in use
            PNPCWorld* tWorld = it->second;
            if ( ClientManager->IsWorldInUse( tWorld->mWorldID ) == true )
            {
                //if(lastdebug < Time::nowTimeT())
                //    if(gDevDebug) Console->Print("[DEBUG] World is in use, poking management class to update itself");
                // World is in use. Now send "dirty" npcs or alive messages
                tWorld->Update();
            }
            else
            {
                //if(lastdebug < Time::nowTimeT())
                //    if(gDevDebug) Console->Print("[DEBUG] World not in use. Checking lifetimer...");

                if ((tWorld->mCreation + ZONE_RESET_AFTER ) <= Time::nowTimeT())
                    //if(tWorld->mCreation <= Time::nowTimeT())
                {
                    if ( gDevDebug ) Console->Print( "[DEBUG] World reached ZONE_RESET timeout. Erasing..." );
                    // World is unused sine ZONE_RESET_AFTER minutes, deleting now
                    delete it->second;
                    mWorlds.erase( it );
                }
                else
                {
                    // Keep LastAliveMessage up to date until someone enters zone
                    tWorld->mLastAliveMsg = Time::nowTimeT();
                    //  if(lastdebug < Time::nowTimeT())
                    //      if(gDevDebug) Console->Print("[DEBUG] World still within ZONE_RESET timeout");
                }
            }
         }
     }
    if (lastdebug < Time::nowTimeT())
    {
        lastdebug = Time::nowTimeT() + 3;
//if(gDevDebug) Console->Print("[DEBUG] next updateloopmsg in 3 seconds");
    }
 }

 PNPCWorld* PNPCManager::GetWorld( uint32_t nWorldID )
 {
    if ( gDevDebug ) Console->Print( "[DEBUG] Trying to get instance for worldid %d", nWorldID );
    PNPCWorldMap::const_iterator it = mWorlds.find( nWorldID );
    if ( it == mWorlds.end() )
    {
        if ( gDevDebug ) Console->Print( "[DEBUG] Not found, returning NULL" );
        return nullptr;
    }
    else
    {
        if ( gDevDebug ) Console->Print( "[DEBUG] Found. Returning address" );
        return it->second;
    }
 }
