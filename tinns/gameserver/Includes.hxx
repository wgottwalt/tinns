#pragma once

#include "gameserver/Accounts.hxx"
#include "gameserver/Appartements.hxx"
#include "gameserver/BuddyList.hxx"
#include "gameserver/Chars.hxx"
#include "gameserver/Chat.hxx"
#include "gameserver/Client.hxx"
#include "gameserver/ClientManager.hxx"
#include "gameserver/Commands.hxx"
#include "gameserver/ConfigTemplate.hxx"
#include "gameserver/Container.hxx"
#include "gameserver/DoorTemplate.hxx"
#include "gameserver/FurnitureTemplate.hxx"
#include "gameserver/GameServer.hxx"
#include "gameserver/GenrepList.hxx"
#include "gameserver/Inventory.hxx"
#include "gameserver/Isc.hxx"
#include "gameserver/Item.hxx"
#include "gameserver/LuaEngine.hxx"
#include "gameserver/Main.hxx"
#include "gameserver/MessageBuilder.hxx"
#include "gameserver/MultiPart.hxx"
#include "gameserver/Npc.hxx"
#include "gameserver/NpcTemplate.hxx"
#include "gameserver/Outpost.hxx"
#include "gameserver/RemoteConsole.hxx"
#include "gameserver/Server.hxx"
#include "gameserver/Skill.hxx"
#include "gameserver/Sql.hxx"
#include "gameserver/Subway.hxx"
#include "gameserver/Terminal.hxx"
#include "gameserver/Vehicle.hxx"
#include "gameserver/VehicleAccessRequest.hxx"
#include "gameserver/WorldActors.hxx"
#include "gameserver/WorldDataTemplate.hxx"
#include "gameserver/Worlds.hxx"
#include "gameserver/Zoning.hxx"

extern class PLuaEngine *LuaEngine;

extern class ServerSocket *ServerSock;
extern class PConsole *Console;
//extern class PRConsole *RemoteConsole;
extern class PConfig *Config;
extern class PConfig *CmdAccess;

extern class PGameDefs *GameDefs;
extern class PFileSystem *Filesystem;
extern class PServer *Server;

extern class PGameServer *GameServer;
extern class PChars *Chars;

extern class PMsgBuilder *MsgBuilder;
extern class PWorlds *Worlds;
extern class PAppartements* Appartements;
extern class PWorldActors* WorldActors;
extern class PTerminal* Terminal;
//extern class gmMachine machine;   // Virtual Machine instance

//multiuser chat
extern class PClientManager *ClientManager;
extern class PNPCManager* NPCManager;
extern class PChat *Chat;
extern class PCommands *GameCommands;

extern class POutpost *Outposts;
extern class PMultiPart *MultiPartHandler;

// Development debug output control
extern bool gDevDebug;

//MySQL-Support
extern class PMySQL* MySQL;

//Vehicles
extern class PVehicles *Vehicles;
extern class PSubway* Subway;

//Infoserver update
extern class PISC *ISC;

bool InitTinNS();
void Shutdown();
