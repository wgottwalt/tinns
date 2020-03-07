#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefDamage : public PDef {
private:
    //int32_t mIndex;
    //int32_t mSoundIndex; // no use
    int32_t mDamageNum;
    int32_t mDamageValue[4];
    int32_t mDamageEffect[4];
    int32_t mDamageType[4];
    /*  int32_t mEffectNum; // Are effects needed ? (server or client triggered ?)
        int32_t mEffectId[4]; // ?
        int32_t mEffectTarget[4]; // float ?
        int32_t mEffectValue[4]; // float ?
        int32_t mEffectamorId[4]; // ????
    */
public:
    PDefDamage();
    //~PDefDamage();

    bool LoadFromDef ( PTokenList *Tokens );

    inline int32_t GetDamageNum() const { return mDamageNum; }
    inline int32_t GetDamageValue ( int32_t nIdx ) const { return ( ( ( nIdx >= 0 ) && ( nIdx < mDamageNum ) ) ? mDamageValue[nIdx] : 0 ) ; }
    inline int32_t GetDamageEffect ( int32_t nIdx ) const { return ( ( ( nIdx >= 0 ) && ( nIdx < mDamageNum ) ) ? mDamageEffect[nIdx] : 0 ) ; }
    inline int32_t GetDamageType ( int32_t nIdx ) const { return ( ( ( nIdx >= 0 ) && ( nIdx < mDamageNum ) ) ? mDamageType[nIdx] : 0 ) ; }
    /*  inline int32_t GetEffectNum() const { return mEffectNum; }
        inline int32_t GetEffectId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectId[nIdx] : 0) ; }
        inline int32_t GetEffectTarget(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectTarget[nIdx] : 0) ; }
        inline int32_t GetEffectValue(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectValue[nIdx] : 0) ; }
        inline int32_t GetEffectamorId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectamorId[nIdx] : 0) ; }
        */
};
