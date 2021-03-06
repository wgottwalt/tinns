#pragma once

#define MAXARGS 10       // Define the max nr. of args here. Should never be more than 4, but who knows...
#define MAXDUMBSIZE 256 // Max size of packetdumb

class PCommands
{
private:
    char OrgPacketDumb[MAXDUMBSIZE];    // Raw dumb of original packet
    char Command[30];           // Command itself without @
    char ArgV[MAXARGS][30];     // Array of arg-values
    int ArgC;                   // Nr of args in array (To perform checks only, ArgV is terminated!)
    bool DumbMade;              // Set TRUE if dumb is made

    PClient* GetClientByID(int charid);
    PClient* GetClientByNick(const char *nick);

    PClient *source;            // "sender" of command
    PClient *target;            // possible "target" of command

    bool GetTarget(int ArgNum);     // Get target by ArgV index
    bool CheckPermission();         // Checks permission depending on commands.conf file
    bool IsArgNumeric(int ArgNum);  // is_numeric function from VB, loved this one :D
    //bool SendRawFile(PClient *Client, char *FileName, int protokoll);  <- UnUsed??
    bool RequirePacketDumb();       // Detect if command needs access to the entire packet or just the args
    bool IsDevCommand();            // To add Developer only commands without documentation in commands.conf
    bool IsAdmin();                 // Check if sender is Admin

    int GetArgInt(int ArgNum);  // Get ArgV[x] as integer (if possible)

    bool GetArgText(int ArgNum, char* output, int output_size); // Get ArgV[x] as char array

    void FlushArgs();           // Empty all vars for next command


    /// ------- Begin commandset -------
    void doCmddebug();
    void doCmdsettime();
    void doCmdwarp();
    void doCmdrawf();
    // Not required anymore, TinNS gets is worlditemdata from .dat files now
    // void doCmddelworlditem();
    // void doCmdaddworlditem();
    // void doCmdadddor();
    void doCmdconlist();
    void doCmdskin();
    void doCmdeffect();
    void doCmdspeed();
    void doCmdcolor();
    void doCmdbrightness();
    void doCmdremove();
    void doCmduptime();
    void doCmdversion();
    void doCmdkick();
    void doCmdinfo();
    void doCmdsetlevel();
    void doCmdwarpto();
    void doCmdrecall();
    void doCmdbroadcast();
    void doCmd_dev_t();
    void doCmd_dev_h();
    void doCmd_dev_v();
    void doCmdban();
    void doCmdunban();
    void doCmdlistbans();
    void doCmdshun();
    void doCmdunshun();
    void doCmdjail();
    void doCmdunjail();
    void doCmdteleport();
    void doCmdgivemoney();
    void doCmdtakemoney();
    void doCmdspawnactor();
    void doCmdeditactor();
    void doCmdweather();
    void doCmdSetMainSkill();
    void doCmdSetSubSkill();
    void doNPC();
    void doNPC_Shop();

    void doCmdtest();
    /// ------- End commandset -------

public:
    PCommands();
    ~PCommands();

    void HandleGameCommand(char *packet, PClient *Client);
};
