#include "gameserver/definitions/Includes.hxx"

#define GAMEDEFS_DEFITEMSMAXSEQ 100

PDefItems::PDefItems()
{
  //mIndex = 0;
  mModel = 0;
  mType = 0;
  mValue1 = 0;
  mValue2 = 0;
  mValue3 = 0;
  //mBmNum = 0;
  //mmBmNumIndex = 0;
  mSizeX = 1;
  mSizeY = 1;
  //mSmallbmnum = 0;
  mWeight = 0;
  mStackable = 0;
  mFillWeight = 0;
  mQualifier = 0;
  mGfxMods = 0;
  mItemGroupID = 0;
  mTextDescID = 0;
  mBasePrice = 0;
  mTechlevel = 0;
  mItemflags = 0;
  // std::mShortname = "";
}

bool PDefItems::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0: // setentry
        continue;
      case 1: // index
        mIndex = atoi( i->c_str() ); break;
      case 2: // name
        mName = *i; break;
      case 3:
        mModel = atoi(i->c_str()); // used for IG display
        break;
      case 4:
        mType = atoi( i->c_str() ); break;
      case 5:
        mValue1 = atoi( i->c_str() ); break;
      case 6:
        mValue2 = atoi( i->c_str() ); break;
      case 7:
        mValue3 = atoi( i->c_str() ); break;
      case 8:
        // mBmNum = atoi(i->c_str()); // used IG for inventory display
        break;
      case 9:
        // mmBmNumIndex = atoi(i->c_str()); // used IG for inventory display
        break;
      case 10:
        mSizeX = atoi( i->c_str() ); break;
      case 11:
        mSizeY = atoi( i->c_str() ); break;
      case 12:
        // mSmallbmnum = atoi(i->c_str()); // used IG for inventory display
        break;
      case 13:
        mWeight = atof( i->c_str() ); break;
      case 14:
        mStackable = atoi( i->c_str() ); break;
      case 15:
        mFillWeight = atof( i->c_str() ); break;
      case 16:
        mQualifier = atoi( i->c_str() ); break;
      case 17:
        mGfxMods = atoi( i->c_str() ); break;
      case 18:
        mItemGroupID = atoi( i->c_str() ); break;
      case 19:
        mTextDescID = atoi( i->c_str() ); break;
      case 20:
        mBasePrice = atoi( i->c_str() ); break;
      case 21:
        mTechlevel = atoi( i->c_str() ); break;
      case 22:
        mItemflags = atoi( i->c_str() ); break;
      case 23:
        // mShortname = *i; // used IG for display on inventories Icons
        break;
    }
  }

  return true;
}


PDefItemsMap::PDefItemsMap()
{
  mMapItCache = nullptr;
  mMapItCacheCount = 0;
  mMaxItemGroupId = -1;
}

PDefItemsMap::~PDefItemsMap()
{
  delete [] mMapItCache;
}

void PDefItemsMap::BuildMapItCache()
{
  int CacheEntryIdx = 0;
  int EntrySeqIdx = 1;

  mMapItCacheCount = 1 + ( mDefs.size() / GAMEDEFS_DEFITEMSMAXSEQ );

  if ( mMapItCacheCount )
  {
    if( mMapItCache )
    {
      delete [] mMapItCache;
    }
    mMapItCache = new std::map<int, PDefItems*>::const_iterator[mMapItCacheCount];
    mMapItCache[CacheEntryIdx++] = mDefs.begin();

    std::map<int, PDefItems*>::const_iterator It = mDefs.begin();
    while ( It != mDefs.end() )
    {
      if ( EntrySeqIdx++ == GAMEDEFS_DEFITEMSMAXSEQ )
      {
        mMapItCache[CacheEntryIdx++] = It;
        EntrySeqIdx = 1;
      }
      It++;
    }
  }
}

void PDefItemsMap::BuildItemGroups()
{
  //std::map<int, std::vector<int> > mItemGroups;
  // Implementation more complicated than needed but avoid too many realloc
  
  // Group size conting
  std::map<int, int> groupSize;
  for ( std::map<int, PDefItems*>::const_iterator i = mDefs.begin(); i != mDefs.end(); i++ )
    groupSize[ i->second->GetItemGroupID() ]++;

  // Item group vectors size reservation
  mItemGroups.clear();
  for ( std::map<int, int>::const_iterator i = groupSize.begin(); i != groupSize.end(); i++ )
  {
    mItemGroups[i->first].reserve(i->second);
    if(i->first > mMaxItemGroupId)
      mMaxItemGroupId = i->first;
    //Console->Print("Item group %d : %d items", i->first, i->second);
  }

  // Effective groups building
  for ( std::map<int, PDefItems*>::const_iterator i = mDefs.begin(); i != mDefs.end(); i++ )
    mItemGroups[ i->second->GetItemGroupID() ].push_back(i->first); // i->first is ItemIndex
}

bool PDefItemsMap::Load(const char* nName, const char* nFilename)
{
  if( PDefMap<PDefItems>::Load( nName, nFilename) )
  {
    BuildMapItCache();
    BuildItemGroups();
    return ( true );
  }
  else
    return ( false );
}

const PDefItems* PDefItemsMap::GetDefBySeqIndex( int nSeqIndex ) const
{
  int CacheEntryIdx = nSeqIndex / GAMEDEFS_DEFITEMSMAXSEQ;
  if ( CacheEntryIdx >= mMapItCacheCount )
    return nullptr;

  std::map<int, PDefItems*>::const_iterator It = mMapItCache[CacheEntryIdx];
  int EntrySeqIdx = CacheEntryIdx * GAMEDEFS_DEFITEMSMAXSEQ;

  while (( EntrySeqIdx < nSeqIndex ) && ( It != mDefs.end() ) )
  {
    EntrySeqIdx++;
    It++;
  }

  if (( EntrySeqIdx == nSeqIndex ) && ( It != mDefs.end() ) )
  {
    return It->second;
  }
  else
    return nullptr;
}

int PDefItemsMap::GetRandomItemIdFromGroup( int nGroupId ) const
{
  if( (nGroupId >= 0) && (nGroupId <= mMaxItemGroupId) )
  {
    std::map<int, std::vector<int> >::const_iterator selectedEntry = mItemGroups.find(nGroupId);
    if(selectedEntry != mItemGroups.end())
    {
      int groupSize = selectedEntry->second.size();
      if( groupSize )
      {
        return selectedEntry->second[Misc::GetRandom(groupSize - 1, 0)];
      }
    }
  }

  return 0;
}
