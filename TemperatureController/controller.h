#ifndef __CONTROLLER__
#define __CONTROLLER__

#include "includeall.h"


#define MAX_DISPLAY_VAL          99
#define CONVERT_C_TO_F( tc )     ( tc ) = ( ( tc ) * 9 / 5 ) + 32
#define CONVERT_F_TO_C( tf )     ( tf ) = ( ( tf ) - 32 ) * 5 / 9

typedef enum
{
   noChange = 0,
   changeDesiredTemp,
   changeTempRange,
   printSettings,
} ControllerState_e;

void Controller_Init( );
void Controller_StateMachine( );
void Controller_ChangeState( );
void Controller_SetCurrentTemp( uint8_t newTemp );
void Controller_SetDesiredTemp( uint8_t newTemp );
void Controller_SetTempRange( uint8_t newRange );
void Controller_ChangeDisplay( uint8_t value );

#endif //__CONTROLLER__
