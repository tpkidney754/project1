#ifdef FRDM
#include "diags.h"

//*************************************************************************
// Function:  ParseDiag                                                   *
//                                                                        *
// Description: Receives a string signifying a diag command from          *
//              the serial port.                                          *
//                                                                        *
// Parameters: uint8_t * buffer: pointer to a string buffer.              *
//                                                                        *
// Return Value:  NONE                                                    *
//*************************************************************************
void ParseDiag( uint8_t * buffer )
{
   if( strstr( buffer, "color" ) )
   {
      Color_t color = NONE;
      if( strstr( buffer, "red" ) )
      {
         color = RED;
      }
      else if( strstr( buffer, "green" ) )
      {
         color = GREEN;
      }
      else if( strstr( buffer, "blue" ) )
      {
         color = BLUE;
      }
      else if( strstr( buffer, "purple" ) )
      {
         color = PURPLE;
      }
      else if( strstr( buffer, "yellow" ) )
      {
         color = YELLOW;
      }
      else if( strstr( buffer, "cyan") )
      {
         color = CYAN;
      }
      else if( strstr( buffer, "white" ) )
      {
         color = WHITE;
      }
      else if( strstr( buffer, "off" ) )
      {
         color = OFF;
      }

      SwitchLEDs( color );
      BuildMessage( changeColor, 1, ( uint8_t * )&color );
   }
   else if ( strstr( buffer, "power" ) )
   {
      uint16_t pulseWidth = 0;
      sscanf( buffer, "set power %d", &pulseWidth );
      ChangeLEDPW( pulseWidth );
      BuildMessage( changePWM, 1, ( uint8_t * )&pulseWidth );
   }
   else if( strstr( buffer, "cycle" ) )
   {
      BuildMessage( cycleLEDs, 0, ( uint8_t * ) 0xFFFFFFFF );
      CycleLEDs( );
   }
}

#endif
