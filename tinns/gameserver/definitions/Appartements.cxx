#include "gameserver/definitions/Includes.hxx"

PDefAppartement::PDefAppartement()
{
}

bool PDefAppartement::LoadFromDef(PTokenList *Tokens)
{
  mFaction = 0;
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

            case 3: // world name
                mWorldName = *i; break;

            case 4: // value
                mValue = atoi(i->c_str()); break;

            case 5: // number of places
                mPlaceCount = atoi(i->c_str()); break;

          case 6:
          case 7:
          case 8:
          case 9:
          case 10:
          case 11:
          case 12:
          case 13:
            mPlace[Idx-6] = atoi(i->c_str()); break;

            case 14: // faction if base appartement
                mFaction = atoi(i->c_str()); break;
        }
    }
//Console->Print("%04d:%s file:%s val:%d places:%d pl1:%d pl2:%d pl8:%d faction:%d",
//      mIndex, mName.c_str(), mWorldName.c_str(), mValue, mPlaceCount, mPlace[0], mPlace[1], mPlace[7], mFaction);
    return true;
}
