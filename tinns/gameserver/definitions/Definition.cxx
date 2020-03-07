#include "gameserver/definitions/Includes.hxx"

const std::string EmptyString;

PDef::PDef()
: mIndex(0)
{
}

PDef::~PDef()
{
}

bool PDef::LoadFromDef(PTokenList * /*Tokens*/)
{
    //Tokens = Tokens;

    return false;
}

int32_t PDef::GetIndex() const
{
    return mIndex;
}

const std::string &PDef::GetName() const
{
    return EmptyString;
}
