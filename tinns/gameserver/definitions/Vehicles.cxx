#include "gameserver/definitions/Includes.hxx"

PDefVhc::PDefVhc()
{
    mNumSeats = mArmor = mHealth = mModel = 0;
    mName = "undefined";
    for(int i=0; i<8; ++i)
      mSeatId[i] = -1;
}

bool PDefVhc::LoadFromDef( PTokenList *Tokens )
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
      case 3 :
        mName = *i; break;
      case 34 :
        mHealth = atoi( i->c_str() ); break;
      case 35 :
        mArmor = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 16) && (Idx <= 23) )
        {
          mSeatId[Idx - 16] = atoi( i->c_str() );
          if(mSeatId[Idx - 16] >= 0) // In theroy, we should check that it is a valid VhcSeat Index
            ++mNumSeats;
        }
        break;
    }

    if ( Idx >= 36 )
      break;
  }

  return ((Idx >= 35));
}
