#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/*
NOTE ABOUT ACCESS LEVELS IN THE MYSQL DATABASE:
a_priv:
0 = unregistered user
1 = Registered user
30 = volunteer
50 = GM
100 = Admin

a_status:
0 = Offline
1 = Online
2 = Banned
*/

/** Static members **/
RegEx* PAccount::mUsernameRegexFilter = nullptr;
RegEx* PAccount::mPasswordRegexFilter = nullptr;

bool PAccount::SetUsernameRegexFilter(const char* RegexStr)
{
  if(mUsernameRegexFilter)
  {
    delete mUsernameRegexFilter;
    mUsernameRegexFilter = nullptr;
  }

  if(RegexStr)
  {
    try {
      mUsernameRegexFilter = new RegEx(RegexStr, PCRE_CASELESS);
    }
    catch (...) {
      return false;
    }
  }
  return true;
}

bool PAccount::SetPasswordRegexFilter(const char* RegexStr)
{
  if(mPasswordRegexFilter)
  {
    delete mPasswordRegexFilter;
    mPasswordRegexFilter = nullptr;
  }

  if(RegexStr)
  {
    try {
      mPasswordRegexFilter = new RegEx(RegexStr, PCRE_CASELESS);
    }
    catch (...) {
      return false;
    }
  }
  return true;
}

bool PAccount::IsUsernameWellFormed(const char *Username)
{
  if(mUsernameRegexFilter)
  {
    return mUsernameRegexFilter->Search(Username);
  }
  else
    return true;
}

bool PAccount::IsPasswordWellFormed(const char *Password)
{
  if(mPasswordRegexFilter)
  {
    return mPasswordRegexFilter->Search(Password);
  }
  else
    return true;
}

/** Instance members **/
PAccount::PAccount()
{
    mID = 0;
    mLevel = PAL_BANNED;
  mStatus = PAS_OFFLINE;
  mBannedUntil = 0;
}

PAccount::PAccount(const uint32_t AccountId)
{
  char query[256];
  mID = 0;
  snprintf(query, 256, "SELECT * FROM accounts WHERE a_id = %d LIMIT 1;", AccountId);
  LoadFromQuery(query);
}

PAccount::PAccount(const char *Username)
{
    std::string query;
    mID = 0;
    if (IsUsernameWellFormed(Username))
    {
        char escUsername[256];
        MySQL->EscapeString(Username, escUsername, 256);
        query.append("SELECT * FROM accounts WHERE a_username = '").append(escUsername)
             .append("' LIMIT 1;");
        LoadFromQuery(query.c_str());
    }
}

bool PAccount::LoadFromQuery(const std::string &query)
{
  MYSQL_ROW row = 0;
  MYSQL_RES *result = 0;

  bool FinalResult = false;

  result = MySQL->InfoResQuery(query.c_str());
  if(result == nullptr)
  {
      Console->Print(RED, BLACK, "Failed to load AccountData from SQL");
      MySQL->ShowInfoSQLError();
      return false;
  }

  if((row = mysql_fetch_row(result)))
  {
    mID = atoi(row[a_id]);
    mName = row[a_username];
    mPassword = row[a_password];

    mBannedUntil = atoi(row[a_bandate]);
    if(mBannedUntil > Time::nowTimeT())
    {
      mStatus = PAS_BANNED;
      mLevel = PAL_BANNED;
    }
    else
    {
      mStatus = PAS_OFFLINE;
      mLevel = atoi(row[a_priv]);
    }

    FinalResult = true;
  }
  else
  {
Console->Print(YELLOW, BLACK, "Failed to load AccountData from SQL; Nothing to load...");
  }

  MySQL->FreeInfoSQLResult(result);
  return FinalResult;
}

bool PAccount::SetName(const std::string &Username)
{
  if(IsUsernameWellFormed(Username.c_str()))
  {
    mName = Username;
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::SetPassword(const std::string &Password)
{
  if(IsPasswordWellFormed(Password.c_str()))
  {
    mPassword = Password;
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::SetPasswordEncoded(const uint8_t* PasswordData, int PassLen, const uint8_t* Key)
{
    char Pass[128];

    if(DecodePassword(PasswordData, PassLen, Key, Pass))
  {
        return SetPassword((std::string)Pass);
    }
    else
    {
        Console->Print(RED, BLACK, "[Error]: user %s : malformed auth data (size=%d)", mName.c_str(), PassLen);
        return false;
    }
}

bool PAccount::SetLevel(int newLevel)
{
  if((newLevel >= PAL_BANNED) && (newLevel <= PAL_ADMIN))
  {
    mLevel = newLevel;
    return true;
  }
  else
  {
    return false;
  }
}

std::string PAccount::GetLevelString() const
{
    switch(mLevel)
    {
        case PAL_BANNED : return "banned";
        case PAL_UNREGPLAYER : return "unregplayer";
        case PAL_REGPLAYER : return "regplayer";
        case PAL_VOLUNTEER : return "volunteer";
        case PAL_GM : return "gm";
        case PAL_ADMIN : return "admin";
    }

    return "custom";
}

bool PAccount::SetStatus(PAccountStatus Status)
{
  mStatus = Status;
  return true;
}

bool PAccount::SetBannedUntilTime(time_t BannedUntil)
{
  if ((BannedUntil == 0) || (BannedUntil > Time::nowTimeT()))
  {
    mBannedUntil = BannedUntil;
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::IsBanned() const
{
    return (mBannedUntil > Time::nowTimeT());
}

bool PAccount::DecodePassword(const uint8_t* PasswordData, int PassLen, const uint8_t *Key, char* ClearPassword)
{
  ClearPassword[0] = 0;

  if(PassLen < 128)
    {
        if(Key[0]>7) // TODO: >7 correct?
        {
            for(int i=0; i<PassLen; i+=2)
                ClearPassword[i>>1] = (char)(((PasswordData[i]&0xf0)>>4)
                    +((PasswordData[i+1]&0x0f)<<4)-Key[0]);
            ClearPassword[PassLen>>1]=0;
        }
        else
        {
            for(int i=0; i<PassLen; i++)
                ClearPassword[i] = (char)(PasswordData[i]-Key[0]);
            ClearPassword[PassLen]=0;
        }
        return true;
    }
    else
      return false;
}

bool PAccount::Authenticate(const uint8_t* PasswordData, int PassLen, const uint8_t *Key)
{
    char Pass[128];

    if(DecodePassword(PasswordData, PassLen, Key, Pass))
  {
        return Authenticate(Pass);
    }
    else
    {
        Console->Print(RED, BLACK, "[Error]: user %s : malformed auth data (size=%d)", mName.c_str(), PassLen);
        return false;
    }
}

bool PAccount::Authenticate(const char *Password) const
{
    if(mID == 0) // User doesn't exist and that hasn't been checked !
    {
      Console->Print(RED, BLACK, "[Bug]: user %s doesn't exist and was not checked by code !", mName.c_str());
      return false;
    }

    return(mPassword == Password);
}

bool PAccount::Create()
{
  if(Save(true)) {
    mID = MySQL->GetLastInfoInsertId();
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::Save(bool CreateMode)
{
  char escUsername[256];
  char escPassword[256];
  MySQL->EscapeString(mName.c_str(), escUsername, 256);
  MySQL->EscapeString(mPassword.c_str(), escPassword, 256);

  std::string Query;
  Query = CreateMode ? "INSERT INTO" : "UPDATE";
  Query += "accounts SET ";
  Query += Misc::String::create("a_username='", escUsername, "', a_password = '", escPassword, "'");
  Query += Misc::String::create(", a_priv = ", mLevel,", a_status = ", mStatus, ", a_bandate = ", mBannedUntil);
  if(!CreateMode )
  {
    Query += Misc::String::create(" WHERE a_id = ", mID, " LIMIT 1");
  }

  if(MySQL->InfoQuery(Query.c_str()))
  {
      Console->Print(RED, BLACK, "[Error] Failed to %s account %s (id %d)", CreateMode ? "create" : "update", mName.c_str(), mID);
      MySQL->ShowInfoSQLError();
      return false;
  }
  return true;
}

uint32_t PAccount::GetCharIdBySlot(const uint32_t SlotId)
{
  char query[256];
  uint32_t CharId = 0;

  MYSQL_ROW row = 0;
  MYSQL_RES *result = 0;

  snprintf(query, 256, "SELECT c_id FROM characters WHERE a_id = %d AND c_slot = %d LIMIT 1;", mID, SlotId);

  result = MySQL->GameResQuery(query);
  if (result == nullptr)
  {
      Console->Print(RED, BLACK, "Failed to load CharacterData from SQL");
      MySQL->ShowGameSQLError();
      return 0;
  }

  if((row = mysql_fetch_row(result)))
  {
    CharId = atoi(row[0]);
  }

  MySQL->FreeGameSQLResult(result);

  /*** Temporary workaround to cope with DB where c_slot is not set ***/
  if(!CharId)
  {
    snprintf(query, 256, "SELECT c_id FROM characters WHERE a_id = %d ORDER BY c_slot ASC, c_id ASC LIMIT %d, 1;", mID, SlotId);

    result = MySQL->GameResQuery(query);
    if (result == nullptr)
    {
        Console->Print(RED, BLACK, "Failed to load CharacterData from SQL");
        MySQL->ShowGameSQLError();
        return 0;
    }

    if((row = mysql_fetch_row(result)))
    {
      CharId = atoi(row[0]);
    }

    MySQL->FreeGameSQLResult(result);
  }
  /*** End of workaround ***/

  return CharId;
}


std::string PAccount::GetBannedTime() const
{
  const char* unit[5] = {"seconds", "minutes", "hours", "days", "weeks"};

  time_t timediff = mBannedUntil - Time::nowTimeT();
  if(timediff <=0)
  {
    return "0 more seconds. Please relog";
  }

  long counter;
  int type;

  if(timediff > 86400)   // days
  {
    counter = timediff / 86400;
    type = 3;
  }
  else if(timediff > 3600)    // hours
  {
    counter = timediff / 3600;
    type = 2;
  }
  else if(timediff > 60)      // Minutes
  {
    counter  = timediff / 60;
    type = 1;
  }
  else      // Seconds
  {
    counter = timediff;
    type = 0;
  }

  return Misc::String::create(counter, " more ", unit[type]);
}
