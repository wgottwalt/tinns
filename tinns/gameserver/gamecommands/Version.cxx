#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdversion()
{
    char tmpChatMsg[300];
    snprintf(tmpChatMsg, 299, "You are on TinNS server %s runnig version %s - SVN Rev. %s",
             Config->GetOption("server_name").c_str(), DefMain::Version.c_str(), DefMain::Version.c_str());
    tmpChatMsg[299] = '\0';

    Chat->send(source, CHAT_DIRECT, "System", tmpChatMsg);
}
