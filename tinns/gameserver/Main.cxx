#include <csignal>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

// for handling strg-c signal to shutdown in correct way
void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        //cout << "Shutting down TinNS" << endl;

        //exit(0);
        Shutdown();
    }
    else
    {
        psignal(signal, "Unkown signal: ");
    }
}

int main()
{
    // Connect signal with handlerfunction
    signal(SIGINT, signal_handler); // TODO: change for sigaction()


    if (!InitTinNS())
    {
        if(Console)
            Console->Print("%s Aborting startup.", Console->ColorText(RED, BLACK, "[Fatal]"));
        Shutdown(); // exits with 0 ...
    }

    //RemoteConsole->Start();
    GameServer->Start();
    //GameServer->SetGameTime(0);
    ISC->Start();

    Console->Print("Gameserver is now %s. Waiting for clients...", Console->ColorText(GREEN, BLACK, "Online"));


    while(1)
    {
      ServerSock->update();
    NPCManager->Update();
        Server->Update();
        Chars->Update();
        GameServer->Update();
        PMessage::CheckMsgCount(); // Memory leak check
        MySQL->Update(); // Memory leak check   and MySQL keepalive
        ISC->Update();
        Console->Update();
        MultiPartHandler->Update();
    }

    return 0;
}
