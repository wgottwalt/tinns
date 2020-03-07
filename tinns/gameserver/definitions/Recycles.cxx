#include "gameserver/definitions/Includes.hxx"

PDefRecycle::PDefRecycle()
{
}

bool PDefRecycle::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 4;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        //mIndex = atoi ( i->c_str() );
        break;
      case 2 : // We use resultitem as index
        mIndex = mResultItemId = atoi ( i->c_str() ); break;
      case 3:
        mBuildTime = atoi ( i->c_str() ); break;
      case 4 :
      {
        mNumParts = atoi ( i->c_str() );
        if ( mNumParts > 8 )
          mNumParts = 8;
        maxFields = 4 + mNumParts;
        break;
      }
      default :
        if ( ( Idx >= 5 ) && ( Idx <= maxFields ) )
        {
              mPartId[ Idx - 5 ] = atoi ( i->c_str() ); break;
        }
        break;
    }

    if ( Idx >= 12 )
      break;
  }

  return ( ( Idx >= 5 ) );
}
