#pragma once

#include <chrono>
#include <cstdint>
#ifdef MYSQL_INC_DIR
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

// TODO: take all non-pure SQL DB access stuff out of this class

class PMySQL {
private:
    int info_port;
    char info_host[100];
    char info_userName[100];
    char info_password[100];
    char info_database[100];
    MYSQL *info_dbHandle;
    time_t mKeepaliveDelay;
    time_t mLastKeepaliveSent;

    int game_port;
    char game_host[100];
    char game_userName[100];
    char game_password[100];
    char game_database[100];
    MYSQL *game_dbHandle;

    int GameDBInuse;
    int InfoDBInuse;

public:
    PMySQL();
    ~PMySQL();

    void Update();
    void CheckResCount();

    inline MYSQL *GetInfoHandle() { return info_dbHandle; };
    inline MYSQL *GetGameHandle() { return game_dbHandle; };

    bool Connect();

    int InfoQuery(const char *query);
    MYSQL_RES *InfoResQuery(const char *query);
    int GameQuery(const char *query);
    MYSQL_RES *GameResQuery(const char *query);

    /*int GetWorldItemType(unsigned short ID, int Location);
    int GetWorldItemOption(unsigned short ID, int Location, int option);
    int GetWorldDoorType(unsigned int ID, int Location);*/

    void ShowInfoSQLError();
    void ShowGameSQLError();
    void FreeGameSQLResult(MYSQL_RES *res);
    void FreeInfoSQLResult(MYSQL_RES *res);

    inline uint32_t GetLastGameInsertId() { return mysql_insert_id(game_dbHandle); };
    inline uint32_t GetLastInfoInsertId() { return mysql_insert_id(info_dbHandle); };

    uint32_t EscapeString(const char* nText, char* dText, uint32_t dMaxLength);
};
