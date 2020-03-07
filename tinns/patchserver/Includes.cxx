#include "patchserver/Includes.hxx"
#include "common/Includes.hxx"

ServerSocket* ServerSock = 0;
PConsole *Console = 0;
PServer *Server = 0;
PConfig *Config = 0;
PFileSystem *Filesystem = 0;
PPatchServer *PatchServer = 0;

bool InitTinNS()
{
    Console = new PConsole("log/patchserver.log");  // Make that from config file !!!
    Console->Print("Starting TinNS Patchserver...");
    Console->Print(WHITE, BLUE, "/-------------------------------------------------------------------\\");
    Console->Print(WHITE, BLUE, "|               TinNS (TinNS is not a Neocron Server)               |");
    Console->Print(WHITE, BLUE, "|            Copyright (C) 2005 Linux Addicted Community            |");
    Console->Print(WHITE, BLUE, "|             maintainer Akiko <akiko@linux-addicted.net>           |");
    Console->Print(WHITE, BLUE, "|             ==========================================            |");
    Console->Print(WHITE, BLUE, "|      Head coders:                   The packet analyzing team:    |");
    Console->Print(WHITE, BLUE, "|      - Akiko                         - MaxxJag                    |");
    Console->Print(WHITE, BLUE, "|      - bakkdoor                      - Sting                      |");
    Console->Print(WHITE, BLUE, "|      - Namikon                       - Balm                       |");
    Console->Print(WHITE, BLUE, "|      - Hammag                                                     |");
    Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
    Console->Print(WHITE, BLUE, "|  This project would'nt be at its current stage without the help   |");
    Console->Print(WHITE, BLUE, "|        from the NeoPolis team, special thanks to you guys!        |");
    Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
    Console->Print(WHITE, BLUE, "|  This project is under GPL, see any source file for more details  |");
    Console->Print(WHITE, BLUE, "\\-------------------------------------------------------------------/");

    //char svnrev[10];
    //GetSVNRev(svnrev);
    Console->LPrint("You are running TinNS Patchserver version");
    Console->LPrint(GREEN, BLACK, " %s", DefMain::Version);
    Console->LPrint(WHITE, BLACK, " - Revision");
    Console->LPrint(GREEN, BLACK, " %s", DefMain::Revision);
    Console->LClose();

    Config = new PConfig();
    if(!Config->LoadOptions(PatchConfigTemplate ,"./conf/patchserver.conf"))
    Shutdown();

    ServerSock = new ServerSocket();
    Server = new PServer();
    PatchServer = new PPatchServer();

    return true;
}

void Shutdown()
{
    if(PatchServer) delete PatchServer;
    if(Config) delete Config;
    if(Console) delete Console;
    if(ServerSock) delete ServerSock;
    exit(0);
}
