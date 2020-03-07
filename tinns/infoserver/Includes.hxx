#pragma once

#include "infoserver/Accounts.hxx"
#include "infoserver/ConfigTemplate.hxx"
#include "infoserver/Client.hxx"
#include "infoserver/InfoServer.hxx"
#include "infoserver/Server.hxx"
#include "infoserver/Sql.hxx"

extern class ServerSocket* ServerSock;
extern class PConsole *Console;
extern class PServer *Server;
extern class PConfig *Config;
extern class PInfoServer *InfoServer;

extern class PMySQL* MySQL;
//extern class PAccounts* Accounts;  // To be removed

bool Init();
void Shutdown();
bool AdditionnalConfigChecks();
