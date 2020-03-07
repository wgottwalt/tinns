#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefNpcArmor : public PDef {
private:
    //int32_t mIndex;
    int32_t mValue[7]; // force piercing fire energy xray psi poison

public:
    PDefNpcArmor();
    //~PDefNpcArmor();

    bool LoadFromDef ( PTokenList *Tokens );

    inline int32_t GetValue ( int32_t nIdx ) const { return ( ( ( nIdx >= 0 ) && ( nIdx < 7 ) ) ? mValue[nIdx] : 0 ) ; }
};
