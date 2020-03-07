#pragma once

#if 0
#include <list>

struct PRConClient;

class PRConsole {
private :
    typedef std::list<PRConClient *> ClientList;
    ClientList mClients;

    void ProcessClient(PRConClient* Client);
    void Prompt(PRConClient *Client);
public :
    PRConsole();
    ~PRConsole();

    void Start();
    void Update();
};
#endif
