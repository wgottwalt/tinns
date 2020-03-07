#include "gameserver/Includes.hxx"

void PCommands::doCmdspawnactor()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == false || IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }

    uint16_t tmpActorID = (uint16_t)GetArgInt(1);
    uint16_t tmpFunctionID = (uint16_t)GetArgInt(2);
    uint16_t tmpOption1 = 0;
    uint16_t tmpOption2 = 0;
    uint16_t tmpOption3 = 0;

    if(ArgC > 2)
    {
        if(IsArgNumeric(3) == false)
        {
            SyntaxError = true;
        }
        else
        {
            tmpOption1 = (uint16_t)GetArgInt(3);
            if(ArgC > 3)
            {
                if(IsArgNumeric(4) == false)
                {
                    SyntaxError = true;
                }
                else
                {
                    tmpOption1 = (uint16_t)GetArgInt(4);
                    if(ArgC > 4)
                    {
                        if(IsArgNumeric(4) == false)
                        {
                            SyntaxError = true;
                        }
                        else
                        {
                            tmpOption1 = (uint16_t)GetArgInt(5);
                        }
                    }
                }
            }
        }
    }

    if(tmpActorID == 0)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@spawnactor <actorID> <functionID> [<option1> <option2> <option3>]");
        return;
    }

    if(WorldActors->IsValidWAFunction(tmpFunctionID) == true)
    {
        if(WorldActors->RequiresLinkedObject(tmpFunctionID == true))
        {
            if(WorldActors->IsValidLinkedObject(source, tmpOption1, tmpFunctionID) == false)
            {
                if(tmpOption1 == 0)
                    Chat->send(source, CHAT_DIRECT, "System", "Error: This functionID requires an linked object");
                else if(tmpOption1 == 18)
                    Chat->send(source, CHAT_DIRECT, "System", "Error: Invalid destination world");
                else
                    Chat->send(source, CHAT_DIRECT, "System", "Error: Invalid worldobjectID to link to");
                return;
            }
        }
        WorldActors->AddWorldActor(source, tmpActorID, tmpFunctionID, tmpOption1, tmpOption2, tmpOption3);
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "System", "Error: This is an invalid function ID");
        return;
    }
    //PMessage* tmpMsg = MsgBuilder->BuiltSpawnObjectMsg(source, tmpActorID, tmpFunctionID, mWOID++);
    //ClientManager->UDPBroadcast(tmpMsg, source);
    //tmpMsg = nullptr;
}
