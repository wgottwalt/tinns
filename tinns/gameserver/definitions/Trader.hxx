#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

#define DEF_TRADER_MAXENTRIES 17

class PDefTrader : public PDef {
private:
    //int32_t mIndex;
    int32_t mType;
    float mMaxWealth;
    float mWealthRespawn;
    int32_t mQuality;
    int32_t mItemId[DEF_TRADER_MAXENTRIES]; // <0: item group
    float mItemPriceScale[DEF_TRADER_MAXENTRIES]; // sometime string like "1/5" ??? <=> 0.2 or Sell:1 / Buy 5 ????

public:
    PDefTrader();
    //~PDefTrader();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetType() const { return mType; }
    inline float GetMaxWealth() const { return mMaxWealth; }
    inline float GetWealthRespawn() const { return mWealthRespawn; }
    inline int32_t GetQuality() const { return mQuality; }
    inline int32_t GetItemId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < DEF_TRADER_MAXENTRIES)) ? mItemId[nIdx] : 0 ) ; }
    inline float GetItemPriceScale(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < DEF_TRADER_MAXENTRIES)) ? mItemPriceScale[nIdx] : 0 ) ; }
};
/* Trader/Buyer mType
//  1   weapons
//  2   ammo
//  3   armor
//  4   tools
//  5   psi equipment
//
//  10  chemicals
//  11  item parts
//  12  implants
//  13  bone enforcements
//
//  16  Buy anything
//
//  20  refreshments
//  21  drugs
//  22  medicaments
//  23  real estate
//  24  vehicles
//
*/
