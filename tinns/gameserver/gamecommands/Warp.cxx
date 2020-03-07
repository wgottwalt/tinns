#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdwarp()
{
  int SpawnPointID = 0;
  int zoneID = 0;
  int ZoningType = 0;

  bool SyntaxError = false;
  if ( ArgC < 1 )
  {
    SyntaxError = true;
  }
  else
  {
    if ( IsArgNumeric( 1 ) == true )
    {
      zoneID = GetArgInt( 1 );
      if ( zoneID == 0 )
      {
        SyntaxError = true;
      }
    }
    else
    {
      SyntaxError = true;
    }

    if ( ArgC > 1 )
    {
      if ( IsArgNumeric( 2 ) == true )
      {
        SpawnPointID = GetArgInt( 2 );
        if ( ArgC > 2 )
        {
          ZoningType = 1;
        }
      }
      else
      {
        SyntaxError = true;
      }
    }
  }
  if ( SyntaxError == true )
  {
    Chat->send( source, CHAT_DIRECT, "Usage", "@warp <zoneid> [<spawn location> [<use .def: 1>]]" );
    return;
  }

  if ( !Worlds->IsValidWorld( zoneID ) )
  {
    Chat->send( source, CHAT_DIRECT, "System", "Invalid zoneID" );
    return;
  }

  if ( source->ChangeCharLocation( zoneID, true ) )
  {
    if ( gDevDebug ) Console->Print( "IngameCommand: Warping player %d to zone %d (%s)", source->GetCharID(), zoneID, Worlds->GetWorld( zoneID )->GetName().c_str() );

    PMessage* tmpMsg = MsgBuilder->BuildAptLiftUseMsg( source, zoneID, SpawnPointID, ZoningType );
    source->SendUDPMessage( tmpMsg );
  }
  else
  {
    Console->Print( RED, BLACK, "IngameCommand: Can't change location when trying to warp player %d to zone %d", source->GetCharID(), zoneID );
  }
}
