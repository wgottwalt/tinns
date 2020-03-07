#include "gameserver/definitions/Includes.hxx"

PDefNpcArmor::PDefNpcArmor()
{
}

bool PDefNpcArmor::LoadFromDef ( PTokenList *Tokens )
{
  int Idx=0;
  for ( PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;;
      case 1 :
        mIndex = atoi ( i->c_str() ); break;
      default :
        if ( ( Idx - 2 ) < 7 )
        {
          mValue[Idx - 2] = atoi ( i->c_str() );
        }
        break;
    }
  }

  return ( Idx >= 8 );
}
