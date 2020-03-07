#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdrecall()
{
  bool SyntaxError = false;
  if ( ArgC < 1 )
  {
    SyntaxError = true;
  }

  if ( SyntaxError == true )
  {
    Chat->send( source, CHAT_DIRECT, "Usage", "@recall <charID or nickname>" );
    return;
  }

  if ( IsArgNumeric( 1 ) == true )
  {
    target = GetClientByID( GetArgInt( 1 ) );
  }
  else
  {
    char tmp_destNick[50];
    GetArgText( 1, tmp_destNick, 50 );
    target = GetClientByNick( tmp_destNick );
  }

  if (target == nullptr) // If victim isnt found, return error
  {
    Chat->send( source, CHAT_DIRECT, "System", "No such player" );
    return;
  }
  // Check accountlevel (Only higher's can perform commands on lower's)
  if ( source->GetAccountLevel() <= target->GetAccountLevel() )
  {
    char tmpMsg[200];
    snprintf( tmpMsg, 199, "Cant warp %s to you, target level is higher or equal to yours!", Chars->GetChar( target->GetCharID() )->GetName().c_str() );
    tmpMsg[199] = '\0';
    Chat->send( source, CHAT_DIRECT, "System", tmpMsg );
    return;
  }

  // Warp target player to GM/Admin
  int destZone = Chars->GetChar( source->GetCharID() )->GetLocation();
  if ( !Worlds->IsValidWorld( destZone ) )
  {
    Console->Print( "%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText( RED, BLACK, "[PANIC]" ), destZone );
    return;
  }
// *************** Checks done, proceed with command

  if ( target->ChangeCharLocation( destZone ), true )
  {
    PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg( target, destZone, 0 );
    target->SendUDPMessage( tmpMsg_zone );
    tmpMsg_zone = nullptr;

    uint16_t nNewX, nNewY, nNewZ;
    nNewX = source->GetChar()->Coords.mX;
    nNewY = source->GetChar()->Coords.mY;
    nNewZ = source->GetChar()->Coords.mZ;
    target->SetAwaitingWarpto( true, nNewX, nNewY, nNewZ );

    char tmpMsg_success[81];
    snprintf( tmpMsg_success, 80, "Successfully recalled %s", target->GetChar()->GetName().c_str() );
    tmpMsg_success[80] = '\0';
    Chat->send( source, CHAT_DIRECT, "System", tmpMsg_success );
    return;
  }
  else
  {
    Console->Print( "%s Unable to change location for player %d to %d", Console->ColorText( RED, BLACK, "[PANIC]" ), target->GetCharID(), destZone );
  }
}
