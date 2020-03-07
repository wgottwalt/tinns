#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefBlueprintPieces : public PDef {
private:
    //int32_t mIndex; // related Item Type
    int32_t mMaxPieceNum;
    int32_t mPieceNum;
    int32_t mPieceId[20];

public:
    PDefBlueprintPieces();
    //~PDefBlueprint32_tPieces();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetMaxPieceNum() const { return mMaxPieceNum; }
    inline int32_t GetPieceNum() const { return mPieceNum; }
    inline int32_t GetPieceId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mPieceNum)) ? mPieceId[nIdx] : 0) ; }
};
