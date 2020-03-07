#pragma once

#include <cstdint>
#include <string>

class RegEx;

/*
0 = unregistered user
1 = Registered user
30 = volunteer
50 = GM
100 = Admin
*/

// New way of AccountLevel handling:
// Every level is possible, the following values are only edge values. We need a bit control about that
#define PAL_BANNED -1
#define PAL_UNREGPLAYER 0
#define PAL_REGPLAYER 1
#define PAL_VOLUNTEER 30
#define PAL_GM 50
#define PAL_ADMIN 100

// Max number of char slots per account
#define MAX_CHARS_PER_ACCOUNT  4

/*
0 = Offline
1 = Online
2 = Banned
*/

enum PAccountStatus
{
    PAS_OFFLINE = 0,
    PAS_ONLINE = 1,
    PAS_BANNED = 2
};

class PAccount
{
    private :
    // SQL Layout
    enum {
        a_id,
        a_username,
        a_password,
        a_priv,
        a_status,
        a_bandate,
        a_emailaddress,
        a_creationdate,
        a_lastused
    };

    // static members
    static RegEx *mUsernameRegexFilter;
    static RegEx *mPasswordRegexFilter;

    // instance members
    uint32_t mID;
    std::string mName;
    std::string mPassword;
    int32_t mLevel;
    PAccountStatus mStatus;
    time_t mBannedUntil;

    bool LoadFromQuery(char *query);
    bool DecodePassword(const uint8_t *PasswordData, int32_t PassLen, const uint8_t *Key, char *ClearPassword);

public:
    PAccount();
    PAccount(const uint32_t AccountId);
    PAccount(const char *Username);

    static bool SetUsernameRegexFilter(const char *RegexStr);
    static bool SetPasswordRegexFilter(const char *RegexStr);
    static bool IsUsernameWellFormed(const char *Username);
    static bool IsPasswordWellFormed(const char *Password);

    uint32_t GetID() const;
    bool SetName(const std::string &Pass);
    const std::string &GetName() const;
    bool SetPassword(const std::string &Pass);
    bool SetPasswordEncoded(const uint8_t *PasswordData, int32_t PassLen, const uint8_t *Key);
    const std::string &GetPassword() const;
    bool SetLevel(int32_t newLevel);
    int32_t GetLevel() const;
    std::string GetLevelString() const;
    bool SetStatus(PAccountStatus Status);
    PAccountStatus GetStatus() const;
    bool SetBannedUntilTime(time_t BannedUntil);
    bool IsBanned() const;
    std::string GetBannedTime() const;
    bool Authenticate(const uint8_t *PasswordData, int32_t PassLen, const uint8_t *Key);
    bool Authenticate(const char *Password) const;
    bool Create();
    bool Save(bool CreateMode = false);

    //u32 GetCharIdBySlot(const u32 SlotId);
};
