#include "gameserver/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"

const std::string EmptyString;

PFurnitureItemTemplate::PFurnitureItemTemplate()
{
  mObjectID = 0;
  mModelID = 0;
  mWorldmodelID = 0;
  mDefWorldModel = nullptr;
  mLinkedObjectID = 0;
}

PFurnitureItemTemplate::~PFurnitureItemTemplate()
{
}

uint32_t PFurnitureItemTemplate::GetID() const
{
    return mObjectID;
}

uint16_t PFurnitureItemTemplate::GetUseFlags() const
{
    return (mDefWorldModel ? mDefWorldModel->GetUseFlags() : 0);
}

uint16_t PFurnitureItemTemplate::GetFunctionType() const
{
    return (mDefWorldModel ? mDefWorldModel->GetFunctionType() : 0);
}

int PFurnitureItemTemplate::GetFunctionValue() const
{
    return (mDefWorldModel ? mDefWorldModel->GetFunctionValue() : 0);
}

const std::string &PFurnitureItemTemplate::GetName() const
{
    return (mDefWorldModel ? mDefWorldModel->GetName() : EmptyString);
}

const PDefWorldModel *PFurnitureItemTemplate::GetDefWorldModel() const
{
    return mDefWorldModel;
}

uint8_t PFurnitureItemTemplate::GetFrontLR() const
{
    return mFrontLR;
}

void PFurnitureItemTemplate::GetFrontPos(uint16_t *nFrontPosX, uint16_t *nFrontPosY, uint16_t *nFrontPosZ) const
{
    *nFrontPosY = mFrontPosY;
    *nFrontPosZ = mFrontPosZ;
    *nFrontPosX = mFrontPosX;
}

void PFurnitureItemTemplate::GetPos(float *nPosX, float *nPosY, float *nPosZ) const
{
    *nPosY = mPosY;
    *nPosZ = mPosZ;
    *nPosX = mPosX;
}

void PFurnitureItemTemplate::SetLinkedObjectID(uint32_t nID)
{
    mLinkedObjectID = nID;
}

uint32_t PFurnitureItemTemplate::GetLinkedObjectID() const
{
    return mLinkedObjectID;
}
