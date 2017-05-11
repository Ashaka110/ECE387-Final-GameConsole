                                                       /* Blinker Demo */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */

#define SPI_SS                     PB2
#define SPI_SS_PORT                PORTB
#define SPI_SS_PIN                 PINB
#define SPI_SS_DDR                 DDRB

#define SPI_MOSI                     PB3
#define SPI_MOSI_PORT                PORTB
#define SPI_MOSI_PIN                 PINB
#define SPI_MOSI_DDR                 DDRB

#define SPI_MISO                     PB4
#define SPI_MISO_PORT                PORTB
#define SPI_MISO_PIN                 PINB
#define SPI_MISO_DDR                 DDRB

#define SPI_SCK                     PB5
#define SPI_SCK_PORT                PORTB
#define SPI_SCK_PIN                 PINB
#define SPI_SCK_DDR                 DDRB


#define ACCEL_REG_OUT 0x28
#define SLAVE_SELECT    SPI_SS_PORT &= ~(1<<SPI_SS)
#define SLAVE_DESELECT  SPI_SS_PORT |= (1<<SPI_SS)

#define SETPORTD PORTD = (PORTD & 0xF0) | 

uint16_t SPI_getReg(uint8_t reg);
void setupAccel(uint8_t rangeVal);
void getAccel(uint16_t* x, uint16_t* y, uint16_t* z);
void initSPI(void);
void SPI_tradeByte(uint8_t byte);

int main(void) {

  // -------- Inits --------- //
  DDRB |= 0b11000001;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */
  DDRD |= 0b00001111;
  initSPI();
  setupAccel(0);
  int outputMode = 1;
  
  // ------ Event loop ------ //
  while (1) {
    if(PINC>>5&1){
      while (PINC>>5 & 1);
      outputMode += 1;
      outputMode %=2;
    }

    if(outputMode){
        if(PINC>>4&1){
           SETPORTD 5;
        }else if(PINB>>1 & 0x1){
           SETPORTD 1;                                          /* wait */
        }else if(PIND>>7 & 1){
           SETPORTD 2; 
        }else if(PIND>>6 & 1){
           SETPORTD 3; 
        }else if(PIND>>5 & 1){
           SETPORTD 4;  
        }else{
            SETPORTD 0;
        }
    }else{
        uint16_t x, y, z;
        getAccel(&x, &y, &z);
      
        if(PINC>>4&1)
        {
           
          SETPORTD 5;  
        } else if( y > 0x8000 && y < 0xF000)
        {
            SETPORTD 3; 
        }else if( y > 0x1000 && y < 0x8000)
        {
            SETPORTD 4; 
        }else if(x > 0xF000)
        {
            SETPORTD 0; 
        } else if( x > 0x8000)
        {
            SETPORTD 2; 
        }else if( x > 0x1000)
        {
            SETPORTD 1; 
        }else 
        {
            SETPORTD 0; 
        }
    }

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}

void getAccel(uint16_t* x, uint16_t* y, uint16_t* z){
  *x = SPI_getReg(ACCEL_REG_OUT);
  *y = SPI_getReg(ACCEL_REG_OUT+2);
  *z = SPI_getReg(ACCEL_REG_OUT+4);
  
}

uint16_t SPI_getReg(uint8_t reg){
   uint16_t val;
   SLAVE_SELECT;
   SPI_tradeByte(0x80 | (reg+1));
   SPI_tradeByte(0x80 );            //read MSB
    
   val = SPDR << 8;

   SPI_tradeByte(0x80 | reg);
   SPI_tradeByte(0x80 );            // read LSB
   SLAVE_DESELECT;
    
   val |= SPDR;

   return val;
}

void SPI_tradeByte(uint8_t byte){
  SPDR = byte;
  loop_until_bit_is_set(SPSR, SPIF);
}

void initSPI(void) {
  SPI_SS_DDR |= (1 << SPI_SS);                        /* set SS output */
  SPI_SS_PORT |= (1 << SPI_SS);       /* start off not selected (high) */

  SPI_MOSI_DDR |= (1 << SPI_MOSI);                   /* output on MOSI */
  SPI_MISO_PORT |= (1 << SPI_MISO);                  /* pullup on MISO */
  SPI_SCK_DDR |= (1 << SPI_SCK);                      /* output on SCK */

  /* Don't have to set phase, polarity b/c
   * default works with 25LCxxx chips */
  SPCR |= (1 << SPR1);                /* div 16, safer for breadboards */
  SPCR |= (1 << MSTR);                                  /* clockmaster */
  SPCR |= (1 << SPE);                                        /* enable */
}

void setupAccel(uint8_t rangeVal)
{
    SLAVE_SELECT;
    
    //reset Chip
    SPI_tradeByte(0x22);
    SPI_tradeByte(0x05);

    
    SLAVE_DESELECT;

    _delay_ms(10);  //Wait for reset
    
    SLAVE_SELECT;
    
    SPI_tradeByte(0x1F);
    SPI_tradeByte(0x38); //enable accelorometer
    
    
    SPI_tradeByte(0x20);
    SPI_tradeByte(0xC0);

    
    
    SPI_tradeByte(0x80 | 0x20);
    SPI_tradeByte(0x80);
    uint8_t regVal = SPDR;
    regVal &= ~(0b00011000);
    regVal |= rangeVal; 

    SPI_tradeByte(0x20);
    SPI_tradeByte(regVal); //set accel procision


    
    
    SLAVE_DESELECT;
}


