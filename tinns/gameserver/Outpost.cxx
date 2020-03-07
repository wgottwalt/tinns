#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

POutpost::POutpost()
{
}

POutpost::~POutpost()
{
}

uint32_t POutpost::GetCurrentClan(uint32_t nOutpostID)
{
    MYSQL_RES *result = nullptr;
    char tQuery[200];

    snprintf(tQuery, 200, "SELECT o_clan FROM outposts WHERE o_outnum = %d", nOutpostID);
    result = MySQL->GameResQuery(tQuery);

    if(!result)
    {
        Console->Print("%s [POutpost::GetCurrentClan] Cannot query SQL. Query was: %s", Console->ColorText(RED,BLACK,"Error"), tQuery);
        MySQL->ShowGameSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        Console->Print("%s [POutpost::GetCurrentClan] Invalid or missing Outpost data for Outpost: %d", Console->ColorText(RED,BLACK,"Error"), nOutpostID);
        return 0;
    }

    uint32_t tClanID = atoi(mysql_fetch_row(result)[0]);

    MySQL->FreeGameSQLResult(result);
    return tClanID;
}

// Check if given nZoneID is an outpost area
bool POutpost::IsZoneOPArea(uint32_t nZoneID)
{
    // Outpost ID == WorldID
    // Outpost Underground = WorldID + 2000
    // Only process possible outpost zones and their undergrounds

    if((nZoneID > 2005 && nZoneID < 2213) || (nZoneID > 4005 && nZoneID < 4213))
    {
        const PDefWorld* tWorldDef = GameDefs->Worlds()->GetDef( nZoneID );
        if ( tWorldDef )
            return true;
    }
    return false;
}

// Sends Outpostinfo to client
void POutpost::SendOPAreaData(PClient* nClient)
{
    if(!nClient)
        return;

    MYSQL_RES *result = nullptr;
    MYSQL_ROW row = nullptr;
    char tQuery[200];
    int nZoneID = nClient->GetChar()->GetLocation();

    if(IsZoneOPArea(nZoneID) == false)
    {
        if (gDevDebug) Console->Print("ZoneID %d is not an outpost. Skipping", nZoneID);
        return;
    }
    // Fix underground ID
    if(nZoneID > 4000)
        nZoneID -= 2000;

    snprintf(tQuery, 200, "SELECT cl_id, cl_faction FROM clans INNER JOIN outposts ON (o_clan = cl_id) WHERE o_outnum = %d", nZoneID);
    result = MySQL->GameResQuery(tQuery);

    if(!result)
    {
        Console->Print("%s [POutpost::SendOPAreaData] Cannot query SQL. Query was: %s", Console->ColorText(RED,BLACK,"Error"), tQuery);
        MySQL->ShowGameSQLError();
        return;
    }

    if(mysql_num_rows(result) == 0)
    {
        Console->Print("%s [POutpost::SendOPAreaData] Invalid or missing Outpost data for ZoneID: %d", Console->ColorText(RED,BLACK,"Error"), nZoneID);
        return;
    }

    row = mysql_fetch_row(result);

    uint32_t tClanID = atoi(row[0]);
    uint8_t tFactionID = atoi(row[1]);

    MySQL->FreeGameSQLResult(result);

    Console->Print("ZoneID %d is an outpost. Sending Clan/Faction information... Clan: %d Faction: %d", nZoneID, tClanID, tFactionID);
    PMessage* tMsg = MsgBuilder->BuildOutpostClanInfoMsg( nClient, tClanID, tFactionID );
    nClient->SendUDPMessage( tMsg );
}
