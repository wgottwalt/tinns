#include <cstring>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

bool PTerminal::HandleQueryDB(PClient* nClient, std::string *nDBCommandName, std::string *nCommandName, std::string *nOptions, uint8_t nNumOptions)
{
    EraseVars();

    bool tOk = false;

    if(!strcmp(nDBCommandName->c_str(), "ACM"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CHANGELEADER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CHANGERANK"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CHECKRUNNER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CSM"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "DELETECLAN"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "DISMISSMEMBER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "DISMISSVEHICLE"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "GIVEMONEY"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "JOINCLAN"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "KICKPLAYER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "QUITCLAN"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "REPAIRVEHICLE"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "SPAWNVEHICLE"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "TAKEFACTIONMONEY"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "TAKEMONEY"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "TRADESTOCKX"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "WARPPLAYER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "WARPPLAYERTOWORLD"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else
    {
        // Ok this REALLY should'nt happen.. But who knows..?
        Console->Print("%s [PTerminal::HandleQueryDB] Unknown ServerMessage: %s", Console->ColorText(RED,BLACK,"Error"), nDBCommandName->c_str());
        return false;
    }

    return tOk;
}
