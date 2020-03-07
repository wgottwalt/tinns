#include <csignal>
#include "infoserver/Includes.hxx"
#include "common/Includes.hxx"

void signal_handler(int signal)
{
    if (signal == SIGINT)
        Shutdown();
}

int main()
{
    signal(SIGINT, signal_handler);

    if (!Init())
    {
        if(Console)
            Console->Print("%s Aborting startup.", Console->ColorText(RED, BLACK, "[Fatal]"));
        Shutdown(); // exits with 0 ...
    }

    InfoServer->Start();
    Console->Print("Infoserver is now %s. Waiting for clients...", Console->ColorText(GREEN, BLACK, "Online"));

    while (true)
    {
        ServerSock->update();
        Server->Update();
        InfoServer->Update();
        MySQL->Update(); // MySQL keepalive
        Console->Update();
    }

    return 0;
}
