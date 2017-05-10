#include <avr/pgmspace.h>

#define boardx 14
#define boardy 0

#define boardScale 2 //pixel size of one data square
#define boardSizeX 8
#define boardSizeY 16

#define pieceStartX 2
#define pieceStartY 0
#define Xoffset 3

#define piecePreviewX 2
#define piecePreviewY 22

#define startFallRate 30

const uint16_t colors[7] PROGMEM = {8192, 288,4,8196,8480,292,8768};//matrix.Color333(1, 0, 0), matrix.Color333(0, 1, 0), matrix.Color333(0, 0, 1), matrix.Color333(1, 0, 1), matrix.Color333(1, 1, 0), matrix.Color333(0, 1, 1), matrix.Color333(1, 2, 0)};  // index matches with pieces index
//char data[16*3];
//char datatest[19];
                          // 0,0,0,0    0,1,0,0   0,0,1,0   0,1,0,0   0,0,0,0   0,0,0,0   0,0,0,0
                          // 0,1,0,0    0,1,0,0   0,0,1,0   0,1,0,0   0,0,1,1   0,1,1,0   0,1,1,0
                          // 1,1,1,0    0,1,0,0   0,1,1,0   0,1,1,0   0,1,1,0   0,0,1,1   0,1,1,0
                          // 0,0,0,0    0,1,0,0   0,0,0,0   0,0,0,0   0,0,0,0   0,0,0,0   0,0,0,0

const uint16_t pieces[7] PROGMEM = {0x04e0, 0x4444,  0x2260,   0x4460,   0x0360,   0x0630,   0x0660}; // 7 4x4 arrays of bits   

//char piecePosX, piecePosY;
short currentPiece;
short nextPiece;
short pieceData;
//short nextData;
//char buttonState;
char fallRate = startFallRate;
//int Score = 18;
char color;

short dropTimer;

void tetrisSetup() {
  randomSeed(analogRead(0));
  initGame();
}

void initGame(){  
   resetGame();
}


void resetGame(){
     char i = random(7);
    nextPiece = pgm_read_word(&pieces[i]); //initializes nextPiece

    for(int x = 0; x<boardSizeY*3; x++){
        data[x] = 0;
    }
    newPiece();
}

void tetrisLoop() {

     matrix.drawRect(boardx-1, boardy-1, (boardSizeX + 1)  *boardScale, (boardSizeY+ 1)*boardScale, matrix.Color333(1, 1, 1));

    renderBoard();

    if(isLeftButtonPressed()){
        clearPieceImage();
        short testData = pieceData & 0xff0f | ((pieceData>>4 & 15)-1)<<4;
        if(testPosition(currentPiece, testData)){
            clearPieceImage();
            pieceData = testData;  
        }
       
    }

    if(isSelectButtonPressed()){
        clearPieceImage();
        short testData =  rotatePiece(currentPiece);
        if(testPosition(testData, pieceData))
          currentPiece = testData;
      
    }
    if(isRightButtonPressed()){
       clearPieceImage();
        short testData = pieceData & 0xff0f | ((pieceData>>4 & 15)+1)<<4;
        if(testPosition(currentPiece, testData)){
            clearPieceImage();
            pieceData = testData;  
        }
       
    }
    if(isDownButtonPressed()){ 
          onTickUpdate(); 
    }

    dropTimer++;
    if(dropTimer > fallRate){
      dropTimer = 0;
      onTickUpdate();  
    }

    matrix.setCursor(1, 1);   // next line
    matrix.setTextColor(matrix.Color333(0,1,1)); 
   // String sc = String(Score);
    char a = startFallRate - fallRate;
    char b = a%10 + 48;
    char c = (a/10)%10 + 48;
    matrix.print(c);
    matrix.print(b);
}

void renderBoard(){
  for(char x = 0; x<boardSizeX; x++){
    for(char y = 0; y < boardSizeY; y++){
      if(getdata(x,y) != 0){
        matrix.drawRect(x*boardScale + boardx, y*boardScale + boardy,boardScale,boardScale,pgm_read_word(&colors[getdata(x, y)-1])); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
      }else{
         //matrix.drawRect(x*boardScale + boardx, y*boardScale + boardy,boardScale,boardScale,matrix.Color333(0, 0, 0)); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
      }
    }
  }
  for(char x = 0; x<4; x++){
    for(char y = 0; y < 4; y++){
      if(getPiecePixel(x,y, currentPiece) != 0){
        matrix.drawRect((x+(pieceData>>4)&15)*boardScale + boardx - Xoffset*boardScale, (y+(pieceData)&15)*boardScale + boardy,boardScale,boardScale,pgm_read_word(&colors[(pieceData>>8)&7 ])); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
      }else{
        // matrix.drawRect((x+(pieceData>>4)&15)*boardScale + boardx, (y+(pieceData)&15)*boardScale + boardy,boardScale,boardScale,matrix.Color333(0, 0, 0)); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
      }
    }
  }
  
  matrix.drawRect(piecePreviewX -1, piecePreviewY - 1, boardScale*4 +2, boardScale*4 + 2, matrix.Color333(1,1,1));
  for(char x = 0; x<4; x++){
    for(char y = 0; y < 4; y++){
      if(getPiecePixel(x,y, nextPiece) != 0){
        matrix.drawRect((x)*boardScale + piecePreviewX, (y)*boardScale+piecePreviewY, boardScale, boardScale,pgm_read_word(&colors[(pieceData>>12)&7 ])); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
      }else{
        matrix.drawRect((x)*boardScale + piecePreviewX, (y)*boardScale+piecePreviewY, boardScale, boardScale,matrix.Color333(0, 0, 0)); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
     }
    }
  }
  
//   randNumber = random(300);
}

char getdata(char x, char y){
  return ((data[y*3]>>x)&1)  | ((data[y*3 +1]>>x)&1)<<1  | ((data[y*3+2]>>x)&1)<<2;
}

void setdata(char x, char y, char val)
{
   data[y*3]   = data[y*3] & ~(1<<x) |  (((val)&1)<<x);
   data[y*3+1] = data[y*3+1] & ~(1<<x) | (((val>>1)&1)<<x);
   data[y*3+2] = data[y*3+2] & ~(1<<x) |(((val>>2)&1)<<x);
}
uint16_t getPiece(uint8_t index){
  pgm_read_word(&pieces[index]);
}

void onTickUpdate()
{
   short testData = pieceData & 0xfff0 | ((pieceData & 15)+1);
   if(testPosition(currentPiece, testData)){
       clearPieceImage();
       pieceData = testData;  
   }else if(! (pieceData>>11 & 1)){
       addPieceTodata();
       newPiece();  
   }
}

void newPiece()
{
  char i = random(7);
  currentPiece = nextPiece;
    
   nextPiece = pgm_read_word(&pieces[i]);
   
   Serial.println("Piece: ");
   Serial.println(currentPiece);
   pieceData =  i<<12| (pieceData>>12 & 0x7)<<8  | (pieceStartX+Xoffset)<<4 | pieceStartY;
   
  if(!testPosition(currentPiece, pieceData))
  {
      pieceData |= 1<<11;
  }
}

char getPiecePixel(char x, char y, short piece){
  
  return (piece>>( (3-x) + 4*(3-y)))&1;
}

short rotatePiece(short a)
{
  short b = 0;
  
  for(char x = 0; x<4; x++){
    for(char y = 0; y < 4; y++){
      if(getPiecePixel(x,y, currentPiece) != 0){
        b |= 1<<(4*(3-x) + y);
      }
    }
  }
  
  return b;
}

char testPosition(short piece, short pieceData)
{
    char pieceX = (pieceData >>4) &15;
    char pieceY = pieceData & 15;
    
    for(char x = 0; x<4; x++){
      for(char y = 0; y < 4; y++){
        if ( getdata(x+pieceX - Xoffset,y+pieceY)!=0 && ((piece>>( (3-x) + 4*(3-y)))&1) !=0)
        {
          return 0;
        }
        if(((x+pieceX - Xoffset) < 0 || (x+pieceX - Xoffset) >= boardSizeX || (y+ pieceY) > boardSizeY -1)  && ((piece>>( (3-x) + 4*(3-y)))&1) != 0)
        {
          return 0;
        }
        
      }
    }
  return 1;
}
void clearPieceImage(){
  for(char x = 0; x<4; x++){
    for(char y = 0; y < 4; y++){
      //if(getPiecePixel(x,y) != 0){
       // matrix.drawRect((x+(pieceData>>4)&15)*boardScale + boardx, (y+(pieceData)&15)*boardScale + boardy,boardScale,boardScale,pgm_read_word(&colors[(pieceData>>8)&7 ])); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
     // }else{
         matrix.drawRect((x+(pieceData>>4)&15)*boardScale + boardx - Xoffset*boardScale, (y+(pieceData)&15)*boardScale + boardy,boardScale,boardScale,matrix.Color333(0, 0, 0)); //matrix.Color333(0, 0, 0));//colors[getdata(x,y)-1]); //pgm_read_word_near(colors + 2));//
      //}
    }
  }
 }

void addPieceTodata()
{
  char pieceX = (pieceData >>4) &15;
    char pieceY = pieceData & 15;
   
    for(char x = 0; x<4; x++){
      for(char y = 0; y < 4; y++){
        if(getPiecePixel(x,y, currentPiece) != 0){
          setdata(x+pieceX-Xoffset, y + pieceY, ((pieceData>>8) & 7) +1);
        }
      }
    }
   eliminateRows();
}

void eliminateRows(){
    for(char y = 0; y < boardSizeY; y++){
      char isRowFull = 1;
      for(char x = 0; x < boardSizeX; x++){
          if(getdata(x, y) == 0)
          {
              isRowFull = 0;
          }          
      }
      if(isRowFull)
      {
        for(char dy = y; dy > 0; dy--){
          for(char dx = 0; dx < boardSizeX; dx++){
           // setdata(1,1,5);
    
            setdata(dx, dy, getdata(dx, dy - 1));        
           // setdata(1,1,0);
          }
        }
        for(char dx = 0; dx < boardSizeX; dx++){
        //  setdata(dx, 0, 0);
        }
        fallRate--;
        matrix.fillScreen(matrix.Color333(0, 0, 0));
      }
  
    }
}



