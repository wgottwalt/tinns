#pragma once

#include <cstdint>

struct PWorldFileHeader {
    uint32_t mHeaderSize; // must be 08 00 00 00, = header size after this field
    uint32_t mHeaderSig; // must be CF CF 0F 00
    uint32_t mSection; // must be 01 00 00 00
};

struct PSectionHeader {
    uint32_t mHeaderSize; // must be 0C 00 00 00
    uint32_t mHeaderSig; // must be CF FF 00 00
    uint32_t mSection; // 00 00 00 00 means end
    uint32_t mDataSize;
};

struct PSec2ElemHeader {
    uint32_t mHeaderSize; // must be 0C 00 00 00
    uint32_t mHeaderSig; // must be F1 FE FE 0F
    uint32_t mElementType; // 1000003, 1000005 or 1000014
    uint32_t mDataSize;
};

struct PSec2ElemType3a { //static object ?
    float mPosY; //= uint32_t16_t PosY - 32000
    float mPosZ;
    float mPosX;
    float mRotY;
    float mRotZ;
    float mRotX;
    float mScale; //00 00 80 3F ? = float(1.000000) !!! => scale factor ?????
    uint32_t mUnknown2; //01 00 00 00 ?
    uint16_t mModelID; // point32_ts to models.ini
    uint32_t mUnknown3; //00 00 00 00 ?
    uint32_t mUnknown4; //00 00 00 00 ?
    uint16_t mWorldmodelID; // point32_ts to worldmodel.def
    uint16_t mUnknown5; //12 00 ?
    uint32_t mObjectID;
};

struct PSec2ElemType3b { //this part is optional
    float mBoxLowerY; //Bounding box, for useflag "64 - selfconstructing colisionbox"
    float mBoxLowerZ; // int32_t32_t or uint32_t32_t ?
    float mBoxLowerX;
    float mBoxUpperY;
    float mBoxUpperZ;
    float mBoxUpperX;
};

struct PSec2ElemType5Start { //door
    uint16_t mUnknown1; //18 00
    uint16_t mUnknown1bis; //00 00 ? varies
    float mPosY;
    float mPosZ;
    float mPosX;
    uint8_t mActorStringSize; //string size with ending 0
    uint8_t mParamStringSize; //string size with ending 0
    uint16_t mUnknown5; //00 00 ? second byte varies
    uint16_t mDoorID; // but what is the link with ObjectID sent in Use message (can't find the base offset .. or 0x80 for doors ???)
    uint16_t mWorldmodelID; //door type from worldmodel.def
};
//Actor As String //null terminated string
//Params As String //null terminated string - for DDOOR, 2nd param is the ID of the other (half)door (*)
//param1 = 2 => simple lateral move ?, 3 => door frontal+lateral move (as at Typherra memorial) ?
//last param = 0/1 for lateral move direction ? no ...
//(*) here is the bug(?) that makes open only one half of a double door

/*
struct PSec2ElemType6Start //npc
{
  uint16_t mUnknown1; //20 00 ?
  uint16_t mUnknown2; //12 00 ?
  float mPosY;
  float mPosZ;
  float mPosX;
  uint32_t mNPCTypeID; //npc type in npc.def
  uint8_t mActorStringSize; //string size with ending 0
  uint8_t mParamStringSize; //string size with ending 0
  uint16_t mNpcID; // kind of ?
  uint32_t mUnknown3; //01 00 00 00 ?
  uint16_t mUnknown4; //00 00 ?
  uint16_t mUnknown5; //04 00 ?
};
  //Actor As String //null terminated string
  //Params As String //null terminated string - Seem to be the facing angle in degres
struct PSec2ElemType6End
{
  float mPosY2; //second position for movement ?
  float mPosZ2; //
  float mPosX2; //
};
*/
struct PSec2NPC_EntryPart1 {
    uint32_t       mUnknown1; // Is always 0x20001200, in every log. maybe header for NPCs?
    float       mPosY;
    float       mPosZ;
    float       mPosX;
    uint32_t       mNPCTypeID; //npc type in npc.def
    uint8_t        mActorStringSize; //string size with ending 0
    uint8_t        mAngleStringSize; //string size with ending 0
    uint16_t       mNpcID;
    uint8_t        mHasAdditionalCoords;
    uint8_t        mUnknown2a;
    uint8_t        mUnknown2b;
    uint8_t        mUnknown2c;
    uint16_t       mTradeID; //mUnknown3; //00 00 ?
    uint16_t       mUnknown4; //04 00 ?
};

//    uint32_t8_t    mActorName[mActorStringSize];
//    uint32_t8_t    mAngle[mAngleStringSize];

struct PSec2NPC_EntryPart2 { // Waypoint32_ts! or something like that...
    float       mPosY;
    float       mPosZ;
    float       mPosX;
};

//  uint32_t16_t mStrSize; //non-zero terminated string size
//  Name As String //non-zero terminated string
struct PSec2ElemType15End { //area definition/sound ?
    float mUnknown1;
    float mUnknown2;
    float mUnknown3;
    float mUnknown4;
    float mUnknown5;
};
