#include "gameserver/Includes.hxx"

void PCommands::doCmdshun()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@shun <charID or nickname>");
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
    if(source->GetAccountLevel() <= target->GetAccountLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant shun %s, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }
    if(target->GetChar()->IsShunned() == false)
    {
        target->GetChar()->SetShun(true);

        char tmpMsg_success[81];
        snprintf(tmpMsg_success, 80, "Successfully shunned %s", target->GetChar()->GetName().c_str());
        tmpMsg_success[80] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg_success);
        return;
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "System", "Player is already shunned");
        return;
    }
}
