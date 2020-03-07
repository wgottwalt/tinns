INCLUDE_DIRECTORIES     (${CURRENT_SOURCE_DIR} ${LUA_INCLUDE_DIR})

ADD_SUBDIRECTORY        (decoder)
ADD_SUBDIRECTORY        (definitions)
ADD_SUBDIRECTORY        (gamecommands)

ADD_EXECUTABLE          (GameServer Accounts.cxx Appartements.cxx BuddyList.cxx Chars.cxx Chat.cxx
                                    Client.cxx ClientManager.cxx Commands.cxx Container.cxx
                                    DoorTemplate.cxx FurnitureTemplate.cxx GameServer.cxx
                                    GenrepList.cxx Includes.cxx Isc.cxx Inventory.cxx Item.cxx
                                    LuaEngine.cxx Main.cxx MessageBuilder.cxx MultiPart.cxx Npc.cxx
                                    NpcAi.cxx NpcConversation.cxx NpcTemplate.cxx Outpost.cxx
                                    RemoteConsole.cxx Server.cxx Skill.cxx Sql.cxx Subway.cxx
                                    Terminal.cxx TerminalQueryDatabase.cxx
                                    TerminalReceiveDatabase.cxx TerminalTryAccess.cxx
                                    TerminalUpdateDatabase.cxx Vehicle.cxx VehicleAccessRequest.cxx
                                    WorldActors.cxx WorldDataTemplate.cxx Worlds.cxx Zoning.cxx)

TARGET_LINK_LIBRARIES   (GameServer Common Decoder Definitions Gamecommands ${MYSQL_LIBRARY}
                                    ${PCRE_LIBRARY} ${ZLIB_LIBRARY} ${LUA_LIBRARY})
