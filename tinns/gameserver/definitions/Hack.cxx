#include "gameserver/definitions/Includes.hxx"

PDefHack::PDefHack()
{
}

bool PDefHack::LoadFromDef(PTokenList *Tokens)
{
    int Idx=0;
    for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
    {
        switch(Idx)
        {
            case 0: // setentry
                continue ;


            case 1: // index
                mIndex = atoi(i->c_str()); break;


        }
    }

    return true;
}

