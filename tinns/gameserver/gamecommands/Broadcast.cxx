#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdbroadcast()
{
    if(DumbMade == false)
    {
        Console->Print("%s Missing packetdumb in PCommands::doCmdbroadcast for declared broadcast function!", Console->ColorText(RED, BLACK, "[PANIC]"));
        return;
    }

    Chat->sendBroadcast(OrgPacketDumb+11);
}
