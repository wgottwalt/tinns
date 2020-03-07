#include "gameserver/Includes.hxx"

void PCommands::doCmdsetlevel()
{
    int destLevel = 0;
    bool SyntaxError = false;
    if (ArgC < 2)
    {
        SyntaxError = true;
    }

    if (IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }
    else
    {
        destLevel = GetArgInt(2);
    }

    if (SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@setlevel <charID or nickname> <newlevel 1-99>");
        return;
    }

    if (IsArgNumeric(1) == true)
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
    if (source->GetAccountLevel() <= target->GetAccountLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant set new level for %s, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    PAccount Acc(target->GetAccountID());
    Acc.SetLevel(destLevel);
    Acc.Save();
    source->RefreshAccountInfo(&Acc);

    char tmpMsg[60], tmpMsg2[60];
    snprintf(tmpMsg, 59, "Set level for player %s to %d", Chars->GetChar(target->GetCharID())->GetName().c_str(), destLevel);
    snprintf(tmpMsg2, 59, "**POOF** Your new accesslevel is now %d", destLevel);

    tmpMsg[59] = '\0';
    tmpMsg2[59] = '\0';

    Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
    Chat->send(target, CHAT_DIRECT, "System", tmpMsg2);
}
