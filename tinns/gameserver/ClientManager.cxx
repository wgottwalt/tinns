#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

// TODO:   - these are just begining of modif, as the Client Manager is bound to be a major component for multiplayer management

PClientManager::PClientManager()
{
  //mLastID = 0;
}


PClientManager::~PClientManager()
{
  /*
    for(PClientMap::iterator it=mClientList.begin(); it!=mClientList.end(); it++)
    {
        delete it->second;
    }
  */
}

bool PClientManager::addClientToList( PClient* newClient )
{
  if ( !newClient )
    return false;

  PClientMap::const_iterator it = mClientList.find( newClient->GetLocalID() );
  if ( it == mClientList.end() ) // only if client not found in list
  {
    /*     mClientList.insert(std::make_pair(m_LastID, newClient));
            ++m_LastID; */

    mClientList.insert( std::make_pair( newClient->GetLocalID(), newClient ) );
    //Console->Print(GREEN, BLACK, "Client %d added to clientmanager", newClient->GetIndex());
    /*        if(newClient)
            {
                return true;
            } */
    return true;
  }
  else
    return false;
}

// Check if a zone is in use
bool PClientManager::IsWorldInUse( uint32_t nWorldID ) const
{
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( it->second &&  it->second->GetChar() )
    {
      if ( it->second->GetChar()->GetLocation() == nWorldID )
        return true;
    }
  }
  return false;
}

// Check if a rawObjectId belongs to a char is in use
PClient* PClientManager::GetClientByCharLocalId( uint32_t rawObjectId, uint32_t nWorldID ) const
{
  PClient* nClient;
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    // note: atm, charid = clientid+1 in any zone (client.h)
    if ( (nClient = it->second) && ( nClient->GetLocalID() == rawObjectId ) )
    {
      if ( nClient->GetChar() && ( nClient->GetChar()->GetLocation() == nWorldID ) )
        return nClient;
    }
  }
  return 0;
}

void PClientManager::deleteClientFromList( uint32_t id )
{
  PClientMap::iterator it = mClientList.find( id );
  if ( it != mClientList.end() )
  {
    mClientList.erase( it );
    //Console->Print(YELLOW, BLACK, "Client %d removed from clientmanager", ((PClient*)(it->second))->GetIndex());
  }
}

PClient* PClientManager::getClientByID( uint32_t id ) const
{
  PClientMap::const_iterator it = mClientList.find( id );
  return (( it != mClientList.end() ) ? ( PClient* )( it->second ) : nullptr);
}

PClient* PClientManager::getClientByChar( uint32_t CharID ) const
{
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( it->second )
    {
      if ( it->second->GetCharID() == CharID )
        return it->second;
    }
  }
  return nullptr;
}

PClient* PClientManager::getClientByChar( const std::string &Name ) const
{
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( it->second )
    {
      if ( it->second->GetChar()->GetName() == Name )
        return it->second;
    }
  }
  return nullptr;
}

// Distance checking doesn't care for Z axis ATM
int PClientManager::UDPBroadcast( PMessage* nMessage, uint32_t nZoneID, uint16_t nX, uint16_t nY, uint16_t nZ, uint16_t nMaxDist, uint32_t nSkipCharId, bool nNPCPing )
{
  int msgCount = 0;
  PChar* nChar;
  PMessage* tmpMsg;
  PClient* itClient;
  uint16_t Dapprox;

  for ( PClientMap::iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    itClient = ( PClient* )( it->second );

    // Dont send NPC alive messages when client is not ready for them
    if ( !itClient->IsAcceptingNPCUpdates() && nNPCPing )
      continue;

    if ( itClient->getUDPConn() )
    {
      nChar = itClient->GetChar();
      if ( nChar && ( nChar->GetLocation() != nZoneID ) )// if limited to zone, do check
        continue;

      if (itClient->GetCharID() == nSkipCharId ) // if source of broadcast should be skipped
        continue;

      if ( nMaxDist ) // if limited to distance, do check
      {
        Dapprox = Misc::DistanceApprox(( nChar->Coords ).mX, ( nChar->Coords ).mY, ( nChar->Coords ).mZ, nX, nY, nZ );
        if ( Dapprox >  nMaxDist )
          continue;
      }

      /*tmpMsg = new PMessage(nMessage->GetMaxSize());
      (*tmpMsg) = (*nMessage);*/
      tmpMsg = new PMessage( *nMessage );

      itClient->FillInUDP_ID( tmpMsg );
      itClient->SendUDPMessage( tmpMsg );
      ++msgCount;
    }
  }

  //Console->Print("Broadcast in zone %d to %d chars", nZoneID, msgCount);
  delete nMessage;
  return msgCount;
}

int PClientManager::UDPBroadcast( PMessage* nMessage, PClient* nClient, uint16_t nMaxDist, bool nSkipSource, bool nNPCPing )
{
  PChar* nChar;
  uint32_t skipCharId = nSkipSource ? nClient->GetCharID() : 0 ;

  if ( nClient && ( nChar = nClient->GetChar() ) )
  {
    return UDPBroadcast( nMessage, nChar->GetLocation(), ( nChar->Coords ).mX, ( nChar->Coords ).mY, ( nChar->Coords ).mZ, nMaxDist, skipCharId, nNPCPing );
  }
  else
  {
    delete nMessage;
    return 0;
  }
}

int PClientManager::SendUDPZoneWelcomeToClient( PClient* nClient )
{
  int msgCount = 0;
  PChar* nChar;
  PChar* itChar;
  PMessage* tmpMsg;
  uint32_t nZoneID;
  PClient* itClient;

  if ( nClient && ( nChar = nClient->GetChar() ) ) // if nClient is set, always use its zone
  {
    nZoneID = nChar->GetLocation();
  }
  else
    return 0;

  for ( PClientMap::iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( nClient->GetLocalID() == it->first )
      continue;

    itClient = ( PClient* )( it->second );
    if ( itClient->getUDPConn() )
    {
      itChar = itClient->GetChar();
      if ( itChar->GetLocation() != nZoneID ) // limit to zone
        continue;

      tmpMsg = MsgBuilder->BuildCharHelloMsg( itClient );

      nClient->FillInUDP_ID( tmpMsg );
      nClient->SendUDPMessage( tmpMsg );
      //Console->Print("Welcome data sent from client %d to client %d", itClient->GetIndex(), nClient->GetIndex());
      //tmpMsg->Dump();

      if ( itChar->GetSeatInUse() != seat_none )
      {
        tmpMsg = MsgBuilder->BuildCharPosUpdateMsg( itClient );
        nClient->FillInUDP_ID( tmpMsg );
        nClient->SendUDPMessage( tmpMsg );

        //Console->Print("Sit on chair %d sent from client %d to client %d", (itChar->GetChairInUse()+1)*1024, itClient->GetIndex(), nClient->GetIndex());
        /*tmpMsg = MsgBuilder->BuildCharUseSeatMsg(itClient, (itChar->GetChairInUse()+1)*1024);
        nClient->FillInUDP_ID(tmpMsg);
        nClient->SendUDPMessage(tmpMsg);*/
      }
      ++msgCount;
    }
  }

  return msgCount;
}
