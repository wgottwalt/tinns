#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdteleport()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@teleport <charID or nickname> <destination worldID>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if (target == nullptr) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    // Check accountlevel (Only higher's can perform commands on lower's)
    if(source->GetAccountLevel() <= target->GetAccountLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant teleport %s, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    int destZone = GetArgInt(2);
    if (!Worlds->IsValidWorld(destZone))
    {
        Chat->send(source, CHAT_DIRECT, "System", "Invalid zoneID");
        return;
    }
// *************** Checks done, proceed with command
    source->InitWarpCircle();
    source->InitCharVanish();

    if (target->ChangeCharLocation(destZone, true))
    {
        target->InitWarpCircle();
        target->InitCharVanish();

        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (target, destZone, 0);
        target->SendUDPMessage(tmpMsg_zone);
        tmpMsg_zone = nullptr;

        char tmpMsg_success[81];
        snprintf(tmpMsg_success, 80, "Successfully teleported %s to World %d", target->GetChar()->GetName().c_str(), destZone);
        tmpMsg_success[80] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg_success);
        return;
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), target->GetCharID(), destZone);
    }
}
