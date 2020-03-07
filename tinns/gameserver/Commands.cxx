#include <cstring>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PCommands::PCommands()
{
    FlushArgs();
}

PCommands::~PCommands()
{
    FlushArgs();
}

void PCommands::HandleGameCommand(char *packet, PClient *Client)
{
    // Start over with an empty class
    FlushArgs();

    // Grab client class
    source = Client;

    int posPacket = 1;
    int tmpPos = 0;

    //First of all, split the command itself. We only know here that it
    // *seems* to be an command, we have no idea if the command is valid
    do
    {
        // Copy command from packet to final var
        Command[tmpPos] = packet[posPacket];

        // Increment both pointers
        posPacket++;
        tmpPos++;
    }
    while (packet[posPacket] != ' ' && packet[posPacket] != '\0');

    // Terminate command
    Command[tmpPos] = '\0';


    // To save time, only copy org packet into dumb when we really need it.
    // On the other side, dont try to get the Args if a dumb is made.
    if (RequirePacketDumb() == true)
    {
        int loopy = 0;
        while (packet[loopy] != '\0' && loopy < (MAXDUMBSIZE - 1))
        {
            OrgPacketDumb[loopy] = packet[loopy];
            loopy++;
        }
        OrgPacketDumb[loopy] = '\0';
        DumbMade = true;
        ArgC = 0; // Just to be sure...
    }
    else
    {
        // Search next arg (Maybe someone typed more than one whitespace)
        while (packet[posPacket] == ' ' && packet[posPacket] != '\0')
        {
            posPacket++;
        }

        // ok, we have the command, now read the args
         // Loop till entire chatpacket is parsed or MAXARGS is reached
        bool tEncapsedString = false;
        while (packet[posPacket] != '\0' && ArgC <= MAXARGS)
        {
            // First set tmpPos for next Arg to 0
            tmpPos = 0;

            // Now loop until next space ' '  or end '\0' is reached
            // Added extraction of encapsulated strings "test 123 123"
            while (packet[posPacket] != ' ' || tEncapsedString == true)
             {
                // Watch out for >"<
                if(packet[posPacket] == '"')
                    tEncapsedString = !tEncapsedString;
                else
                {
                    // Copy arg from chatpacket to final arg var
                    ArgV[ArgC][tmpPos] = packet[posPacket];
                    tmpPos++;
                }
                    // Increment tmpPos and posPacket
                 posPacket++;
                if(packet[posPacket] == '\0')
                    break;
            }
            tEncapsedString = false;
            // Reached either the end of packet or an whitespace
            // Terminate current ArgV
            ArgV[ArgC][tmpPos] = '\0';

             // Heh, we got one! Now more to next arg
             ArgC++;

            // Search next arg (Maybe someone typed more than one whitespace)
            while (packet[posPacket] == ' ' && packet[posPacket] != '\0')
            {
                posPacket++;
            }
        }
        if (packet[posPacket] != '\0' && ArgC == MAXARGS && gDevDebug)
            Console->Print("%s MAXARGS reached, cant parse more args for command!", Console->ColorText(YELLOW, BLACK, "[Warning]"));
    }
    // Check if client is allowed to perform command
    if (CheckPermission() == false)
        return;

    // Also, if command is a developercommand, only let admins perform it
    if (IsDevCommand() == true && IsAdmin() == false)
        return;

    if (strcmp(Command, "debug") == 0)
    {
        doCmddebug();
    }
    else if (strcmp(Command, "settime") == 0)
    {
        doCmdsettime();
    }
    else if (strcmp(Command, "warp") == 0)
    {
        doCmdwarp();
    }
    else if (strcmp(Command, "rawf") == 0)
    {
        doCmdrawf();
    }
    else if ( (strcmp(Command, "online") == 0) || (strcmp(Command, "who") == 0) ) // Was: connectedList
    {
        doCmdconlist();
    }
    else if (strcmp(Command, "skin") == 0)
    {
        doCmdskin();
    }
    else if (strcmp(Command, "effect") == 0)
    {
        doCmdeffect();
    }
    else if (strcmp(Command, "speed") == 0)
    {
        doCmdspeed();
    }
    else if (strcmp(Command, "color") == 0)
    {
        doCmdcolor();
    }
    else if (strcmp(Command, "brightness") == 0)
    {
        doCmdbrightness();
    }
    else if (strcmp(Command, "remove") == 0)
    {
        doCmdremove();
    }
    /*    else if(strcmp(Command, "rehash") == 0)
        {
            doCmdrehash();
        }*/
    else if (strcmp(Command, "uptime") == 0)
    {
        doCmduptime();
    }
    else if (strcmp(Command, "version") == 0)
    {
        doCmdversion();
    }
    else if (strcmp(Command, "kick") == 0)
    {
        doCmdkick();
    }
    else if ( (strcmp(Command, "info") == 0) || (strcmp(Command, "whois") == 0) )
    {
        doCmdinfo();
    }
    else if (strcmp(Command, "setlevel") == 0)
    {
        doCmdsetlevel();
    }
    else if (strcmp(Command, "warpto") == 0)
    {
        doCmdwarpto();
    }
    else if (strcmp(Command, "recall") == 0)
    {
        doCmdrecall();
    }
    else if (strcmp(Command, "broadcast") == 0)
    {
        doCmdbroadcast();
    }
    else if (strcmp(Command, "t") == 0)
    {
        doCmd_dev_t();
    }
    else if (strcmp(Command, "h") == 0)
    {
        doCmd_dev_h();
    }
    else if (strcmp(Command, "v") == 0)
    {
        doCmd_dev_v();
    }
    else if (strcmp(Command, "ban") == 0)
    {
        doCmdban();
    }
    else if (strcmp(Command, "unban") == 0)
    {
        doCmdunban();
    }
    else if (strcmp(Command, "listbans") == 0)
    {
        doCmdlistbans();
    }
    else if (strcmp(Command, "shun") == 0)
    {
        doCmdshun();
    }
    else if (strcmp(Command, "unshun") == 0)
    {
        doCmdunshun();
    }
    else if (strcmp(Command, "jail") == 0)
    {
        doCmdjail();
    }
    else if (strcmp(Command, "unjail") == 0)
    {
        doCmdunjail();
    }
    else if (strcmp(Command, "teleport") == 0)
    {
        doCmdteleport();
    }
    else if (strcmp(Command, "test") == 0)
    {
        doCmdtest(); // Testcommand for various testings.
    }
    else if (strcmp(Command, "givemoney") == 0)
    {
        doCmdgivemoney();
    }
    else if (strcmp(Command, "takemoney") == 0)
    {
        doCmdtakemoney();
    }
    else if (strcmp(Command, "spawnactor") == 0)
    {
        doCmdspawnactor(); // Spawns actor next to player
    }
    else if (strcmp(Command, "weather") == 0)
    {
        doCmdweather(); // Control weather in player zone
    }
    else if (strcmp(Command, "setmainskill") == 0)
    {
        doCmdSetMainSkill();
    }
    else if (strcmp(Command, "setsubskill") == 0)
    {
        doCmdSetSubSkill();
     }
    else if (strcmp(Command, "npc") == 0)
     {
        doNPC();
     }
    else if (strcmp(Command, "npcshop") == 0)
     {
        doNPC_Shop();
     }
    // Else: unknown command. Ignore
}

bool PCommands::CheckPermission()
{
    if (CmdAccess->GetOptionInt(Command) > source->GetAccountLevel())
        return false;
    else
        return true;
}

bool PCommands::IsAdmin()
{
    if (source->GetAccountLevel() >= PAL_ADMIN)
        return true;
    else
        return false;
}


bool PCommands::GetTarget(int ArgNum)
{
    if (ArgNum > ArgC)
        return false;

    if (IsArgNumeric(ArgNum) == true)
        target = GetClientByID(GetArgInt(ArgNum));
    else
    {
        char tmp[50];
        GetArgText(ArgNum, tmp, 50);
        target = GetClientByNick(tmp);
    }

    if (target == nullptr)
        return false;
    else
        return true;
}

bool PCommands::IsArgNumeric(int ArgNum)
{
    if (ArgNum > ArgC)
        return false;

    int tmpPos = 0;
    while (ArgV[ArgNum - 1][tmpPos] != '\0')
    {
        if (!isdigit(ArgV[ArgNum - 1][tmpPos]))
            return false;

        tmpPos++;
    }
    return true;
}

void PCommands::FlushArgs()
{
    // Empty all variables
    for (int i = 0; i < MAXARGS; i++)
        ArgV[i][0] = '\0';

    Command[0] = '\0';
    OrgPacketDumb[0] = '\0';

    ArgC = 0;
    DumbMade = false;
    source = nullptr;
    target = nullptr;

}

int PCommands::GetArgInt(int ArgNum)
{
    if (ArgNum > ArgC)
        return 0;

    int tmpIntRet = 0;
    tmpIntRet = atoi(ArgV[ArgNum - 1]);
    return tmpIntRet;
}

bool PCommands::GetArgText(int ArgNum, char* output, int output_size)
{
    if (ArgNum > ArgC)
        return false;

    int i = 0;

    // Copy arg to given char array
    while (ArgV[ArgNum - 1][i] != '\0')
    {
        output[i] = ArgV[ArgNum - 1][i];
        i++;
        if ((i + 1) == output_size)
        {
            output[i] = '\0';
            return true;
        }
    }
    output[i] = '\0';
    return true;
}

PClient* PCommands::GetClientByID(int charid)
{
    return ClientManager->getClientByChar( charid );
}

PClient* PCommands::GetClientByNick(const char *nick)
{
    return ClientManager->getClientByChar( (std::string) nick );
}

bool PCommands::RequirePacketDumb()
{
    bool tmpNeedDumb = false;

    if (strcmp(Command, "broadcast") == 0)
        tmpNeedDumb = true;
    // Add more checks here

    return tmpNeedDumb;
}

bool PCommands::IsDevCommand()
{
    bool tmpIsDev = false;

    if (strcmp(Command, "t") == 0)
        tmpIsDev = true;
    else if (strcmp(Command, "h") == 0)
        tmpIsDev = true;
    else if (strcmp(Command, "v") == 0)
        tmpIsDev = true;
    else if (strcmp(Command, "test") == 0)
        tmpIsDev = true;
    // Add more checks here

    return tmpIsDev;
}
