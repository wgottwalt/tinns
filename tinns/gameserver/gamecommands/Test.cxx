#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"

void PCommands::doCmdtest()
{
    if (GetArgInt(1) == 1)
    {
        if(ArgC < 5)
            return;

        PNPC* targetNPC = nullptr;
        PNPCWorld* currentNPCWorld = nullptr;
        currentNPCWorld = NPCManager->GetWorld( source->GetChar()->GetLocation() );
        if ( currentNPCWorld )
        {
            targetNPC = currentNPCWorld->GetNPC( GetArgInt(2) );
            if(!targetNPC)
                return;
            // @test 15 <npcID> <target_to_attack> <unknown uint8_t value>
            targetNPC->Attack(GetArgInt(3), (uint8_t)GetArgInt(5), (uint8_t)GetArgInt(4));
        }
    }
    else if (GetArgInt(1) == 2)
    {
        if(ArgC < 3)
            return;

        int tF1 = GetArgInt(2);
        int tF2 = GetArgInt(3);
        const PDefFaction* tFactionA = nullptr;
        const PDefFaction* tFactionB = nullptr;

        if(tF1 > tF2)
        {
            tFactionA = GameDefs->Factions()->GetDef(tF1);
            tFactionB = GameDefs->Factions()->GetDef(tF2);
        }
        else
        {
            tFactionA = GameDefs->Factions()->GetDef(tF2);
            tFactionB = GameDefs->Factions()->GetDef(tF1);
        }

        if(tFactionA && tFactionB)
        {
            int tRel = -99;
            if(tF1 > tF2)
                tRel = tFactionA->GetRelation(tF2);
            else
                tRel = tFactionA->GetRelation(tF1);

            char buff[150];
            snprintf(buff, 150, "Relation between <%s> and <%s> is: %d", tFactionA->GetName().c_str(), tFactionB->GetName().c_str(), tRel);
            Chat->send(source, CHAT_DIRECT, "Relations", buff);
        }
        else
            Chat->send(source, CHAT_DIRECT, "Relations", "Invalid faction");
    }
}
