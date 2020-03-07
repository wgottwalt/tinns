#include "gameserver/definitions/Includes.hxx"

PDefOutpost::PDefOutpost()
{
  for ( int i = 0;  i < 8; ++i )
    mInfluenceZone[i] = 0;
}

bool PDefOutpost::LoadFromDef ( PTokenList *Tokens )
{
  int Idx=0;
  for ( PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0: // setentry
        break;
      case 1: // index
        mIndex = atoi ( i->c_str() ); break;
      case 2:
        mName = *i; break;
      case 3:
        mType = atoi ( i->c_str() ); break;
      case 4:
        mStandardFaction = atoi ( i->c_str() ); break;
      case 5:
        mRevenue = atof ( i->c_str() ); break;
      case 6:
        mConquestReward = atof ( i->c_str() ); break;
      case 7:
        mMaxSecurity = atoi ( i->c_str() ); break;
      default :
        if ( ( Idx - 8 ) < 8 )
        {
          mInfluenceZone[Idx - 8] = atoi ( i->c_str() );
        }
        break;
    }

    if ( Idx == 15 )
      break;
  }

  return ( Idx >= 7 );
}
