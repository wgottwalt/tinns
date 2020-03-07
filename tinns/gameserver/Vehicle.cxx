#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

// PVhcCoordinates
void PVhcCoordinates::SetInterpolate( const PVhcCoordinates& Pos1, const PVhcCoordinates& Pos2, float nCoef )
{
  if (( nCoef < 0 ) || ( nCoef > 1 ) )
  {
    Console->Print( RED, BLACK, "[Error] PVhcCoordinates::Interpolate : Invalid nCoef value: %f", nCoef );
    nCoef = 0;
  }
  float rCoef = 1 - nCoef;

  mY = ( uint16_t )( rCoef * Pos1.mY + nCoef * Pos2.mY );
  mZ = ( uint16_t )( rCoef * Pos1.mZ + nCoef * Pos2.mZ );
  mX = ( uint16_t )( rCoef * Pos1.mX + nCoef * Pos2.mX );
  mUD = ( uint8_t )( rCoef * Pos1.mUD + nCoef * Pos2.mUD );
  mLR = ( uint16_t )( rCoef * Pos1.mLR + nCoef * Pos2.mLR );
  mRoll = ( uint16_t )( rCoef * Pos1.mRoll + nCoef * Pos2.mRoll );
}

void PVhcCoordinates::SetPosition( uint16_t nY, uint16_t nZ, uint16_t nX, uint8_t nUD, uint16_t nLR, uint16_t nRoll, uint8_t nAct, uint16_t nUnknown, uint8_t nFF )
{
  mY = nY;
  mZ = nZ;
  mX = nX;
  mUD = nUD;
  mLR = nLR;
  mRoll = nRoll;
  mAct = nAct;
  mUnknown = nUnknown;
  mFF = nFF;
}

// PVehicleInformation
//Tmp
const uint8_t VhcTypes[] = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                         11, 12, 13, 50, 60, 62, 64, 65, 70
                      }; // (adv.) assault gliders discarded because not ok
#define VHC_DISABLE_NOVULARI
#ifndef VHC_DISABLE_NOVULARI
const uint8_t VhcTypesNum = 19;
#else
const uint8_t VhcTypesNum = 18;
#endif

bool PVehicleInformation::Load( uint32_t nVehicleId )
{
  uint8_t i;
  uint8_t nVId = nVehicleId % 100; //Tmp
  for ( i = 0; ( i < VhcTypesNum ) && ( VhcTypes[i] < nVId ); i++ ) ; //Tmp
  if (( i < VhcTypesNum ) && ( VhcTypes[i] == nVId ) ) //Tmp
  {
    mVehicleId = nVehicleId;
    mOwnerCharId = nVehicleId / 100; // tmp
    mHealth = 200; // Tmp
    mVehicleType = nVId; // tmp
    mStatus = 0;
    return true;
  }
  else
    return false;
}

bool PVehicleInformation::Save()
{
  // Shall we Destroy() when mStatus == 2, or keep destroyed vhc in DB ?
  return true;
}

bool PVehicleInformation::Destroy()
{
  if ( mStatus == 2 )
  {
    // Delete from DB
    mVehicleId = 0;
    return true;
  }
  else
    return false;
}

bool PVehicleInformation::SetStatus( uint8_t nStatus )
{
  if (( mStatus != 2 ) && ( nStatus <= 2 ) )
  {
    mStatus = nStatus;
    return true;
  }
  else
    return false;
}

// PSpawnedVehicule
const uint8_t PSpawnedVehicle::mSeatsFlags[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

PSpawnedVehicle::PSpawnedVehicle( uint32_t nLocalId, PVehicleInformation const* nVhcInfo, uint32_t nLocation, PVhcCoordinates const* nVhcPos )
{
  mLocalId = nLocalId;
  mInfo = *nVhcInfo;
  mLocation = nLocation;
  mCoords = *nVhcPos;
  for ( int i = 0; i < 8; ++i )
    mSeatUserId[i] = 0;

  mVhcDef = GameDefs->Vhcs()->GetDef( mInfo.mVehicleType );
  if ( ! mVhcDef )
  {
    Console->Print( RED, BLACK, "[ERROR] Invalid vhc type %d for vhc id %d (local %d) owner char id %d", mInfo.mVehicleType, mInfo.mVehicleId, mLocalId, mInfo.mOwnerCharId );
    Console->Print( RED, BLACK, "[NOTICE] Setting vhc type to %d for vhc id %d (local %d) owner char id %d", VhcTypes[0], mInfo.mVehicleId, mLocalId, mInfo.mOwnerCharId );
    mInfo.mVehicleType = VhcTypes[0];
    mVhcDef = GameDefs->Vhcs()->GetDef( mInfo.mVehicleType );
  }

  mNbFreeSeats = GetNumSeats();
  if ( mNbFreeSeats > 8 )
  {
    Console->Print( RED, BLACK, "[ERROR] Vhc type %d has more than 8 seats (%d)", mInfo.mVehicleType, mNbFreeSeats );
    mNbFreeSeats = 8;
  }
  for ( int i = 0; i < mNbFreeSeats; ++i )
    mFreeSeatsFlags |= mSeatsFlags[i];

  //Temp
  for ( int i = 0; i < 4; ++i )
  {
    minmax[i][0] = 0xffff;
    minmax[i][1] = 0;
  }
}

void PSpawnedVehicle::SetLocation( uint32_t nLocation )
{
  mLocation = nLocation;
}

void PSpawnedVehicle::SetPosition( PVhcCoordinates const* nVhcPos )
{
  mCoords = *nVhcPos;
  // Temp
  if(gDevDebug)
  {
    if ( mCoords.mUD < minmax[0][0] ) minmax[0][0] = mCoords.mUD;
    if ( mCoords.mUD > minmax[0][1] ) minmax[0][1] = mCoords.mUD;
    if ( mCoords.mLR < minmax[1][0] ) minmax[1][0] = mCoords.mLR;
    if ( mCoords.mLR > minmax[1][1] ) minmax[1][1] = mCoords.mLR;
    if ( mCoords.mRoll < minmax[2][0] ) minmax[2][0] = mCoords.mRoll;
    if ( mCoords.mRoll > minmax[2][1] ) minmax[2][1] = mCoords.mRoll;
    Console->Print( "%s Min/Max: UD:%d/%d(%d) LR:%d/%d(%d) Roll:%d/%d(%d)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), minmax[0][0], minmax[0][1], mCoords.mUD, minmax[1][0], minmax[1][1], mCoords.mLR, minmax[2][0], minmax[2][1], mCoords.mRoll );
  }
}

int PSpawnedVehicle::GetNumSeats() const
{
  return mVhcDef->GetNumSeats();
}

bool PSpawnedVehicle::SetSeatUser( uint8_t nSeatId, uint32_t nCharId )
{
  if ( nSeatId < mVhcDef->GetNumSeats() )
  {
    if ( ! mSeatUserId[nSeatId] )
    {
      mSeatUserId[nSeatId] = nCharId;
      mFreeSeatsFlags &= ~mSeatsFlags[nSeatId];
      --mNbFreeSeats;
      return true;
    }
  }
  return false;
}

bool PSpawnedVehicle::UnsetSeatUser( uint8_t nSeatId, uint32_t nCharId )
{
  if ( nSeatId < mVhcDef->GetNumSeats() )
  {
    if ( mSeatUserId[nSeatId] == nCharId )
    {
      mSeatUserId[nSeatId] = 0;
      mFreeSeatsFlags |= mSeatsFlags[nSeatId];
      ++mNbFreeSeats;
      return true;
    }
  }
  return false;
}

bool PSpawnedVehicle::IsCharInside( uint32_t nCharId ) const
{
  for ( int i = 0; i < mVhcDef->GetNumSeats(); ++i )
  {
    if ( mSeatUserId[i] == nCharId )
      return true;
  }
  return false;
}

uint8_t PSpawnedVehicle::GetFirstFreeSeat() const
{
  for ( int i = 0; i < mVhcDef->GetNumSeats(); ++i )
  {
    if ( ! mSeatUserId[i] )
      return i;
  }

  return 255;
}
/*uint8_t PSpawnedVehicle::GetFreeSeats() const
{
  uint8_t bitField = 0;

  for(int i = mVhcDef->GetNumSeats() - 1; i >= 0 ; --i)
  {
    bitField <<= 1;
    if ( ! mSeatUserId[i] )
      bitField |= 1;
  }

  return bitField;
}*/

// PVehicles
PVehicles::PVehicles()
{

}

PVehicles::~PVehicles()
{

}

bool PVehicles::RegisterSpawnedVehicle( PSpawnedVehicle* nSpawnedVehicle )
{
  std::pair<PSpawnedVhcMap::iterator, bool> Result = mSpawnedVehicles.insert( std::make_pair( nSpawnedVehicle->GetVehicleId(), nSpawnedVehicle ) );
  return Result.second;
}

bool PVehicles::UnregisterSpawnedVehicle( uint32_t nVehicleId )
{
  PSpawnedVhcMap::iterator it = mSpawnedVehicles.find( nVehicleId );
  if ( it != mSpawnedVehicles.end() )
  {
    mSpawnedVehicles.erase( it );
    return true;
  }
  else
  {
    return false;
  }
}

//uint32_t PVehicles::CreateVehicle(uint32_t nOwnerChar, uint8_t mVehicleType) {}
//bool PVehicles::RegisterVehicleOwner(uint32_t nVehicleId, uint32_t nOwnerChar) {}
//bool PVehicles::DestroyVehicle(uint32_t nVehicleId) {}

bool PVehicles::IsValidVehicle( uint32_t nVehicleId, bool nCheckOwner, uint32_t nOwnerId ) const
{
  // Look in DB
  // tmp
  uint32_t tVehicleId = nVehicleId;
  bool tCheckOwner = nCheckOwner;
  uint32_t tOwnerId = nOwnerId;
  return true; // tmp
}

PSpawnedVehicle* PVehicles::GetSpawnedVehicle( uint32_t nVehicleId ) const
{
  PSpawnedVhcMap::const_iterator it = mSpawnedVehicles.find( nVehicleId );
  if ( it != mSpawnedVehicles.end() )
  {
    return ( it->second );
  }
  else
  {
    return nullptr;
  }
}

bool PVehicles::GetVehicleInfo( uint32_t nVehicleId, PVehicleInformation* nInfo ) const
{
  PSpawnedVehicle* tVhc = GetSpawnedVehicle( nVehicleId );
  if ( tVhc )
  {
    *nInfo = tVhc->GetInformation();
    return true;
  }
  else
  {
    return nInfo->Load( nVehicleId );
  }
}

PVhcInfoList* PVehicles::GetCharVehicles( uint32_t nCharId, uint16_t nMaxCount, uint16_t nStartIndex )
{
  PVhcInfoList* Entries = new PVhcInfoList();
  PVehicleInformation* InfoEntry;
  // Tmp implementation
  uint16_t LimitIndex = nStartIndex + nMaxCount;
  if ( !nMaxCount || ( VhcTypesNum < LimitIndex ) )
  {
    LimitIndex = VhcTypesNum;
  }

  for ( uint16_t i = nStartIndex; ( i < LimitIndex ) ; ++i )
  {
    InfoEntry = new PVehicleInformation();
    if ( GetVehicleInfo( nCharId * 100 + VhcTypes[i], InfoEntry ) )
    {
      Entries->push( InfoEntry );
    }
    else
    {
      delete InfoEntry;
    }
  }

  return Entries;
}

PSpawnedVehicle* PVehicles::SpawnVehicle( uint32_t nVehicleId, uint32_t nLocation, PVhcCoordinates const* nVhcPos )
{
  PSpawnedVehicle* newVhc = nullptr;
  PWorld* cWorld;
  PVehicleInformation nVhcInfo;

  if (( nLocation != PWorlds::mNcSubwayWorldId ) && IsValidVehicle( nVehicleId ) && !IsSpawned( nVehicleId ) )
  {
    cWorld = Worlds->LeaseWorld( nLocation );

    if ( cWorld && GetVehicleInfo( nVehicleId, &nVhcInfo ) )
    {
      newVhc = cWorld->GetSpawnedVehicles()->SpawnVehicle( &nVhcInfo, nVhcPos );
      if ( newVhc )
      {
        if ( !RegisterSpawnedVehicle( newVhc ) )
        {
          Console->Print( RED, BLACK, "[Error] PVehicles::SpawnVehicle : Could not register spawned vhc" );
        }
        if( gDevDebug )
          Console->Print( "%d Spawned vhc %d (local 0x%04x) type %d (requested: %d)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), newVhc->GetVehicleId(), newVhc->GetLocalId(), newVhc->GetInformation().GetVehicleType(), nVhcInfo.GetVehicleType() );
      }
      else
        Console->Print( RED, BLACK, "[Error] PVehicles::SpawnVehicle : Could not create vhc" );
    }

    Worlds->ReleaseWorld( nLocation );
  }

  return newVhc;
}

bool PVehicles::UnspawnVehicle( uint32_t nVehicleId )
{
  PWorld* cWorld;
  bool Result = false;

  PSpawnedVhcMap::iterator it = mSpawnedVehicles.find( nVehicleId );
  if ( it != mSpawnedVehicles.end() )
  {
    cWorld = Worlds->LeaseWorld( it->second->GetLocation() );

    if ( cWorld )
    {
      if ( !it->second->Save() )
      {
        Console->Print( RED, BLACK, "[Error] PVehicles::UnspawnVehicle : Could not save vhc %d", nVehicleId );
      }

      Result = cWorld->GetSpawnedVehicles()->UnspawnVehicle( it->second->GetLocalId() );
      if ( Result )
      {
        if ( !UnregisterSpawnedVehicle( nVehicleId ) )
        {
          Console->Print( RED, BLACK, "[Error] PVehicles::UnspawnVehicle : Could not unregister vhc %d", nVehicleId );
        }
      }
    }

    Worlds->ReleaseWorld( it->second->GetLocation() );
  }

  return Result;
}

//PSpawnedVehicles
PSpawnedVehicles::PSpawnedVehicles()
{
  mNextFreeHint = 0;
  mLocation = 0;
}

PSpawnedVehicles::~PSpawnedVehicles()
{

}

PSpawnedVehicle* PSpawnedVehicles::SpawnVehicle( PVehicleInformation const* nVhcInfo, PVhcCoordinates const* nVhcPos )
{
  PSpawnedVehicle* newVhc = nullptr;
  uint32_t nSize;

  if ( nVhcInfo->GetStatus() == 0 ) // only if in garage
  {
    nSize = mSpawnedVehicles.size();
    while (( mNextFreeHint < nSize ) && mSpawnedVehicles[mNextFreeHint] )
    {
      ++mNextFreeHint;
    }
    if ( mNextFreeHint > nSize )
    {
      mNextFreeHint = nSize;
    }
    if ( mNextFreeHint == nSize )
    {
      mSpawnedVehicles.push_back( static_cast<PSpawnedVehicle *>(nullptr));
    }

    if ( mNextFreeHint < mMaxLocalVhc )
    {
      newVhc = new PSpawnedVehicle( mVhcBaseLocalId - mNextFreeHint, nVhcInfo, mLocation, nVhcPos );
      mSpawnedVehicles[mNextFreeHint++] = newVhc;
      newVhc->SetStatus( 1 );
    }
  }

  return newVhc;
}

PSpawnedVehicle* PSpawnedVehicles::GetVehicle( uint32_t nLocalId )
{
  if (( nLocalId <= mVhcBaseLocalId ) && ( nLocalId > ( mVhcBaseLocalId - mSpawnedVehicles.size() ) ) )
  {
    return mSpawnedVehicles[mVhcBaseLocalId - nLocalId];
  }
  else
  {
    return nullptr;
  }
}

PSpawnedVehicle* PSpawnedVehicles::GetVehicleByGlobalId( uint32_t nVehicleId ) const
{
  for ( PSpawnedVhcVector::const_iterator it = mSpawnedVehicles.begin(); it != mSpawnedVehicles.end(); it++ )
  {
    if (( *it )->GetVehicleId() == nVehicleId )
    {
      return ( *it );
    }
  }

  return nullptr;
}

bool PSpawnedVehicles::UnspawnVehicle( uint32_t nLocalId )
{
  uint16_t Index;
  PSpawnedVehicle* tVhc;

  if (( nLocalId <= mVhcBaseLocalId ) && ( nLocalId > ( mVhcBaseLocalId - mSpawnedVehicles.size() ) ) )
  {
    Index = mVhcBaseLocalId - nLocalId;
    tVhc = mSpawnedVehicles[Index];
    if ( tVhc )
    {
      if ( tVhc->GetInformation().GetStatus() != 2 )
      {
        tVhc->SetStatus( 0 );
      }
      delete tVhc;
      mSpawnedVehicles[Index] = nullptr;
      if ( mNextFreeHint > Index )
      {
        mNextFreeHint = Index;
      }
      return true;
    }
  }

  return false;
}

PSpawnedVhcList* PSpawnedVehicles::GetSpawnedVehicles() const
{
  PSpawnedVhcList* Entries = new PSpawnedVhcList();

  for ( PSpawnedVhcVector::const_iterator i = mSpawnedVehicles.begin(); i !=  mSpawnedVehicles.end(); ++i )
  {
    if ( *i )
    {
      Entries->push( *i );
    }
  }

  return Entries;
}
