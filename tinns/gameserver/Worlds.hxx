#pragma once

#include <cstdint>
#include <map>
#include <string>

class PDefWorldModel;

typedef std::map<uint32_t, PNPCTemplate*> PNPCsMap;
typedef std::map<uint32_t, int32_t> PChairsInUseMap;

class PWorld {
    friend class PWorlds;

public:
    static uint16_t const mZoneOutLimitOffset;
    static uint16_t const mBottomZoneOutLimit;
    static uint16_t const mBottomZoneInLimit;
    static uint16_t const mTopZoneOutLimit;
    static uint16_t const mTopZoneInLimit;

private:
    uint32_t mID;
    bool mIsAppartment;
    int32_t mUseCount;
    PWorldDataTemplate* mWorldDataTemplate;
    PChairsInUseMap mChairsInUseMap;
    PSpawnedVehicles mSpawnedVehicles;

    inline void IncreaseUseCount()
    {
        ++mUseCount;
    }
    inline int32_t DecreaseUseCount()
    {
        return ( mUseCount ? --mUseCount : 0 );
    }
    inline int32_t GetUseCount()
    {
        return mUseCount;
    }
    bool Load( uint32_t nWorldID );

public:
    PWorld();
    ~PWorld();

    std::string GetName();
    std::string GetBspName();
    bool IsAppartment();
    const PFurnitureItemTemplate* GetFurnitureItemTemplate( uint32_t nItemID );
    const PDefWorldModel *GetFurnitureItemModel(uint32_t nItemID);
    const PDoorTemplate *GetDoor(uint32_t nDoorID);
    bool getPositionItemPosition(uint8_t PosID, float *pX, float *pY, float *pZ);

    bool CharUseChair( int32_t CharLocalID, uint32_t nItemID );
    void CharLeaveChair( int32_t CharLocalID, uint32_t nItemID );

    PClient* GetClientByCharLocalId( uint32_t rawObjectId ) const; // returns Client if object is a PC char, and 0 if not.

    PSpawnedVehicles *GetSpawnedVehicles();
    bool CheckVhcNeedZoning(PVhcCoordinates const *nPos) const;
    uint32_t GetVhcZoningDestination(PSpawnedVehicle const* nVhc, PVhcCoordinates* nPos = 0) const;

    // Evil thing... bounced through stuff :| wasnt able to find a better solution for this
    const PNPCsMap *GetNPCMap() const;
    const PNPCTemplate *GetNPCTemplate(uint32_t nNPCID) const;
};


typedef std::map<uint32_t, PWorld*> PWorldsMap;
typedef std::map<std::string, PWorldDataTemplate*> PWorldDataTemplatesMap;

class PWorlds
{
    friend class PWorld;

public:
    static uint32_t const mNcSubwayWorldId;
    static uint32_t const mAptBaseWorldId;
    static uint32_t const mOutdoorBaseWorldId;
    static uint32_t const mOutdoorWorldIdVIncrement;
    static uint8_t const mOutdoorWorldmapHSize;
    static uint8_t const mOutdoorWorldmapVSize;
    static uint32_t const mOutdoorMaxWorldId;

private:
    bool mPreloadWorldsTemplates;
    bool mPreloadStaticWorlds;

    PWorldsMap mStaticWorldsMap; // lists all valid static worlds, with second=NULL if not loaded
    PWorldsMap mOnDemandWorldsMap; // mostly appartments. Could be used for instance dungeons too, but instance dungeons are crap :p
    PWorldDataTemplatesMap mWorldDataTemplatesMap;

    bool LeaseWorldDataTemplate( const std::string& nBspName, const std::string& nFileName, const bool nPreloadPhase = false );
    void ReleaseWorldDataTemplate( const std::string& nFileName );
    void UnloadWorldDataTemplate( const std::string& nFileName );
    PWorldDataTemplate* GetWorldDataTemplate( const std::string& nFileName );
    PWorld* LeaseWorld( uint32_t nWorldID, const bool nPreloadPhase );

public:
    PWorlds();
    ~PWorlds();

    bool LoadWorlds();
    bool IsValidWorld( uint32_t nWorldID ) const;
    inline PWorld* LeaseWorld( uint32_t nWorldID )
    {
        return LeaseWorld( nWorldID, false );
    }
    PWorld* GetWorld( uint32_t nWorldID );
    void ReleaseWorld( uint32_t nWorldID );
    bool IsAppartment( uint32_t nWorldID );
    inline bool IsPotentialAppartement( uint32_t nWorldID )
    {
        return ( nWorldID > PWorlds::mAptBaseWorldId );
    }

    void Update();
    void Shutdown();

    uint32_t GetWorldIdFromWorldmap( uint8_t mapH, uint8_t mapV ) const;  // H & V pos are 0-based
    bool GetWorldmapFromWorldId( uint32_t nWorldId, uint8_t& mapH, uint8_t& mapV ) const;
};
