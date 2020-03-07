#include <chrono>
#include <thread>
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

void PCommands::doCmd_dev_t()
{
  const char* usage = "Usage: @t r <rawid> (to remove object) | @t d <rawid> [<v1=74> [<v2=29>]] (to send death packet to object with args v1 v2)";
  char tmpStr[128];
  const char* textMsg = usage;
  PMessage* tmpMsg = nullptr;
  char Arg1[30];

  if ( IsAdmin() == false )
    return;

  Arg1[0] = tmpStr[0] = '\0';

  if ( ArgC >= 2 )
  {
    GetArgText( 1, Arg1, 30 );
    uint32_t targetObjectId = GetArgInt( 2 ) & 0xffffffff;
    if ( Arg1[0] == 't' )
    {
      uint8_t val1;
      if ( ArgC >= 3 )
      {
        int val2 = GetArgInt( 3 );
        tmpMsg = new PMessage( 32 );
        source->IncreaseUDP_ID();
        *tmpMsg << ( uint8_t )0x13;
        *tmpMsg << ( uint16_t )source->GetUDP_ID();
        *tmpMsg << ( uint16_t )source->GetSessionID();
        *tmpMsg << ( uint8_t )0x00; // Message length
        *tmpMsg << ( uint8_t )0x03;
        *tmpMsg << ( uint16_t )source->GetUDP_ID();
        *tmpMsg << ( uint8_t )0x2d;
        *tmpMsg << ( uint32_t )targetObjectId;
        *tmpMsg << ( uint8_t )0x01;
        *tmpMsg << ( uint32_t )val2;

        ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
        source->SendUDPMessage( tmpMsg );
        snprintf( tmpStr, 127, "Sent 13/03/2d msg to object id 0x%08x with values 6/%d", targetObjectId, val2 );
        textMsg = tmpStr;
      }
      else for(val1 = 2; val1 < 255; ++val1)
      {
        if(val1 == 6) continue;
        tmpMsg = new PMessage( 15 );
        source->IncreaseUDP_ID();
        *tmpMsg << ( uint8_t )0x13;
        *tmpMsg << ( uint16_t )source->GetUDP_ID();
        *tmpMsg << ( uint16_t )source->GetSessionID();
        *tmpMsg << ( uint8_t )0x00; // Message length
        *tmpMsg << ( uint8_t )0x03;
        *tmpMsg << ( uint16_t )source->GetUDP_ID();
        *tmpMsg << ( uint8_t )0x2d;
        *tmpMsg << ( uint32_t )targetObjectId;
        *tmpMsg << ( uint8_t )val1;

        ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
        source->SendUDPMessage( tmpMsg );
        snprintf( tmpStr, 127, "Sent 13/03/2d msg to object id 0x%08x with value %d", targetObjectId, val1 );
        textMsg = tmpStr;
        tmpStr[127] = '\0';
        Chat->send( source, CHAT_DIRECT, "System", textMsg );
        source->getUDPConn()->update();
        source->getTCPConn()->update();
        //sleep(1);
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      tmpMsg = nullptr;
    }
    else if ( Arg1[0] == 'd' )
    {
      uint8_t val1 = 0x4a; // default values
      uint8_t val2 = 0x1e;

      if ( ArgC >= 3 )
        val1 = GetArgInt( 3 ) & 0xff;
      if ( ArgC >= 4 )
        val2 = GetArgInt( 4 ) & 0xff;
      tmpMsg = MsgBuilder->BuildNpcDeathMsg( source, targetObjectId, val1, val2 );
      snprintf( tmpStr, 127, "Sending Death update to object id 0x%08x with values 0x%02x 0x%02x", targetObjectId, val1, val2 );
      textMsg = tmpStr;
    }
    else if ( Arg1[0] == 'm' )
    {
      uint8_t nTxtGroupID = targetObjectId & 0xff;
      uint16_t nTxtID = 10;
      //uint32_t nVal = 0;
      if ( ArgC >= 3 )
        nTxtID = GetArgInt( 3 ) & 0xffff;
      //if(ArgC >= 4)
      //  val2 = GetArgInt(4) & 0xff;

      tmpMsg = new PMessage( 20 );

      source->IncreaseUDP_ID();
      *tmpMsg << ( uint8_t )0x13;
      *tmpMsg << ( uint16_t )source->GetUDP_ID();
      *tmpMsg << ( uint16_t )source->GetSessionID();
      *tmpMsg << ( uint8_t )0x0e; // Message length
      *tmpMsg << ( uint8_t )0x03;
      *tmpMsg << ( uint16_t )source->GetUDP_ID();
      *tmpMsg << ( uint8_t )0x1f;
      *tmpMsg << ( uint16_t )source->GetLocalID();
      *tmpMsg << ( uint8_t )0x25; // ??
      *tmpMsg << ( uint8_t )0x15; // ??
      *tmpMsg << nTxtGroupID;
      *tmpMsg << nTxtID;
      *tmpMsg << ( uint16_t )0x00; // ??
      *tmpMsg << ( uint8_t )0x01;
      *tmpMsg << ( uint8_t )0x04;
      *tmpMsg << ( uint32_t )0x00;
      //*tmpMsg << (uint8_t)0x00; // ??
      //*tmpMsg << (uint8_t)0x00; // ??
      //*tmpMsg << (uint32_t)nVal;

      ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
      snprintf( tmpStr, 127, "Using msg n %d from group %d", nTxtID, nTxtGroupID );
      textMsg = tmpStr;

      source->SendUDPMessage( tmpMsg );
      tmpMsg = nullptr;
    }
    else if ( Arg1[0] == 'e' )
    {
      targetObjectId = 1004; //target=int
      uint16_t nval = 5;
      uint8_t nType = 1; // 1=+ 2=-
      uint16_t nDur = 20;
      uint8_t nparam = 1;

        nparam = GetArgInt( 2 ) & 0xff;

        tmpMsg = new PMessage( 32 );

        source->IncreaseUDP_ID();
        *tmpMsg << ( uint8_t )0x13;
        *tmpMsg << ( uint16_t )source->GetUDP_ID();
        *tmpMsg << ( uint16_t )source->GetSessionID();
        *tmpMsg << ( uint8_t )0x0e; // Message length
        *tmpMsg << ( uint8_t )0x03;
        *tmpMsg << ( uint16_t )source->GetUDP_ID();
        *tmpMsg << ( uint8_t )0x1f;
        *tmpMsg << ( uint16_t )source->GetLocalID();
        *tmpMsg << ( uint8_t )0x25; // ??
        *tmpMsg << ( uint8_t )0x06; // ??
        *tmpMsg << ( uint8_t )0x01; // 1 effect
        *tmpMsg << ( uint8_t )0x01; // effect on intox level ????
        *tmpMsg << ( uint16_t )nDur;
        *tmpMsg << ( uint16_t )(700+(targetObjectId % 100)); //item id Thyronol
        *tmpMsg << ( uint8_t )nType;
        *tmpMsg << ( uint16_t )(nval*100); //uint32_t in nc2.2
        *tmpMsg << ( uint16_t )targetObjectId;


        ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );
        snprintf( tmpStr, 127, "Sendind drug mod to stat %d, %s %d", targetObjectId, (nType == 1 ? "inc" : "dec"), nval );
        textMsg = tmpStr;

        source->SendUDPMessage( tmpMsg );
        tmpMsg = nullptr;
    }
    else if ( Arg1[0] == 'w' )
    {
      /*targetObjectId &= 0xffff;
      uint16_t val1 = 0;
      if ( ArgC >= 3 )
        val1 = GetArgInt( 3 ) & 0xffff;
*/
      tmpMsg = new PMessage( 15 );

      source->IncreaseUDP_ID();

      *tmpMsg << ( uint8_t )0x13;
      *tmpMsg << ( uint16_t )source->GetUDP_ID();
      *tmpMsg << ( uint16_t )source->GetSessionID();
      *tmpMsg << ( uint8_t )0x0a; // Message length place;
      *tmpMsg << ( uint8_t )0x03;
      *tmpMsg << ( uint16_t )source->GetUDP_ID();
      *tmpMsg << ( uint8_t )0x23;
      *tmpMsg << ( uint16_t )0x0012; // cmd = ?
      *tmpMsg << ( uint16_t )0x0007;
      *tmpMsg << ( uint32_t )0x00000000;

      ( *tmpMsg )[5] = ( uint8_t )( tmpMsg->GetSize() - 6 );

      snprintf( tmpStr, 127, "Sending w msg " );
      textMsg = tmpStr;

      source->SendUDPMessage( tmpMsg );
      tmpMsg = nullptr;
    }
    else if ( Arg1[0] == 'x' )
    {
      uint8_t val1 = targetObjectId &= 0xff;

      tmpMsg = MsgBuilder->BuildUndefineduseMsg( source, val1 );

      snprintf( tmpStr, 127, "Sending x msg with param %d (0x%2x)", val1, val1 );
      textMsg = tmpStr;

      source->SendUDPMessage( tmpMsg );
      tmpMsg = nullptr;
    }
  }

  tmpStr[127] = '\0';
  Chat->send( source, CHAT_DIRECT, "System", textMsg );

  if ( tmpMsg )
    ClientManager->UDPBroadcast( tmpMsg, source );
}

/*** Packet fields testing. Please do not delete (Hammag)
void PCommands::doCmd_dev_t()
{
    if(IsAdmin() == false)
        return;

    char Arg1[30], Arg2[30];

    Arg1[0] = '\0';
    Arg2[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
        if(ArgC > 1)
        {
            GetArgText(2, Arg2, 30);
        }
    }

    uint32_t val1;
    uint8_t val2;
    char tmpStr[128];
    static PMessage* tmpMsg = nullptr;
    bool SetUDP_IDNeeded = true;
    PChar* nChar = source->GetChar();
    (nChar->Coords).mY += 20;
    (nChar->Coords).mZ += 20;
    (nChar->Coords).mX += 20;

    if (!tmpMsg)
    {
      tmpMsg = MsgBuilder->BuildPacket0Msg(source);
      //tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
      SetUDP_IDNeeded = false;
    }

    if(Arg1[0] != '\0' && Arg2[0] != '\0')
    {
        val1 = atoi(Arg1);
        val2 = (uint8_t)(atoi(Arg2) & 0xff);
        //tmpMsg->U8Data(16 + val1) = val2;
        tmpMsg->U8Data(10 + val1) = val2;
        snprintf(tmpStr, 127, "Data #%d set to value 0x%02x", val1, val2);
    }
    else
    {
      if (tmpMsg)
        delete tmpMsg;
      //tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
      tmpMsg = MsgBuilder->BuildPacket0Msg(source);
      SetUDP_IDNeeded = false;
      snprintf(tmpStr, 127, "Data reset to normal values");
    }

    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);

    PMessage* SendMsg = new PMessage(*tmpMsg);
    if(SetUDP_IDNeeded) {
      source->FillInUDP_ID(SendMsg);
    }
    SendMsg->Dump();
    //ClientManager->UDPBroadcast(SendMsg, source);
    source->SendUDPMessage(SendMsg);
}
***/

/*** Subwy testing. Please do not delete (Hammag)
void PCommands::doCmd_dev_t()
{
  if(IsAdmin() == false)
      return;

  char tmpStr[128];
 uint8_t SubWay[] = {0x13, 0x71, 0x00, 0x9b, 0xde,
  //Subway 1 (fc 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xfc, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xfc, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 2 (fb 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xfb, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0x41, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xfb, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 3 (fa 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xfa, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x70, 0x42, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xfa, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 4 (f9 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf9, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xb4, 0x42, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf9, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 5 (f8 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf8, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0x42, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf8, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 6 (f7 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf7, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x16, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf7, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00};

 uint8_t SubWay2[] = {0x13, 0x71, 0x00, 0x9b, 0xde,
  //Subway 7 (f6 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf6, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x34, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf6, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 8 (f5 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf5, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x52, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf5, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 9 (f4 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf4, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x70, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf4, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 10 (f3 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf3, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x87, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf3, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 11 (f2 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf2, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x96, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf2, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00};

 if (source->GetChar()->GetLocation() != 1000)
  return;

 //SubWay List
 //Subway 1
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[7] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[25] = source->GetUDP_ID();

 //Subway 2
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[39] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[57] = source->GetUDP_ID();

 //Subway 3
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[71] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[89] = source->GetUDP_ID();

 //Subway 4
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[103] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[121] = source->GetUDP_ID();

 //Subway 5
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[135] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[153] = source->GetUDP_ID();

 //Subway 6
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[167] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay[185] = source->GetUDP_ID();

 //Subway 7
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[7] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[25] = source->GetUDP_ID();

 //Subway 8
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[39] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[57] = source->GetUDP_ID();

 //Subway 9
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[71] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[89] = source->GetUDP_ID();

 //Subway 10
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[103] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[121] = source->GetUDP_ID();

 //Subway 11
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[135] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(uint16_t*)&SubWay2[153] = source->GetUDP_ID();

 *(uint16_t*)&SubWay[1] = source->GetUDP_ID();
 *(uint16_t*)&SubWay[3] = source->GetSessionID();

 PMessage* msg;
 msg = new PMessage(197);
 msg->Write(SubWay, sizeof(SubWay));
Console->Print("---- Working 1 ----");
msg->Dump();
  source->SendUDPMessage(msg);
msg = MsgBuilder->BuildSubwaySpawnMsg(source, false);
Console->Print("---- Not Working 1 ----");
msg->Dump();
source->SendUDPMessage(msg);

 *(uint16_t*)&SubWay2[1] = source->GetUDP_ID();
 *(uint16_t*)&SubWay2[3] = source->GetSessionID();

 msg = new PMessage(197);
 msg->Write(SubWay2, sizeof(SubWay2));
Console->Print("---- Working 2 ----");
msg->Dump();
  source->SendUDPMessage(msg);
msg = MsgBuilder->BuildSubwaySpawnMsg(source, true);
Console->Print("---- Not Working 2 ----");
msg->Dump();
source->SendUDPMessage(msg);

  snprintf(tmpStr, 127, "Initial subway data sent");
  tmpStr[127] = '\0';
  Chat->send(source, CHAT_DIRECT, "System", tmpStr);
}
***/
