#pragma once

#include <cstdint>

class PTerminal {
private:
    void SendTryAccessAnswer(PClient* nClient, char *nArea, bool nAllowed);
    char mSQLQuery[500];

    int mResultFields;
    void EraseVars();

    char mConPrefix[50];

    inline bool ChkOpt(uint8_t nNumOptions, uint8_t nReqOpt) { if(nNumOptions < nReqOpt) return false; else return true; };
    bool DoStockXCheck(PClient* nClient, int nAmountEntered, int nNewAmount);

public:
    PTerminal();
    //~PTerminal();
    // Check accesslevel of Player for various Terminal actions
    bool CheckAccess(PClient* nClient, char *nArea, uint16_t nCmdNr, char *nOption1, char *nOption2, char *nOption3);
    uint8_t GetNewEmailCount(PClient* nClient, bool nNoticeClient = true);
    // Handle ReceiveDB queries
    bool HandleQueryDB(PClient* nClient, std::string *nDBCommandName, std::string *nCommandName, std::string *nOptions, uint8_t nNumOptions);
    bool HandleReceiveDB(PClient* nClient, uint16_t mTerminalSessionId, std::string *nCommandName, std::string *nOptions, uint8_t nNumOptions, uint16_t nDBID, uint8_t nUnknown);
    bool HandleTryAccess(PClient* nClient, uint16_t mTerminalSessionId, std::string *nCommandName, std::string *nOptions, uint8_t nNumOptions, uint16_t nDBID, uint8_t nUnknown, bool nCheckOnly = false);
    bool HandleUpdateDB(PClient* nClient, uint16_t mTerminalSessionId, std::string *nCommandName, std::string *nOptions, uint8_t nNumOptions, uint16_t nDBID, uint8_t nUnknown);
};
