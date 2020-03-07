#include "gameserver/definitions/Includes.hxx"

PDefTrader::PDefTrader()
{
  mQuality = mType = 0;
  mWealthRespawn = mMaxWealth = 0;
  for(int i=0; i<DEF_TRADER_MAXENTRIES; ++i)
  {
      mItemId[i] = 0;
      mItemPriceScale[i] = 0;
  }
}

bool PDefTrader::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 5 + 2 * DEF_TRADER_MAXENTRIES;

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
        mMaxWealth = atof( i->c_str() ); break;
      case 4 :
        mWealthRespawn = atof( i->c_str() ); break;
      case 5 :
        mQuality = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 6) && (Idx <= maxFields) )
        {
          if(Idx & 1)
          {
            if(i->find_first_of('/') == std::string::npos) // Take care of the "1/5" format
            {
              mItemPriceScale[int((Idx - 6) / 2)] = atof( i->c_str() );
            }
            else
            {
              mItemPriceScale[int((Idx - 6) / 2)] = 1; // Workaround until we know what to do with "1/5"
            }
          }
          else
          {
            mItemId[int((Idx - 6) / 2)] = atoi( i->c_str() );
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return ((Idx >= 7)); // We suppose there must be at least one item entry
}
