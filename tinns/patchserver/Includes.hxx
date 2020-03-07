#pragma once

#include "patchserver/Client.hxx"
#include "patchserver/ConfigTemplate.hxx"
#include "patchserver/PatchServer.hxx"
#include "patchserver/Server.hxx"

extern class ServerSocket* ServerSock;
extern class PConsole *Console;
extern class PConfig *Config;
extern class PFileSystem *Filesystem;
extern class PServer *Server;
extern class PPatchServer *PatchServer;

bool InitTinNS();
void Shutdown();
