#include "gameserver/definitions/Includes.hxx"

PDefCharAction::PDefCharAction()
{
}

bool PDefCharAction::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 2;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
      {
        mNumOfSsq = atoi( i->c_str() );
        if ( mNumOfSsq > 8 )
          mNumOfSsq = 8;
        maxFields = 2 + 2 * mNumOfSsq;
        break;
      }
      default :
        if (( Idx >= 3 ) && ( Idx <= maxFields ) )
        {
          if ( Idx & 1 )
          {
            mSsqId[( Idx - 3 ) / 2] = atoi( i->c_str() );
          }
          else
          {
            mModFactor[( Idx - 3 ) / 2] = atof( i->c_str() );
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return (( Idx == maxFields ) );
}
