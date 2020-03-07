#pragma once

#include <cstdint>

#define BUDDYLIST_ALLOC_SIZE 4 // atomicity of list entries allocation

class PBuddyList {
  private:
    enum { // buddy_list DB Table fields
        bud_id = 0,
        bud_charid,
        bud_buddyid
    };

    uint32_t mOwnerCharID;
    uint8_t mListMaxSize;
    uint8_t mListSize;
    uint32_t* mCharIDList;

    void IncreaseMaxSize(uint8_t nNewMax = 0);
    uint8_t FindEntry(uint32_t CharID);

  public:
    PBuddyList(uint32_t nOwnerCharID);
    ~PBuddyList();
    bool AddChar(uint32_t nBuddyCharID);
    bool RemoveChar(uint32_t nBuddyCharID);
    inline uint8_t Count() { return mListSize; }
    uint16_t GetListDataSize() { return (sizeof(uint32_t) * mListSize); }
    const void* GetListData() { return (const void*)mCharIDList; }
    bool SQLLoad();

    bool IsInBuddy(uint32_t CharID);
//    bool SQLSave();
};
