#pragma once

#include <cstdint>
#include <map>

#define WORLDDATATEMPLATE_MAXPOSITEMS 11

class PFurnitureItemTemplate;
typedef std::map<uint32_t, PFurnitureItemTemplate*> PFurnitureItemsMap;

class PDoorTemplate;
typedef std::map<uint32_t, PDoorTemplate*> PDoorsMap;

class PNPCTemplate;
typedef std::map<uint32_t, PNPCTemplate*> PNPCsMap;

class PWorldDataTemplate
{
private:
    std::string mName; // (datfile) relative path+filename without leading ./ or ./worlds/ nor .dat extension
    std::string mBspName; // (bsp file) relative path+filename without leading ./ or ./worlds/ nor .bsp extension
    PFurnitureItemsMap mFurnitureItems;
    PDoorsMap mDoors;
    PNPCsMap mNPCs;
    PFurnitureItemTemplate* mPositionItems[WORLDDATATEMPLATE_MAXPOSITEMS];

    int mUseCount;

    void DatFileDataCleanup();
    void SetLinkedObjects(); // This method implements some workarouds for some world objects on which we lack info.

public:
    PWorldDataTemplate();
    ~PWorldDataTemplate();

    bool LoadDatFile(const std::string& WorldTemplateName, const std::string& nFilename, const bool nTestAccesOnly = false);
    inline const std::string& GetName()
    {
        return mName;
    }
    inline const std::string& GetBspName()
    {
        return mBspName;
    }

    inline void IncreaseUseCount()
    {
        ++mUseCount;
    }
    inline int DecreaseUseCount()
    {
        return (mUseCount ? --mUseCount : 0);
    }
    inline int GetUseCount()
    {
        return mUseCount;
    }

    uint32_t AddFurnitureItem(PFurnitureItemTemplate* nItem);
    const PFurnitureItemTemplate* GetFurnitureItem(uint32_t ItemID);
    bool getPositionItemPosition(uint8_t PosID, float* pX, float* pY, float* pZ);

    uint32_t AddDoor(PDoorTemplate* nDoor);
    const PDoorTemplate* GetDoor(uint32_t DoorID);

    uint32_t AddNPC(PNPCTemplate* nNPC);

    // External functions for NPCManager
    const PNPCTemplate* GetNPC(uint32_t NPCID);
    inline const PNPCsMap *GetNPCMap() const
    {
        return &mNPCs;
    }; // called by class PNPCWorld to get all NPCs for this world
};
