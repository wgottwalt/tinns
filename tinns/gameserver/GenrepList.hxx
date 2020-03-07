#pragma once

#include <cstdint>

#define GENREPLIST_ALLOC_SIZE 4 // atomicity of list entries allocation

class PGenrepList {
private:
    enum { // genrep DB Table fields
        g_id = 0,
        g_worldid,
        g_stationid,
        g_charid
    };
    struct PGenrepEntry
    {
      uint16_t mWorldID;
      uint16_t mStationID;
    };

    uint32_t mOwnerCharID;
    uint8_t mListMaxSize;
    uint8_t mListSize;
    PGenrepEntry* mGenrepList;

    void IncreaseMaxSize(uint8_t nNewMax = 0);
    uint8_t FindEntry(uint16_t nWorldID, uint16_t nStationID);

  public:
    PGenrepList(uint32_t nOwnerCharID);
    ~PGenrepList();
    bool AddGenrep(uint16_t nWorldID, uint16_t nStationID);
    //bool RemoveChar(uint32_t nBuddyCharID);
    inline uint8_t Count() { return mListSize; }
    uint16_t GetListDataSize() { return (sizeof(PGenrepEntry) * mListSize); }
    const void* GetListData() { return (const void*)mGenrepList; }
    bool SQLLoad();
//    bool SQLSave();
};
