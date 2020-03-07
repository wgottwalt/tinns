#include "gameserver/Includes.hxx"

void PCommands::doCmdgivemoney()
{
    uint32_t cashtoadd = 0;
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == false)
    {
        SyntaxError = true;
    }
    else
    {
        cashtoadd = (uint32_t)GetArgInt(1);
    }
    if(cashtoadd == 0)
        SyntaxError = true;

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@givemoney <amount> [<charID or nickname>]");
        return;
    }

    if(ArgC == 2)
    {
        if(IsArgNumeric(2) == true)
        {
            target = GetClientByID(GetArgInt(2));
        }
        else
        {
            char tmp_destNick[50];
            GetArgText(2, tmp_destNick, 50);
            target = GetClientByNick(tmp_destNick);
        }
        if (target == nullptr)
        {
            Chat->send(source, CHAT_DIRECT, "System", "No such player");
            return;
        }
        if(source->GetAccountLevel() <= target->GetAccountLevel())
        {
            char tmpMsg[200];
            snprintf(tmpMsg, 199, "Cant manipulate %s's credits, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
            tmpMsg[199] = '\0';
            Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
            return;
        }

        uint32_t oldcashval = target->GetChar()->GetCash();
        uint32_t newcashvalue = target->GetChar()->SetCash(oldcashval + cashtoadd);
        uint32_t diffcash = newcashvalue - oldcashval;

        PMessage* tmpMsg_cash = MsgBuilder->BuildCharMoneyUpdateMsg(target, newcashvalue);
        target->SendUDPMessage(tmpMsg_cash);
        tmpMsg_cash = nullptr;

        char tmpMsg_success[200];
        snprintf(tmpMsg_success, 199, "Added %d NC to %s's wallet (Has now %d NC)", diffcash, target->GetChar()->GetName().c_str(), newcashvalue);
        tmpMsg_success[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg_success);
    }
    else
    {
        uint32_t oldcashval = source->GetChar()->GetCash();
        uint32_t newcashvalue = source->GetChar()->SetCash(oldcashval + cashtoadd);

        PMessage* tmpMsg_cash = MsgBuilder->BuildCharMoneyUpdateMsg(source, newcashvalue);
        source->SendUDPMessage(tmpMsg_cash);
        tmpMsg_cash = nullptr;
        //Console->Print("oldcashval: %d  newcashvalue: %d", oldcashval, newcashvalue);
    }
}
