#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "common/Includes.hxx"

PWorldActors::PWorldActors()
{
    DoActorCheck();
}

PWorldActors::~PWorldActors()
{
}

void PWorldActors::SpawnWA(uint32_t nWorld, uint16_t nActorID, uint16_t nFunctionID, uint32_t nWOID, uint16_t nPosX, uint16_t nPosY, uint16_t nPosZ, uint8_t nRotX, uint8_t nRotY, uint8_t nRotZ)
{
    PMessage* tmpMsg = MsgBuilder->BuildSpawnWorldObjectMsg(nActorID, nFunctionID, nWOID, nPosX, nPosY, nPosZ, nRotX, nRotY, nRotZ);
    ClientManager->UDPBroadcast(tmpMsg, nWorld);
}

void PWorldActors::VanishWA(uint32_t nWorld, uint32_t nWAid)
{
    PMessage* tmpMsg = MsgBuilder->BuildRemoveWorldObjectMsg(nWAid);
    ClientManager->UDPBroadcast(tmpMsg, nWorld);
}

uint32_t PWorldActors::GetNextFreeWAID()
{
    if (gDevDebug) Console->Print("DEBUG: Getting next free worldactor ID...");
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `world_actors` ORDER BY `wa_actor_id` DESC LIMIT 1");
    if (gDevDebug) Console->Print("DEBUG: Executing query %s", query);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetNextFreeWAID could not get next free WorldActorID");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return 0;
    }

    row = mysql_fetch_row(result);
    uint32_t newWAid;
    if(mysql_num_rows(result) == 1)
    {
        newWAid = atoi(row[wa_actor_id]);
        newWAid++;
    }
    else
    {
        newWAid = DYNACTORIDSTART;
    }

    if(newWAid < DYNACTORIDSTART)
    {
        newWAid = DYNACTORIDSTART;
    }

    MySQL->FreeGameSQLResult(result);
    return newWAid;
}

void PWorldActors::InitWorld(PClient* nClient)
{
    DoActorCheck();
    if (gDevDebug) Console->Print("DEBUG: Initializing WorldActors for client...");
    uint32_t tZone = nClient->GetChar()->GetLocation();
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `world_actors` WHERE `wa_actor_map` = %d", tZone);
    //if (gDevDebug) Console->Print("DEBUG: Executing query: %s", query);
    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::InitWorld could not load WorldActor definition");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return;
    }
    if(mysql_num_rows(result) == 0)
    {
        if (gDevDebug) Console->Print("DEBUG: No dynamic actors found, skipping");
        MySQL->FreeGameSQLResult(result);
        return;
    }
    if (gDevDebug) Console->Print("DEBUG: We have dynamic actors, building message...");
    PMessage* tmpActorSpawn = new PMessage(256);
    *tmpActorSpawn << (uint8_t)0x13;
    *tmpActorSpawn << (uint16_t)0x0000; // Placeholder
    *tmpActorSpawn << (uint16_t)0x0000; // Placeholder

    uint16_t tFuncID = 0;
    uint16_t tOpt1 = 0;
    uint32_t tActorID = 0;

    bool tActorOk = false;

    while((row = mysql_fetch_row(result)))
    {
        tFuncID = (uint16_t)atoi(row[wa_actor_type]);
        tOpt1 = (uint16_t)atoi(row[wa_option1]);
        tActorID = (uint32_t)atoi(row[wa_actor_id]);
        // First make sure we have an VALID worldactor here
        tActorOk = false;
        if(IsValidWAFunction(tFuncID) == true)
        {
            if(RequiresLinkedObject(tFuncID == true))
            {
                if(IsValidLinkedObject(nClient, tOpt1, tFuncID) == true)
                {
                    tActorOk = true;
                }
            }
            else
            {
                tActorOk = true;
            }
        }

        if(tActorOk == true)
        {
            // Build multiframe message
            nClient->IncreaseUDP_ID();
            *tmpActorSpawn << (uint8_t)0x16;
            *tmpActorSpawn << (uint8_t)0x03;
            *tmpActorSpawn << (uint16_t)nClient->GetUDP_ID();
            *tmpActorSpawn << (uint8_t)0x1b;
            *tmpActorSpawn << tActorID;
            *tmpActorSpawn << (uint8_t)0x19;
            *tmpActorSpawn << (uint16_t)atoi(row[wa_posY]);
            *tmpActorSpawn << (uint16_t)atoi(row[wa_posZ]);
            *tmpActorSpawn << (uint16_t)atoi(row[wa_posX]);
            *tmpActorSpawn << (uint8_t)atoi(row[wa_rotY]);
            *tmpActorSpawn << (uint8_t)atoi(row[wa_rotZ]);
            *tmpActorSpawn << (uint8_t)atoi(row[wa_rotX]);
            *tmpActorSpawn << (uint16_t)atoi(row[wa_actor_model]);
            *tmpActorSpawn << tFuncID;
            if((tmpActorSpawn->GetSize() + 23) >= (tmpActorSpawn->GetMaxSize() - tmpActorSpawn->GetSize()))
            {
                //if (gDevDebug) Console->Print("DEBUG: Message is full, sending part-msg");
                tmpActorSpawn->U16Data(0x01) = nClient->GetUDP_ID();  // Set final UDP ID
                tmpActorSpawn->U16Data(0x03) = nClient->GetSessionID();  // Set final SessionID
                nClient->SendUDPMessage(tmpActorSpawn);
                tmpActorSpawn = nullptr;

                // ReInit message
                tmpActorSpawn = new PMessage(256);
                *tmpActorSpawn << (uint8_t)0x13;
                *tmpActorSpawn << (uint16_t)0x0000; // Placeholder
                *tmpActorSpawn << (uint16_t)0x0000; // Placeholder
                //if (gDevDebug) Console->Print("DEBUG: Done. Starting over!");
            }
        }
        else
        {
            Console->Print("%s Dynamic worldactor %d is invalid, skipping", Console->ColorText(RED, BLACK, "[Warning]", tActorID));
        }
    }
    tmpActorSpawn->U16Data(0x01) = nClient->GetUDP_ID();  // Set final UDP ID
    tmpActorSpawn->U16Data(0x03) = nClient->GetSessionID();  // Set final SessionID

    if(tmpActorSpawn->GetSize() > 5)
        nClient->SendUDPMessage(tmpActorSpawn);
    else
    {
        delete tmpActorSpawn;
    }
    MySQL->FreeGameSQLResult(result);
}

uint32_t PWorldActors::AddWorldActor(PClient* nClient, uint16_t nActorID, uint16_t nFuncID, uint16_t nOpt1, uint16_t nOpt2, uint16_t nOpt3)
{
    PChar *tChar = nClient->GetChar();
    uint16_t tPosX = tChar->Coords.mX + 768;
    uint16_t tPosY = tChar->Coords.mY + 768;
    uint16_t tPosZ = tChar->Coords.mZ + 768;
    uint16_t tLoc = tChar->GetLocation();

    //if (gDevDebug) Console->Print("DEBUG: Adding worldactor %d function %d to world %d", nActorID, nFuncID, tLoc);
    return AddWorldActor(tLoc, nActorID, nFuncID, tPosX, tPosY, tPosZ, (uint8_t)194, (uint8_t)128, (uint8_t)128, nOpt1, nOpt2, nOpt3);
}

uint32_t PWorldActors::AddWorldActor(uint32_t nWorldID, uint16_t nActorID, uint16_t nFuncID, uint16_t nPosX, uint16_t nPosY, uint16_t nPosZ, uint8_t nRotX, uint8_t nRotY, uint8_t nRotZ, uint16_t nOpt1, uint16_t nOpt2, uint16_t nOpt3)
{
    if (gDevDebug) Console->Print("DEBUG: Adding new worldactor: ActorModel: %d, Function: %d, Option1: %d", nActorID, nFuncID, nOpt1);
    uint32_t tNextWAID = GetNextFreeWAID(); // Grab next free WorldActorID
    //if (gDevDebug) Console->Print("DEBUG: Next ID will be: %d", tNextWAID);

    char query[512];
    snprintf(query, 512, "INSERT INTO `world_actors`(wa_actor_id,wa_actor_map,wa_actor_model,wa_actor_type,wa_posX,wa_posY,wa_posZ,wa_rotX,wa_rotY,wa_rotZ,wa_option1,wa_option2,wa_option3)VALUES('%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d')",tNextWAID, nWorldID, nActorID, nFuncID, nPosX, nPosY, nPosZ, nRotX, nRotY, nRotZ, nOpt1, nOpt2, nOpt3);
    //if (gDevDebug) Console->Print("DEBUG: Executing SQL query %s", query);
    if ( MySQL->GameQuery(query) )
    {
        Console->Print(RED, BLACK, "WorldActors::AddWorldActor could not add worldactor to database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return 0;
    }
    //if (gDevDebug) Console->Print("DEBUG: Successfully added WorldActor to Database");

    // Now spawn the actor right away!
    SpawnWA(nWorldID, nActorID, nFuncID, tNextWAID, nPosX, nPosY, nPosZ, nRotX, nRotY, nRotX);

    return tNextWAID;
}

void PWorldActors::DelWorldActor(PClient* nClient, uint32_t nWAid)
{
    char query[100];
    uint16_t tLoc = nClient->GetChar()->GetLocation();
    if (gDevDebug) Console->Print("DEBUG: Removing worldactor %d from world %d", nWAid, tLoc);

    snprintf(query, 100, "DELETE FROM `world_actors` WHERE `wa_actor_map` = %d AND `wa_actor_id` = %d", tLoc, nWAid);
    //if (gDevDebug) Console->Print("DEBUG: Executing query %s", query);
    if(MySQL->GameQuery(query))
    {
        Console->Print(RED, BLACK, "PWorldActors::DelWorldActor could not delete WorldActor");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return;
    }
    VanishWA(tLoc, nWAid);
}

void PWorldActors::GetWAoption(uint32_t nWAid, uint16_t nWorld, uint16_t &nValue1, uint16_t &nValue2, uint16_t &nValue3)
{
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    nValue1 = 0;
    nValue2 = 0;
    nValue3 = 0;
    snprintf(query, 100, "SELECT * FROM `world_actors` WHERE `wa_actor_map` = %d AND `wa_actor_id` = %d", nWorld, nWAid);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetWAoption could not get WorldActor options");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return;
    }
    if(mysql_num_rows(result) == 0)
    {
        MySQL->FreeGameSQLResult(result);
        return;
    }

    row = mysql_fetch_row(result);
    nValue1 = atoi(row[wa_option1]);
    nValue2 = atoi(row[wa_option2]);
    nValue3 = atoi(row[wa_option3]);

    MySQL->FreeGameSQLResult(result);
}

int PWorldActors::GetWASQLID(uint32_t nWAid, uint32_t nWorld)
{
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `world_actors` WHERE `wa_actor_map` = %d AND `wa_actor_id` = %d", nWorld, nWAid);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetWASQLID could not get WorldActor SQL ID");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return -1;
    }
    if(mysql_num_rows(result) == 0)
    {
        MySQL->FreeGameSQLResult(result);
        return 0;
    }

    row = mysql_fetch_row(result);
    int tWAid = atoi(row[wa_id]);
    MySQL->FreeGameSQLResult(result);

    return tWAid;
}

bool PWorldActors::IsDynamicActor(uint32_t nWAid)
{
    MYSQL_RES *result = nullptr;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `world_actors` WHERE `wa_actor_id` = %d", nWAid);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::IsDynamicActor could not check if worldactor is dynamic");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }
    if(mysql_num_rows(result) == 0)
    {
        MySQL->FreeGameSQLResult(result);
        return false;
    }
    else if(mysql_num_rows(result) == 1)
    {
        MySQL->FreeGameSQLResult(result);
        return true;
    }
    else if(mysql_num_rows(result) > 1)
    {
        MySQL->FreeGameSQLResult(result);
        Console->Print("%s Duplicate entry for WorldActorID %d found", Console->ColorText(YELLOW, BLACK, "[Notice]"), nWAid);
        return true;
    }
    MySQL->FreeGameSQLResult(result);
    return false;
}

int PWorldActors::GetWorldActorFunctionID(uint32_t nWAid)
{
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `world_actors` WHERE `wa_actor_id` = %d", nWAid);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetWorldActorFunctionID could not get WorldActor Function ID");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return -1;
    }
    if(mysql_num_rows(result) == 0)
    {
        MySQL->FreeGameSQLResult(result);
        return 0;
    }

    row = mysql_fetch_row(result);
    int tFuncID = atoi(row[wa_actor_type]);
    MySQL->FreeGameSQLResult(result);

    return tFuncID;
}

void PWorldActors::GetFrontPos(uint32_t nWAID, uint16_t* mX, uint16_t* mY, uint16_t* mZ)
{
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    uint16_t tNewX = 0;
    uint16_t tNewY = 0;
    uint16_t tNewZ = 0;

    snprintf(query, 100, "SELECT * FROM `world_actors` WHERE `wa_actor_id` = %d", nWAID);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetFrontPos could not get X/Y/Z values for worldactor");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return;
    }
    if(mysql_num_rows(result) == 0)
    {
        MySQL->FreeGameSQLResult(result);
        return;
    }

    row = mysql_fetch_row(result);
    tNewX = (uint16_t)atoi(row[wa_posX]);
    tNewY = (uint16_t)atoi(row[wa_posY]);
    tNewZ = (uint16_t)atoi(row[wa_posZ]);

    if(tNewX > 0) *mX = tNewX - 768;
    if(tNewY > 0) *mY = tNewY - 768;
    if(tNewZ > 0) *mZ = tNewZ - 768;

    MySQL->FreeGameSQLResult(result);

    return;
}

int PWorldActors::GetLinkedObjectID(uint32_t nWAID)
{
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `world_actors` WHERE `wa_actor_id` = %d", nWAID);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetLinkedObjectID could not get Linked object ID");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return 0;
    }
    if(mysql_num_rows(result) == 0)
    {
        MySQL->FreeGameSQLResult(result);
        return 0;
    }

    row = mysql_fetch_row(result);
    int tLinkID = atoi(row[wa_option1]);
    MySQL->FreeGameSQLResult(result);

    return tLinkID;
}

bool PWorldActors::IsValidWAFunction(int nFunctionID)
{
    if(nFunctionID == 0) // Special case for "unuseable" objects. 0 means "just stay in the world and do nothing"
        return true;

    // Check if given WO function is a valid one
    const PDefWorldModel* tFurnitureModel = GameDefs->WorldModels()->GetDef(nFunctionID);
    if (tFurnitureModel == nullptr)
        return false;
    else
        return true;
}

bool PWorldActors::RequiresLinkedObject(int nFunctionID)
{
    // Some worldactors need linked objects.
    if(nFunctionID == 6 || nFunctionID == 11 || nFunctionID == 12 || nFunctionID == 13 || nFunctionID == 18 || nFunctionID == 23)
        return true;
    else
        return false;
}

bool PWorldActors::IsValidLinkedObject(PClient *nClient, uint16_t nOption1, int nFunctionID)
{
    /*

    Attention is required for the following WorldActors:
    6 - Genrep
        "LinkedObjectID" must be a value called GROrder. For whatever this is...
    11 - Apartment Access / Communicator IF
        "LinkedObjectID" must link to an Door in that sector
    12 - Standard button
        "LinkedObjectID" must link to an Door in that sector
    13 - Hackbutton
        "LinkedObjectID" must link to an Door in that sector
    23 - Money Buttons
        "LinkedObjectID" must link to an Door in that sector

    6   - Not possible yet.
    11  - Needs door
    12  - Needs door
    13  - Needs door
    18  - Needs target world and entry point
    23  - Needs door
    */
    // Check if given linked ObjectID is a valid worldobject
    switch (nFunctionID)
    {
        case 6:
        {
            return false;
        }
        case 11:
        case 12:
        case 13:
        case 23:
        {
            PWorld* CurrentWorld = Worlds->GetWorld(nClient->GetChar()->GetLocation());
            const PDoorTemplate* tmpDoor = CurrentWorld->GetDoor(nOption1);
            if(tmpDoor)
                return true;
            break;
        }
        case 18:
        {
            if(Worlds->IsValidWorld(nOption1) == true)
                return true;
        }
    }
    return false;
}

void PWorldActors::DoActorCheck()
{
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[200];
    char query2[100];

    snprintf(query, 200, "SELECT COUNT(`wa_actor_id`),`wa_actor_id` FROM `world_actors` GROUP BY `wa_actor_id` HAVING COUNT(`wa_actor_id`)>1");

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PWorldActors::DoActorCheck unable to check for double actorIDs");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return;
    }
    if(mysql_num_rows(result) == 0)  // Fine, we dont have double IDs
    {
        MySQL->FreeGameSQLResult(result);
        return;
    }

    int tCount = 0;
    int tID = 0;
    while((row = mysql_fetch_row(result)))
    {
        tCount = atoi(row[0]);
        tID = atoi(row[1]);
        Console->Print("%s found double worldactor ID! (ID: %d Count: %d) Now erasing...", Console->ColorText(YELLOW, BLACK, "[Notice]"), tID, tCount);
        snprintf(query2, 100, "DELETE FROM `world_actors` WHERE `wa_actor_id` = %d", tID);
        if(MySQL->GameQuery(query2))
        {
            Console->Print(RED, BLACK, "PWorldActors::DoActorCheck unable to delete double entry %d", tID);
            Console->Print("Query was:");
            Console->Print("%s", query2);
            MySQL->ShowGameSQLError();
        }
    }
    MySQL->FreeGameSQLResult(result);
    return;
}
