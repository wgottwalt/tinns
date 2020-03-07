#pragma once

#include <cstdint>

class PDefWorldModel;

class PDoorTemplate {
    friend class PWorldDatParser;

private:
    uint16_t mDoorID;

    // The commented out values are not loaded from dat file atm because they are not used yet.
    //uint16_t mUnknown1; //18 00
    //uint16_t mUnknown1bis; //00 00 ? varies
    float mPosY;
    float mPosZ;
    float mPosX;
    //uint16_t mUnknown5; //00 00 ? second byte varies
    uint16_t mWorldmodelID; //door type from worldmodel.def

    std::string mDoorTypeName;
    int mDoorParameters[4];
    bool mIsDoubleDoor;
    bool mIsTriggeredDoor;

    const PDefWorldModel* mDefWorldModel;

public:
    PDoorTemplate();
    ~PDoorTemplate();

    uint16_t GetID();
    uint16_t GetUseFlags();
    uint16_t GetFunctionType();
    int GetFunctionValue();
    const std::string& GetName() const; /// !!!!
    const PDefWorldModel *GetDefWorldModel() const;

    void GetPos(float *nPosX, float *nPosY, float *nPosZ) const;
    uint16_t GetOtherDoorID();
    bool IsDoubleDoor() const;
    bool IsTriggeredDoor() const;

    void SetDoorTypeName(char* nDoorTypeName);
    void SetDoorParameters(char* nDoorParametersString);
};
