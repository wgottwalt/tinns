#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

//weathersectionid weathercnt weatherid length end
class PDefWeather : public PDef {
private:
    //int32_t mIndex;
    int32_t mSectionId;
    int32_t mNumWeathers;
    int32_t mWeatherId[8];
    int32_t mDuration[8];

public:
    PDefWeather();
    //~PDefWeather();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetSectionId() const { return mSectionId; }
    inline int32_t GetNumWeathers() const { return mNumWeathers; }
    inline int32_t GetWeatherId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumWeathers)) ? mWeatherId[nIdx] : 0 ) ; }
    inline int32_t GetDuration(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumWeathers)) ? mDuration[nIdx] : 0 ) ; }
};

/* mWeatherId
//BRIGHTSKY     (1)
//SINGLECLOUDS      (2)
//CLOUDY        (3)

//LIGHTRAIN     (4)
//HEAVYRAIN     (5)
//THUNDERSTORM      (6)

//SNOW          (7)
//SANDSTORM     (8)
//FALLOUT       (9)

//SEA_BRIGHTSKY     (100)
*/
