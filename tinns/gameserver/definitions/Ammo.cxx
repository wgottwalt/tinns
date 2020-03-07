#include "gameserver/definitions/Includes.hxx"

PDefAmmo::PDefAmmo()
{
  mShotId = mMagSize = mWeaponShotId = mDamageId = 0;
}

bool PDefAmmo::LoadFromDef( PTokenList *Tokens )
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
        mDamageId = atoi( i->c_str() ); break;
      case 3 :
        mWeaponShotId = atoi( i->c_str() ); break;
      case 4 :
        mMagSize = atoi( i->c_str() ); break;
      case 5 :
        mShotId = atoi( i->c_str() ); break;
      default :
        break;
    }

    if ( Idx >= 5 )
      break;
  }

  return ((Idx >= 4));
}
