#include "gameserver/definitions/Includes.hxx"

/*
 --------------------------------------------------------------------
 WARNING:
 When adding new .def support, don't forget to add required stuff in PGameDefs::Init()
   (compiler won't complain if you don't add that)
 --------------------------------------------------------------------

 NOTA: as PGameDefs uses the PDefParser class, which uses the PFileSystem and PFile classes,
   files are searched in the same way as the NC client does:
     - first try to load the file in unpacked form from the given directory,
     - else tries to load the file in packed form from the given directory,
         appending pak_ to its name,
     - else tries to load the packed file from the .pak archive which name is
         derived from the last part of the path.
*/

PGameDefs::PGameDefs() {}
PGameDefs::~PGameDefs() {}

bool PGameDefs::Init()
{
  Console->Print( "Initializing game defs..." );
  bool Res = true;

  Res |= mActionModsDefs.Load ("Action mods", "defs/actionmod.def");
  Res |= mAmmosDefs.Load ("Ammos", "defs/ammo.def");
  Res |= mAppartementsDefs.Load ("Apartments", "defs/appartements.def");
  Res |= mAppPlacesDefs.Load ("App Places", "defs/appplaces.def");
  Res |= mBlueprintPiecesDefs.Load ("Blueprint pieces", "defs/blueprintpieces.def");
  Res |= mCharsDefs.Load ("Chars", "defs/characters.def");
  Res |= mCharActionsDefs.Load ("Char actions", "defs/charaction.def");
  Res |= mDamagesDefs.Load ("Damage", "defs/damage.def");
  Res |= mDrugsDefs.Load ("Drugs", "defs/drugs.def");
  Res |= mFactionsDefs.Load ("Factions", "defs/fractions.def");
  //Res |= mHacksDefs.Load ("Hack", "defs/hack.def");
  Res |= mImplantsDefs.Load ("Implants", "defs/implants.def");
  Res |= mItemContainersDefs.Load ("Item containers", "defs/itemcontainer.def");
  Res |= mItemModsDefs.Load ("Item mods", "defs/itemmod.def");
  Res |= mItemRestrictionsDefs.Load ("Item restrictions", "defs/itemres.def");
  Res |= mItemsDefs.Load ("Items", "defs/items.def");
  Res |= mMissionsDefs.Load ("Missions", "defs/missionbase.def");
  Res |= mNpcArmorsDefs.Load ("NPC Armors", "defs/npcarmor.def");
  Res |= mNpcGroupSpawnsDefs.Load ("NPC group spawns", "defs/npcgroupspawn.def");
  Res |= mNpcsDefs.Load ("NPC", "defs/npc.def");
  Res |= mOutpostsDefs.Load ("Outposts", "defs/outposts.def");
  Res |= mRecyclesDefs.Load ("Recycles", "defs/recycles.def");
  Res |= mRespawnsDefs.Load ("Respawns", "defs/respawn.def");
  Res |= mShotsDefs.Load ("Shots", "defs/shots.def");
  Res |= mSubSkillsDefs.Load ("Subskills", "defs/subskill.def"); // Loading before skills is required
  Res |= mSkillsDefs.Load ("Skills", "defs/skills.def");
  Res |= mCharKindsDefs.Load ("Char kinds", "defs/charkinds.def"); // Loading after skills is required
  Res |= mTradersDefs.Load ("Traders", "defs/trader.def");
  Res |= mVhcsDefs.Load ("Vehicles", "defs/vehicles.def");
  Res |= mVhcSeatsDefs.Load ("Vehicle seats", "defs/vehiclesits.def");
  Res |= mWeaponsDefs.Load ("Weapons", "defs/weapons.def");
  Res |= mWeathersDefs.Load ("Weathers", "defs/weather.def");
  Res |= mWorldsDefs.Load ("Worldinfo", "defs/worldinfo.def");
  Res |= mWorldFilesDefs.Load ("World files", "worlds/worlds.ini");
  Res |= mWorldModelsDefs.Load ("World models", "defs/worldmodel.def");
  Res |= mScriptDefs.Load ("Script defs", "defs/scripts.def");

  return ( Res );
}
