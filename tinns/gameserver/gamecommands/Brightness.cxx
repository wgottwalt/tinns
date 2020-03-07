#include "gameserver/Includes.hxx"

void PCommands::doCmdbrightness()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@brightness -|<head brightness: 0..255> [-|<torso brightness>]  [-|<legs brightness>]");
        return;
    }

    char tmp_v1[30];
    char tmp_v2[30];
    char tmp_v3[30];

    GetArgText(1, tmp_v1, 30);
    GetArgText(2, tmp_v2, 30);
    GetArgText(3, tmp_v3, 30);

    uint8_t val1, val2, val3, val4, val5, val6;
    char effStr[128];
    PMessage* tmpMsg;

    source->GetChar()->GetCurrentBodyColor(val1, val2, val3, val4, val5, val6);
    if(tmp_v1[0] != '-')
    val4 = (uint8_t)(atoi(tmp_v1) & 0xff);
    if((tmp_v2[0] != '-') && (tmp_v2[0] != '\0'))
    val5 = (uint8_t)(atoi(tmp_v2) & 0xff);
    if((tmp_v3[0] != '-') && (tmp_v3[0] != '\0'))
    val6 = (uint8_t)(atoi(tmp_v3) & 0xff);
    source->GetChar()->SetCurrentBodyColor(val1, val2, val3, val4, val5, val6);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Body brightness set to values %d %d %d", val4, val5, val6);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}
