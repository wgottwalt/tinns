#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PVhcAccessRequest::PVhcAccessRequest()
{
  mVhcGlobalId = mCharId = 0;
  mStatus = 0;
  mTimestamp = 0;
}

PVhcAccessRequest::PVhcAccessRequest( uint32_t nRequesterCharId, uint32_t nVhcGlobalId )
{
  mCharId = nRequesterCharId;
  mVhcGlobalId = nVhcGlobalId;
  mStatus = 0;
  mTimestamp = Time::nowTimeT();
}

/*
typedef std::map<uint32_t, PVhcAccessRequest> PVhcAccessRequestMap;

PVhcAccessRequestMap mActiveRequests;
uint32_t mNextRequestId;

time_t mResponseWaitTime; // How long do we wait for owner answer
time_t mCheckWaitTime; // How long do we wait for user access check (0: remaining time from mResponseWaitTime)
time_t mReuseWaitTime; // How long do we allow user to re-use the autorization after first check
*/
void PVhcAccessRequestList::DropTimedOut()
{
  time_t timeout;

  for ( PVhcAccessRequestMap::iterator it = mActiveRequests.begin(); it != mActiveRequests.end(); it++ )
  {
    switch ( it->second.mStatus )
    {
      case 0:
        timeout = mResponseWaitTime;
        break;
      case 1:
        timeout = ( mCheckWaitTime ? mCheckWaitTime : mResponseWaitTime );
        break;
      case 2:
        timeout = mReuseWaitTime;
        break;
      default:
        timeout = 0;
        break;
    }
    if (it->second.mTimestamp > (Time::nowTimeT() + timeout))
      mActiveRequests.erase( it );
  }
}

PVhcAccessRequestList::PVhcAccessRequestList()
{
  mNextRequestId = 1;
  SetParameters( 10, 20, 0 );
}

PVhcAccessRequestList::~PVhcAccessRequestList()
{
}

void PVhcAccessRequestList::SetParameters(time_t nResponseWaitTime, time_t nCheckWaitTime, time_t nReuseWaitTime)
{
  mResponseWaitTime = nResponseWaitTime;
  mCheckWaitTime = nCheckWaitTime;
  mReuseWaitTime = nReuseWaitTime;
}

uint32_t PVhcAccessRequestList::Add( uint32_t nRequesterCharId, uint32_t nVhcGlobalId )
{
  DropTimedOut();
  // We could check for existing entries before
  PVhcAccessRequest newReq( nRequesterCharId, nVhcGlobalId );
  uint32_t newId = mNextRequestId;
  mActiveRequests[newId] = newReq;

  if ( ++mNextRequestId == 0 )
    mNextRequestId = 1;

  return ( newId );
}

bool PVhcAccessRequestList::GetInfo(uint32_t nRequestId, uint32_t* oRequesterCharId, uint32_t* oVehicleId) const
{
  PVhcAccessRequestMap::const_iterator it = mActiveRequests.find( nRequestId );
  if ( it != mActiveRequests.end() )
  {
    *oRequesterCharId = it->second.mCharId;
    *oVehicleId = it->second.mVhcGlobalId;
    return true;
  }
  else
    return false;
}

bool PVhcAccessRequestList::RegisterResponse( uint32_t nRequestId, bool nStatus )
{
  DropTimedOut();
  PVhcAccessRequestMap::iterator it = mActiveRequests.find( nRequestId );
  if ( it != mActiveRequests.end() )
  {
    if ( nStatus )
    {
      it->second.mStatus = 1;
      if ( mCheckWaitTime )
      {
        it->second.mTimestamp = Time::nowTimeT();
      }
      return true;
    }
    else
    {
      mActiveRequests.erase( it );
    }
  }

  return false;
}

bool PVhcAccessRequestList::Check( uint32_t nRequestId, uint32_t nRequesterCharId, uint32_t nVhcGlobalId )
{
  DropTimedOut();
  PVhcAccessRequestMap::iterator it = mActiveRequests.find( nRequestId );
  if ( it != mActiveRequests.end() )
  {
    if ( (!nRequesterCharId || (( it->second.mCharId == nRequesterCharId ) && ( it->second.mVhcGlobalId == nVhcGlobalId ))) && ( it->second.mStatus > 0 ) )
    {
      if ( mReuseWaitTime && ( it->second.mStatus == 1 ) )
      {
        it->second.mStatus = 2;
        it->second.mTimestamp = Time::nowTimeT();
      }
      else
      {
        mActiveRequests.erase( it );
      }

      return true;
    }
  }
  return false;
}
