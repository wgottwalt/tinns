#include "gameserver/definitions/Includes.hxx"

PDefDrug::PDefDrug()
{
}

bool PDefDrug::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 5;
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
        break;
      case 4:
        mDuration = atoi( i->c_str() ); break;
      case 5 :
      {
        mChangeNum = atoi( i->c_str() );
        if ( mChangeNum > 8 )
          mChangeNum = 8;
        maxFields = 5 + 3 * mChangeNum;
        break;
      }
      default :
        if (( Idx >= 6 ) && ( Idx <= maxFields ) )
        {
          switch (( Idx - 6 ) % 3 )
          {
            case 0:
              mChangeType[( Idx - 6 ) / 3] = atoi( i->c_str() ); break;
            case 1:
              mChangeScale[( Idx - 6 ) / 3] = atof( i->c_str() ); break;
            case 2:
              mChangeTarget[( Idx - 6 ) / 3] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return (( Idx == maxFields ) );
}
