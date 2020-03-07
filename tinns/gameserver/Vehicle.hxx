#pragma once

#include <cstdint>
#include <map>
#include <queue>
#include <vector>

class PDefVhc;

class PVhcCoordinates {
    friend class PSpawnedVehicle;

  private:
    uint16_t mY;     // Y-Position in world
    uint16_t mZ;     // Z-Position in world
    uint16_t mX;     // X-Position in world
    uint8_t mUD;     // Up - Mid - Down (d6 - 80 - 2a) // Pitch
    uint16_t mLR;     // Yaw
    uint16_t mRoll;
    uint16_t mUnknown; // Usually 0x0001
    uint8_t mFF; // Usually 0xff ...
    uint8_t mAct;    // Last user action state
    // mAct: bit field
    //0x00 = not moving
    //0x01 = Left
    //0x02 = Right
    //0x04 = Forward
    //0x08 = Back
    //0x20 = Pushing down
    //0x40 = Pulling up

  public:
    inline PVhcCoordinates() { mX = mY = mZ = mAct = 0; mUD = 128; mLR = 34683; mRoll = 32403;}

    void SetPosition( uint16_t nY, uint16_t nZ, uint16_t nX, uint8_t nUD, uint16_t nLR, uint16_t nRoll, uint8_t nAct = 0, uint16_t nUnknown = 1, uint8_t nFF = 0xff );
    void SetInterpolate( const PVhcCoordinates& Pos1, const PVhcCoordinates& Pos2, float nCoef );
    inline uint16_t GetX() const { return mX; }
    inline uint16_t GetY() const { return mY; }
    inline uint16_t GetZ() const { return mZ; }
    inline uint8_t GetUD() const { return mUD; }
    inline uint16_t GetLR() const { return mLR; }
    inline uint16_t GetRoll() const { return mRoll; }
    inline uint8_t GetAct() const { return mAct; }
    inline uint8_t GetUnknown() const { return mUnknown; }
    inline uint8_t GetFF() const { return mFF; }
};

class PVehicleInformation
{
    friend class PSpawnedVehicle;
    friend class PVehicles;

  private:
    uint32_t mVehicleId;
    uint32_t mOwnerCharId;
    uint32_t mHealth; // or float ???
    uint8_t mVehicleType;
    uint8_t mStatus; //vhcStatus 0:parking, 1:in_service, 2:destroyed

  public:
    inline PVehicleInformation( uint32_t nVehicleId = 0, uint32_t nOwnerCharId = 0, uint32_t nHealth = 0, uint8_t nVehicleType = 0, uint8_t nStatus = 0 ) :
        mVehicleId( nVehicleId ),
        mOwnerCharId( nOwnerCharId ),
        mHealth( nHealth ),
        mVehicleType( nVehicleType ),
        mStatus( nStatus )
    { }

    inline uint32_t GetVehicleId() const { return mVehicleId; }
    inline uint32_t GetOwnerCharId() const { return mOwnerCharId; }
    inline uint32_t GetHealth() const { return mHealth; } // or float ???
    inline uint8_t GetVehicleType() const { return mVehicleType; }
    inline uint8_t GetStatus() const { return mStatus; }
    bool SetStatus( uint8_t nStatus );

    bool Load( uint32_t nVehicleId );
    bool Save();
    bool Destroy();
};

class PSpawnedVehicle
{
  private:
    static const uint8_t mSeatsFlags[];

  private:
    uint32_t mLocalId;
    PVehicleInformation mInfo;
    uint32_t mLocation;
    PVhcCoordinates mCoords;
    const PDefVhc* mVhcDef;

    uint32_t mSeatUserId[8];
    uint8_t mFreeSeatsFlags;
    uint8_t mNbFreeSeats;

    uint16_t minmax[4][2]; //Temp

  public:
    PSpawnedVehicle( uint32_t nLocalId, PVehicleInformation const* nVhcInfo, uint32_t nLocation, PVhcCoordinates const* nVhcPos );

    inline uint32_t GetVehicleId() const { return mInfo.mVehicleId; }
    inline uint32_t GetLocalId() const { return mLocalId; }
    inline const PVhcCoordinates& GetPosition() const { return mCoords; }
    inline const PVehicleInformation& GetInformation() const { return mInfo; }
    inline bool SetStatus( uint8_t nStatus ) { return mInfo.SetStatus( nStatus ); }

    void SetLocation( uint32_t nLocation );
    inline uint32_t GetLocation() const { return mLocation; }
    void SetPosition( PVhcCoordinates const* nVhcPos );

    inline bool Save() { return mInfo.Save(); }

    int GetNumSeats() const;
    inline uint32_t GetSeatUser( uint8_t nSeatId ) const { return (( nSeatId < 8 ) ? mSeatUserId[nSeatId] : 0 ); }
    bool SetSeatUser( uint8_t nSeatId, uint32_t nCharId );
    bool UnsetSeatUser( uint8_t nSeatId, uint32_t nCharId );
    bool IsCharInside( uint32_t nCharId ) const;
    inline uint8_t GetFreeSeatsFlags() const { return mFreeSeatsFlags; }
    inline uint8_t GetNbFreeSeats() const { return mNbFreeSeats; }
    uint8_t GetFirstFreeSeat() const;

    //SetHealth(const uint32_t nHealth);
    //uint32_t DoDamage(const uint32_t nHealthDec);
    //uint32_t DoRepair(const uint32_t nHealthInc);
};

typedef std::map<uint32_t, PSpawnedVehicle*> PSpawnedVhcMap;
typedef std::queue<PVehicleInformation*> PVhcInfoList;
class PVehicles
{
  private:
    PSpawnedVhcMap mSpawnedVehicles;

    bool RegisterSpawnedVehicle( PSpawnedVehicle*  nSpawnedVehicle );
    bool UnregisterSpawnedVehicle( uint32_t nVehicleId );

  public:
    PVehicles();
    ~PVehicles();

    //uint32_t CreateVehicle(uint32_t nOwnerChar, uint8_t mVehicleType);
    //bool RegisterVehicleOwner(uint32_t nVehiculeId, uint32_t nOwnerChar);
    //bool DestroyVehicle(uint32_t nVehiculeId);
    bool IsValidVehicle( uint32_t nVehicleId, bool nCheckOwner = false, uint32_t nOwnerId = 0 ) const;
    inline bool IsSpawned( uint32_t nVehicleId ) const { return ( mSpawnedVehicles.find( nVehicleId ) != mSpawnedVehicles.end() ); }
    PSpawnedVehicle* GetSpawnedVehicle( uint32_t nVehicleId ) const;
    bool GetVehicleInfo( uint32_t nVehicleId, PVehicleInformation* nInfo ) const;
    PSpawnedVehicle* SpawnVehicle( uint32_t nVehicleId, uint32_t nLocation, PVhcCoordinates const* nVhcPos ); // Refuses for subway zone atm
    bool UnspawnVehicle( uint32_t nVehicleId );

    PVhcInfoList* GetCharVehicles( uint32_t nCharId, uint16_t nMaxCount = 0, uint16_t nStartIndex = 0 );

};

typedef std::vector<PSpawnedVehicle*> PSpawnedVhcVector;
typedef std::queue<PSpawnedVehicle*> PSpawnedVhcList;

class PSpawnedVehicles
{
    friend class PWorld;

  public:
    static const uint32_t mVhcBaseLocalId = 0x03fd; //Vhc local Id are allocated downward from this value. Could br 0x3ff, but 0x3fe is used as "target" when shooting nowhere
    static const uint16_t mMaxLocalVhc = 127;

    inline static bool IsPotentialSpawnedVehicle( uint32_t nLocalId )
    { return (( nLocalId <= mVhcBaseLocalId ) && (( mVhcBaseLocalId - nLocalId ) < mMaxLocalVhc ) ); }

  private:
    PSpawnedVhcVector mSpawnedVehicles;
    uint16_t mNextFreeHint;
    uint32_t mLocation;

    inline void SetLocation( uint32_t nLocation ) { mLocation = nLocation; }

  public:
    PSpawnedVehicles();
    ~PSpawnedVehicles();

    inline bool IsSpawned( uint32_t nLocalId ) const { return (( nLocalId <= mVhcBaseLocalId ) && (( mVhcBaseLocalId - nLocalId ) < mSpawnedVehicles.size() ) && mSpawnedVehicles[mVhcBaseLocalId-nLocalId] ); }
    PSpawnedVehicle* SpawnVehicle( PVehicleInformation const* nVhcInfo, PVhcCoordinates const* nVhcPos );
    PSpawnedVehicle* GetVehicle( uint32_t nLocalId );
    PSpawnedVehicle* GetVehicleByGlobalId( uint32_t nVehicleId ) const;
    bool UnspawnVehicle( uint32_t nVehicleId );

    PSpawnedVhcList* GetSpawnedVehicles() const;
};
