#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdkick()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@kick <charID or nickname>");
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

    // Make sure only people with a higher level than victim can kick victim
    if(source->GetAccountLevel() <= target->GetAccountLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant kick %s, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

// *************** Checks done, proceed with command
    int final_bantime = Time::nowTimeT() + 60;               // Ban 60 seconds (Anti-Rejoin)
    PAccount Acc(target->GetAccountID());
    Acc.SetBannedUntilTime(final_bantime);
    Acc.Save();

    target->InitCharVanish();
    GameServer->ClientDisconnected(target);                 // Kick

    Console->Print("%s %s (Lv %d) kicked %s (Lv %d)", Console->ColorText(YELLOW, BLACK, "[GameCommand]"), Chars->GetChar(source->GetCharID())->GetName().c_str(), source->GetAccountLevel(), Chars->GetChar(target->GetCharID())->GetName().c_str(), target->GetAccountLevel());
    char successmsg[100];
    snprintf(successmsg, 99, "Kicked %s", Chars->GetChar(target->GetCharID())->GetName().c_str());
    successmsg[99] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", successmsg);
}
