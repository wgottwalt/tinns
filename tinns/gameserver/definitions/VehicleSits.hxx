#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefVhcSeat : public PDef {
private:
    //int32_t mIndex; //Field 1
    int32_t mType;
    std::string mName; //Field 3
    //BasePosX Y Z
    //BaseAngleX Y Z
    float mLeavePos[3]; //X Y Z - Fields 10-12
    float mLeaveAngle[3]; //X Y Z - Fields 13-15
    //FirePosX Y Z //Field 16
    //XLock Ylock MinX MaxX TurnSpeed ForceExternalCam ShowActor
    //SitBone RotXBone RotYBone WeaponBone
    int32_t mWeaponId; //Field 30
    int32_t mTL;
    float mDamageFactor;
    //SitAnimType SitYOffset //Field 33
    //SitFlags

public:
    PDefVhcSeat();
    //~PDefVhcSeat();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetType() const { return mType; }
    inline const std::string &GetName() const { return mName; }
    inline float GetLeavePos(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < 3)) ? mLeavePos[nIdx] : 0) ; }
    inline float GetLeaveAngle(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < 3)) ? mLeaveAngle[nIdx] : 0) ; }
    inline int32_t GetWeaponId() const { return mWeaponId; }
    inline int32_t GetTL() const { return mTL; }
    inline float GetDamageFactor() const { return mDamageFactor; }
};
/*
mType:
0 - Driver          (Ground vhc)
1 - Gunner
2 - Passenger
3 - Driver/Gunner
4 - Pilot           (Flying vhc)
5 - Pilot/Gunner        (Flying vhc)
6 - Pilot           (Senkrechtstarter ????)

mFlags:
1 - Winkel f�r Spieler und Waffe wird auf Fahrzeugrichtung gelockt. Zielrichtung darf nur leicht von der Fahrzeugrichtung abweichen
2 - Spieler wird gelockt
4 - Waffe ist nicht um X Achse drehbar
8 - Waffe ist nicht um Y Achse drehbar
16 - Spielermodel um 180 Grad gedreht
*/
