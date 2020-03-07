#include <chrono>
#include <csignal>
#include <thread>
#include "patchserver/Includes.hxx"
#include "common/Includes.hxx"

void signal_handler(int signal)
{
    if (signal == SIGINT)
        Shutdown();
    else
        psignal(signal, "Unkown signal: ");
}

int main()
{
    signal(SIGINT, signal_handler);


    if (!InitTinNS())
        while (true)
            std::this_thread::sleep_for(std::chrono::seconds(1));

    PatchServer->Start();
    Console->Print("Patchserver is now %s. Waiting for clients...", Console->ColorText(GREEN, BLACK, "Online"));

    while (true)
    {
        ServerSock->update();
        Server->Update();
        PatchServer->Update();
        // sched_yield();
    }

    return 0;
}
