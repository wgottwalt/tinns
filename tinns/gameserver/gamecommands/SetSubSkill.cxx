#include "gameserver/Includes.hxx"

void PCommands::doCmdSetSubSkill()
{
    uint8_t tNewLevel = 0;
    uint8_t tSubSkill = 0;

    bool SyntaxError = false;
    if (ArgC < 2)
    {
        SyntaxError = true;
    }

    if (IsArgNumeric(1) == false)
        SyntaxError = true;
    else
        tSubSkill = (uint16_t)GetArgInt(1);

    if (IsArgNumeric(2) == false)
        SyntaxError = true;
    else
        tNewLevel = (uint8_t)GetArgInt(2);

    if (tNewLevel == 0)
        SyntaxError = true;

    if ( source->GetChar()->Skill->IsValidSubSkill(tSubSkill) == false)
        SyntaxError = true;

    if (SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@setsubskill <1-45> <1-255> [<charID or nickname>]");
        Chat->send(source, CHAT_DIRECT, "Usage", "See the GameMaster reference for IDs");
        return;
    }

    PClient *temp_target = nullptr;
    if ( ArgC == 3 )
    {
        if (IsArgNumeric(3) == true)
        {
            temp_target = GetClientByID(GetArgInt(3));
        }
        else
        {
            char tmp_destNick[50];
            GetArgText(3, tmp_destNick, 50);
            temp_target = GetClientByNick(tmp_destNick);
        }
        if (temp_target == nullptr)
        {
            Chat->send(source, CHAT_DIRECT, "System", "No such player");
            return;
        }
    }
    else
        temp_target = source;


    // Now setting player's Skill tMainSkill to tNewLevel
    uint8_t tOldLevel = 0;
    int tLevelDiff = 0;

    // Grab old Level
    tOldLevel = temp_target->GetChar()->Skill->GetSubSkill(tSubSkill);
    tLevelDiff = tNewLevel - tOldLevel;
    if ( tLevelDiff == 0 )
    {
        Chat->send(source, CHAT_DIRECT, "System", "Not setting anything. No difference at all");
        return;
    }
    temp_target->GetChar()->Skill->SetSubSkill(tSubSkill, tNewLevel);

    char tmp[50];
    snprintf(tmp, 50, "Setting Level to %d", tNewLevel);
    Chat->send(source, CHAT_DIRECT, "System", tmp);

    MAIN_SKILLS tMainSkill = temp_target->GetChar()->Skill->GetMSfromSS(tSubSkill);
    PMessage* tmpMsg_setSLv = MsgBuilder->BuildSubskillIncMsg(temp_target, tSubSkill, temp_target->GetChar()->Skill->GetSP(tMainSkill));
    temp_target->SendUDPMessage(tmpMsg_setSLv);
    tmpMsg_setSLv = nullptr;
    temp_target = nullptr;
}
