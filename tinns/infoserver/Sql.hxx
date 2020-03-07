#pragma once

#include <cstdint>
#ifdef MYSQL_INC_DIR
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

class PMySQL {
private:
    int32_t port;
    char host[100];
    char userName[100];
    char password[100];
    char database[100];
    MYSQL *dbHandle;
    time_t mKeepaliveDelay;
    time_t mLastKeepaliveSent;

public:
    PMySQL();
    ~PMySQL();

    void Update();
    MYSQL *GetHandle();

    bool Connect();
    int32_t Query(const char *query);
    MYSQL_RES *ResQuery(const char *query);
    void ShowSQLError();
    void FreeSQLResult(MYSQL_RES *res);
    uint32_t GetLastInsertId();
    uint32_t EscapeString(const char *nText, char *dText, uint32_t dMaxLength);
};
