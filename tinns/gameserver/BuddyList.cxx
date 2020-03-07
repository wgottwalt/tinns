#include <cstring>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PBuddyList::PBuddyList(uint32_t nOwnerCharID)
{
    mOwnerCharID = nOwnerCharID;
    mListMaxSize = mListSize = 0;
    mCharIDList = nullptr;
}

PBuddyList::~PBuddyList()
{

    if (mCharIDList)
        delete[] mCharIDList;
}

bool PBuddyList::AddChar(uint32_t nBuddyCharID)
{
    char query[256];

    if ((FindEntry(nBuddyCharID) >= mListSize) && (mListSize < 255))
    {
        if (mListSize == mListMaxSize)
        {
            IncreaseMaxSize();
        }
        mCharIDList[mListSize++] = nBuddyCharID;

        snprintf(query, 256, "INSERT INTO buddy_list (bud_id,bud_charid,bud_buddyid) VALUES (NULL,'%u','%u');", mOwnerCharID, nBuddyCharID);
        if ( MySQL->GameQuery(query) )
        {
            Console->Print(RED, BLACK, "PBuddyList::AddChar could not add some buddylisdt entry in the database");
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

bool PBuddyList::RemoveChar(uint32_t nBuddyCharID)
{
    char query[256];
    uint8_t rEntry, i;

    if ((rEntry = FindEntry(nBuddyCharID)) < mListSize)
    {
        --mListSize;
        for (i = rEntry; i < mListSize; i++)
        {
            mCharIDList[i] = mCharIDList[i+1];
        }
        // Remove from DB here
        snprintf(query, 256, "DELETE FROM buddy_list WHERE ((bud_charid='%u') AND (bud_buddyid='%u')) LIMIT 1;", mOwnerCharID, nBuddyCharID);
        if ( MySQL->GameQuery(query) )
        {
            Console->Print(RED, BLACK, "PBuddyList::AddChar could not delete some buddylist entry from the database");
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

bool PBuddyList::SQLLoad()
{
    char query[256];
    MYSQL_RES *result;
    MYSQL_ROW row;
    uint8_t EntriesNum;

    snprintf(query, 256, "SELECT * FROM buddy_list WHERE (bud_charid='%u')", mOwnerCharID);
    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "PBuddyList::SQLLoad could not load buddylist entry from the database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }
    //Console->Print(GREEN, BLACK, "PBuddyList::SQLLoad Loading buddylist for char %d", mOwnerCharID);
    mListSize = 0;
    if((EntriesNum = mysql_num_rows(result)))
    {
        IncreaseMaxSize(EntriesNum);

        while((row = mysql_fetch_row(result)))
        {
            mCharIDList[mListSize++] = atoi(row[bud_buddyid]);
            //Console->Print(GREEN, BLACK, "PBuddyList::SQLLoad Buddylist entry %d : char %d", mListSize, mCharIDList[mListSize-1]);
        }
    }
    MySQL->FreeGameSQLResult(result);
    return true;
}

void PBuddyList::IncreaseMaxSize(uint8_t nNewMax)
{
    uint16_t tmpSize;

    if (!nNewMax)
    {
        tmpSize = mListMaxSize + BUDDYLIST_ALLOC_SIZE;
    }
    else if (nNewMax > mListMaxSize)
    {
        tmpSize = nNewMax / BUDDYLIST_ALLOC_SIZE;
        if (nNewMax % BUDDYLIST_ALLOC_SIZE)
            ++tmpSize;
        tmpSize *= BUDDYLIST_ALLOC_SIZE;
    }
    else
        return;

    mListMaxSize = (tmpSize < 256) ? tmpSize : 255;

    uint32_t* tmpList = new uint32_t[mListMaxSize];
    if (mCharIDList)
    {
        if (mListSize)
        {
            memcpy(tmpList, mCharIDList, sizeof(uint32_t) * mListSize);
        }

        delete[] mCharIDList;
    }
    mCharIDList = tmpList;
}

uint8_t PBuddyList::FindEntry(uint32_t CharID)
{
    uint8_t i = 255;

    if (mCharIDList)
    {
        for (i = 0; i < mListSize; i++)
        {
            if (mCharIDList[i] == CharID)
                break;
        }
    }

    return i;
}

bool PBuddyList::IsInBuddy(uint32_t CharID)
{
    if (mCharIDList)
    {
        for (uint8_t i = 0; i < mListSize; i++)
        {
            if (mCharIDList[i] == CharID)
                return true;
        }
    }
    return false;
}
