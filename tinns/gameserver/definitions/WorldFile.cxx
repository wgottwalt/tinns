#include "gameserver/definitions/Includes.hxx"
#include "gameserver/Includes.hxx"

/*
    NOTA: Some entries in worlds.ini share the same Id ...
          e.g: 505: techtown_enter 1/1a/1b/2
          As we don't know what it means & how it is supposed to be handled,
          we keep the first one for the moment. (this is managed in gamedef.cpp)
    TODO: Add a config entry to select the Nth entry for each such case,
          as well as a config entry to select the default entry to be kept (first/last)
*/

PDefWorldFile::PDefWorldFile()
{
    mFileInWorldsDir = false;
}

bool PDefWorldFile::LoadFromDef(PTokenList *Tokens)
{
    int Idx=0;
    int StartPos = 0;
    int TailLen = 0;
    int Len;

    for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
    {
        switch(Idx)
        {
            case 0: // setentry
                continue;

            case 1: // index
                mIndex = atoi(i->c_str()); break;

            case 2: // name
            {
              Len = (*i).length();

        while((StartPos = (*i).find("\\",StartPos))>-1)
          (*i)[StartPos]='/';
        StartPos = 0;

        if ((*i)[0] == '\"')
          StartPos = 1;

              if ( (*i).substr(StartPos,9) == "./worlds/" )
              {
                mFileInWorldsDir = true;
                StartPos += 9;
              }
              else if ( (*i).substr(StartPos,2) == "./" )
          StartPos += 2;

        if ((*i)[Len-1] == '\"')
          TailLen = 1;
              if ( (Len > (StartPos+TailLen+4)) && ((*i)[Len-TailLen-4] == '.') )
                TailLen += 4;

              mName = (*i).substr(StartPos,Len-StartPos-TailLen); // remove prefix, extension and doublequotes

                break;
            }
        }
    }

    return (Idx == 3);
}

// class PDefWorldFileMap defined in gamedefs.h
bool PDefWorldFilesMap::Load(const char* nName, const char* nFilename)
{
  mName = nName;
  if(mName.empty())
  {
    Console->Print( "%s Defs name not defined", Console->ColorText( RED, BLACK, "[ERROR]" ) );
    return (false);
  }

  if(! *nFilename)
  {
    Console->Print( "%s Filename not defined for %s defs", Console->ColorText( RED, BLACK, "[ERROR]" ), mName.c_str() );
    return (false);
  }

  PDefParser parser;
  int nDefs = 0, nErrors = 0, nDup = 0;

  if ( parser.Parse( nFilename ) )
  {
    const PDefTokenList &t = parser.GetTokens();

    for ( PDefTokenList::const_iterator i = t.begin(); i != t.end(); i++ )
    {
      PDefWorldFile *it = new PDefWorldFile();
      bool loadfail = !it->LoadFromDef( *i ), insertfail = false;

      if ( !loadfail )
        insertfail = !mDefs.insert( std::make_pair( it->GetIndex(), it ) ).second;
      if ( loadfail || insertfail )
      {
        if ( insertfail )
        {
          ++nDup;
          if ( gDevDebug ) Console->Print( "%s ini error (new duplicate id %i discarded)", mName.c_str(), it->GetIndex(), it->GetName().c_str() );
        }
        else
        {
          Console->Print( "%s ini load error @ %i", mName.c_str(), nDefs + nErrors );
          ++nErrors;
        }
        delete it;
      }
      else
        ++nDefs;
    }
  }
  else
  {
    Console->Print( "%s Error loading %s ini defs", Console->ColorText( RED, BLACK, "[ERROR]" ), mName.c_str() );
    return ( false );
  }

  if ( nErrors > 0 )
    Console->Print( "%s Loaded %i %s ini defs, %i error(s).", Console->ColorText( RED, BLACK, "[ERROR]" ), nDefs, mName.c_str(), nErrors );
  else
    Console->Print( "%s Loaded %i %s ini defs, %i error(s).", Console->ColorText( GREEN, BLACK, "[Success]" ), nDefs, mName.c_str(), nErrors );

  if ( nDup )
    Console->Print( "%s %d duplicate entries ignored in %s ini defs.", Console->ColorText( YELLOW, BLACK, "[Notice]" ), nDup, mName.c_str() );

  return ( true );
}
