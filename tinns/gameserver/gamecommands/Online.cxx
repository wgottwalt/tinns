#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdconlist()
{
    if (gDevDebug) Console->Print("IngameCommand: Sending connected-player-list to charID %d", source->GetCharID());
    Chat->sendConnectedList(source, false);
}
