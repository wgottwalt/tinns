#include "gameserver/definitions/Includes.hxx"

PDefWeather::PDefWeather()
{
  mNumWeathers = mSectionId = 0;
  for(int i=0; i<8; ++i)
      mDuration[i] = mWeatherId[i] = 0;
}

bool PDefWeather::LoadFromDef( PTokenList *Tokens )
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
        mSectionId = atoi( i->c_str() ); break;
      case 3 :
        mNumWeathers = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 4) && (Idx <= (3 + 2*mNumWeathers))  && (Idx <= 19) )
        {
          if(Idx & 1)
          {
            mDuration[int((Idx - 4)/2)] = atoi( i->c_str() );
          }
          else
          {
            mWeatherId[int((Idx - 4)/2)] = atoi( i->c_str() );
          }
        }
        break;
    }

    if ( Idx >= 19 )
      break;
  }

  return ((Idx >= (3 + 2*mNumWeathers)));
}
