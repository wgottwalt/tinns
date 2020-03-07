#include "gameserver/definitions/Includes.hxx"

PDefItemMod::PDefItemMod()
{
  for ( int i = 0; i < 4; ++i )
  {
    mChangeTarget[i] = 0;
    mChangeScale[i] = mChangeValue[0] = 0;
  }
}

bool PDefItemMod::LoadFromDef ( PTokenList *Tokens )
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
        mIndex = atoi ( i->c_str() ); break;
      case 2 :
        mType = atoi ( i->c_str() ); break;
      case 3:
        break;
      case 4 :
      {
        mChangeNum = atoi ( i->c_str() );
        if ( mChangeNum > 4 )
          mChangeNum = 4;
        maxFields = 4 + 3 * mChangeNum;
        break;
      }
      case 17 :
        mName = *i; break;
      default :
        if ( ( Idx >= 5 ) && ( Idx <= maxFields ) )
        {
          switch ( ( Idx - 5 ) % 3 )
          {
            case 0:
              mChangeTarget[ ( Idx - 5 ) / 3] = atoi ( i->c_str() ); break;
            case 1:
              mChangeValue[ ( Idx - 5 ) / 3] = atof ( i->c_str() ); break;
            case 2:
              mChangeScale[ ( Idx - 5 ) / 3] = atof ( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= 18 )
      break;
  }

  return ( ( Idx >= 17 ) );
}
