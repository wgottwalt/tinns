#include "gameserver/definitions/Includes.hxx"

// characters.def contains all ingame characters (player types, npc's, monsters)

PDefCharacter::PDefCharacter()
{
    mIndex = -1;
    mName = "unknown";
    mModel = -1;
}

bool PDefCharacter::LoadFromDef(PTokenList *Tokens)
{
    int Idx=0;
    for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
    {
        switch(Idx)
        {
            case 0 :    // setentry
                continue;

            case 1 :
                mIndex = atoi(i->c_str()); break;

            case 2 :
                mName = *i; break;

            case 3 :
                mModel = atoi(i->c_str()); break;

            case 4 : // 0
                continue;

            case 5 :
                mHead = atoi(i->c_str()); break;

            case 6 :
                mTorso = atoi(i->c_str()); break;

            case 7 :
                mLegs = atoi(i->c_str()); break;

            case 8 :
                mColor = atoi(i->c_str()); break;

            case 9 :
                mBrightness = atoi(i->c_str()); break;
        }

        if(Idx==9)
            return true;
    }

    return false;
}
