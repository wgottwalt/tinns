#pragma once

#include <cstdint>
#include <map>

struct s_floatcoords {
    float mX;
    float mY;
    float mZ;
};
typedef std::map<uint32_t, s_floatcoords*> PWaypointMap;

class PNPCTemplate {
    friend class PWorldDatParser;

private:

    PWaypointMap mWayPoints;

    uint32_t       mUnknown1; // Is always 0x20001200, in every log. maybe header for NPCs?
    float       mPosY;
    float       mPosZ;
    float       mPosX;
    uint32_t       mNPCTypeID; //npc type in npc.def
    uint8_t        mActorStringSize; //string size with ending 0
    uint8_t        mAngleStringSize; //string size with ending 0
    uint16_t       mNpcID;
    uint8_t        mHasAdditionalCoords; // Seems to be the NUMBER OF additional Coords for the NPC ("Coords" = X/Y/Z in float, not uint16_t!!)
    uint8_t        mUnknown2a;
    uint8_t        mUnknown2b;
    uint8_t        mUnknown2c;
    uint16_t       mTradeID;  //mUnknown3; //00 00 ?
    uint16_t       mUnknown4; //04 00 ?
    std::string    mActorName;
    std::string    mAngle;

    /* // Not sure about that. Commented out until someone finds out how to deal with those "extra" informations
    float mWaypoint1_Y;
    float mWaypoint1_Z;
    float mWaypoint1_X;
    float mWaypoint2_Y;
    float mWaypoint2_Z;
    float mWaypoint2_X;
    float mWaypoint3_Y;
    float mWaypoint3_Z;
    float mWaypoint3_X;
    float mWaypoint4_Y;
    float mWaypoint4_Z;
    float mWaypoint4_X;
    float mWaypoint5_Y;
    float mWaypoint5_Z;
    float mWaypoint5_X;
    */

public:
    PNPCTemplate();
    ~PNPCTemplate();

    void AddWayPoint(float nX, float nY, float nZ, uint8_t nID);
    // TODO: Add GetWaypoint function

    inline uint32_t GetUnknown1() const
    {
        return mUnknown1;
    };
    inline float GetPosX() const
    {
        return mPosY;
    };
    inline float GetPosY() const
    {
        return mPosZ;
    };
    inline float GetPosZ() const
    {
        return mPosX;
    };
    inline uint32_t GetNPCTypeID() const
    {
        return mNPCTypeID;
    };
    inline uint8_t GetActorStrSize() const
    {
        return mActorStringSize;
    };
    inline uint8_t GetAngleStrSize() const
    {
        return mAngleStringSize;
    };
    inline uint16_t GetNpcID() const
    {
        return mNpcID;
    };
    inline uint8_t GetHasAdditionalCoords() const
    {
        return mHasAdditionalCoords;
    };
    inline uint8_t GetUnknown2a() const
    {
        return mUnknown2a;
    };
    inline uint8_t GetUnknown2b() const
    {
        return mUnknown2b;
    };
    inline uint8_t GetUnknown2c() const
    {
        return mUnknown2c;
    };
    inline uint16_t /*GetUnknown3*/GetTradeID() const
    {
        return mTradeID;//mUnknown3;
    };
    inline uint16_t GetUnknown4() const
    {
        return mUnknown4;
    };
    inline std::string GetActorName() const
    {
        return mActorName;
    };
    inline std::string GetAngle() const
    {
        return mAngle;
    };


    inline void SetUnknown1( uint32_t nValue )
    {
        mUnknown1 = nValue;
    };
    inline void SetPosX( float nValue )
    {
        mPosY = nValue;
    };
    inline void SetPosY( float nValue )
    {
        mPosZ = nValue;
    };
    inline void SetPosZ( float nValue )
    {
        mPosX = nValue;
    };
    inline void SetNPCTypeID( uint32_t nValue )
    {
        mNPCTypeID = nValue;
    };
    inline void SetActorStrSize( uint8_t nValue )
    {
        mActorStringSize = nValue;
    };
    inline void SetAngleStrSize( uint8_t nValue )
    {
        mAngleStringSize = nValue;
    };
    inline void SetNpcID( uint16_t nValue )
    {
        mNpcID = nValue;
    };
    inline void SetHasAdditionalCoords( uint8_t nValue )
    {
        mHasAdditionalCoords = nValue;
    };
    inline void SetUnknown2a( uint8_t nValue )
    {
        mUnknown2a = nValue;
    };
    inline void SetUnknown2b( uint8_t nValue )
    {
        mUnknown2b = nValue;
    };
    inline void SetUnknown2c( uint8_t nValue )
    {
        mUnknown2c = nValue;
    };
    inline void /*SetUnknown3*/SetTradeID( uint16_t nValue )
    {
        /*mUnknown3*/mTradeID = nValue;
    };
    inline void SetUnknown4( uint16_t nValue )
    {
        mUnknown4 = nValue;
    };
    inline void SetActorName( std::string nValue )
    {
        mActorName = nValue;
    };
    inline void SetAngle( std::string nValue )
    {
        mAngle = nValue;
    };
};
