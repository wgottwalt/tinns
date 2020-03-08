#include "gameserver/definitions/Includes.hxx"

PDefScripts::PDefScripts()
{
    mIdentifier = "";
    mLuaFile = "";
}

bool PDefScripts::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        continue;
      case 1 : // index
        mIndex = atoi(i->c_str()); break;
      case 2 :
        mIdentifier = *i;
        Misc::cleanUpString(mIdentifier);
        break;
      case 3 :
        mLuaFile = *i;
        Misc::cleanUpString(mLuaFile);
        break;
      case 4 :
        mScriptHeader = *i;
        Misc::cleanUpString(mScriptHeader);
        break;
        continue;
    }
  }

  return true;
}

/*
bool PDefScriptsMap::Load(const char* nName, const char* nFilename)
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
      PDefScripts *it = new PDefScripts();
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
*/
