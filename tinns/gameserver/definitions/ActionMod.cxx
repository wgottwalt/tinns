#include "gameserver/definitions/Includes.hxx"

PDefActionMod::PDefActionMod()
{
}

bool PDefActionMod::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 3;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        mStartValue = atof( i->c_str() ); break;
      case 3 :
      {
        mNumOfSsq = atoi( i->c_str() );
        if( mNumOfSsq > 8 )
          mNumOfSsq = 8;
        maxFields = 3 + 2 * mNumOfSsq;
        break;
      }
      default :
        if( (Idx >= 4) && (Idx <= maxFields) )
        {
          if(Idx & 1)
          {
            mModFactor[(Idx - 4) / 2] = atof( i->c_str() );
          }
          else
          {
            mSsqId[(Idx - 4) / 2] = atoi( i->c_str() );
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return ((Idx == maxFields));
}
