#include "gameserver/Includes.hxx"

void PCommands::doCmdlistbans()
{
    Chat->send(source, CHAT_DIRECT, "System", "Sorry, namikon was too lazy to code that ^^");
    Chat->send(source, CHAT_DIRECT, "System", "Please wait until the ban is removed automaticly or edit your sqlDB");
    return;
}
