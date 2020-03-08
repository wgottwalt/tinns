#include <cstring>
#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

// PCharCoordinates
void PCharCoordinates::SetInterpolate( PCharCoordinates& Pos1, PCharCoordinates& Pos2, float nCoef )
{
  if (( nCoef < 0 ) || ( nCoef > 1 ) )
  {
    Console->Print( RED, BLACK, "[Error] PCharCoordinates::Interpolate : Invalid nCoef value: %f", nCoef );
    nCoef = 0;
  }
  float rCoef = 1 - nCoef;

  mY = ( uint16_t )( rCoef * Pos1.mY + nCoef * Pos2.mY );
  mZ = ( uint16_t )( rCoef * Pos1.mZ + nCoef * Pos2.mZ );
  mX = ( uint16_t )( rCoef * Pos1.mX + nCoef * Pos2.mX );
  mUD = ( uint8_t )( rCoef * Pos1.mUD + nCoef * Pos2.mUD );
  if ( abs( Pos1.mLR - Pos2.mLR ) < 90 )
  {
    mLR = ( uint8_t )( rCoef * Pos1.mLR + nCoef * Pos2.mLR );
  }
  else
  {
    mLR = ( uint8_t )(( uint16_t )( rCoef * ( 180.0 + ( float )Pos1.mLR ) + nCoef * Pos2.mLR ) % 180 );
  }
}

uint16_t mY;     // Y-Position in world
uint16_t mZ;     // Z-Position in world
uint16_t mX;     // X-Position in world
uint8_t mUD;     // Up - Mid - Down (d6 - 80 - 2a)
uint8_t mLR;     // Compass direction (S..E..N..W..S [0-45-90-135-179])
uint8_t mAct;    // Last user action state

void PCharCoordinates::SetPosition( uint16_t nY, uint16_t nZ, uint16_t nX, uint8_t nUD, uint8_t nLR )
{
  mY = nY;
  mZ = nZ;
  mX = nX;
  mUD = nUD;
  if ( mUD < 0x2a )
  {
    mUD = 0x2a;
  }
  else if ( mUD > 0xd6 )
  {
    mUD = 0xd6;
  }
  mLR = nLR % 180;
}


// SQL Layout
enum
{
    c_id,
    c_name,
    c_str_lvl,
    c_str_pts,
    c_int_lvl,
    c_int_pts,
    c_dex_lvl,
    c_dex_pts,
    c_con_lvl,
    c_con_pts,
    c_psi_lvl,
    c_psi_pts,
    a_id,
    c_class,
    c_profession,
    c_sex,
    c_location,
    c_mc,
    c_hc,
    c_tra,
    c_pc,
    c_rc,
    c_tc,
    c_vhc,
    c_agl,
    c_rep,
    c_rec,
    c_rcl,
    c_atl,
    c_end,
    c_for,
    c_fir,
    c_enr,
    c_xrr,
    c_por,
    c_hlt,
    c_hck,
    c_brt,
    c_psu,
    c_wep,
    c_cst,
    c_res,
    c_imp,
    c_ppu,
    c_apu,
    c_mst,
    c_ppw,
    c_psr,
    c_wpw,
    c_apt,
    c_cash,
    c_head,
    c_torso,
    c_legs,
    c_str_xp,
    c_int_xp,
    c_dex_xp,
    c_psi_xp,
    c_con_xp,
    c_pos_x,
    c_pos_y,
    c_pos_z,
    c_angle_ud,
    c_angle_lr,
    c_faction,
    c_slot,
    c_online,
    c_clan,
    c_soullight
};

RegEx* PChar::mCharnameRegexFilter = nullptr;

PChar::PChar()
{
  mID = 0;
  mAccount = 0;
  mGender = 0;
  mClass = 0;
  mProfession = 1;
  mFaction = 1;
  //mModel = 0;
  //mType = 0;
  mRealHead = 0;  // Base Skin elements, in complement of (computed) mType
  mRealTorso = 0; // " Head shouldn't be changeable, except in case of surgery !!!
  mRealLegs = 0;  // "
  mSkin = 0;  // Current Skin elements
  mHead = 0;  // "
  mTorso = 0; // "
  mLegs = 0;  // "
  mHeadColor = mTorsoColor = mLegsColor = 0; // "
  mHeadDarkness = mTorsoDarkness = mLegsDarkness = 0; // "

  mBodyEffect = 0;
  mBodyEffectDensity = 0;
  mQuickBeltActiveSlot = INV_WORN_QB_NONE;

  mLookingAt = 0;
  mLookAtTimestamp = 0;
  mLastUsedWorldObjectId = 0;

  mSpeedOverride = 255; // means no override. Value 0 can be used to forbid any move.

  mLocationLeased = false; // temp until char on-demand load/unload
  mLocation = 0;
  mStartApt = 0;
  mPrimaryApt = 0;
  mCash = 0;

  mSeatInUseType = seat_none;
  mSeatInUseObjectId = 0;
  mSeatInUseSeatId = 0;
  mVhcAccessRequestList = nullptr;

  mContainerInExclusiveUse = nullptr;

  mIsOnline = false;
  mDirtyFlag = false;

  mShunned = false;
  mJailed = false;

  mDialogNPC = 0;
  mCurrentDialogNode = 0;

  Skill = new PSkillHandler();
  mBuddyList = nullptr;
  mGenrepList = nullptr;

  // Required for initial OOC Broadcast welcome message.
  //Gets overwritten as soon as the first PingPacket arrives
  mActiveChatChannels = 262144;
  mClanLevel = 0;
  mClanID = 0;
}

PChar::~PChar()
{
  // Addition; Set char's online status to OFFLINE
  char sqlqry[50];
  snprintf(sqlqry, 50, "UPDATE characters SET c_online = 0 WHERE c_id = %d", mID);
  MySQL->GameQuery(sqlqry);

  delete Skill;
  delete mBuddyList;
  delete mGenrepList;
  delete mVhcAccessRequestList;

  if ( mContainerInExclusiveUse )
  {
    mContainerInExclusiveUse->EndUse( mID );
    Console->Print( RED, BLACK, "Warning: PChar::~PChar : Char still had exclusive use of container. Now freed." );
  }

  if ( mSeatInUseType != seat_none )
  {
    Console->Print( RED, BLACK, "Warning: PChar::~PChar : Char still using seat %d of vhc or chair %d.", mSeatInUseSeatId, mSeatInUseObjectId );
  }
}

bool PChar::SetCharnameRegexFilter( const char* RegexStr )
{
  if ( mCharnameRegexFilter )
  {
    delete mCharnameRegexFilter;
    mCharnameRegexFilter = nullptr;
  }

  if ( RegexStr )
  {
    try
    {
      mCharnameRegexFilter = new RegEx( RegexStr, PCRE_CASELESS );
    }
    catch ( ... )
    {
      return false;
    }
  }
  return true;
}

bool PChar::IsCharnameWellFormed( const char *Charname )
{
  if ( mCharnameRegexFilter )
  {
    return mCharnameRegexFilter->Search( Charname );
  }
  else
    return true;
}

void PChar::SetProfession( uint32_t Profession )
{
  const PDefCharKind *def = GameDefs->CharKinds()->GetDef( Profession );
  if (def == nullptr)
  {
    Console->Print( RED, BLACK, "Char %d: Invalid profession %d", mID, Profession );
    mProfession = 10;
    mClass = 0;
  }
  else
  {
    mProfession = Profession;
    mClass = def->GetType();
  }
  SetDirtyFlag();
}

uint32_t PChar::GetSkinFromCharType( uint32_t nType )
{
  const PDefCharacter* nDefCharacter = GameDefs->Chars()->GetDef( nType );
  if ( nDefCharacter )
  {
    return (( uint32_t )( nDefCharacter->GetModel() ) );
  }
  else
    return 0;
}

inline uint32_t PChar::GetBaseModel()
{
  return GetSkinFromCharType( GetType() );
}

void PChar::SetRealLook( uint32_t nHead, uint32_t nTorso, uint32_t nLegs )
{
  mRealHead = nHead;
  mRealTorso = nTorso;
  mRealLegs = nLegs;
  SetDirtyFlag();
  ResetCurrentLook();
}

void PChar::GetRealLook( uint32_t &nSkin, uint32_t &nHead, uint32_t &nTorso, uint32_t &nLegs )
{
  nSkin = GetBaseModel();
  nHead = mRealHead;
  nTorso = mRealTorso;
  nLegs = mRealLegs;
}

void PChar::SetCurrentLookFromCharType( uint32_t nType )
{
  int iHead, iTorso, iLegs;
  uint32_t nSkin, nHead, nTorso, nLegs;
  uint8_t nColor, nBrightness;

  const PDefCharacter* nDefCharacter = GameDefs->Chars()->GetDef( nType );
  if ( nDefCharacter )
  {
    nSkin = ( uint32_t ) nDefCharacter->GetModel();
    iHead = nDefCharacter->GetHead();
    iTorso = nDefCharacter->GetTorso();
    iLegs = nDefCharacter->GetLegs();
    nColor = ( uint8_t )( 0xff & nDefCharacter->GetColor() );
    nBrightness = ( uint8_t )( 0xff & nDefCharacter->GetBrightness() );


    if (( iHead < 0 ) || ( iTorso < 0 ) || ( iLegs < 0 ) )
    {
      nHead = nTorso = nLegs = 0;
    }
    else
    {
      nHead = iHead % 10;
      nTorso = iTorso % 10;
      nLegs = iLegs % 10;
    }

    SetCurrentLook( nSkin, nHead, nTorso, nLegs );
    SetCurrentBodyColor( nColor, nColor, nColor, nBrightness, nBrightness, nBrightness );
  }
}

void PChar::SetCurrentLook( uint32_t nSkin, uint32_t nHead, uint32_t nTorso, uint32_t nLegs )
{
  mSkin = nSkin;
  mHead = nHead;
  mTorso = nTorso;
  mLegs = nLegs;
  // Ingame skin update will be done automagically in the normal update flow
  // A flag could also be set somewhere (preferably in Char si that we don't have to care if ingame or not)
  //    to request quicker ingame update
}

void PChar::ResetCurrentLook()
{
  SetCurrentLook( GetSkinFromCharType( GetType() ), mRealHead, mRealTorso, mRealLegs );
}

// GetCurrentLook will later have to take Power Armors and GM overrides into account
void PChar::GetCurrentLook( uint32_t &nSkin, uint32_t &nHead, uint32_t &nTorso, uint32_t &nLegs )
{
  nSkin = mSkin;
  nHead = mHead;
  nTorso = mTorso;
  nLegs = mLegs;
}

void PChar::SetCurrentBodyColor( uint8_t nHeadColor, uint8_t nTorsoColor, uint8_t nLegsColor, uint8_t nHeadDarkness, uint8_t nTorsoDarkness, uint8_t nLegsDarkness )
{
  mHeadColor = nHeadColor;
  mTorsoColor = nTorsoColor;
  mLegsColor = nLegsColor;
  mHeadDarkness = nHeadDarkness;
  mTorsoDarkness = nTorsoDarkness;
  mLegsDarkness = nLegsDarkness;
}

void PChar::GetCurrentBodyColor( uint8_t &nHeadColor, uint8_t &nTorsoColor, uint8_t &nLegsColor, uint8_t &nHeadDarkness, uint8_t &nTorsoDarkness, uint8_t &nLegsDarkness )
{
  nHeadColor = mHeadColor;
  nTorsoColor = mTorsoColor;
  nLegsColor = mLegsColor;
  nHeadDarkness = mHeadDarkness;
  nTorsoDarkness = mTorsoDarkness;
  nLegsDarkness = mLegsDarkness;
}

void PChar::SetBaseSkills()
{
  const PDefCharKind *def = GameDefs->CharKinds()->GetDef( mProfession );
  //Console->Print(YELLOW, BLACK, "PChar::SetBaseSkills() Profession: %d",def->GetIndex());
  if (def == nullptr)
  {
    Console->Print( RED, BLACK, "PChar::SetBaseSkills: GetCharKindDef=NULL" );
    return;
  }
  Skill->SetMainSkill( MS_INT, def->GetSkillInfo( MS_INT ).mStart );
  Skill->SetMainSkill( MS_CON, def->GetSkillInfo( MS_CON ).mStart );
  Skill->SetMainSkill( MS_DEX, def->GetSkillInfo( MS_DEX ).mStart );
  Skill->SetMainSkill( MS_STR, def->GetSkillInfo( MS_STR ).mStart );
  Skill->SetMainSkill( MS_PSI, def->GetSkillInfo( MS_PSI ).mStart );
  // management of SP needed ? NC Client seem to calculate what remains ...
  // or put SP setting after subskill setting ?
  /* Skill->SetSP(MS_INT, (short) ??? ));
  Skill->SetSP(MS_CON, ((short) ??? ));
  Skill->SetSP(MS_DEX, (short) ??? ));
  Skill->SetSP(MS_STR, (short) ??? ));
  Skill->SetSP(MS_PSI, ((short) ??? )); */
  // what about XPs ?
  /* Skill->SetXP(MS_INT, ??? ));
  Skill->SetXP(MS_CON, ??? ));
  Skill->SetXP(MS_DEX, ??? ));
  Skill->SetXP(MS_STR, ??? ));
  Skill->SetXP(MS_PSI, ??? )); */
  Console->Print( YELLOW, BLACK, "PChar::SetBaseSkills() not fully functionnal - unused skill points will be lost" );
}

void PChar::SetBaseSubskills( uint8_t NSZNb, const char* NonZeroSubskills )
{
  int i;

  if ( NSZNb == 0 )
    return;

  for ( i = 0; i < NSZNb; i++ )
  {
    Skill->SetSubSkill(( SUB_SKILLS ) NonZeroSubskills[2 * i], ( int ) NonZeroSubskills[2 * i +1] );
  }
}

void PChar::SetBaseInventory()
{
  uint8_t i;
  uint32_t BaseItemID;
  const PDefCharKind *def = GameDefs->CharKinds()->GetDef( mProfession );

  //mCash = 5000;
  mCash = def->GetStartMoney();

  for ( i = 0; i < 8 ; i++ )
  {
    BaseItemID = def->GetStartInventory( i );
    if ( BaseItemID )
    {
      if ( gDevDebug ) Console->Print( GREEN, BLACK, "Adding item %d to base inventory", BaseItemID );
      PItem* NewItem = new PItem( BaseItemID );
      if ( NewItem->GetItemID() )
      {
        NewItem->MakeItemStandard( 120, 180 ); // global quality range
        mInventory.AddItem( NewItem );
      }
      else
      {
        Console->Print( RED, BLACK, "Invalid item ID %d in base inventory for profession %d", BaseItemID, mProfession );
      }
    }
  }
}

void PChar::LoadClanLevel()
{
    MYSQL_RES *result;
    char query[200];

    snprintf(query, 200, "SELECT cll_level FROM clanlevels WHERE cll_charid = %d AND cll_clanid = %d", mID, mClanID);
    result = MySQL->GameResQuery( query );
    if (result == nullptr)
    {
        mClanID = 0;
        Console->Print( RED, BLACK, "PChar::GetClanLevel could not load ClanLevel from the database" );
        Console->Print( "Query was:" );
        Console->Print( "%s", query );
        MySQL->ShowGameSQLError();
        return;
    }
    else if(mysql_num_rows(result) == 0)
    {
        mClanID = 0;
        Console->Print( RED, BLACK, "PChar::GetClanLevel No level entry found for clan!" );
        return;
    }
    mClanLevel = atoi(mysql_fetch_row(result)[0]);
    if (gDevDebug) Console->Print("Loaded ClanLevel %d for char %d, clan %d", mClanLevel, mID, mClanID);
}

bool PChar::SQLLoad( int CharID )
{
  MYSQL_RES *result;
  MYSQL_ROW row;
  char query[1024];

  snprintf( query, 1024, "SELECT * FROM characters WHERE c_id = %d LIMIT 1", CharID );
  result = MySQL->GameResQuery( query );
  if (result == nullptr)
  {
    Console->Print( RED, BLACK, "PChar::SQLLoad could not load Chardata from the database" );
    Console->Print( "Query was:" );
    Console->Print( "%s", query );
    MySQL->ShowGameSQLError();
    return false;
  }
  if (( row = mysql_fetch_row( result ) ) )
  {
    SetID( CharID );
    SetName( row[c_name] );
    SetAccount( atoi( row[a_id] ) );

    // Gender
    int genvalue = atoi( row[c_sex] );
    if (( genvalue == 0 ) || ( genvalue == 1 ) )
      mGender = static_cast<uint32_t>( genvalue );
    else
    {
      Console->Print( RED, BLACK, "Bad gender value: %d (Char ID %d)", genvalue, mID );
      mGender = 0;
    }

    // Profession
    int profvalue = atoi( row[c_profession] );
    SetProfession( static_cast<uint32_t>( profvalue ) );

    // Class
    //int classvalue = atoi(row[c_class]);
    //if(classvalue < 4)
    //    mClass = static_cast<uint32_t>(classvalue);
    //else
    //{
    //  Console->Print(RED, BLACK, "Bad class value: %d (Char ID %d)", classvalue, mID);
    //  classvalue = 0;
    //}

    // Faction
    int facvalue = atoi( row[c_faction] );
    if ( GameDefs->Factions()->GetDef( facvalue ) )
      mFaction = static_cast<uint32_t>( facvalue );
    else
      mFaction = 1;

    /* // Model
    int modvalue = atoi(row[c_model]);
    mModel = static_cast<uint32_t>(modvalue);
    mModel = 10; */
    int headvalue = atoi( row[c_head] );
    int torsovalue = atoi( row[c_torso] );
    int legsvalue = atoi( row[c_legs] );
    SetRealLook( static_cast<uint32_t>( headvalue ), static_cast<uint32_t>( torsovalue ), static_cast<uint32_t>( legsvalue ) );

    // Type
    /*
    int typevalue = std::atoi(row[c_type]);
    mType = static_cast<uint32_t>(typevalue);
    //mType = 1; */

    // Location
    int locvalue = atoi( row[c_location] );
    mLocation = static_cast<uint32_t>( locvalue );

//This assumption is not so good ... hardcoding jailed state linked with location ...
    if ( mLocation == 550 || mLocation == 551 )
      mJailed = true;

    int posvalue = atoi( row[c_pos_x] );
    Coords.mX = static_cast<uint16_t>( posvalue );
    posvalue = atoi( row[c_pos_y] );
    Coords.mY = static_cast<uint16_t>( posvalue );
    posvalue = atoi( row[c_pos_z] );
    Coords.mZ = static_cast<uint16_t>( posvalue );
    posvalue = atoi( row[c_angle_ud] );
    Coords.mUD = static_cast<uint8_t>( posvalue );
    posvalue = atoi( row[c_angle_lr] );
    Coords.mLR = static_cast<uint8_t>( posvalue );

    int primapt = atoi( row[c_apt] );
    mPrimaryApt = static_cast<uint32_t>( primapt );
    mStartApt = mPrimaryApt;

    mSoullight = atoi( row[c_soullight] );
    mClanID = atoi( row[c_clan] );
    if(mClanID > 0)
        LoadClanLevel();

    // Cash
    float cashvalue = atof( row[c_cash] );
    mCash = static_cast<uint32_t>( cashvalue );

    // ---------------------------------------------
    // Loading skills --- MAIN Skills with SP and XP
    // ---------------------------------------------
    Skill->SetMainSkill( MS_INT, atoi( row[c_int_lvl] ) );
    Skill->SetMainSkill( MS_CON, atoi( row[c_con_lvl] ) );
    Skill->SetMainSkill( MS_DEX, atoi( row[c_dex_lvl] ) );
    Skill->SetMainSkill( MS_STR, atoi( row[c_str_lvl] ) );
    Skill->SetMainSkill( MS_PSI, atoi( row[c_psi_lvl] ) );
    // ---------------------------------------------
    Skill->SetSP( MS_INT, ( short )atoi( row[c_int_pts] ) );
    Skill->SetSP( MS_CON, ( short )atoi( row[c_con_pts] ) );
    Skill->SetSP( MS_DEX, ( short )atoi( row[c_dex_pts] ) );
    Skill->SetSP( MS_STR, ( short )atoi( row[c_str_pts] ) );
    Skill->SetSP( MS_PSI, ( short )atoi( row[c_psi_pts] ) );
    // ---------------------------------------------
    Skill->SetXP( MS_INT, atof( row[c_int_xp] ) );
    Skill->SetXP( MS_CON, atof( row[c_con_xp] ) );
    Skill->SetXP( MS_DEX, atof( row[c_dex_xp] ) );
    Skill->SetXP( MS_STR, atof( row[c_str_xp] ) );
    Skill->SetXP( MS_PSI, atof( row[c_psi_xp] ) );
    // ---------------------------------------------
    // SubSkills
    // ---------------------------------------------
    Skill->SetSubSkill( SK_MC, atoi( row[c_mc] ) );
    Skill->SetSubSkill( SK_HC, atoi( row[c_hc] ) );
    Skill->SetSubSkill( SK_TRA, atoi( row[c_tra] ) );
    Skill->SetSubSkill( SK_FOR, atoi( row[c_for] ) );
    Skill->SetSubSkill( SK_PC, atoi( row[c_pc] ) );
    Skill->SetSubSkill( SK_RC, atoi( row[c_rc] ) );
    Skill->SetSubSkill( SK_TC, atoi( row[c_tc] ) );
    Skill->SetSubSkill( SK_VHC, atoi( row[c_vhc] ) );
    Skill->SetSubSkill( SK_AGL, atoi( row[c_agl] ) );
    Skill->SetSubSkill( SK_REP, atoi( row[c_rep] ) );
    Skill->SetSubSkill( SK_REC, atoi( row[c_rec] ) );
    Skill->SetSubSkill( SK_RCL, atoi( row[c_rcl] ) );
    Skill->SetSubSkill( SK_ATL, atoi( row[c_atl] ) );
    Skill->SetSubSkill( SK_END, atoi( row[c_end] ) );
    Skill->SetSubSkill( SK_FIR, atoi( row[c_fir] ) );
    Skill->SetSubSkill( SK_ENR, atoi( row[c_enr] ) );
    Skill->SetSubSkill( SK_XRR, atoi( row[c_xrr] ) );
    Skill->SetSubSkill( SK_POR, atoi( row[c_por] ) );
    Skill->SetSubSkill( SK_HLT, atoi( row[c_hlt] ) );
    Skill->SetSubSkill( SK_HCK, atoi( row[c_hck] ) );
    Skill->SetSubSkill( SK_BRT, atoi( row[c_brt] ) );
    Skill->SetSubSkill( SK_PSU, atoi( row[c_psu] ) );
    Skill->SetSubSkill( SK_WEP, atoi( row[c_wep] ) );
    Skill->SetSubSkill( SK_CST, atoi( row[c_cst] ) );
    Skill->SetSubSkill( SK_RES, atoi( row[c_res] ) );
    Skill->SetSubSkill( SK_IMP, atoi( row[c_imp] ) );
    Skill->SetSubSkill( SK_PPU, atoi( row[c_ppu] ) );
    Skill->SetSubSkill( SK_APU, atoi( row[c_apu] ) );
    Skill->SetSubSkill( SK_MST, atoi( row[c_mst] ) );
    Skill->SetSubSkill( SK_PPW, atoi( row[c_ppw] ) );
    Skill->SetSubSkill( SK_PSR, atoi( row[c_psr] ) );
    Skill->SetSubSkill( SK_WPW, atoi( row[c_wpw] ) );
    // --------------------------------------------
    // Inventory
    // ---------------------------------------------
    mInventory.SetCharId( mID );
    mInventory.SQLLoad();

    // temp value forcing, not get/saved from DB atm
    mCombatRank = ( uint8_t )( random() % 127 ); // bad result there on randomness
    mSynaptic = 0;
    mIsDead = false;

    mDirectCharID = 0; // until saved/loaded with char
    mBuddyList = new PBuddyList( mID );
    if ( !mBuddyList->SQLLoad() )
    {
      Console->Print( RED, BLACK, "Char ID %d : Can't load buddy list", mID );
    }
    //to add: Chats settings

    mGenrepList = new PGenrepList( mID );
    if ( !mGenrepList->SQLLoad() )
    {
      Console->Print( RED, BLACK, "Char ID %d : Can't load genrep list", mID );
    }

  }
  MySQL->FreeGameSQLResult( result );

  // Addition; Set char's online status to ONLINE
  char sqlqry[50];
  snprintf(sqlqry, 50, "UPDATE characters SET c_online = 1 WHERE c_id = %d", mID);
  MySQL->GameQuery(sqlqry);

  return true;
}

bool PChar::SQLCreate() // Specific method for creation in order to avoid existence check with each save
{
  std::string query, queryv;

  query = "INSERT INTO characters (c_id";
  queryv = ") VALUES (NULL";

  query += ",c_name";
  queryv = queryv + ",'" + mName + "'";

  query += ",a_id";
  queryv += Misc::Ssprintf( ",'%u'", mAccount );
  query += ",c_class";
  queryv += Misc::Ssprintf( ",'%u'", mClass );
  query += ",c_sex";
  queryv += Misc::Ssprintf( ",'%u'", mGender );
  query += ",c_profession";
  queryv += Misc::Ssprintf( ",'%u'", mProfession );
  query += ",c_faction";
  queryv += Misc::Ssprintf( ",'%u'", mFaction );
  query += ",c_head";
  queryv += Misc::Ssprintf( ",'%u'", mRealHead );
  query += ",c_torso";
  queryv += Misc::Ssprintf( ",'%u'", mRealTorso );
  query += ",c_legs";
  queryv += Misc::Ssprintf( ",'%u'", mRealLegs );
  //query += ",c_model";
  //queryv += Misc::Ssprintf(",'%u'", mModel);
  //query += ",c_type";
  //queryv += Misc::Ssprintf(",'%u'", mType);
  query += ",c_location";
  queryv += Misc::Ssprintf( ",'%u'", mLocation );
  query += ",c_cash";
  queryv += Misc::Ssprintf( ",'%u'", mCash );
  query += ",c_slot";
  queryv += Misc::Ssprintf( ",'%u'", mSlot );

  query = query + queryv + ");";

  if ( MySQL->GameQuery( query.c_str() ) )
  {
    Console->Print( RED, BLACK, "PChar::SQLCreate could not add char %s to database", mName.c_str() );
    Console->Print( "Query was:" );
    Console->Print( "%s", query.c_str() );
    MySQL->ShowGameSQLError();
    return false;
  }
  else
  {
    mID = MySQL->GetLastGameInsertId();
//Console->Print(GREEN, BLACK, "New char %s got ID %d", mName.c_str(), mID);
    mDirtyFlag = true;
    return true;
  }
}

bool PChar::CreateNewChar( uint32_t Account, const std::string &Name, uint32_t Gender, uint32_t Profession, uint32_t Faction,
                           uint32_t Head, uint32_t Torso, uint32_t Legs, uint8_t NZSNb, const char *NonZeroSubskills, uint32_t Slot )
{
  SetName( Name );
  SetGender( Gender );
  SetProfession( Profession );
  SetFaction( Faction );
  SetRealLook( Head, Torso, Legs );
  SetBaseSkills();
  SetBaseSubskills( NZSNb, NonZeroSubskills );
  SetBaseInventory();
  SetAccount( Account );
  SetCharSlot( Slot );
  mLocation = Config->GetOptionInt( "new_char_location" );

  // This part will have to be rewritten with proper methods
  mSoullight = 10;
  mCombatRank = ( uint8_t )( random() % 80 ); // bad result there on randomness
  mSynaptic = 0;
  mIsDead = false;

  mDirectCharID = 0; // until saved/loaded with char

  SetDirtyFlag();

  if ( SQLCreate() ) // mID isn't defined before that
  {
    mBuddyList = new PBuddyList( mID );
    mGenrepList = new PGenrepList( mID );
    mStartApt = mPrimaryApt = Appartements->CreateBaseAppartement( mID, mName, mFaction );
    mInventory.SetCharId( mID );

    if ( mStartApt && SQLSave() && mInventory.SQLSave() )
    {
      return true;
    }
    else
    {
      Console->Print( YELLOW, BLACK, "New char %s (id %d) : creation aborted", mName.c_str(), mID );
      if ( mID )
      {
        SQLDelete();
        if ( mStartApt )
        {
          Appartements->DeleteCharAppartements( mID );
        }
      }
    }
  }
  return false;
}

bool PChar::SQLSave()
{
  std::string query;
  //std::string ts;

  /* TODO:
    - Mostly at creation/load :
              c_apt, (or when first GR to primary apt to avoid creation of unused apt?)
              (c_slot)
    - At save/load :
              SoulLight ???
              FactionSymp[] ???
              Chest change: style, brightness, color
              Legs change: style, brightness, color
              mHealt, mStamina, mMana (not in DB !!!)
              How to compute MaxHealth etc. ?
  */
  query = "UPDATE characters SET";

  query += Misc::Ssprintf( " c_location='%u'", mLocation );
  query += Misc::Ssprintf( ",c_pos_x='%u'", Coords.mX );
  query += Misc::Ssprintf( ",c_pos_y='%u'", Coords.mY );
  query += Misc::Ssprintf( ",c_pos_z='%u'", Coords.mZ );
  query += Misc::Ssprintf( ",c_angle_ud='%u'", Coords.mUD );
  query += Misc::Ssprintf( ",c_angle_lr='%u'", Coords.mLR );
  query += Misc::Ssprintf( ",c_cash='%u'", mCash );
  query += Misc::Ssprintf( ",c_apt='%u'", mPrimaryApt );

  query += Misc::Ssprintf( ",c_head='%u'", mRealHead );
  query += Misc::Ssprintf( ",c_torso='%u'", mRealTorso );
  query += Misc::Ssprintf( ",c_legs='%u'", mRealLegs );

  query += Misc::Ssprintf( ",c_faction='%u'", mFaction );

  /* This group of fiels shouldn't change in-game
  query = query + ",c_name='" + mName + "'";
  query += Misc::Ssprintf(",a_id='%u'", mAccount);
  query += Misc::Ssprintf(",c_class='%u'", mClass);
  query += Misc::Ssprintf(",c_sex='%u'", mGender);
  query += Misc::Ssprintf(",c_profession='%u'", mProfession);
  query += Misc::Ssprintf(",c_slot='%u'", mSlot);
  query += Misc::Ssprintf(",c_model='%u'", mModel);
  query += Misc::Ssprintf(",c_type='%u'", mType);
  */

  // ---------------------------------------------
  // Saving skills --- MAIN Skills with SP and XP
  // ---------------------------------------------
  query += Misc::Ssprintf( ",c_int_lvl='%u'", Skill->GetMainSkill( MS_INT ) );
  query += Misc::Ssprintf( ",c_con_lvl='%u'", Skill->GetMainSkill( MS_CON ) );
  query += Misc::Ssprintf( ",c_dex_lvl='%u'", Skill->GetMainSkill( MS_DEX ) );
  query += Misc::Ssprintf( ",c_str_lvl='%u'", Skill->GetMainSkill( MS_STR ) );
  query += Misc::Ssprintf( ",c_psi_lvl='%u'", Skill->GetMainSkill( MS_PSI ) );
  // ---------------------------------------------
  query += Misc::Ssprintf( ",c_int_pts='%u'", Skill->GetSP( MS_INT ) );
  query += Misc::Ssprintf( ",c_con_pts='%u'", Skill->GetSP( MS_CON ) );
  query += Misc::Ssprintf( ",c_dex_pts='%u'", Skill->GetSP( MS_DEX ) );
  query += Misc::Ssprintf( ",c_str_pts='%u'", Skill->GetSP( MS_STR ) );
  query += Misc::Ssprintf( ",c_psi_pts='%u'", Skill->GetSP( MS_PSI ) );
  // ---------------------------------------------
  query += Misc::Ssprintf( ",c_int_xp='%u'", Skill->GetXP( MS_INT ) );
  query += Misc::Ssprintf( ",c_con_xp='%u'", Skill->GetXP( MS_CON ) );
  query += Misc::Ssprintf( ",c_dex_xp='%u'", Skill->GetXP( MS_DEX ) );
  query += Misc::Ssprintf( ",c_str_xp='%u'", Skill->GetXP( MS_STR ) );
  query += Misc::Ssprintf( ",c_psi_xp='%u'", Skill->GetXP( MS_PSI ) );
  // ---------------------------------------------
  // SubSkills
  // ---------------------------------------------
  query += Misc::Ssprintf( ",c_mc='%u'", Skill->GetSubSkill( SK_MC ) );
  query += Misc::Ssprintf( ",c_hc='%u'", Skill->GetSubSkill( SK_HC ) );
  query += Misc::Ssprintf( ",c_tra='%u'", Skill->GetSubSkill( SK_TRA ) );
  query += Misc::Ssprintf( ",c_for='%u'", Skill->GetSubSkill( SK_FOR ) );
  query += Misc::Ssprintf( ",c_pc='%u'", Skill->GetSubSkill( SK_PC ) );
  query += Misc::Ssprintf( ",c_rc='%u'", Skill->GetSubSkill( SK_RC ) );
  query += Misc::Ssprintf( ",c_tc='%u'", Skill->GetSubSkill( SK_TC ) );
  query += Misc::Ssprintf( ",c_vhc='%u'", Skill->GetSubSkill( SK_VHC ) );
  query += Misc::Ssprintf( ",c_agl='%u'", Skill->GetSubSkill( SK_AGL ) );
  query += Misc::Ssprintf( ",c_rep='%u'", Skill->GetSubSkill( SK_REP ) );
  query += Misc::Ssprintf( ",c_rec='%u'", Skill->GetSubSkill( SK_REC ) );
  query += Misc::Ssprintf( ",c_rcl='%u'", Skill->GetSubSkill( SK_RCL ) );
  query += Misc::Ssprintf( ",c_atl='%u'", Skill->GetSubSkill( SK_ATL ) );
  query += Misc::Ssprintf( ",c_end='%u'", Skill->GetSubSkill( SK_END ) );
  query += Misc::Ssprintf( ",c_fir='%u'", Skill->GetSubSkill( SK_FIR ) );
  query += Misc::Ssprintf( ",c_enr='%u'", Skill->GetSubSkill( SK_ENR ) );
  query += Misc::Ssprintf( ",c_xrr='%u'", Skill->GetSubSkill( SK_XRR ) );
  query += Misc::Ssprintf( ",c_por='%u'", Skill->GetSubSkill( SK_POR ) );
  query += Misc::Ssprintf( ",c_htl='%u'", Skill->GetSubSkill( SK_HLT ) );
  query += Misc::Ssprintf( ",c_hck='%u'", Skill->GetSubSkill( SK_HCK ) );
  query += Misc::Ssprintf( ",c_brt='%u'", Skill->GetSubSkill( SK_BRT ) );
  query += Misc::Ssprintf( ",c_psu='%u'", Skill->GetSubSkill( SK_PSU ) );
  query += Misc::Ssprintf( ",c_wep='%u'", Skill->GetSubSkill( SK_WEP ) );
  query += Misc::Ssprintf( ",c_cst='%u'", Skill->GetSubSkill( SK_CST ) );
  query += Misc::Ssprintf( ",c_res='%u'", Skill->GetSubSkill( SK_RES ) );
  query += Misc::Ssprintf( ",c_imp='%u'", Skill->GetSubSkill( SK_IMP ) );
  query += Misc::Ssprintf( ",c_ppu='%u'", Skill->GetSubSkill( SK_PPU ) );
  query += Misc::Ssprintf( ",c_apu='%u'", Skill->GetSubSkill( SK_APU ) );
  query += Misc::Ssprintf( ",c_mst='%u'", Skill->GetSubSkill( SK_MST ) );
  query += Misc::Ssprintf( ",c_ppw='%u'", Skill->GetSubSkill( SK_PPW ) );
  query += Misc::Ssprintf( ",c_psr='%u'", Skill->GetSubSkill( SK_PSR ) );
  query += Misc::Ssprintf( ",c_wpw='%u'", Skill->GetSubSkill( SK_WPW ) );
  // ---------------------------------------------

  query += Misc::Ssprintf( " WHERE c_id='%u' LIMIT 1;", mID );

  if ( MySQL->GameQuery( query.c_str() ) )
  {
    Console->Print( RED, BLACK, "PChar::SQLSave could not save char %s (%u) to database", mName.c_str(), mID );
    Console->Print( "Query was:" );
    Console->Print( "%s", query.c_str() );
    MySQL->ShowGameSQLError();
    return false;
  }

  // Chats settings (?), directs

  mDirtyFlag = false;
  return true;
}

bool PChar::SQLDelete()
{
  return true;
}

void PChar::SetOnlineStatus( bool IsOnline )
{
  // Deactivated, until Maxx added c_isonline row to `characters`
  //char query[255];
  int onlinestatus = 0;

  if ( IsOnline )
  {
    onlinestatus = 0; // Strange ????
    mIsOnline = true;
  }
  else
  {
    onlinestatus = 1; // Strange ????
    mIsOnline = false;
  }

//    snprintf(query, 255, "UPDATE charlist SET c_isonline = %d WHERE a_id = %d AND c_id = %d", onlinestatus, mAccount, mID);
//    if(MySQL->Query(query))
//    {
//        Console->Print("Error: Cant set onlinestatus to '%d' for Account: %d, Char: %d", onlinestatus, mAccount, mID);
//        MySQL->ShowSQLError();
//        return;
//    }
  return;
}

uint8_t PChar::GetCombatRank()
{
    // Override for Special Account Levels
    PAccount Acc(mAccount);
    if(Acc.GetLevel() == PAL_ADMIN)
        return 127;
    else if(Acc.GetLevel() >= PAL_GM)
        return 120;
    else if(Acc.GetLevel() >= PAL_VOLUNTEER)
        return 50;
    else
        return mCombatRank;
}


uint8_t PChar::GetMainRank()
{
    // Override for Special Account Levels
    PAccount Acc(mAccount);
    if(Acc.GetLevel() == PAL_ADMIN)
        return 127;
    else if(Acc.GetLevel() >= PAL_GM)
        return 120;
    else if(Acc.GetLevel() >= PAL_VOLUNTEER)
        return 50;
    else
    {
      uint16_t total;
      total  = Skill->GetMainSkill( MS_STR ) + Skill->GetMainSkill( MS_DEX );
      total += Skill->GetMainSkill( MS_CON ) + Skill->GetMainSkill( MS_INT );
      total += Skill->GetMainSkill( MS_PSI );
      return (( uint8_t )( total / 5 ) );
    }
}

bool PChar::AddGenrep(uint16_t nWorldID, uint16_t nStationID)
{
    return mGenrepList->AddGenrep(nWorldID, nStationID );
}

uint16_t PChar::GetGenrepListDataSize()
{
    return mGenrepList->GetListDataSize();
}

const void *PChar::GetGenrepListData()
{
    return mGenrepList->GetListData();
}

uint8_t PChar::GetGenrepCount()
{
    return mGenrepList->Count();
}

uint32_t PChar::AddCash( uint32_t nAmount )
{
    return SetCash(nAmount + mCash);
}

uint32_t PChar::TakeCash( uint32_t nAmount )
{
    if(nAmount > mCash)
    {
        //Tries to take away more cash that user has, set to zero
        return SetCash(0);
    }
    else
    {
        return SetCash(mCash-nAmount);
    }
}

uint32_t PChar::SetCash( uint32_t nCash )
{
  //Console->Print("Trying to set cash to nCash: %d", nCash);
  if (( int )nCash > Config->GetOptionInt( "max_cash" ) )
  {
    //Console->Print("Newcash would be more than dynamic maxcash, setting to maxcash");
    mCash = ( uint32_t )Config->GetOptionInt( "max_cash" );
  }
  else if ( nCash > MAXCASH )
  {
    //Console->Print("Newcash would be more than hardcoded maxcash, setting to maxcash");
    mCash = MAXCASH;
  }
  else
  {
    //Console->Print("Allright, setting to new value");
    mCash = nCash;
  }
  //Console->Print("Returning mCash: %d", mCash);
  return mCash;
}


bool PChar::SetQuickBeltActiveSlot( uint8_t nSlotID )
{
  if (( nSlotID == INV_WORN_QB_HAND ) || ( nSlotID == INV_WORN_QB_NONE ) )
  {
    mQuickBeltActiveSlot = nSlotID;
    return true;
  }
  else if ( nSlotID <= ( INV_WORN_QB_END - INV_WORN_QB_START ) )
  {
    PContainer* tWorn = mInventory.GetContainer( INV_LOC_WORN );

    if ( ! tWorn->IsSlotFree( nSlotID ) ) // => TODO: MUST ALSO CHECK that item is currently usable and can be held in hand
    {
      mQuickBeltActiveSlot = nSlotID;
      return true;
    }
    else
    {
      Console->Print( "SetQuickBeltActiveSlot: SlotID %d greater than %d or free (%d)", nSlotID, INV_WORN_QB_END - INV_WORN_QB_START, tWorn->IsSlotFree( nSlotID ) );
    }
  }
  return false;
}

PSeatType PChar::GetSeatInUse( uint32_t* nObjectId, uint8_t* nSeatId )
{
  if ( nObjectId )
  {
    *nObjectId = mSeatInUseObjectId;
  }
  if ( nSeatId )
  {
    *nSeatId = mSeatInUseSeatId;
  }

  return mSeatInUseType;
}

void PChar::SetSeatInUse( PSeatType nSeatType, uint32_t nObjectId, uint8_t nSeatId )
{
  mSeatInUseType = nSeatType;
  mSeatInUseObjectId = nObjectId;
  mSeatInUseSeatId = nSeatId;
}

PVhcAccessRequestList* PChar::GetVhcAccessRequestList()
{
  if( ! mVhcAccessRequestList )
    mVhcAccessRequestList = new PVhcAccessRequestList();

  return mVhcAccessRequestList;
}

void PChar::SetLookingAt( uint16_t nLocalCharID )
{
  mLookingAt = nLocalCharID;
  mLookAtTimestamp = Time::nowTimeT();
}

uint16_t PChar::GetLookingAt( uint16_t nMaxDelaySec )
{
  return ((( mLookAtTimestamp + nMaxDelaySec ) >= Time::nowTimeT() ) ? mLookingAt : 0 );
}

// ===================================

PChars::PChars()
{
  mLastID = 0;
  mLastSave = Time::nowTimeT();

  mAutoSavePeriod = Config->GetOptionInt( "auto_save_period" );
  if ( mAutoSavePeriod < 0 )
  {
    Console->Print( "%s auto_save_period (%d) must be strict positive.", Console->ColorText( RED, BLACK, "[Error]" ), mAutoSavePeriod );
    mAutoSavePeriod = 0;
  }
  else if ( mAutoSavePeriod > 3600 )
  {
    Console->Print( "%s auto_save_period (%d) too high. Forced to 3600 sec.", Console->ColorText( YELLOW, BLACK, "[Warning]" ), mAutoSavePeriod );
    mAutoSavePeriod = 0;
  }

  if ( mAutoSavePeriod == 0 )
  {
    Console->Print( "%s Auto-save disabled.", Console->ColorText( YELLOW, BLACK, "[Info]" ) );
  }
  else if ( mAutoSavePeriod < 60 )
  {
    Console->Print( "%s auto_save_period (%d) is low and might lead to high server load.", Console->ColorText( YELLOW, BLACK, "[Warning]" ), mAutoSavePeriod );
  }

}

PChars::~PChars()
{
  for ( CharMap::iterator i = mChars.begin(); i != mChars.end(); i++ )
    delete i->second;
}

bool PChars::LoadChar( uint32_t CharID )
{
  if ( !CharID )
    return false;

  PChar *nChar = new PChar();
  if ( nChar->SQLLoad( CharID ) )
  {
    nChar->SetDirtyFlag( false );
    return AddChar( nChar );
  }
  else
  {
    Console->Print( RED, BLACK, "[ERROR] Could not load char id %d from database", CharID );
    return false;
  }
}

bool PChars::AddChar( PChar* nChar )
{
  if ( !nChar )
    return false;

  mLastID = std::max( mLastID, nChar->GetID() );
  if ( mChars.insert( std::make_pair( nChar->GetID(), nChar ) ).second )
  {
    if ( gDevDebug )
      Console->Print( "%s Char: %s (id %d) added", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nChar->GetName().c_str() , nChar->GetID() );
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[ERROR] Trying to load char twice : %s (id %d)", nChar->GetName().c_str(), nChar->GetID() );
    return false;
  }
}

PChar* PChars::RemoveChar( uint32_t CharID )
{
  PChar* Result = nullptr;

  CharMap::iterator i = mChars.find( CharID );
  if ( i != mChars.end() )
  {
    Result = i->second;
    mChars.erase( i );
    if ( gDevDebug )
      Console->Print( "%s Char: %s (id %d) removed", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), Result->GetName().c_str() , Result->GetID() );
  }

  return Result;
}

void PChars::SQLSave()
{
  // saves all dirty-flagged chars
  int nDirtyChars = 0, nSavedChars = 0;
  int nDirtyInv = 0, nSavedInv = 0;

  for ( CharMap::const_iterator i = mChars.begin(); i != mChars.end(); i++ )
  {
    PChar* Char = i->second;
    if ( Char->IsDirty() )
    {
      ++nDirtyChars;
      if ( Char->SQLSave() )
        ++nSavedChars;
    }
    if ( Char->GetInventory()->IsDirty() )
    {
      ++nDirtyInv;
      if ( Char->GetInventory()->SQLSave() )
        ++nSavedInv;
    }
  }
  Console->Print( "%s %i chars saved on %i dirty, %i inventories saved on %i dirty", Console->ColorText( GREEN, BLACK, "[DEBUG]" ), nSavedChars, nDirtyChars, nSavedInv, nDirtyInv );
  return;
}

PChar* PChars::GetChar( uint32_t CharID ) const
{
  PChar *Result = 0;
  CharMap::const_iterator i = mChars.find( CharID );
  if ( i != mChars.end() )
    Result = i->second;

  return Result;
}

PChar* PChars::GetChar( const std::string &Name ) const
{
  PChar *Result = 0;
  for ( CharMap::const_iterator i = mChars.begin(); i != mChars.end(); i++ )
  {
    if ( !/*std::*/strcasecmp( i->second->GetName().c_str(), Name.c_str() ) )
    {
      Result = i->second;
      break;
    }
  }
  return Result;
}

bool PChars::CharExist( const std::string &Name ) const
{
    std::string query;
    int EntriesNb;
    MYSQL_RES *result = 0;

    char escUsername[256];
    MySQL->EscapeString( Name.c_str(), escUsername, 256 );
    query.append("SELECT 1 FROM characters WHERE c_name = '").append(escUsername)
         .append("' LIMIT 1;");

    result = MySQL->GameResQuery(query.c_str());
    if (result == nullptr)
    {
        Console->Print( RED, BLACK, "[ERROR] Failed to get CharacterData from SQL" );
        MySQL->ShowGameSQLError();
        return true;
    }

    EntriesNb = mysql_num_rows( result );
    MySQL->FreeGameSQLResult( result );
    return( EntriesNb > 0 );
}

void PChars::Update()
{
  time_t t = Time::nowTimeT(); // changed to time() to have real time instead of cpu used time

  if ( mAutoSavePeriod && (( t - mLastSave ) >= mAutoSavePeriod ) )
  {
    bool NeedSave = false;
    for ( CharMap::const_iterator i = mChars.begin(); i != mChars.end(); i++ )
    {
      if ( i->second->IsAnyDirty() )
      {
        NeedSave = true;
        break;
      }
    }

    if ( NeedSave )
    {
      if ( gDevDebug ) Console->Print( "%s Some characters need autosaving...", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      SQLSave();
      if ( gDevDebug ) Console->Print( "%s Autosave done.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    }
    mLastSave = t;
  }
}

//MAX_CHARS_PER_ACCOUNT
int PChars::GetCharProfiles( const uint32_t AccountID, PCharProfile* CharSlotsArray, const uint8_t ArraySize )
{
  char query[256];
  int EntriesNb = 0;

  MYSQL_ROW row = 0;
  MYSQL_RES *result = 0;

  snprintf( query, 256, "SELECT * FROM characters WHERE a_id = %d ORDER BY c_slot ASC", AccountID );

  result = MySQL->GameResQuery( query );
  if (result == nullptr)
  {
    Console->Print( RED, BLACK, "[ERROR] Failed to load CharacterData from SQL" );
    MySQL->ShowGameSQLError();
    return 0;
  }

  //EntriesNb = mysql_num_rows(result);
  int SlotID;
  uint32_t CharID;
  PChar* tmpChar = new PChar();

  while (( row = mysql_fetch_row( result ) ) )
  {
    SlotID = atoi( row[c_slot] );
    CharID = atoi( row[c_id] );
    if (( SlotID >= 0 ) && ( SlotID < ArraySize ) )
    {
      if ( !CharSlotsArray[SlotID].in_use )
      {
        tmpChar->SetID( CharID );
        tmpChar->SetGender( atoi( row[c_sex] ) );
        tmpChar->SetProfession( atoi( row[c_profession] ) );

        CharSlotsArray[SlotID].CharID = CharID;
        CharSlotsArray[SlotID].Type = tmpChar->GetType();
        CharSlotsArray[SlotID].Location = static_cast<uint32_t>( atoi( row[c_location] ) );
        CharSlotsArray[SlotID].Head = atoi( row[c_head] );
        CharSlotsArray[SlotID].Torso = atoi( row[c_torso] );
        CharSlotsArray[SlotID].Legs = atoi( row[c_legs] );
        CharSlotsArray[SlotID].Name = row[c_name];
        CharSlotsArray[SlotID].NameLen = CharSlotsArray[SlotID].Name.length() + 1;

        CharSlotsArray[SlotID].in_use = true;
        ++EntriesNb;
      }
      else
      {
        Console->Print( YELLOW, BLACK, "[Warning]Character %d using slot %d already used by char %d - Ignored", \
                        CharID, SlotID, CharSlotsArray[SlotID].CharID );
      }
    }
    else
    {
      Console->Print( YELLOW, BLACK, "[Warning]Character %d using invialid slot %d - Ignored", CharID, SlotID );
    }
  }

  delete tmpChar;
  MySQL->FreeGameSQLResult( result );
  return EntriesNb;
}
