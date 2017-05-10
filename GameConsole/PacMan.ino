
short playerData;

short mazeData[16];

short dotData[8];

void pmSetup() {

  for(int x = 0; x < 32; x++){
      for(int y = 0; y < 32; y++){
          if(x%2 == 0 && y%2==0){
              matrix.drawPixel(x, y, matrix.Color333(0,0,1)); 
          }else if(x%2 == 0 || y%2==0){
              if(random(3) == 2)
              matrix.drawPixel(x, y, matrix.Color333(0,0,1)); 
             // mazeData[x] |= 1<<(y/2);
          }
         // playerData = 5;
         // dotData[5] = 8;
      }
  }
}

void pmLoop() {
    
}
