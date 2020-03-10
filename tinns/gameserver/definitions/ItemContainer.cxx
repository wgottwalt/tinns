#include "gameserver/definitions/Includes.hxx"

PDefItemContainer::PDefItemContainer()
{
  mNumItems = 0;
  for(int i=0; i<6; ++i)
  {
      mChance[i] = mItemId[i] = mCumulatedChance[i] = 0;
      mQuality[i] = 0;
  }
}

bool PDefItemContainer::LoadFromDef( PTokenList *Tokens )
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
        mNumItemsAtOnce = atoi( i->c_str() ); break;
      case 3 :
        mRespawnTime = atoi( i->c_str() ); break;
      case 4 :
        mNumItems = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 5) && (Idx <= 22) )
        {
          switch ( ((Idx-5) % 3) )
          {
            case 0:
              mItemId[int((Idx - 5)/3)] = atoi( i->c_str() ); break;
            case 1:
              mQuality[int((Idx - 5)/3)] = atof( i->c_str() ); break;
            case 2:
              mChance[int((Idx - 5)/3)] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= 22 )
      break;
  }

  if (Idx >= 22)
  {
    BuildCumulatedChance();
    return true;
  }
  else
    return false;
}

void PDefItemContainer::BuildCumulatedChance()
{
  mCumulatedChance[0] = mChance[0];
  for(int i = 1; i < mNumItems; ++i)
  {
    mCumulatedChance[i] = mCumulatedChance[i-1] + mChance[i];
  }
}

int PDefItemContainer::GetRandomItemIdx() const
{
  int i = -1;

  if(mNumItems > 0)
  {
    int randomValue = Misc::Random::get(mCumulatedChance[mNumItems-1], 1);
    for(i = 0; i < mNumItems; ++i)
    {
      if(randomValue <= mCumulatedChance[i])
        break;
    }
  }
  return i;
}
