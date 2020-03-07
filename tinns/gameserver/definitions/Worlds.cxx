#include "gameserver/definitions/Includes.hxx"

PDefWorld::PDefWorld()
{
}

bool PDefWorld::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int StartPos = 0;
  int TailLen = 0;
  int Len;

  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0: // setentry
        continue;
      case 3: // music
        continue;

      case 1: // index
        mIndex = atoi( i->c_str() ); break;

      case 2: // name
      {
        Len = ( *i ).length();

        if (( *i )[0] == '\"' )
          StartPos = 1;

        while (( *i )[StartPos] == ' ' )
          ++StartPos;

        if (( *i )[Len-1] == '\"' )
          TailLen = 1;

        while (( Len - TailLen > 0 ) && (( *i )[Len-TailLen-1] == ' ' ) )
          ++TailLen;

        if ( StartPos >= Len - TailLen )
          mName = "";
        else
          mName = ( *i ).substr( StartPos, Len - StartPos - TailLen );

        break;
      }

      case 4: // datfile
      {
        Len = ( *i ).length();

        while (( StartPos = ( *i ).find( "\\", StartPos ) ) > -1 )
          ( *i )[StartPos] = '/';
        StartPos = 0;

        if (( *i )[0] == '\"' )
          StartPos = 1;

        while (( *i )[StartPos] == ' ' )
          ++StartPos;

        if (( StartPos <= Len - 2 ) && (( *i ).substr( StartPos, 2 ) == "./" ) )
          StartPos += 2;

        if (( *i )[Len-1] == '\"' )
          TailLen = 1;

        while (( Len - TailLen > 0 ) && (( *i )[Len-TailLen-1] == ' ' ) )
          ++TailLen;

        if ( StartPos >= Len - TailLen )
          mDatFile = "";
        else
          mDatFile = ( *i ).substr( StartPos, Len - StartPos - TailLen );

        break;
      }

      case 5:
        mFlags = atoi( i->c_str() ); break;
    }
  }

  return true;
}

