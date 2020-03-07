#include "gameserver/definitions/Includes.hxx"

PDefDamage::PDefDamage()
{
}

bool PDefDamage::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 3;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        break;
      case 3 :
      {
        mDamageNum = atoi( i->c_str() );
        if ( mDamageNum > 4 )
          mDamageNum = 4;
        maxFields = 3 + 3 * mDamageNum;
        break;
      }
      default :
        if (( Idx >= 4 ) && ( Idx <= maxFields ) )
        {
          switch (( Idx - 4 ) % 3 )
          {
            case 0:
              mDamageValue[( Idx - 4 ) / 3] = atoi( i->c_str() ); break;
            case 1:
              mDamageEffect[( Idx - 4 ) / 3] = atoi( i->c_str() ); break;
            case 2:
              mDamageType[( Idx - 4 ) / 3] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  // Try to fix wrong size field
  if( Idx < maxFields )
  {
    mDamageNum = ( Idx - 4 ) / 3;
    if( (( Idx - 4 ) % 3) == 2 )
      ++mDamageNum;
  }
  return ( Idx >= 3 );
}
/*
class PDefDamage : public PDef
{
  private :
    //int mIndex;
    //int mSoundIndex; // no use
    int mDamageNum;
    int mDamageValue[4];
    int mDamageEffect[4];
    int mDamageType[4];
    // int mEffectNum; // Are effects needed ? (server or client triggered ?)
    // int mEffectId[4]; // ?
    // int mEffectTarget[4]; // float ?
    // int mEffectValue[4]; // float ?
    // int mEffectamorId[4]; // ????

  public :
    PDefDamage();
    //~PDefDamage();
*/
