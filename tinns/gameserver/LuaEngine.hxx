#pragma once

#include <lua.hpp>

#define PLUAENGINE_DEFAULT      0
#define PLUAENGINE_GETANSWER    4096
#define PLUAENGINE_EXECUTENODE  8192

class PLuaEngine {
private:
    PClient* mTargetClient;

    bool mRunning;
    lua_State *mLua;

    std::vector<int> mReturnValues;


    // Return Values
    int mReturn_INT;
    // std::string mReturn_STR;
    // add more if needed...

    void CleanUp();
    bool ExecuteScript(std::string nLUAScript, int nNode, int nDialogClass = 0);

public:
    PLuaEngine();
    ~PLuaEngine();

    // To check if LUA Script has syntax errors or whatever
    bool CheckLUAFile(std::string nLUAScript);
    void AddScriptResult(int nResult);
    void ProcessDialogScript(PClient* nClient, std::string mLUAFile, int nAnswer);
    inline PClient* GetBoundClient() { return mTargetClient; };

    inline void SetReturnINT(int nValue) { mReturn_INT = nValue; };
    // inline void SetReturnSTR(std::string nValue) { mReturn_STR = nValue; };
};
