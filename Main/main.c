#include "includeall.h"

extern CircularBuffer_t * UART0_RXBuffer;
extern CircularBuffer_t * UART0_TXBuffer;
extern CircularBuffer_t * SPI_RXBuffer[ SPI_CHANNELS ];
extern CircularBuffer_t * SPI_TXBuffer[ SPI_CHANNELS ];

int main()
{
   Uart0Setup( 57600, 0 );
#ifdef FRDM
   LEDSetup();
   InitDisplay( 0 );
   InitDisplay( 1 );
   ADC_Init( ADC_CHANNEL );
   Button_Init( 0 );
   Controller_Init( );
   InitWaitTimer( );
   SWC_Init( );
   float temperature = 0;
   temperature = ReadTemp( );
   Controller_SetCurrentTemp( temperature );
   Message_t msg;
#endif

#ifdef TESTING
   Testing();
#endif

   uint8_t buffer[ 100 ];
   uint32_t length = 0;
while( 1 )
{
#ifdef FRDM

   Controller_StateMachine( );
   Controller_SetCurrentTemp( ReadTemp( ) );
   if( UART0_RXBuffer->numItems == 3 )
   {
      CBufferRemove( UART0_RXBuffer, &msg.cmd, DMACH_UART0RX  );
      CBufferRemove( UART0_RXBuffer, &msg.data, DMACH_UART0RX );
      CBufferRemove( UART0_RXBuffer, &msg.checksum, DMACH_UART0RX );

      DecodeRxMessage( &msg );

      parseDiag = 0;
   }

#else
   printf( "Enter command: " );
   fgets( buffer, 100, stdin );
   length = MyStrLen( buffer );
   ParseDiag( buffer );
   //UartTX( buffer, length );
   //UartRX( );

#endif
}

return 0;
}
