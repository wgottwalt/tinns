#include "gameserver/Includes.hxx"

void PCommands::doCmdSetMainSkill()
{
    uint8_t tNewLevel = 0;
    uint8_t tMainSkill = 0;

    bool SyntaxError = false;
    if (ArgC < 2)
    {
        SyntaxError = true;
    }

    if (IsArgNumeric(1) == false)
        SyntaxError = true;
    else
        tMainSkill = (uint16_t)GetArgInt(1);

    if (IsArgNumeric(2) == false)
        SyntaxError = true;
    else
        tNewLevel = (uint8_t)GetArgInt(2);

    if (tNewLevel == 0)
        SyntaxError = true;

    if ( source->GetChar()->Skill->IsValidMainSkill(tMainSkill) == false)
        SyntaxError = true;

    if (SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@setmainskill <1-5> <1-255> [<charID or nickname>]");
        Chat->send(source, CHAT_DIRECT, "Usage", "1:STR 2:DEX 3:CON 4:INT 5:PSI");
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
    uint16_t tNewSkillPoints = 0;
    int tLevelDiff = 0;

    // Grab old Level
    tOldLevel = temp_target->GetChar()->Skill->GetMainSkill(tMainSkill);
    tLevelDiff = tNewLevel - tOldLevel;
    if ( tLevelDiff == 0 )
    {
        Chat->send(source, CHAT_DIRECT, "System", "Not setting anything. No difference at all");
        return;
    }
    else if ( tLevelDiff > 0 ) // INCreasing mainskill
    {
        tNewSkillPoints = tLevelDiff*5;
        temp_target->GetChar()->Skill->SetSP(tMainSkill, tNewSkillPoints);
    }
    else if ( tLevelDiff < 0 ) // DECreasing mainskill
    {
        // Set all subskills to zero, and add tNewLevel*5 skillpoints
        temp_target->GetChar()->Skill->ZeroMSSubSkills(tMainSkill);
        temp_target->GetChar()->Skill->SetSP(tMainSkill, tNewLevel*5);
        tNewSkillPoints = 0;
        // Not perfect, player has to rezone for the correct values. Need a way to do massupdate subskills...
        // But on the other side, its not very common to DEcrease someone's skills, isnt it?
    }
    temp_target->GetChar()->Skill->SetMainSkill(tMainSkill, tNewLevel);

    char tmp[50];
    snprintf(tmp, 50, "Setting Level to %d", tNewLevel);
    Chat->send(source, CHAT_DIRECT, "System", tmp);
    //PMessage* PMsgBuilder::BuildLevelUpMessage( PClient* nClient, uint8_t nMainSkill, uint8_t nNewLevel, uint16_t nFreeSkillPoints)
    PMessage* tmpMsg_setLv = MsgBuilder->BuildLevelUpMessage(temp_target, tMainSkill, tNewLevel, tNewSkillPoints);
    temp_target->SendUDPMessage(tmpMsg_setLv);
    tmpMsg_setLv = nullptr;
    temp_target = nullptr;
}
