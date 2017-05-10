#define cursorX 3  //pointers
#define cursorY 4

//uint8_t grid[3][3];
#define isXturn 5
#define hasPlayerWon 6

void tttSetup() {
   resetBoard();
}
void resetBoard(){ 
   for(int x = 0; x < 3; x++){
      data[x] = 0;
    } 
   data[hasPlayerWon] = 0;
   matrix.fillScreen(matrix.Color333(0, 0, 0));
   matrix.drawLine(10, 1, 10, 30, matrix.Color333(1, 1, 1));
   matrix.drawLine(20, 1, 20, 30, matrix.Color333(1, 1, 1));
   matrix.drawLine(1, 10, 30, 10, matrix.Color333(1, 1, 1));
   matrix.drawLine(1, 20, 30, 20, matrix.Color333(1, 1, 1));
   drawCursor();
  
 }
void tttLoop() {

  if(isLeftButtonPressed()){
      moveCursor(-1);
      if(data[hasPlayerWon])
      {
          resetBoard();    
      }
  }

  if(isSelectButtonPressed()){
      takeTurn();
  }

   if(isRightButtonPressed()){
      moveCursor(1);
      if(data[hasPlayerWon])
      {
          resetBoard();    
      }
  }

  
}


void moveCursor(short dx)
{    
    removeCursor();
    data[cursorX] += dx;
    
    data[cursorY] += data[cursorX] / 3;
    data[cursorY] += 3;
    data[cursorX] += 3;
    data[cursorY] %= 3;
    data[cursorX] %= 3;
    
    drawCursor();
}


void drawCursor()
{
     matrix.drawRect(1 + data[cursorX] *10, 1 + data[cursorY] * 10,  9,  9, matrix.Color333(1, 1, 0));
}    
 
void removeCursor()
{
     matrix.drawRect(1 + data[cursorX] *10, 1 + data[cursorY] * 10, 9, 9, matrix.Color333(0, 0, 0));
}    

void takeTurn(){
    if(getGrid(data[cursorX], data[cursorY]) == 0){
        if(data[isXturn]){
             matrix.drawLine(2 + data[cursorX] *10, 2 + data[cursorY] * 10, 2 + data[cursorX] *10 + 6, 2 + data[cursorY] * 10 + 6, matrix.Color333(1, 0, 0));
             matrix.drawLine(2 + data[cursorX] *10 + 6, 2 + data[cursorY] * 10, 2 + data[cursorX] *10, 2 + data[cursorY] * 10 + 6, matrix.Color333(1, 0, 0));
             data[isXturn] = 0; 
            setGrid(data[cursorX], data[cursorY], 1);

            
             
        }else{
            matrix.drawCircle(5 + data[cursorX] *10, 5 + data[cursorY] * 10, 3, matrix.Color333(0, 0, 1));
            data[isXturn] = 1;
            setGrid(data[cursorX], data[cursorY], 2);
        }
        
    }
    evaluateBoard();
}

void evaluateBoard(){
  for(int i=0; i<3; i++){
      //horisontal
          if(getGrid(i,0) == getGrid(i,1) && getGrid(i,0) == getGrid(i,2) && getGrid(i,0) != 0){
              //matrix.drawLine(2 + data[cursorX] *10, 2 + data[cursorY] * 10, 2 + data[cursorX] *10 + 6, 2 + data[cursorY] * 10 + 6, matrix.Color333(1, 0, 0));
               matrix.drawLine(10*i + 5, 5, 10*i+5, 25, matrix.Color333(0, 1, 1));
              data[hasPlayerWon] = getGrid(i,0);
          }
      //Vert
          if(getGrid(0,i) == getGrid(1,i) && getGrid(0,i) == getGrid(2,i) && getGrid(0,i) != 0){
                matrix.drawLine(5, 10*i+5, 25, 10*i+5, matrix.Color333(0, 1, 1));
            data[hasPlayerWon] = getGrid(0,i);
          }

  }
  //Diagonals
  if(getGrid(0,0) == getGrid(1,1) && getGrid(0,0) == getGrid(2,2) && getGrid(0,0) != 0){
      matrix.drawLine(5, 5, 25, 25, matrix.Color333(0, 1, 1));
    data[hasPlayerWon] = getGrid(0,0);
  }
  if(getGrid(0,2) == getGrid(1,1) && getGrid(0,2) == getGrid(2,0) && getGrid(0,2) != 0){
              
      matrix.drawLine(5, 25, 25, 5, matrix.Color333(0, 1, 1));
    data[hasPlayerWon] = getGrid(0,2);
  }

  short hasEmptySpace = 0;
  for(int x =0; x < 3; x++)
  {
      for(int y = 0; y < 3; y ++)
      {
          if(getGrid(x,y) == 0)
          {
            hasEmptySpace = 1;
          }
      } 
   }
   if(hasEmptySpace == 0 && data[hasPlayerWon] == 0)
   {
     matrix.setCursor(6, 2);   // start at top left, with one pixel of spacing
      matrix.setTextSize(4);    // size 1 == 8 pixels high
      matrix.setTextColor(matrix.Color333(0,1,1));
      matrix.print('C');
      data[hasPlayerWon] = 3;
   }
}

void setGrid(uint8_t x, uint8_t y, uint8_t val){
  data[x] |= val<<(y*2); 
}

uint8_t getGrid(uint8_t x, uint8_t y){
  return data[x]>>(y*2) & 3;
}

