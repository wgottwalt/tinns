#include <cstring>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PGenrepList::PGenrepList(uint32_t nOwnerCharID)
{
    mOwnerCharID = nOwnerCharID;
    mListMaxSize = mListSize = 0;
    mGenrepList = nullptr;
}

PGenrepList::~PGenrepList()
{
    if (mGenrepList)
        delete[] mGenrepList;
}

bool PGenrepList::AddGenrep(uint16_t nWorldID, uint16_t nStationID)
{
    char query[256];

    if ((FindEntry(nWorldID, nStationID) >= mListSize) && (mListSize < 255))
    {
        if (mListSize == mListMaxSize)
        {
            IncreaseMaxSize();
        }
        mGenrepList[mListSize].mWorldID = nWorldID;
        mGenrepList[mListSize++].mStationID = nStationID;

        snprintf(query, 256, "INSERT INTO genrep (g_id,g_worldid,g_stationid,g_charid) VALUES (NULL,'%u','%u','%u');", nWorldID, nStationID, mOwnerCharID);
        if ( MySQL->GameQuery(query) )
        {
            Console->Print(RED, BLACK, "PGenrepList::AddGenrep could not add some genrep entry in the database");
            Console->Print("Query was:");
            Console->Print("%s", query);
            MySQL->ShowGameSQLError();
            return false;
        }

        return true;
    }
    else
        return false;
}

/*bool PGenrepList::RemoveChar(uint32_t nBuddyCharID)
{
  char query[256];
  uint8_t rEntry, i;

  if ((rEntry = FindEntry(nBuddyCharID)) < mListSize)
  {
    --mListSize;
    for (i = rEntry; i < mListSize; i++)
    {
      mGenrepList[i] = mGenrepList[i+1];
    }
    // Remove from DB here
    snprintf(query, 256, "DELETE FROM buddy_list WHERE ((bud_charid='%u') AND (bud_buddyid='%u')) LIMIT 1;", mOwnerCharID, nBuddyCharID);
    if ( MySQL->GameQuery(query) )
    {
        Console->Print(RED, BLACK, "PBuddyList::AddChar could not add some buddylist entry in the database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }

    return true;
  }
  else
    return false;
}*/

bool PGenrepList::SQLLoad()
{
    char query[256];
    MYSQL_RES *result;
    MYSQL_ROW row;
    uint8_t EntriesNum;

    snprintf(query, 256, "SELECT * FROM genrep WHERE (g_charid='%u')", mOwnerCharID);
    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PGenrepList::SQLLoad could not load genreplist from the database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }
    //Console->Print(GREEN, BLACK, "PGenrepList::SQLLoad Loading Genrep list for char %d", mOwnerCharID);
    mListSize = 0;
    if((EntriesNum = mysql_num_rows(result)))
    {
        IncreaseMaxSize(EntriesNum);

        while((row = mysql_fetch_row(result)))
        {
            mGenrepList[mListSize].mWorldID = atoi(row[g_worldid]);
            mGenrepList[mListSize++].mStationID = atoi(row[g_stationid]);
            //Console->Print(GREEN, BLACK, "PGenrepList::SQLLoad Genrep list entry %d : world %d station %d", mListSize, mGenrepList[mListSize-1].mWorldID,mGenrepList[mListSize-1].mStationID);
        }
    }
    MySQL->FreeGameSQLResult(result);
    return true;
}

void PGenrepList::IncreaseMaxSize(uint8_t nNewMax)
{
    uint16_t tmpSize;

    if (!nNewMax)
    {
        tmpSize = mListMaxSize + GENREPLIST_ALLOC_SIZE;
    }
    else if (nNewMax > mListMaxSize)
    {
        tmpSize = nNewMax / GENREPLIST_ALLOC_SIZE;
        if (nNewMax % GENREPLIST_ALLOC_SIZE)
            ++tmpSize;
        tmpSize *= GENREPLIST_ALLOC_SIZE;
    }
    else
        return;

    mListMaxSize = (tmpSize < 256) ? tmpSize : 255;

    PGenrepEntry* tmpList = new PGenrepEntry[mListMaxSize];
    if (mGenrepList)
    {
        if (mListSize)
        {
            memcpy(tmpList, mGenrepList, sizeof(PGenrepEntry) * mListSize);
        }
        delete[] mGenrepList;
    }
    mGenrepList = tmpList;
}

uint8_t PGenrepList::FindEntry(uint16_t nWorldID, uint16_t nStationID)
{
    uint8_t i = 255;

    if (mGenrepList)
    {
        for (i = 0; i < mListSize; i++)
        {
            if ((mGenrepList[i].mWorldID == nWorldID) && (mGenrepList[i].mStationID == nStationID))
                break;
        }
    }

    return i;
}
