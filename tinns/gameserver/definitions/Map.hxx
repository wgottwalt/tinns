#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "gameserver/definitions/Parser.hxx"
#include "common/Includes.hxx"

template <class T>
class PDefMap {
protected:
    std::string mName;
    std::map<int32_t, T *> mDefs;

    void Clear()
    {
        for (auto it = mDefs.begin(); it != mDefs.end(); ++it)
            delete it->second;

        mDefs.clear();
    }

public:
    PDefMap()
    {
    }

    ~PDefMap()
    {
        Clear();
    }

    bool Load(const char* nName, const char* nFilename)
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
      int32_t nDefs = 0, nErrors = 0;

      if ( parser.Parse( nFilename ) )
      {
        const PDefTokenList &t = parser.GetTokens();

        for ( PDefTokenList::const_iterator i = t.begin(); i != t.end(); i++ )
        {
          T* entry = new T();
          bool insertfail = false;
          bool loadfail = ! entry->LoadFromDef( *i );

          if ( !loadfail )
            insertfail = ! mDefs.insert( std::make_pair( entry->GetIndex(), entry ) ).second;

          if ( loadfail || insertfail )
          {
            if ( insertfail )
              Console->Print( "%s %s def error (duplicate id %i): %s", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ),  mName.c_str(), entry->GetIndex(), entry->GetName().c_str() );
            else
              Console->Print( "%s def load error @ %i", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), mName.c_str(), nDefs + nErrors );
            ++nErrors;
            delete entry;
          }
          else
            ++nDefs;
        }
      }
      else
      {
        Console->Print( "%s Error loading %s defs", Console->ColorText( YELLOW, BLACK, "[WARNING]" ),  mName.c_str() );
        return (false);
      }

      if ( nErrors > 0 )
        Console->Print( "%s Loaded %i %s defs, %i error(s).", Console->ColorText( RED, BLACK, "[WARNING]" ), nDefs,  mName.c_str(), nErrors );
      else
        Console->Print( "%s Loaded %i %s defs, %i error(s).", Console->ColorText( GREEN, BLACK, "[Success]" ), nDefs,  mName.c_str(), nErrors );

      return ( true );
    }

    int32_t GetNumDefs() const
    {
        return mDefs.size();
    }

    const T *GetDef(int32_t Index) const
    {
      T* Result;

      auto it = mDefs.find( Index );
      Result = ( ( it != mDefs.end() ) ? it->second : nullptr );

      return ( Result );
    }
};
