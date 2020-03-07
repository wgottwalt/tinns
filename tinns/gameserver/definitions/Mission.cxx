#include "gameserver/definitions/Includes.hxx"

const std::string EmptyString;

PDefMission::PDefMission()
{
}

bool PDefMission::LoadFromDef ( PTokenList *Tokens )
{
  int Idx = 0;
  int arrayIdx, arraySubIdx;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi ( i->c_str() ); break;
      case 2 :
        mSourceId = atoi ( i->c_str() ); break;
      case 3:
        mStartDialog = *i; break;
      case 4 :
        mDescTextId = atoi ( i->c_str() ); break;
      case 33 :
        mEndMoney = atoi ( i->c_str() ); break;
      case 34 :
        mEndXp = atoi ( i->c_str() ); break;
      case 35 :
        mMaxTime = atoi ( i->c_str() ); break;
      case 36 :
        mDifficulty = atoi ( i->c_str() ); break;
      case 37 :
        mMinFactionValue = atoi ( i->c_str() ); break;
      case 38 :
        mPoints = atoi ( i->c_str() ); break;
      case 39 :
        mFlags = atoi ( i->c_str() ); break;
      default :
        if ( ( Idx >= 5 ) && ( Idx <= 16 ) )
        {
          arrayIdx = ( Idx - 5 ) / 3;
          switch ( ( Idx - 5 ) % 3 )
          {
            case 0:
              mNpcType[arrayIdx] = atoi ( i->c_str() ); break;
            case 1:
              mNpcDialog[arrayIdx] = *i; break;
            case 2:
              mNpcDialogStartState[arrayIdx] = atoi ( i->c_str() ); break;
          }
        }
        else if ( ( Idx >= 17 ) && ( Idx <= 32 ) )
        {
          arrayIdx = ( Idx - 17 ) / 4;
          arraySubIdx = ( Idx - 17 ) % 4;
          if ( arraySubIdx == 0 )
            mTargetType[arrayIdx] = atoi ( i->c_str() );
          else
            mTargetValue[arrayIdx][ arraySubIdx - 1 ] = atoi ( i->c_str() );

          break;
        }
        break;
    }

    if ( Idx >= 39 )
      break;
  }

  return ( ( Idx >= 38 ) );
}

int32_t PDefMission::GetSourceId() const
{
    return mSourceId;
}

const std::string &PDefMission::GetStartDialog() const
{
    return mStartDialog;
}

int32_t PDefMission::GetDescTextId() const
{
    return mDescTextId;
}

int32_t PDefMission::GetNpcType(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < 4)) ? mNpcType[nIdx] : 0);
}

const std::string &PDefMission::GetNpcDialog(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < 4)) ? mNpcDialog[nIdx] : EmptyString);
}

int32_t PDefMission::GetNpcDialogStartState(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < 4)) ? mNpcDialogStartState[nIdx] : 0);
}

int32_t PDefMission::GetTargetType(int32_t nIdx) const
{
    return ( ((nIdx >= 0) && (nIdx < 4)) ? mTargetType[nIdx] : 0);
}

int32_t PDefMission::GetTargetValue(int32_t nIdx, int32_t nValIdx) const
{
    return (((nIdx >= 0) && (nIdx < 4) && (nValIdx >= 0) && (nValIdx < 3)) ? mTargetValue[nIdx][nValIdx] : 0);
}

int32_t PDefMission::GetEndMoney() const
{
    return mEndMoney;
}

int32_t PDefMission::GetEndXp() const
{
    return mEndXp;
}

int32_t PDefMission::GetMaxTime() const
{
    return mMaxTime;
}

int32_t PDefMission::GetDifficulty() const
{
    return mDifficulty;
}

int32_t PDefMission::GetMinFactionValue() const
{
    return mMinFactionValue;
}

int32_t PDefMission::GetPoints() const
{
    return mPoints;
}

int32_t PDefMission::GetFlags() const
{
    return mFlags;
}
