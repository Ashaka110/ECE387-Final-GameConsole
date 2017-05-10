#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// If your 32x32 matrix has the SINGLE HEADER input,
// use this pinout:
#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
// If your matrix has the DOUBLE HEADER input, use:
//#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
//#define LAT 9
//#define OE  10
//#define A   A3
//#define B   A2
//#define C   A1
//#define D   A0
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

uint8_t buttonState;
uint8_t buttonHeld;

uint8_t gameMode = 0;
uint8_t data[16*3];

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void setup() {
    matrix.begin();
    Serial.begin(9600);
}

void loop()
{
    updateInput();
    switch (gameMode){
        case 0:
          menuLoop();
          break;
        case 1:
          tetrisLoop();
          break;
        case 2:
          tttLoop();
          break;
        case 3:
          siLoop();
          break;
        case 4:
          pmLoop();
          break;
    }
    Serial.println(buttonState);
}

void startGame(uint8_t index)
{
    switch(index)
    {
        case 0:
          gameMode = 1;
          tetrisSetup();
          break;
        case 1:
          gameMode = 2;
          tttSetup();
          break;
        case 2:
          gameMode = 3;
          siSetup();
          break;
        case 3:
          gameMode = 4;
          pmSetup();
          break;
    }    
}

void updateInput()
{
   uint8_t keycode = getButtonData();
   if(!((buttonState >> keycode) & 1)){
       buttonState = 1 << keycode; 
       buttonHeld = 1 << keycode; 
   }else{
      buttonHeld = 1;
   }
}
uint8_t getButtonData()
{
  return (PINB>>3)&7;
}

bool isUpButtonPressed(){return buttonHeld>>1 & 1;}
bool isUpButtonDown(){return getButtonData() == 1;}
bool isDownButtonPressed(){return buttonHeld>>2 & 1;}
bool isDownButtonDown(){return getButtonData() == 2;}
bool isLeftButtonPressed(){return buttonHeld>>3 & 1;}
bool isLeftButtonDown(){return getButtonData() == 3;}
bool isRightButtonPressed(){return buttonHeld>>4 & 1;}
bool isRightButtonDown(){return getButtonData() == 4;}
bool isSelectButtonPressed(){return buttonHeld>>5 & 1;}
bool isSelectButtonDown(){return getButtonData() == 5;}

