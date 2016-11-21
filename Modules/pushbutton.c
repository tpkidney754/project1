#include "pushbutton.h"

static Color_t color;
void Button_Init( uint8_t buttonNum )
{
   SET_BIT_IN_REG( SIM_SCGC5, SIM_SCGC5_PORTA_MASK );
   SET_BIT_IN_REG( BUTTON0, PORT_PCR_IRQC( IRQRising ) |
                            PORT_PCR_MUX( PIN_GPIO ) );
   CLEAR_BITS_IN_REG( GPIOC_PDOR, BUTTON0_PIN );
   CLEAR_BITS_IN_REG( GPIOC_PDDR, BUTTON0_PIN );
   NVIC_ClearPendingIRQ( PORTA_IRQn );
   NVIC_EnableIRQ( PORTA_IRQn );
   NVIC_SetPriority( PORTA_IRQn, 2 );
   color = RED;
}

void PORTA_IRQHandler( )
{
   NVIC_DisableIRQ( PORTA_IRQn );

   for( uint32_t i = 0; i < 1000000; i++ );
   if( BUTTON0 & PORT_PCR_ISF_MASK )
   {
      SET_BIT_IN_REG( BUTTON0, PORT_PCR_ISF_MASK );
      ChangeState( );
      /*SwitchLEDs( color++ );
      if( color == OFF )
      {
         color = RED;
      }*/
   }
   NVIC_EnableIRQ( PORTA_IRQn );
}
