#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefCharacter : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    int32_t mModel;
    int32_t mHead;
    int32_t mTorso;
    int32_t mLegs;
    int32_t mColor;
    int32_t mBrightness;

public:
    PDefCharacter();
    //~PDefCharacter();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetModel() const { return mModel; }
    inline int32_t GetHead() const { return mHead; }
    inline int32_t GetTorso() const { return mTorso; }
    inline int32_t GetLegs() const { return mLegs; }
    inline int32_t GetColor() const { return mColor; }
    inline int32_t GetBrightness() const { return mBrightness; }
};
