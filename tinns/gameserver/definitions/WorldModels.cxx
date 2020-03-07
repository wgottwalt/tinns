#include "gameserver/definitions/Includes.hxx"

PDefWorldModel::PDefWorldModel()
{
}

bool PDefWorldModel::LoadFromDef(PTokenList *Tokens)
{
    int Idx=0;
    for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
    {
        switch(Idx)
        {
            case 0: // setentry
                    continue;

            case 1: // index
                mIndex = atoi(i->c_str()); break;

            case 2: // name
                mName = *i; break;

            case 3: // use flag
                mUseFlags = atoi(i->c_str()); break;

            case 4: // function type
                mFunctionType = atoi(i->c_str()); break;

            case 5: // function value
                mFunctionValue = atoi(i->c_str()); break;

            case 6: // hack difficulty
                mHackDifficulty = atoi(i->c_str()); break;

            case 7: // hack penalty
                mHackPenalty = atoi(i->c_str()); break;
        }
    }

    return true;
}
