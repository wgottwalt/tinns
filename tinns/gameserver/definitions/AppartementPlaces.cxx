#include "gameserver/definitions/Includes.hxx"

PDefAppPlace::PDefAppPlace()
{
}

bool PDefAppPlace::LoadFromDef(PTokenList *Tokens)
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

            case 3: // Exit World
                mExitWorldID = atoi(i->c_str()); break;

            case 4: // Exit World entity
                mExitWorldEntity = atoi(i->c_str()); break;

            case 5: // Sewer level
                mSewerLevel = atoi(i->c_str()); break;
        }
    }

    return true;
}
