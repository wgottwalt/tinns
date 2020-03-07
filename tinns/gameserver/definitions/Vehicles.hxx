#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefVhc : public PDef {
private:
    //int32_t mIndex; // field 1
    int32_t mModel;
    std::string mName;
    /*
    float mLeftFront; // field 4
    float mFront;
    float mRightBack;
    float mBack;
    float mSideFriction;
    float mDownFriction;
    float mForwardFriction;
    float mAcceleration;
    float mBrakeFactor;
    float mTurnSpeed;
    float mFullTurnDelay;
    int32_t mAnimClass; // field 15
    */
    int32_t mSeatId[8]; // fields 16 - 23
    /*
    float mSpeedTiltFactor; // field 24
    float mSpeedGlideFactor;
    float mMinHover;
    float mMaxHover;
    float mHoverLoopLen;
    int32_t mWheelCnt;
    float mWheelSpeed;
    float mMaxDive;
    int32_t mEffectsID;
    int32_t mShowDebugBouncer; // field 33
    */
    int32_t mHealth;
    int32_t mArmor;
    //int32_t mSoundStartindex; // field 36

    // Additionnal info
    int32_t mNumSeats;

public:
    PDefVhc();
    //~PDefVhc();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetModel() const { return mModel; }
    inline const std::string &GetName() const { return mName; }
    inline int32_t GetSeatId( int32_t nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < 8 ) ) ? mSeatId[nIdx] : -1) ; }
    inline int32_t GetHealth() const { return mHealth; }
    inline int32_t GetArmor() const { return mArmor; }
    inline int32_t GetNumSeats() const { return mNumSeats; }
};
