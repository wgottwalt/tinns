#include "gameserver/definitions/Includes.hxx"

PDefVhcSeat::PDefVhcSeat()
{
  mTL = mWeaponId = mType = 0;
  mName = "undefined";
  for(int i=0; i<3; ++i)
    mLeaveAngle[i] = mLeavePos[i] = 0;
  mDamageFactor = 0;
}

bool PDefVhcSeat::LoadFromDef( PTokenList *Tokens )
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
        mType = atoi( i->c_str() ); break;
      case 3 :
        mName = *i; break;
      case 30 :
        mWeaponId = atoi( i->c_str() ); break;
      case 31 :
        mTL = atoi( i->c_str() ); break;
      case 32 :
        mDamageFactor = atof( i->c_str() ); break;
      default :
        if( (Idx >= 10) && (Idx <= 12) )
        {
          mLeavePos[Idx - 10] = atof( i->c_str() );
        }
        else if( (Idx >= 13) && (Idx <= 15) )
        {
          mLeaveAngle[Idx - 13] = atof( i->c_str() );
        }
        break;
    }

    if ( Idx >= 34 )
      break;
  }

  return ((Idx >= 34));
}
