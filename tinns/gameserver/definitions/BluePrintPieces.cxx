#include "gameserver/definitions/Includes.hxx"

PDefBlueprintPieces::PDefBlueprintPieces()
{
}


bool PDefBlueprintPieces::LoadFromDef( PTokenList *Tokens )
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
        mMaxPieceNum = atoi( i->c_str() ); break;
      case 3 :
      {
        mPieceNum = atoi( i->c_str() );
        if ( mPieceNum > 20 )
          mPieceNum = 20;
        maxFields = 3 + mPieceNum;
        break;
      }
      default :
        if (( Idx >= 4 ) && ( Idx <= maxFields ) )
        {
          mPieceId[ Idx - 4 ] = atoi( i->c_str() );
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return (( Idx == maxFields ) );
}
