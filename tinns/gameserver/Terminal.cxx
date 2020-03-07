#include <cstring>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PTerminal::PTerminal()
{
    snprintf(mConPrefix, 50, "[PConsole]");
    EraseVars();
}

void PTerminal::EraseVars()
{
    memset(mSQLQuery, '\0', 500);
    mResultFields = 0;
}

uint8_t PTerminal::GetNewEmailCount(PClient* nClient, bool nNoticeClient)
{
    MYSQL_RES *result = nullptr;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT count(*) FROM emails WHERE e_toid = %d AND e_new = 1", nClient->GetCharID());
    if(gDevDebug) Console->Print("[DEBUG] Query is: %s", query);
    result = MySQL->GameResQuery(query);
    if(result == nullptr)
    {
        Console->Print(RED, BLACK, "PTerminal::GetNewEmailCount could not get emailcount");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return 0;
    }
    if(mysql_num_rows(result) == 0) // No Mails found
    {
        MySQL->FreeGameSQLResult(result);
        if(gDevDebug) Console->Print("[DEBUG] No new emails for this char");
        return 0;
    }
    if(gDevDebug) Console->Print("[DEBUG] Found new email, sending notice");

    row = mysql_fetch_row(result);
    uint8_t tRetVal = (uint8_t)atoi(row[0]);
    MySQL->FreeGameSQLResult(result);

    if(nNoticeClient)
    {
        PMessage* tmpMsg = MsgBuilder->BuildYouGotEmailsMsg(nClient, tRetVal);
        nClient->SendUDPMessage(tmpMsg);
    }

    return tRetVal;
}
