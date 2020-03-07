#include "gameserver/Includes.hxx"

void PCommands::doCmdweather()
{
  bool SyntaxError = false;
  if ( ArgC < 1 )
  {
    SyntaxError = true;
  }

  if ( SyntaxError == true )
  {
    Chat->send( source, CHAT_DIRECT, "Usage", "@weather ?|<weather id> [<target char id>]" );
    return;
  }

  int wId = 0;
  char tmp_v1[6];
  GetArgText( 1, tmp_v1, 6 );

  if ( tmp_v1[0] != '?' )
  {
    wId = atoi( tmp_v1 );
    if ( wId > 0 )
    {
      if ( ArgC >= 2 )
      {
        if ( !GetTarget( 2 ) )
        {
          Chat->send( source, CHAT_DIRECT, "System", "Invalid target character" );
          return;
        }
      }
      else
        target = source;

      PMessage* tmpMsg = MsgBuilder->BuildWeatherControlMsg(( uint16_t )wId );
      ClientManager->UDPBroadcast( tmpMsg, target );

      char effStr[128];
      snprintf( effStr, 127, "Weather set to value %d", wId );
      effStr[127] = '\0';
      Chat->send( source, CHAT_DIRECT, "System", effStr );

      return;
    }
    else
      Chat->send( source, CHAT_DIRECT, "System", "Invalid weather id" );
  }

  Chat->send( source, CHAT_DIRECT, "System", "Valid weather values:" );
  Chat->send( source, CHAT_DIRECT, "-", "1:Bright sky 2:Single cloud 3:Cloudy" );
  Chat->send( source, CHAT_DIRECT, "-", "4:Light rain 5:Heavy rain 6:Thunderstorm" );
  Chat->send( source, CHAT_DIRECT, "-", "100:Sea bright sky" );

}
