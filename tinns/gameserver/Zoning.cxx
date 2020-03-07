#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

// TODO: Put get the fallback world from config
//       Check for file existence before sending info to client to avoid client crash and bad location in char info

void SendZone(PClient *Client, uint32_t loc)
{
    //TODO : FIX case for worldroute and 5 missing id 505
  std::string worldName;
    ConnectionTCP *Socket = Client->getTCPConn();
    //PChar *Char = Chars->GetChar(Client->GetCharID());

    Socket->SetTimeOutValue(0xffff);

  PWorld* CurrentWorld = Worlds->GetWorld(loc);
  if (CurrentWorld)
  {
    worldName = CurrentWorld->GetBspName();
    if (worldName.empty())
    {
      worldName = "plaza/plaza_p1"; // Should be a config entry
      Console->Print("Client %d: Empty name for world %d. Redirecting to %s", Console->ColorText(RED, BLACK, "Warning"), Client->GetID(), loc, worldName.c_str());
      loc = 1;
    }
  }
  else
  {
    worldName = "plaza/plaza_p1"; // Should be a config entry
    Console->Print("Client %d: Invalid or not loaded world %d. Redirecting to %s", Console->ColorText(YELLOW, BLACK, "Warning"), Client->GetID(), loc, worldName.c_str());
    loc = 1;
  }

  PMessage* cMsg = MsgBuilder->BuildSendZoneTCPMsg(loc, &worldName);
    Client->SendTCPMessage(cMsg);
}
