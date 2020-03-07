#pragma once

#include <cstdint>
#include <string>

class PDefWorldModel;

enum { // Furniture Use flags (cumlative)
  ufTouchable = 1,
  ufUsable = 2,
  ufNoCollision = 4,
  ufChair = 8,
  ufToolTarget = 16,
  ufSelfCollisionBox = 64,
  ufGraphicalEffect = 128,
  ufNoSelectionBox = 256
};

class PFurnitureItemTemplate {
    friend class PWorldDatParser;

private:
    uint32_t mObjectID;

    // The commented out values are not loaded from dat file atm because they are not used yet.
    float mPosY; //= mPosY from dat file + 32000, to be coherent with char Pos scale
    float mPosZ;
    float mPosX;
    //float mRotY;
    float mRotZ;
    //float mRotX;
    //uint32_t mScale; //00 00 80 3F ?  = float(1.0000) scale factor ? // mostly used by holoscreens (passiv object)
    //uint32_t mUnknown2; //01 00 00 00 ?
    uint16_t mModelID; // points to models.ini
    //uint32_t mUnknown3; //00 00 00 00 ?
    //uint32_t mUnknown4; //00 00 00 00 ?
    uint16_t mWorldmodelID; // points to worldmodel.def
    //uint16_t mUnknown5; //12 00 ? // changes sometime (ex: c288 ...)

    //float mBoxLowerY; //Bounding box, for use when ufSelfCollisionBox is set in mUseFlags.
    //float mBoxLowerZ;
    //float mBoxLowerX;
    //float mBoxUpperY;
    //float mBoxUpperZ;
    //float mBoxUpperX;

    uint16_t mFrontPosY;
    uint16_t mFrontPosZ;
    uint16_t mFrontPosX;
    uint8_t mFrontLR;

    const PDefWorldModel* mDefWorldModel;

    uint32_t mLinkedObjectID; // for buttons, stores the corresponding triggered door
    // fo GR, stores order of the GR entity (spawn point) to later choose from respawn.def data

  public:
    PFurnitureItemTemplate();
    ~PFurnitureItemTemplate();

    uint32_t GetID() const;
    uint16_t GetUseFlags() const;
    uint16_t GetFunctionType() const;
    int GetFunctionValue() const;
    const std::string &GetName() const; /// !!!!
    const PDefWorldModel *GetDefWorldModel() const;
    uint8_t GetFrontLR() const;
    void GetFrontPos(uint16_t *nFrontPosX, uint16_t *nFrontPosY, uint16_t *nFrontPosZ) const;
    void GetPos(float *nPosX, float *nPosY, float *nPosZ) const;

    void SetLinkedObjectID(uint32_t nID);
    uint32_t GetLinkedObjectID() const;
};

// *** from worldmodel.def ***

//function Type
// 0 - none
//  1 - Itemcontainer
// 2 - Terminal
// 3 - Outfitter
// 4 - Trader
// 5 - Mineral
// 6 - Respawn Station
// 7 - GoGuardian
// 8 - Hackterminal
// 9 - Appartement Eingang
// 10 - Appartement Ein/Ausgang
// 11 - Appartement Klingel/�ffner
// 12 - Standard Button
// 13 - Hack Button
// 14 - HOLOMATCH ENTRANCE
// 15 - HOLOMATCH EXIT
// 16 - HOLOMATCH REFRESH
// 17 - HOLOMATCH HEAL
// 18 - WORLDCHANGEACTOR
// 19 - CLANTERMINAL
// 20 - DATFILE WORLDCHANGE ACTOR
// 21 - LOCATION FOR 20
// 22 -
// 23 - EINTRITTSGELD BUTTON
// 24- TUTORIALEXIT
// 25 - EXPLOSIVE
// 26 - Outpost Switch
// 27 - Old goguardian
// 28 - Fahrzeug Depot Interface
// 29 - Underground Exit
// 30 - Static FX (Value=Type. 1=Fire 2=Smoke 3=Steam 4=Sparkle)
// 31 - Venture Warp Station
// 32 - functionvalue+100 gibt eine Meldung aus der Text.ini [MISC] an.
//
