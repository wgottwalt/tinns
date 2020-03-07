#include "gameserver/Includes.hxx"

void PCommands::doCmd_dev_h()
{
  if(IsAdmin() == false)
    return;

  if(source->GetAccountLevel() < PAL_ADMIN)
    return;



  PMessage* tmpMsg = MsgBuilder->BuildEntityPositionMsg(source, GetArgInt(1) & 0xffff, GetArgInt(2) & 0xffff, GetArgInt(3) & 0xffff);
  source->SendUDPMessage(tmpMsg);
/*
  u8 val1, val2, val3, val4;
  char tmpStr[128];

  if(ArgC > 0)
  {
    val1 = (u8)(GetArgInt(1) & 0xff);
    val2 = (u8)(GetArgInt(2) & 0xff);
    val3 = (u8)(GetArgInt(3) & 0xff);
    val4 = (ArgC > 3) ? (u8)(GetArgInt(4) & 0xff) : 0x01;
  }
  else
  {
    val1 = 0xff;
    val2 = 0xff;
    val3 = 0xff;
    val4 = 0x01;
  }

    PMessage* tmpMsg = new PMessage(14);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //source->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // source->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)source->GetLocalID();
	*tmpMsg << (u8)0x30;
	*tmpMsg << (u8)val1; //Head Heath (% ?) (45%)
	*tmpMsg << (u8)val2; //Body Heath (35%)
	*tmpMsg << (u8)val3; //Feet Heath (20%)
	*tmpMsg << (u8)val4;

    (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
    ClientManager->UDPBroadcast(tmpMsg, source);

    snprintf(tmpStr, 127, "Data set to values 0x%02x 0x%02x", val1, val2);
    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);
    */
}
