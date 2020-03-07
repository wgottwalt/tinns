#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"
#include "gameserver/definitions/Implants.hxx"
#include "gameserver/definitions/ItemContainer.hxx"
#include "gameserver/definitions/ItemMod.hxx"
#include "gameserver/definitions/ItemRes.hxx"
#include "gameserver/definitions/Items.hxx"
#include "gameserver/definitions/Mission.hxx"
#include "gameserver/definitions/NpcArmor.hxx"
#include "gameserver/definitions/NpcGroupSpawn.hxx"
#include "gameserver/definitions/Npc.hxx"
#include "gameserver/definitions/Outposts.hxx"
#include "gameserver/definitions/Recycles.hxx"
#include "gameserver/definitions/Respawn.hxx"
#include "gameserver/definitions/Scripts.hxx"
#include "gameserver/definitions/Shots.hxx"
#include "gameserver/definitions/Skills.hxx"
#include "gameserver/definitions/SubSkills.hxx"
#include "gameserver/definitions/Trader.hxx"
#include "gameserver/definitions/Vehicles.hxx"
#include "gameserver/definitions/VehicleSits.hxx"
#include "gameserver/definitions/Weapons.hxx"
#include "gameserver/definitions/Weather.hxx"
#include "gameserver/definitions/WorldFile.hxx"
#include "gameserver/definitions/WorldModels.hxx"
#include "gameserver/definitions/Worlds.hxx"

typedef PDefMap<PDefActionMod> PDefActionModsMap;
typedef PDefMap<PDefAmmo> PDefAmmosMap;
class PDefAppartementsMap; // Derived classe for addition of specific members
typedef PDefMap<PDefAppPlace> PDefAppPlacesMap; // No derived class needed here
typedef PDefMap<PDefBlueprintPieces> PDefBlueprintPiecesMap;
typedef PDefMap<PDefCharacter> PDefCharactersMap;
typedef PDefMap<PDefCharAction> PDefCharActionsMap;
typedef PDefMap<PDefCharKind> PDefCharKindsMap;
typedef PDefMap<PDefDamage> PDefDamagesMap;
typedef PDefMap<PDefDrug> PDefDrugsMap;
typedef PDefMap<PDefFaction> PDefFactionsMap;
//typedef PDefMap<PDefHack> PDefHacksMap; // File not used by KK
typedef PDefMap<PDefImplant> PDefImplantsMap;
typedef PDefMap<PDefItemContainer> PDefItemContainersMap;
typedef PDefMap<PDefItemMod> PDefItemModsMap;
typedef PDefMap<PDefItemRestriction> PDefItemRestrictionsMap;
class PDefItemsMap;
typedef PDefMap<PDefMission> PDefMissionsMap;
typedef PDefMap<PDefNpcArmor> PDefNpcArmorsMap;
typedef PDefMap<PDefNpcGroupSpawn> PDefNpcGroupSpawnsMap;
typedef PDefMap<PDefNpc> PDefNpcsMap;
typedef PDefMap<PDefOutpost> PDefOutpostsMap;
typedef PDefMap<PDefRecycle> PDefRecyclesMap;
class PDefRespawnsMap;
typedef PDefMap<PDefShot> PDefShotsMap;
typedef PDefMap<PDefSkill> PDefSkillsMap;
typedef PDefMap<PDefSubSkill> PDefSubSkillsMap;
typedef PDefMap<PDefTrader> PDefTradersMap;
typedef PDefMap<PDefVhc> PDefVhcsMap;
typedef PDefMap<PDefVhcSeat> PDefVhcSeatsMap;
typedef PDefMap<PDefWeapon> PDefWeaponsMap;
typedef PDefMap<PDefWeather> PDefWeathersMap;
typedef PDefMap<PDefWorld> PDefWorldsMap;
class PDefWorldFilesMap;
typedef PDefMap<PDefWorldModel> PDefWorldModelsMap;

class PDefScriptsMap;
//typedef PDefMap<PDefScripts> PDefScriptsMap;
// ___Add new entries here in alpĥabetical order___


class PGameDefs {
private:
    PDefActionModsMap mActionModsDefs;
    PDefAmmosMap mAmmosDefs;
    PDefAppartementsMap mAppartementsDefs;
    PDefAppPlacesMap mAppPlacesDefs;
    PDefBlueprintPiecesMap mBlueprintPiecesDefs;
    PDefCharactersMap mCharsDefs;
    PDefCharActionsMap mCharActionsDefs;
    PDefCharKindsMap mCharKindsDefs;
    PDefDamagesMap mDamagesDefs;
    PDefDrugsMap mDrugsDefs;
    PDefFactionsMap mFactionsDefs;
    //PDefHacksMap mHacksDefs;
    PDefImplantsMap mImplantsDefs;
    PDefItemContainersMap mItemContainersDefs;
    PDefItemModsMap mItemModsDefs;
    PDefItemRestrictionsMap mItemRestrictionsDefs;
    PDefItemsMap mItemsDefs;
    PDefMissionsMap mMissionsDefs;
    PDefNpcArmorsMap mNpcArmorsDefs;
    PDefNpcGroupSpawnsMap mNpcGroupSpawnsDefs;
    PDefNpcsMap mNpcsDefs;
    PDefOutpostsMap mOutpostsDefs;
    PDefRecyclesMap mRecyclesDefs;
    PDefRespawnsMap mRespawnsDefs;
    PDefShotsMap mShotsDefs;
    PDefSkillsMap mSkillsDefs;
    PDefSubSkillsMap mSubSkillsDefs;
    PDefTradersMap mTradersDefs;
    PDefVhcsMap mVhcsDefs;
    PDefVhcSeatsMap mVhcSeatsDefs;
    PDefWeaponsMap mWeaponsDefs;
    PDefWeathersMap mWeathersDefs;
    PDefWorldsMap mWorldsDefs;
    PDefWorldFilesMap mWorldFilesDefs;
    PDefWorldModelsMap mWorldModelsDefs;
    PDefScriptsMap mScriptDefs;
    // ___Add new entries here___

public:
    PGameDefs();
    ~PGameDefs();

    bool Init();

    inline const PDefActionModsMap* ActionMods() const { return &mActionModsDefs; }
    inline const PDefAmmosMap* Ammos() const { return &mAmmosDefs; }
    inline const PDefAppartementsMap* Appartements() const { return &mAppartementsDefs; }
    inline const PDefAppPlacesMap* AppPlaces() const { return &mAppPlacesDefs; }
    inline const PDefBlueprintPiecesMap* BlueprintPieces() const { return &mBlueprintPiecesDefs; }
    inline const PDefCharactersMap* Chars() const { return &mCharsDefs; }
    inline const PDefCharActionsMap* CharActions() const { return &mCharActionsDefs; }
    inline const PDefCharKindsMap* CharKinds() const { return &mCharKindsDefs; }
    inline const PDefDamagesMap* Damages() const { return &mDamagesDefs; }
    inline const PDefDrugsMap* Drugs() const { return &mDrugsDefs; }
    inline const PDefFactionsMap* Factions() const { return &mFactionsDefs; }
    //inline const PDefHacksMap* Hacks() const { return &mHacksDefs; }
    inline const PDefImplantsMap* Implants() const { return &mImplantsDefs;}
    inline const PDefItemContainersMap* ItemContainers() const { return &mItemContainersDefs; }
    inline const PDefItemModsMap* ItemMods() const { return &mItemModsDefs; }
    inline const PDefItemRestrictionsMap* ItemRestrictions() const { return & mItemRestrictionsDefs; }
    inline const PDefItemsMap* Items() const { return &mItemsDefs; }
    inline const PDefMissionsMap* Missions() const { return &mMissionsDefs; }
    inline const PDefNpcArmorsMap* NpcArmors() const { return & mNpcArmorsDefs; }
    inline const PDefNpcGroupSpawnsMap* GroupSpawns() const { return &mNpcGroupSpawnsDefs; }
    inline const PDefNpcsMap* Npcs() const { return &mNpcsDefs; }
    inline const PDefOutpostsMap* Outposts() const { return &mOutpostsDefs; }
    inline const PDefRecyclesMap* Recycles() const { return &mRecyclesDefs; }
    inline const PDefRespawnsMap* Respawns() const { return &mRespawnsDefs; }
    inline const PDefShotsMap* Shots() const { return &mShotsDefs; }
    inline const PDefSkillsMap* Skills() const { return &mSkillsDefs; }
    inline const PDefSubSkillsMap* SubSkills() const { return &mSubSkillsDefs; }
    inline const PDefTradersMap* Traders() const { return &mTradersDefs; }
    inline const PDefVhcsMap* Vhcs() const { return &mVhcsDefs; }
    inline const PDefVhcSeatsMap* VhcSeats() const { return &mVhcSeatsDefs; }
    inline const PDefWeaponsMap* Weapons() const { return &mWeaponsDefs; }
    inline const PDefWeathersMap* Weathers() const { return &mWeathersDefs; }
    inline const PDefWorldsMap* Worlds() const { return &mWorldsDefs; }
    inline const PDefWorldFilesMap* WorldFiles() const { return &mWorldFilesDefs; }
    inline const PDefWorldModelsMap* WorldModels() const { return &mWorldModelsDefs; }
    inline const PDefScriptsMap* Scripts() const { return &mScriptDefs; }
    // ___Add new entries here___
};
