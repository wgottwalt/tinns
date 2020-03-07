#include <cstring>
#include "gameserver/Includes.hxx"

void PCommands::doCmdban()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@ban <charID or nickname> <xS(econds)>/<xM(inutes)>/<xH(ours)>/<xD(ays)>");
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

    if(target == nullptr) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    if(source->GetAccountLevel() <= target->GetAccountLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant ban %s, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }
    char tmpTimeVal[10];
    GetArgText(2, tmpTimeVal, 10);

    int32_t loop_i = 0;
    char tmp_atoi[10];

    while(isdigit(tmpTimeVal[loop_i]) != 0 && loop_i < 10)
    {
        tmp_atoi[loop_i] = tmpTimeVal[loop_i];
        loop_i++;
    }
    char timefactor[1];
    timefactor[0] = tmpTimeVal[loop_i];
    int32_t timevalue = atoi(tmp_atoi);
    int32_t time_to_ban = 0;

    if(strcasecmp(timefactor, "s") == 0 )
    {
        time_to_ban = timevalue;
    }
    else if(strcasecmp(timefactor, "m") == 0 )
    {
        time_to_ban = timevalue * 60;
    }
    else if(strcasecmp(timefactor, "h") == 0 )
    {
        time_to_ban = timevalue * 60 * 60;
    }
    else if(strcasecmp(timefactor, "d") == 0 )
    {
        time_to_ban = timevalue * 60 * 60 * 24;
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@ban <charID or nickname> <xS(econds)>/<xM(inutes)>/<xH(ours)>/<xD(ays)>");
        return;
    }

    int32_t final_bantime = std::time(nullptr) + time_to_ban;
    PAccount Acc(target->GetAccountID());
    Acc.SetBannedUntilTime(final_bantime);
    Acc.Save();

    target->InitCharVanish();

    GameServer->ClientDisconnected(target);  // Now kick the player (Hes banned :) )

    char tmpMsg_success[81];
    snprintf(tmpMsg_success, 80, "Successfully banned %s", target->GetChar()->GetName().c_str());
    tmpMsg_success[80] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpMsg_success);
    return;
}
