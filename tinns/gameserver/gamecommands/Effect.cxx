#include <cstdint>
#include <cstdio>
#include "gameserver/Includes.hxx"

void PCommands::doCmdeffect()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == false)
        SyntaxError = true;

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@effect <effect: 0=none, 1 .. 17> [<density: 0=max .. 255=min>]");
        return;
    }

    uint8_t val1, val2;
    char effStr[128];
    PMessage* tmpMsg;

    val1 = (uint8_t)(GetArgInt(1) & 0xff);
    val2 = (uint8_t)(GetArgInt(2) & 0xff);

    source->GetChar()->SetBodyEffect(val1, val2);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Body effect set to value %d with density %d (but you can see it yourself)", val1, val2);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}
