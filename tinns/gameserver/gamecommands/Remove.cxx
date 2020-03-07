#include <cstring>
#include "gameserver/Includes.hxx"

void PCommands::doCmdremove()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }
    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@remove actor/<raw item id>");
        return;
    }

    char tmp_v1[30];
    GetArgText(1, tmp_v1, 30);

    if(strcmp(tmp_v1, "actor") == 0)
    {
        if(source->IsInRemoveActorMode() == false)
        {
            source->SetRemoveActorMode(true);
            Chat->send(source, CHAT_DIRECT, "System", "You are now in REMOVE ACTOR mode. Rightclick an actor to remove it, type command again to disable mode");
        }
        else
        {
            source->SetRemoveActorMode(false);
            Chat->send(source, CHAT_DIRECT, "System", "REMOVE ACTOR mode disabled");
        }
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        uint32_t tTest = GetArgInt(1);
        if(WorldActors->IsDynamicActor(tTest) == true)
        {
            Chat->send(source, CHAT_DIRECT, "System", "You cannot remove dynamic actors over their ID!");
            return;
        }
        uint32_t TargetID;
        char delStr[128];
        PMessage* tmpMsg;

        TargetID = (uint32_t)(atoi(tmp_v1) & 0xffffffff);
        tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(source, TargetID, 5);

        ClientManager->UDPBroadcast(tmpMsg, source);
        tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(source, TargetID, 9);

        ClientManager->UDPBroadcast(tmpMsg, source);
        snprintf(delStr, 127, "Item %d removed.", TargetID);
        delStr[127] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", delStr);
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "System", "Invalid argument given");
        return;
    }
}
