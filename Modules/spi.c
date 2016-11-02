#ifdef FRDM
#include "spi.h"

CircularBuffer_t * SPI_RXBuffer[ SPI_CHANNELS ];
CircularBuffer_t * SPI_TXBuffer[ SPI_CHANNELS ];

extern uint8_t readRegComplete;

void InitSPI( uint8_t SPI_ch, uint8_t master )
{
   // start with SPRF and SPTEF flags and then move to DMA
   if( SPI_ch == 0 )
   {
      // Enable portc clock for I/O and the SPI0 clock.
      SET_BIT_IN_REG( SIM_SCGC5, SIM_SCGC5_PORTC_MASK );
      SET_BIT_IN_REG( SIM_SCGC4, SIM_SCGC4_SPI0_MASK );
      // Set pins to the needed functionality.
      SET_BIT_IN_REG( SPI0_MOSI, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      SET_BIT_IN_REG( SPI0_SCK, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      SET_BIT_IN_REG( SPI0_MISO, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      master ? SET_BIT_IN_REG( SPI0_CS, PORT_PCR_MUX( PIN_GPIO ) ) :
               SET_BIT_IN_REG( SPI0_CS, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      SET_BIT_IN_REG( SPI0_CE, PORT_PCR_MUX( PIN_GPIO ) );
      SET_BIT_IN_REG( SPI0_IRQ, PORT_PCR_MUX( PIN_GPIO ) );
      // Setting up RX interrupt and SPI enable
      //SET_BIT_IN_REG( SPI0_C1,SPI_C1_SPIE_MASK | SPI_C1_SPE_MASK );
      SET_BIT_IN_REG( SPI0_C1, SPI_C1_SPE_MASK );
      if( master )
      {
         SET_BIT_IN_REG( SPI0_C1, SPI_C1_MSTR_MASK );
         // When both the MODFEN and SSOE are clear the SS pin is set to be a GPIO.
         SET_BIT_IN_REG( GPIOC_PDDR, SPI0_CS_PIN );
         SET_BIT_IN_REG( GPIOC_PSOR, SPI0_CS_PIN );
         // Starting off with 1Mbps to reduce errors. Max for nRF24L01 is 2 Mbps
         SET_BIT_IN_REG( SPI0_BR, SPI_BR_SPPR( SPI_2Mbps_PRESCALER ) | SPI_BR_SPR( SPI_2Mbps_BRD ) );
      }

      // Enable CE as a GPIO
      //SET_BIT_IN_REG( GPIOC_PDDR, SPI0_CE_PIN );

      SPI_RXBuffer[ 0 ] = CBufferInit( sizeof( uint8_t ), SPI0_RXBUFFER_SIZE );
      SPI_TXBuffer[ 0 ] = CBufferInit( sizeof( uint8_t ), SPI0_TXBUFFER_SIZE );

      NVIC_EnableIRQ( SPI0_IRQn );
      NVIC_ClearPendingIRQ( SPI0_IRQn );
      NVIC_SetPriority( SPI0_IRQn, 2 );
   }
   else
   {
      // Enable portc clock for I/O and the SPI0 clock.
      SET_BIT_IN_REG( SIM_SCGC5, SIM_SCGC5_PORTE_MASK );
      SET_BIT_IN_REG( SIM_SCGC4, SIM_SCGC4_SPI1_MASK );
      // Set pins to the needed functionality.
      SET_BIT_IN_REG( SPI1_MOSI, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      SET_BIT_IN_REG( SPI1_SCK, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      SET_BIT_IN_REG( SPI1_MISO, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      master ? SET_BIT_IN_REG( SPI1_CS, PORT_PCR_MUX( PIN_GPIO ) ) :
               SET_BIT_IN_REG( SPI1_CS, PORT_PCR_MUX( ALTERNATIVE_2 ) );
      SET_BIT_IN_REG( SPI1_CE, PORT_PCR_MUX( PIN_GPIO ) );
      SET_BIT_IN_REG( SPI1_IRQ, PORT_PCR_MUX( PIN_GPIO ) );
      // Setting up RX interrupt and SPI enable
      SET_BIT_IN_REG( SPI1_C1, SPI_C1_SPIE_MASK | SPI_C1_SPE_MASK );

      if( master )
      {
         SET_BIT_IN_REG( SPI0_C1, SPI_C1_MSTR_MASK );
         // When both the MODFEN and SSOE are clear the SS pin is set to be a GPIO.
         SET_BIT_IN_REG( GPIOE_PDDR, SPI1_CS_PIN );
         SET_BIT_IN_REG( GPIOE_PSOR, SPI1_CS_PIN );
         // Starting off with 1Mbps to reduce errors. Max for nRF24L01 is 2 Mbps
         SET_BIT_IN_REG( SPI1_BR, SPI_BR_SPPR( SPI_2Mbps_PRESCALER ) | SPI_BR_SPR( SPI_2Mbps_BRD ) );
      }

      // CE is a nRF functionality, not SPI, this should move.
      // Enable CE as a GPIO
      //SET_BIT_IN_REG( GPIOE_PDDR, SPI0_CE_PIN );

      SPI_RXBuffer[ 1 ] = CBufferInit( sizeof( uint8_t ), SPI0_RXBUFFER_SIZE );
      SPI_TXBuffer[ 1 ] = CBufferInit( sizeof( uint8_t ), SPI0_TXBUFFER_SIZE );

      NVIC_EnableIRQ( SPI1_IRQn );
      NVIC_ClearPendingIRQ( SPI1_IRQn );
      NVIC_SetPriority( SPI1_IRQn, 2 );
   }
}

void SPI_TransmitData( uint8_t SPI_ch, size_t numBytes )
{
   SPI_Type * SPI_reg;
   uint8_t data;
   SPI_reg = SPI_ch == 0 ? SPI0 : SPI1;

   for( size_t i = 0; i < numBytes; i++ )
   {
      CBufferRemove( SPI_TXBuffer[ SPI_ch ], &data );
      WAIT_FOR_BIT_SET( SPI_S_REG( SPI_reg ) & SPI_S_SPTEF_MASK );
      SPI_D_REG( SPI_reg ) = data;
   }
}

void SPI0_IRQHandler( )
{
   NVIC_DisableIRQ( SPI0_IRQn );
   /*if( SPI0_S & SPI_S_SPRF_MASK )
   {
      // Data available in the RX data buffer
      uint8_t data = SPI0_D;
      CBufferAdd( SPI_RXBuffer[ 0 ], &data);
   }
   */
   if( ( SPI0_S & SPI_S_SPTEF_MASK ) && ( SPI0_C1 & SPI_C1_SPTIE_MASK ) )
   {
      uint8_t data;
      if( CBufferRemove( SPI_TXBuffer[ 0 ], &data ) == BUFFER_EMPTY )
      {
         CLEAR_BITS_IN_REG( SPI0_C1, SPI_C1_SPTIE_MASK );
         SET_BIT_IN_REG( GPIOC_PSOR, SPI0_CS_PIN );
      }
      else
      {
         SPI0_D = data;
      }
   }

   NVIC_EnableIRQ( SPI0_IRQn );
}

void SPI1_IRQHandler( )
{
   NVIC_DisableIRQ( SPI1_IRQn );
   if( SPI1_S & SPI_S_SPRF_MASK )
   {
      // Data available in the RX data buffer
      uint8_t data = SPI1_D;
      CBufferAdd( SPI_RXBuffer[ 1 ], &data);
   }

   if( ( SPI1_S & SPI_S_SPTEF_MASK ) && ( SPI1_C1 & SPI_C1_SPTIE_MASK ) )
   {
      uint8_t data;
      if( CBufferRemove( SPI_TXBuffer[ 1 ], &data ) == BUFFER_EMPTY )
      {
         CLEAR_BITS_IN_REG( SPI0_C1, SPI_C1_SPTIE_MASK );
         SET_BIT_IN_REG( GPIOE_PSOR, SPI1_CS_PIN );
      }
      else
      {
         SPI1_D = data;
      }
   }

   NVIC_EnableIRQ( SPI1_IRQn );
}
#endif // FRDM
