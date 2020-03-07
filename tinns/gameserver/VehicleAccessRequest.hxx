#pragma once

#include <chrono>
#include <cstdint>
#include <map>

class PVhcAccessRequest {
public:
    //uint32_t mRequestId;
    time_t mTimestamp;
    uint32_t mCharId;
    uint32_t mVhcGlobalId;
    uint8_t mStatus; // 0: no answer yet, 1: access allowed (refused req are dropped) 2: result checked

    PVhcAccessRequest();
    PVhcAccessRequest(uint32_t nRequesterCharId, uint32_t nVhcGlobalId);
};

class PVhcAccessRequestList {
    typedef std::map<uint32_t, PVhcAccessRequest> PVhcAccessRequestMap;

private:
    PVhcAccessRequestMap mActiveRequests;
    uint32_t mNextRequestId;

    time_t mResponseWaitTime; // How long do we wait for owner answer
    time_t mCheckWaitTime; // How long do we wait for user access check (0: remaining time from mResponseWaitTime)
    time_t mReuseWaitTime; // How long do we allow user to re-use the autorization after first check

    void DropTimedOut();

public:
    PVhcAccessRequestList() ;
    ~PVhcAccessRequestList();

    void SetParameters(time_t nResponseWaitTime, time_t nCheckWaitTime, time_t nReuseWaitTime);

    uint32_t Add(uint32_t nRequesterCharId, uint32_t nVhcGlobalId);
    bool GetInfo(uint32_t nRequestId, uint32_t* oRequesterCharId, uint32_t* oVehicleId) const;
    bool RegisterResponse(uint32_t nRequestId, bool nStatus);
    bool Check(uint32_t nRequestId, uint32_t nRequesterCharId = 0, uint32_t nVhcGlobalId = 0);
};
