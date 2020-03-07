#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefMission : public PDef {
private:
    //int32_t mIndex;
    int32_t mSourceId;
    std::string mStartDialog;
    int32_t mDescTextId;
    int32_t mNpcType[4];
    std::string mNpcDialog[4];
    int32_t mNpcDialogStartState[4];
    int32_t mTargetType[4];
    int32_t mTargetValue[4][3];
    int32_t mEndMoney;
    int32_t mEndXp;
    int32_t mMaxTime;
    int32_t mDifficulty;
    int32_t mMinFactionValue;
    int32_t mPoints;
    int32_t mFlags;

public:
    PDefMission();
    //~PDefMission();

    bool LoadFromDef( PTokenList *Tokens );

    int32_t GetSourceId() const;
    const std::string &GetStartDialog() const;
    int32_t GetDescTextId() const;
    int32_t GetNpcType(int32_t nIdx) const;
    const std::string &GetNpcDialog(int32_t nIdx) const;
    int32_t GetNpcDialogStartState(int32_t nIdx) const;
    int32_t GetTargetType(int32_t nIdx) const;
    int32_t GetTargetValue(int32_t nIdx, int32_t nValIdx) const;
    int32_t GetEndMoney() const ;
    int32_t GetEndXp() const;
    int32_t GetMaxTime() const;
    int32_t GetDifficulty() const;
    int32_t GetMinFactionValue() const;
    int32_t GetPoints() const;
    int32_t GetFlags() const;
};

/*
//mission source ID > 0 NPC Type for mission source < 0 Terminal ID for mission source

//mission NPC Type  > 0 NPC Type for mission NPC    < 0 Fraction ID for mission NPC
//mission target Type   1 - kill NPC    2 - kill NPC type   "3 - dialogtrigger" 4 - kill NPC type range 5-dialogtrigger + counter   "6 - conquer outpost"   7 - conquer outpost + counter   8 - conquer + hold outpost      9-KillPlayer    10 - LootNPC    11 - LootNPCType

//target value 1    ID of related mission NPC   > 0 NPC Type of NPCs to kill    < 0 FractionID of NPCs to kill  "ID of related mission NPC" > 0 NPC Type of NPCs to kill    ID of related mission NPC   "> 0 outpost Type < 0 current outpost factionid"    > 0 outpost type < 0 original outpost faction type  > 0 outpost Type < 0 current outpost factionid      PlayerFaction   ID of related Mission NPC(0-4)  > 0 NPC Type of NPCs to kill    < 0 FractionID of NPCs to kill

//target value 2        Count of NPCs to kill   "ID of DialogTrigger"   Count of NPCs to kill   ID of DialogTrigger     Count of outposts to conquer    time to hold outpost        Kill Cnt    ItemID  NPC Cnt

//target value 3                Range of NPC Type   Trigger Counter - must reach 0 to be successfull                    Max Leveldiff       Item ID
//missionflags  (bit field)
// NPC0 No Search   1
// NPC1 No Search   2
// NPC2 No Search   4
// NPC3 No Search   8
*/
