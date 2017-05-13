
short playerData;

#define playerX 20
#define playerY 21
#define playerDir 22


const uint8_t mazeData[16] PROGMEM = {0b00000001, 0b01101110,
                                      0b10000011, 0b00111000,
                                      0b11000111, 0b00011100,
                                      0b00000001, 0b10101011, 
                                      0b00001101, 0b00010000,
                                      0b11011111, 0b00100000,
                                      0b10000011, 0b01101100,
                                      0b00000001, 0b11111111};

short dotData[8];

void pmSetup() {

  for(int x = 0; x < 32; x++){
      for(int y = 0; y < 32; y++){
          if(x%2 == 0 && y%2==0){
            //  matrix.drawPixel(x, y, matrix.Color333(0,0,1)); 
          }else if(x%2 == 0 || y%2==0){
             // if(random(3) == 2)
            //  matrix.drawPixel(x, y, matrix.Color333(0,0,1)); 
             // mazeData[x] |= 1<<(y/2);
          }
         // playerData = 5;
         // dotData[5] = 8;
      }
  }
  reset();
}

void pmLoop() {
    delay(100);
    drawMaze();
    
    if(isUpButtonDown()){
      changeDirection(0);
    }if(isDownButtonDown()){
      changeDirection(2);
    }if(isLeftButtonDown()){
      changeDirection(3);
    }if(isRightButtonDown()){
      changeDirection(1);
    }

    updatePlayer();
}

void reset()
{
    for(int i =0; i < 8; i++)
    {
        data[i] = 0xFF;
    }
}
void changeDirection(uint8_t dir)
{
  if(isOnGrid(data[playerX], data[playerY])&&canMoveInDirection(data[playerX], data[playerY], dir))
    data[playerDir] = dir;
}
void updatePlayer()
{
  hidePacManPlayer();
  if(canMoveInDirection(data[playerX], data[playerY], data[playerDir]) || !isOnGrid(data[playerX], data[playerY]))
  {
       
      switch(data[playerDir])
      {
        case 0:
            data[playerY]--;
            break;
        case 1:
            data[playerX]++;
            break;
        case 2:
            data[playerY]++;
            break;
        case 3:
            data[playerX]--;
            break;
    
      }  
  }
  drawPacManPlayer();
  if(isOnGrid(data[playerX], data[playerY])){
     data[data[playerY]/4] &= ~(1<<(data[playerX]/4));
  }
}

void drawMaze()
{
    for(int x=0; x < 8; x++){
      for(int y=0; y < 8; y++)
      {
          if((pgm_read_word(&mazeData[y*2]) >> (7-x)) & 1)
          {
            matrix.drawLine(x*4+4, y*4, x*4+4, y*4+4 ,matrix.Color333(0,0,1));
          }
          if((pgm_read_word(&mazeData[y*2+1]) >> (7-x)) & 1)
          {
            matrix.drawLine(x*4, y*4+4, x*4+4, y*4+4 ,matrix.Color333(0,0,1));
          }

          if((data[y]>>x)&1)
          {
            matrix.drawPixel(x*4+2, y*4+2, matrix.Color333(1,1,0));
          }else{
            matrix.drawPixel(x*4+2, y*4+2, matrix.Color333(0,0,0));   
          }
      
      }
    }
    
}
void drawPacManPlayer()
{
    matrix.drawRect(data[playerX] + 1, data[playerY] + 1, 3, 3, matrix.Color333(1,1,0));
}
void hidePacManPlayer()
{
    matrix.drawRect(data[playerX] + 1, data[playerY] + 1, 3, 3, matrix.Color333(0,0,0));
}

boolean canMoveInDirection(uint8_t x, uint8_t y, uint8_t dir)
{
  if(x%4!=0 && y%4!=0)
  {
     return false;
  }
    
  switch(dir)
  {
    case 0:
        if(y==0) return false;
        return ((pgm_read_word(&mazeData[(y/4-1)*2+1]) >> (7-x/4)) & 1) == 0;
    case 1:
        return ((pgm_read_word(&mazeData[(y/4)*2]) >> (7-(x/4))) & 1) == 0;
    case 2:
        return ((pgm_read_word(&mazeData[y/4*2+1]) >> (7-x/4)) & 1) == 0;
    case 3:
        if(x==0) return false;
        return ((pgm_read_word(&mazeData[y/4*2]) >> (7-(x/4-1))) & 1) == 0;
  }
}
boolean isOnGrid(uint8_t x, uint8_t y)
{
  return x%4==0 && y%4==0;

}
