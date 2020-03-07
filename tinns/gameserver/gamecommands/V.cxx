#include "gameserver/Includes.hxx"

void PCommands::doCmd_dev_v()
{
    if(IsAdmin() == false)
      return;

    char Arg1[30];

    Arg1[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
    }

    char tmpStr[128];
    int testmode = 1; // change here only for uint8_t/uint16_t/uint32_t testvalue use

    if(Arg1[0] != '\0')
    {
        switch(testmode)
        {
            case 0: source->testval8 = (uint8_t)(atoi(Arg1) & 0xff); break;
            case 1: source->testval16 = (uint16_t)(atoi(Arg1) & 0xffff); break;
            case 2: source->testval32 = (uint32_t)(atoi(Arg1) & 0xffffffff); break;
        }
    }
    else
    {
        source->testval8 = 0;
        source->testval16 = 0;
        source->testval32 = 0;
    }

    switch(testmode)
    {
        case 0: snprintf(tmpStr, 127, "Test value uint8_t set to 0x%02x (%d)", source->testval8, source->testval8); break;
        case 1: snprintf(tmpStr, 127, "Test value uint16_t set to 0x%04x (%d)", source->testval16, source->testval16); break;
        case 2: snprintf(tmpStr, 127, "Test value uint32_t set to 0x%08x (%d)", source->testval32, source->testval32); break;
        default: tmpStr[0] = '\0';
    }
    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);

}
