#pragma once

#include <cstdint>
#include <string>

class PSubway {
  friend class PMsgBuilder;

  struct PSubwayInfo
  {
    uint16_t mVhcId;
    uint16_t mPosition;
    uint8_t mDoorOpened;
    uint32_t mSeatUsersId[4];
  };

  public:
    static const uint16_t mCabsNumber = 11;
    static const uint32_t mCabsBaseId = 0x03f2;
    static const uint32_t mCabsBaseHealth = 100; //should take that from .def instead...
    static const uint8_t mStationsNumber = 8;

  private:
    static const uint16_t mSubwayInitData [];
    static const uint32_t mCabLoopTime;
    static const uint32_t mCab0TimeOffset;
    static const int32_t mTimingAdjust;
    static const uint32_t mCabIntervalTime;
    static const uint32_t mOpenDoorOffset [];
    static const uint32_t mOpenDoorDuration [];
    static const char *mSubwayStationName [];
    static PCharCoordinates mCabExitPositions [2][mStationsNumber];

    PSubwayInfo mSubways[mCabsNumber];

public:
    bool GetInfoIndex(uint32_t nVhcId, uint8_t *Index = nullptr);

  public:
    PSubway();
    //~PSubway();

    inline bool IsValidSubwayCab(uint32_t nVhcId) {return GetInfoIndex(nVhcId); }
    bool UpdateInfo(uint32_t nVhcId, uint16_t nPosition, uint8_t nDoorOpened);
    uint16_t GetPosition(uint32_t nVhcId);

    uint32_t GetTimeOffset(uint32_t nVhcId, uint32_t nTime);
    uint8_t GetStation(uint32_t nVhcId, uint32_t nTime, uint32_t* TimeOffset = nullptr);
    bool IsDoorOpen(uint32_t nVhcId, uint32_t nTime);
    std::string* GetStationName(uint8_t nStationId);
    bool GetStationExitPosition(PCharCoordinates* nPosition, uint8_t nStationId, float nCoef = 0.5);

    uint8_t GetFreeSeat(uint32_t nVhcId);
    bool SetSeatUser(uint32_t nVhcId, uint8_t nSeat, uint32_t nCharId);
    bool UnsetSeatUser(uint32_t nVhcId, uint8_t nSeat, uint32_t nCharId);
};
