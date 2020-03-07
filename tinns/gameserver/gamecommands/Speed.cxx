#include "gameserver/Includes.hxx"

void PCommands::doCmdspeed()
{
// Speed override setting.
// Usage: @speed <newspeed> | #
//  with <speed> = 0 (no move).. 254 , 255 or # meaning "no speed override"

    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@speed <newspeed: 0 .. 254 > | 255 | #");
        return;
    }
    char tmpval[30];
    GetArgText(1, tmpval, 30);

    uint8_t val1;
    char effStr[128];
    PMessage* tmpMsg;

    val1 = ((tmpval[0] == '#') ? 255 : (uint8_t)(atoi(tmpval) & 0xff));
    source->GetChar()->SetSpeedOverride(val1);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Speed override set to value %d ", val1);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}
