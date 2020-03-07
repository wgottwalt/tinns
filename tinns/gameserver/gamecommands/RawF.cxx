#include <cstring>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmdrawf()
{
    bool SyntaxError = false;
    bool DebugMode = false;
    int Prot = 0;  // 0: Error 1: UDP 2: TCP

    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    char file_to_send[50], protocol_to_use[10];
    GetArgText(1, file_to_send, 50);
    GetArgText(2, protocol_to_use, 10);

    if(strcmp(protocol_to_use, "udp") == 0)
    {
        Prot = 1;
    }
    else if(strcmp(protocol_to_use, "tcp") == 0)
    {
        Prot = 2;
    }
    else if(strcmp(protocol_to_use, "debug") == 0)
    {
        DebugMode = true;
    }
    else
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@rawf <file> <udp/tcp/debug>");
        return;
    }

    std::ifstream::pos_type size;
    char *buffer;

    std::ifstream hexdump (file_to_send, std::ios::in | std::ios::binary | std::ios::ate);
    if (hexdump.is_open())
    {
        if (gDevDebug) Console->Print("IngameCommand: Sending packet file %s", file_to_send);

        size = hexdump.tellg();
        buffer = new char [size];
        hexdump.seekg (0, std::ios::beg);

        hexdump.read (buffer, size);
        hexdump.close();
        if(DebugMode == true)
        {
            int k;
            Console->Print("Byte dump of %s:", file_to_send);
            for(k=0;k<size;k++)
            {
                Console->Print("Byte %d: %#x", k, buffer[k]);
            }
        }
        else
        {
            if(Prot == 1)
                source->getUDPConn()->write(buffer, size);
            else if(Prot == 2)
                source->getTCPConn()->write(buffer, size);
            else // Should never happen...
                Console->Print("%s unable to determine protocol in PCommands::doCmdrawf", Console->ColorText(RED, BLACK, "[Error]"));
        }

        delete[] buffer;
    }
    else
    {
        char output[100];
        Console->Print("IngameCommand: Unable to send file %s", file_to_send);
        snprintf(output, 100, "File not found: %s", file_to_send);
        Chat->send(source, CHAT_DIRECT, "System", output);
    }
}
