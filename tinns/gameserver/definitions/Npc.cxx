#include "gameserver/definitions/Includes.hxx"

PDefNpc::PDefNpc()
{
  mFlags = 0;
}

bool PDefNpc::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        mModel = atoi( i->c_str() ); break;
      case 4 :
        mNGT = atoi( i->c_str() ); break;
      case 5 :
        mGender = atoi( i->c_str() ); break;
      case 6 :
        mFaction = atoi( i->c_str() ); break;
      case 7 :
        mHealth = atoi( i->c_str() ); break;
      case 8 :
        mArmorId = atoi( i->c_str() ); break;
      case 9 :
        mWeaponId = atoi( i->c_str() ); break;
      case 10 :
        mDialogScript = *i;
        Misc::String::cleanUpString(mDialogScript);
        break;
      case 11 :
        mCombat = atoi( i->c_str() ); break;
      case 12 :
        mLoot = atoi( i->c_str() ); break;
      case 13 :
        mMovementEnd = atoi( i->c_str() ); break;
      case 14 :
        mFunctionType = atoi( i->c_str() ); break;
      case 15 :
        mModelScaling = atof( i->c_str() ); break;
      case 16 :
        mMoneyLoose = atoi( i->c_str() ); break;
      case 17 :
        mSkillScale = atof( i->c_str() ); break;
      case 18 :
        mStandardScript = *i;
        Misc::String::cleanUpString(mStandardScript);
        break;
      case 19 :
        mStandardParameter = *i;
        Misc::String::cleanUpString(mStandardParameter);
        break;
      case 20 :
        mMass = atoi( i->c_str() ); break;
      case 24 :
        mFlags = atoi( i->c_str() ); break;
      default :
        break;
    }

    if ( Idx >= 24 )
      break;
  }

  return ((Idx >= 20));
}
