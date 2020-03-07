#include "gameserver/definitions/Includes.hxx"

bool PDefImplant::LoadFromDef( PTokenList *Tokens )
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
      case 3:
        mDuration = atoi( i->c_str() ); break;
      case 4 :
      {
        mChangeNum = atoi( i->c_str() );
        if ( mChangeNum > 8 )
          mChangeNum = 8;
        maxFields = 4 + 3 * mChangeNum;
        break;
      }
      default :
        if (( Idx >= 5 ) && ( Idx <= maxFields ) )
        {
          switch (( Idx - 5 ) % 3 )
          {
            case 0:
              mChangeType[( Idx - 5 ) / 3] = atoi( i->c_str() ); break;
            case 1:
              mChangeScale[( Idx - 5 ) / 3] = atof( i->c_str() ); break;
            case 2:
              mChangeTarget[( Idx - 5 ) / 3] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return (( Idx == maxFields ) );
}
