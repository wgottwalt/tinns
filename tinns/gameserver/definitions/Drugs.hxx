#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefDrug : public PDef {
protected:
    //int32_t mIndex;
    int32_t mType;
    //int32_t mUseSound;
    int32_t mDuration;
    int32_t mChangeNum;
    int32_t mChangeType[8]; // 1: bonus, 2:malus, ... other ?
    float mChangeScale[8];
    int32_t mChangeTarget[8];

public:
    PDefDrug();
    //~PDefDrug();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetType() const { return mType; }
    inline int32_t GetDuration() const { return mDuration; }
    inline int32_t GetChangeNum() const { return mChangeNum; }
    inline int32_t GetChangeType( int32_t nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < mChangeNum ) ) ? mChangeType[nIdx] : 0 ) ; }
    inline float GetChangeScale( int32_t nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < mChangeNum ) ) ? mChangeScale[nIdx] : 0 ) ; }
    inline int32_t GetChangeTarget( int32_t nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < mChangeNum ) ) ? mChangeTarget[nIdx] : 0 ) ; }
};

//type of drug:
//1 normal Drug
//2 Skill ver�ndernder PSI Spruch K�mpfer
//3 Skill ver�ndernder PSI Spruch Supporter
//4 Skill ver�ndernder PSI Spruch Resists
//6+A274  PSI entferne Sprcuh

// drugeffects  < 1000  Subskill
//  <2000 Skill
//  <2100 Energy  (permanent)
//  <2200 Maxenergy
//  <2300 Armor
//  <2400 Subskills
//  3000-3500 Actionmods
