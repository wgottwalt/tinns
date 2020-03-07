#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdwarpto()
{
  bool SyntaxError = false;
  if ( ArgC < 1 )
  {
    SyntaxError = true;
  }

  if ( SyntaxError == true )
  {
    Chat->send( source, CHAT_DIRECT, "Usage", "@warpto <charID or nickname>" );
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

  int destZone = 0;
  // Warp GM/Admin to target player
  PChar* targetChar = target->GetChar();
  destZone = targetChar->GetLocation();
  if ( gDevDebug ) Console->Print( "%s Warping %d to location of %d (> %d)", Console->ColorText( YELLOW, BLACK, "[Notice]" ), source->GetCharID(), target->GetCharID(), destZone );
  if ( !Worlds->IsValidWorld( destZone ) )
  {
    Console->Print( "%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText( RED, BLACK, "[PANIC]" ), destZone );
    return;
  }
// *************** Checks done, proceed with command

  if ( source->ChangeCharLocation( destZone ), true )
  {
    source->SetAwaitingWarpto( true, targetChar->Coords.mX, targetChar->Coords.mY, targetChar->Coords.mZ );
    PMessage* tmpMsg_zone = MsgBuilder->BuildChangeLocationMsg( source, destZone, 10, 1, 0 );
    source->SendUDPMessage( tmpMsg_zone );

    char tmpMsg_success[81];
    snprintf( tmpMsg_success, 80, "Successfully warped you to %s", target->GetChar()->GetName().c_str() );
    tmpMsg_success[80] = '\0';
    Chat->send( source, CHAT_DIRECT, "System", tmpMsg_success );
    return;
  }
  else
  {
    Console->Print( "%s Unable to change location for player %d to %d", Console->ColorText( RED, BLACK, "[PANIC]" ), source->GetCharID(), destZone );
  }
}
