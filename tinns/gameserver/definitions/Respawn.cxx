#include "gameserver/definitions/Includes.hxx"

PDefRespawn::PDefRespawn()
{
}

bool PDefRespawn::LoadFromDef( PTokenList *Tokens )
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

      case 2: // value
        mWorldID = atoi( i->c_str() ); break;

      case 3: // value
        mEntityID = atoi( i->c_str() ); break;

      case 4: // value
        mHazardLevel = atoi( i->c_str() ); break;

      case 5: // name
        mName = *i; break;

      case 6: // world name
        mFlag = *i; break;
    }
  }

  return true;
}

// class PDefRespawnMap defined in gamedefs.h
int PDefRespawnsMap::GetRespawnEntity( uint32_t nWorldID, uint16_t nGROrder ) const
{
  uint16_t tOrder = 0;

  for ( std::map<int, PDefRespawn*>::const_iterator it = mDefs.begin(); it != mDefs.end(); it++ )
  {
    if (( uint32_t )( it->second->GetWorldID() ) == nWorldID )
    {
      ++tOrder;
      if (( nWorldID == 1 ) || ( nWorldID == 2 ) ) // hack for zones 1 & 2
      {
        if (( 3 - tOrder ) == nGROrder )
        {
          return ( it->second->GetEntityID() );
        }
      }
      else if ( tOrder == nGROrder )
      {
        return ( it->second->GetEntityID() );
      }
    }
  }
  return 0;
}
