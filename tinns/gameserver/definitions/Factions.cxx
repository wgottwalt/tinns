#include <cstring>
#include "gameserver/definitions/Includes.hxx"

PDefFaction::PDefFaction()
{
    memset(mRelations, 0, sizeof(mRelations)); // ... array members supposed to by auto-initialized by C++
}

bool PDefFaction::LoadFromDef(PTokenList *Tokens)
{
    int Idx=0;
    for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
    {
        // setfracc 19  "Monster"   -1024   0   0   -1024   -1024   -1024   -1
        switch(Idx)
        {
            case 0 : // setfrac
                continue;

            case 1 :
                mIndex = atol(i->c_str()); break;

            case 2 :
                mName = *i;
                Misc::cleanUpString(mName);
                break;

            case 3 :
                mStartValue = atol(i->c_str()); break;

            case 4:
                mAffected = atol(i->c_str())!=0; break;

            case 5:
                mSL = atol(i->c_str()); break;

            default :
            {
                if(Idx-6 < NUMFACTIONS)
                    mRelations[Idx-6] = atol(i->c_str()); break;
            }
        }
    }

    return true;
}

int PDefFaction::GetRelation(int Faction) const
{
    // faction 0 has no relations
    if(Faction <= 0 || Faction > NUMFACTIONS)
        return 0;

    return mRelations[Faction-1];
}

