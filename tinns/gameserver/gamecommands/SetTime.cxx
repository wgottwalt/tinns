#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdsettime()
{
    if(ArgC < 1 && GetArgInt(1) == 0)
    {
       Chat->send(source, CHAT_DIRECT, "Usage", "@settime <raw timevalue>");
       return;
    }

    int newtime = 0;
    newtime = GetArgInt(1);

    GameServer->SetGameTime(newtime);
    Console->Print("IngameCommand: CharID %d set ingametime to %d", source->GetCharID(), newtime);
}
