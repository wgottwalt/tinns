#include <cstring>
#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"

//  TODO: - mem corruption occurs if mDoorParameters[] is given a size of 6, and that 6 max param can be accepted
//          This bug occurs in world 105. Reason not found yet :-x

const std::string EmptyString;

PDoorTemplate::PDoorTemplate()
{
  mDoorID = 0;
  mWorldmodelID = 0;
  mDefWorldModel = nullptr;
  mIsDoubleDoor = false;
  mIsTriggeredDoor = false;
}

PDoorTemplate::~PDoorTemplate()
{
}

uint16_t PDoorTemplate::GetID()
{
    return mDoorID;
}

uint16_t PDoorTemplate::GetUseFlags()
{
    return (mDefWorldModel ? mDefWorldModel->GetUseFlags() : 0);
}

uint16_t PDoorTemplate::GetFunctionType()
{
    return (mDefWorldModel ? mDefWorldModel->GetFunctionType() : 0);
}

int PDoorTemplate::GetFunctionValue()
{
    return (mDefWorldModel ?  mDefWorldModel->GetFunctionValue() : 0);
}

const std::string& PDoorTemplate::GetName() const
{
    return (mDefWorldModel ?  mDefWorldModel->GetName() : EmptyString );
}

const PDefWorldModel *PDoorTemplate::GetDefWorldModel() const
{
    return mDefWorldModel;
}

void PDoorTemplate::GetPos(float *nPosX, float *nPosY, float *nPosZ) const
{
    *nPosY = mPosY;
    *nPosZ = mPosZ;
    *nPosX = mPosX;
}

uint16_t PDoorTemplate::GetOtherDoorID()
{
    return (mIsDoubleDoor ? mDoorParameters[1] : 0);
}

bool PDoorTemplate::IsDoubleDoor() const
{
    return mIsDoubleDoor;
}

bool PDoorTemplate::IsTriggeredDoor() const
{
    return mIsTriggeredDoor;
}

void PDoorTemplate::SetDoorParameters(char* nDoorParametersString)
{
  char* SepPos;
  int ParamNum = 0;
//Console->Print("door parameter: %s", nDoorParametersString);
  while ( *nDoorParametersString && (SepPos = strchr(nDoorParametersString, ',')) && (ParamNum < 4))
  {
    *SepPos = 0;
    mDoorParameters[ParamNum++] = atoi(nDoorParametersString);
    nDoorParametersString = SepPos + 1;
  }
  if (*nDoorParametersString)
  {
    if (ParamNum < 4)
      mDoorParameters[ParamNum] = atoi(nDoorParametersString);
    //else
    //  Console->Print(RED, BLACK, "[ERROR] More than 4 parameters in Sec2ElemType5 ParamString");
  }
}

void PDoorTemplate::SetDoorTypeName(char* nDoorTypeName)
{
  mDoorTypeName = nDoorTypeName;

  if (mDoorTypeName == "DDOOR")
  {
    mIsDoubleDoor = true;
  }
  else if (mDoorTypeName == "TRIGDD")
  {
    mIsDoubleDoor = true;
    mIsTriggeredDoor = true;
  }
  else if (mDoorTypeName == "TRIGDOOR")
  {
    mIsTriggeredDoor = true;
  }
  else if (mDoorTypeName == "NBUTTON")
  {
    mIsTriggeredDoor = false;
  }
}
