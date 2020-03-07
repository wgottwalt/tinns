#include "gameserver/Includes.hxx"

void PCommands::doCmdunban()
{
    Chat->send(source, CHAT_DIRECT, "System", "Unban is not yet possible, sorry.");
}
