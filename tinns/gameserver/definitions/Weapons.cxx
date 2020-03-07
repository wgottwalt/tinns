#include "gameserver/definitions/Includes.hxx"

PDefWeapon::PDefWeapon()
{
  mIndex = -1;
  mName = "unknown";
  mItemIndex = mAmmoUse = mPSIuse = mWeaponType = mDiscardable = mMaxRange = 0;
  mAggressiveWeapon = mAmmoStartFlags = mShotCnt = mBaseWeaponId = 0;
  mAmmoTypes[0] = mAmmoTypes[1] = mAmmoTypes[2] = mAmmoTypes[3] = 0;
  mAmmoTypes[4] = mAmmoTypes[5] = mAmmoTypes[6] = mAmmoTypes[7] = 0;
  mItemModGroup = mItemModGroupFlags = mRareWeaponFx = 0;
  mStaminaUse = mSkillFactor = mDamageMultiplicator = 0;
}

bool PDefWeapon::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        continue;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        mName = *i; break;
      case 3 :
        continue; //int mFpsmodel;
      case 4 :
        continue; //int mAttachmodel;
      case 5 :
        continue; //int mMunactor;
      case 6 :
        continue; //float mDroptime; // useful ?
      case 7 :
        mItemIndex = atoi( i->c_str() ); break;
      case 8 :
      case 9 :
      case 10 :
        continue; //int effectcolor[3]; // R,G,B
      case 11 :
        continue; //int dynamiclight;
      case 12 :
        continue; //float lighttime;
      case 13 :
        continue; //int soundindex;
      case 14 :
        continue; //float mShotTime; // useful ?
      case 15 :
        mAmmoUse = atoi( i->c_str() ); break;
      case 16 :
        mPSIuse = atoi( i->c_str() ); break;
      case 17 :
        mStaminaUse = atof( i->c_str() ); break;
      case 18 :
        continue; //float upthrow;
      case 19 :
        continue; //int weaponHold; // ???
      case 20 :
        mWeaponType = atoi( i->c_str() ); break;
      case 21 :
        mDiscardable = atoi( i->c_str() ); break;
      case 22 :
        mSkillFactor = atof( i->c_str() ); break;
      case 23 :
        continue; //int mintgtRad;
      case 24 :
        continue; //int maxtgtRad;
      case 25 :
        mMaxRange = atoi( i->c_str() ); break;
      case 26 :
        mAggressiveWeapon = atoi( i->c_str() ); break;
      case 27 :
        mDamageMultiplicator = atof( i->c_str() ); break;
      case 28 :
      case 29 :
      case 30 :
      case 31 :
      case 32 :
      case 33 :
      case 34 :
      case 35 :
        mAmmoTypes[Idx - 28] = atoi( i->c_str() ); break; // If mAmmoTypes[0] < 0 => =-damageId, no ammo
      case 36 :
        mAmmoStartFlags = atoi( i->c_str() ); break; // ???
      case 37 :
        continue; //int customclasstype; // ?
      case 38 :
        continue; //int unknown // ?
      case 39 :
        mShotCnt = atoi( i->c_str() ); break;
      case 40 :
        continue; //float shotduration; // maybe useful later
      case 41 :
        continue; //std::string shotfx;
      case 42 :
        continue; //float attachposx;
      case 43 :
        continue; //float attachposy;
      case 44 :
        continue; //float attachposz;
      case 45 :
        continue; //float fpposx;
      case 46 :
        continue; //float fpposy;
      case 47 :
        continue; //float fpposz;
      case 48 :
        mBaseWeaponId = atoi( i->c_str() ); break;
      case 49 :
        continue; //int weaponcolor;
      case 50 :
        continue; //int reloadsound;
      case 51 :
        mItemModGroup = atoi( i->c_str() ); break;
      case 52 :
        mItemModGroupFlags = atoi( i->c_str() ); break;
      case 53 :
        mRareWeaponFx = atoi( i->c_str() ); break;
    }

    if ( Idx >= 53 )
      break;
  }

  return (( Idx == 52 ) || ( Idx == 53 ) );
}
