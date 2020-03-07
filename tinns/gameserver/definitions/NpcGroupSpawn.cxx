#include "gameserver/definitions/Includes.hxx"

const std::string EmptyString;

PDefNpcGroupSpawn::PDefNpcGroupSpawn()
{
  mNumNpc = 0;
  for(int i=0; i<8; ++i)
    mSpawnChance[i] = mFunctionValue[i] = mNpcType[i] = 0;

}


bool PDefNpcGroupSpawn::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        mIgnoreNearPC = atoi( i->c_str() ); break;
      case 3 :
        mNumNpc = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 4) && (Idx <= (3 + 5*mNumNpc)) && (Idx <= (43)) )
        {
          switch ( ((Idx - 4) % 5) )
          {
            case 0:
              mNpcType[int((Idx - 4)/5)] = atoi( i->c_str() ); break;
            case 1:
              mScript[int((Idx - 4)/5)] = *i; break;
            case 2:
              mScriptParameter[int((Idx - 4)/5)] = *i ; break;
            case 3:
              mFunctionValue[int((Idx - 4)/5)] = atoi( i->c_str() ); break;
            case 4:
              mSpawnChance[int((Idx - 4)/5)] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= 43 )
      break;
  }

  return ((Idx >= (3 + 5*mNumNpc)));
}

int32_t PDefNpcGroupSpawn::GetIgnoreNearPC() const
{
    return mIgnoreNearPC;
}

int32_t PDefNpcGroupSpawn::GetNumNpc() const
{
    return mNumNpc;
}

int32_t PDefNpcGroupSpawn::GetNpcType(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mNpcType[nIdx] : 0);
}

const std::string &PDefNpcGroupSpawn::GetScript(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mScript[nIdx] : EmptyString);
}

const std::string &PDefNpcGroupSpawn::GetScriptParameter(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mScriptParameter[nIdx] : EmptyString);
}

int32_t PDefNpcGroupSpawn::GetFunctionValue(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mFunctionValue[nIdx] : 0);
}

int32_t PDefNpcGroupSpawn::GetSpawnChance(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mSpawnChance[nIdx] : 0);
}

/*
class PDefNpcGroupSpawn : public PDef
{
  private :
    //int mIndex;
    int mIgnoreNearPC;
    int mNumNpc; // Size of the list. But that should be the size of the group...
    int mNpcType[8];
    std::string mScript[8];
    std::string mScriptParameter[8];
    int mFunctionValue[8];
    int mSpawnChance[8];

  public :
    PDefNpcGroupSpawn();
    //~PDefNpcGroupSpawn();
*/

