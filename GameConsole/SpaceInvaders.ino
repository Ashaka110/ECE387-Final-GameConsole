
#define MISSILECOUNT 5
//short playerMissiles[MISSILECOUNT];
#define playerMissiles 10
#define enemyMissiles 20

short getMissile(uint8_t arrayStart,uint8_t m){
  return  data[arrayStart + 2*m + 1]<<8 | data[arrayStart + 2*m];
}
short setMissile(uint8_t arrayStart, uint8_t m, short missiledata)
{
    data[arrayStart + 2*m + 1] =(uint8_t) (missiledata>>8) & 0xFF;
    data[arrayStart + 2*m] =(uint8_t) missiledata & 0xFF;
}

#define ENEMYMISSILECOUNT 5
//short enemyMissiles[ENEMYMISSILECOUNT];


#define ENEMYSPACING 5
#define enemyYoffset 56
#define enemyXoffset 48

int enemyX;
int enemyY;
uint8_t enemyData[3];

#define timeToRespawn 5;
uint8_t respawnTimer;
uint8_t wave;
//short playerData;
#define playerY 29


void siSetup() {

  wave = 0;
  incrementWave();
}

void incrementWave()
{
  enemyX = 0;
  enemyY = 0;
  
  clearMissiles();
  
  wave++;
  switch (wave){
      case 1:
          enemyData[0] =  0x55;
          enemyData[1] =  0x55;
          enemyData[2] =  0x55;
          break;
      case 2:
          enemyData[0] =  0xFF;
          enemyData[1] =  0xAA;
          enemyData[2] =  0x55;
          break;
      case 3:
          enemyData[0] =  0xFF;
          enemyData[1] =  0xFF;
          enemyData[2] =  0xFF;  
          break;
      default:
          enemyData[0] =  0x00;
          enemyData[1] =  0x00;
          enemyData[2] =  0x00;  
          break;
          
      }
  }


void siLoop() {
  // put your main code here, to run repeatedly:

 //     Serial.print("HI  ");
  //  Serial.println(freeRam() );
   delay(100);
    
    matrix.fillScreen(matrix.Color333(0,0,0));
    Serial.println((PINB>>3)&7);
    if(!(playerData>>8 & 1) ){
         uint8_t vert = analogRead(A4); 
        
         
        if(isLeftButtonDown()){//(PINB>>3)&7 == 3){//vert > 50  ){//digitalRead(11)){
            playerData = (playerData-1)&0x1F | (playerData & 0xFFE0);
        }
    
        else if (isRightButtonDown()){//(PINB>>3)&7 == 4){//vert < 40){// if(digitalRead(13)){
            playerData = (playerData+1)&0x1F | (playerData & 0xFFE0);
        }
        ///char shootValue = analogRead(A5);
        if(isSelectButtonPressed()){//(PINB>>3)&7 == 5){//!(analogRead(A5) < 50)){//digitalRead(12)){
              shoot();
        }
        
        if(enemyData[0] == 0 && enemyData[1] == 0 &&enemyData[2] == 0)
        {
            incrementWave();
        }
   
        updateMissiles();
        updateEnemies();
        enemyShoot();
    }else
    {
        if(respawnTimer > 0){
            respawnTimer--;
        }else{
            respawn();
        }
        
    }
    drawPlayer();
    drawMissiles();
    drawEnemies();
}

void updateMissiles()
{
    for(int i = 0; i < MISSILECOUNT; i++){
        if( ((getMissile(playerMissiles,i)>> 7) & 1))
        {
            setMissile(playerMissiles,i,
              ((((getMissile(playerMissiles,i)>>8) & 0x1F)-1)<<8) |
               getMissile(playerMissiles,i) & 0xE0FF);

            if(hitEnemies(getMissile(playerMissiles,i) & 0x1F ,getMissile(playerMissiles,i)>>8 & 0x1F))
            {
               setMissile(playerMissiles,i, 0); 
            }
                        
            if((getMissile(playerMissiles,i)>>8 & 0x1F) < 1){
                setMissile(playerMissiles,i,0); //remove Missile if goes off screen
            }
            for(int e = 0; e < ENEMYMISSILECOUNT; e++)
            {
                if( ((getMissile(enemyMissiles,e)>> 7) & 0x1))
                {
                    if( ((getMissile(enemyMissiles,e)>>8)&0x1F) == ((getMissile(playerMissiles,i)>>8)&0x1F) && ((getMissile(enemyMissiles,e))&0x1F) == ((getMissile(playerMissiles,i))&0x1F)  ){
                      setMissile(enemyMissiles,e, 0);
                      setMissile(playerMissiles,i, 0);
                    }
                }
            }
        }
    }
    for(int i = 0; i < ENEMYMISSILECOUNT; i++)
    {
        if( ((getMissile(enemyMissiles,i)>> 7) & 0x1))
        {
             setMissile(enemyMissiles,i,
              ((((getMissile(enemyMissiles,i)>>8) & 0x1F)+1)<<8) |
               getMissile(enemyMissiles,i) & 0xE0FF);

            if(  colidesWithPlayer(((getMissile(enemyMissiles,i)) & 0x1F) , ((getMissile(enemyMissiles,i)>>8) & 0x1F) ))
            {
                onPlayerDie();
                //playerData |= 1<<8;
            }
            
            if((getMissile(enemyMissiles,i)>>8 & 0x1F) > 30){
                setMissile(enemyMissiles,i, 0); //remove Missile if goes off screen
            }
            for(int e = 0; e < MISSILECOUNT; e++){
                if( ((getMissile(playerMissiles,e)>> 7) & 1))
                {
                    if( ((getMissile(enemyMissiles,i)>>8)&0x1F) == ((getMissile(playerMissiles,e)>>8)&0x1F) && ((getMissile(enemyMissiles,i))&0x1F) == ((getMissile(playerMissiles,e))&0x1F)  ){
                        setMissile(enemyMissiles,i, 0);
                        setMissile(playerMissiles,e, 0);
                    }
                }
            }
        }
    }
}

void shoot()
{
    for(int i = 0; i < MISSILECOUNT; i++){
        if( !(getMissile(playerMissiles,i)>> 7 & 1))
        {
            setMissile(playerMissiles,i,  ((playerY-1) << 8) | (1<<7) | ((playerData & 0x1F)+1));
            return;
        }
    }
}

void enemyShoot()
{
    //for(int i = 0; i < ENEMYMISSILECOUNT; i++){
        int i = random(ENEMYMISSILECOUNT);
        if( !(getMissile(enemyMissiles,i)>> 7 & 1))
        {
            int col = random(4);
            char yOffset = 0;
            if(enemyData[2]>>(col*2) & 0x3)
            { 
                yOffset = 12;
            }else if(enemyData[1]>>(col*2) & 0x3){
                yOffset = 7;
            }else if(enemyData[0]>>(col*2) & 0x3){
                yOffset = 2;
            }
            if(yOffset != 0)
            setMissile(enemyMissiles,i,  ((enemyY + yOffset) << 8) | (1<<7) | ((enemyX + col*5 + 1)));
            return;
        }
   // }
}

void drawPlayer()
{
    matrix.drawPixel( (playerData & 0x1F) +1, playerY, matrix.Color333(1,1,1));
    matrix.drawPixel( (playerData & 0x1F) , playerY+1, matrix.Color333(1,1,1));
    matrix.drawPixel( (playerData & 0x1F) +1, playerY+1, matrix.Color333(1,1,1));
    matrix.drawPixel( (playerData & 0x1F) +2, playerY+1, matrix.Color333(1,1,1));
}

void drawMissiles(){
    for(int i = 0; i < MISSILECOUNT; i++)
    {
        if( ((getMissile(playerMissiles,i)>> 7) & 0x1))
        {
            drawMissile(getMissile(playerMissiles,i));
        }
    }for(int i = 0; i < ENEMYMISSILECOUNT; i++)
    {
        if( ((getMissile(enemyMissiles,i)>> 7) & 0x1))
        {
            drawMissile(getMissile(enemyMissiles,i));
        }
    }
}

void drawMissile(short missileData){
    matrix.drawPixel( missileData & 0x1F, ((missileData>>8) &0x1F), matrix.Color333(1,1,1));
    matrix.drawPixel( missileData & 0x1F, ((missileData>>8) &0x1F)+1, matrix.Color333(1,1,1));

    
   // Serial.println((missileData>>8)) ;
}

bool colidesWithPlayer(char x, char y)
{
    return (y == playerY && ((playerData & 0x1F)) <= x && ((playerData & 0x1F))+2 >= x) ;   
}

bool hitEnemies(char x, char y)
{ 
    char xGrid = (x - enemyX)/ENEMYSPACING;
    char yGrid = (y - enemyY)/ENEMYSPACING;
    
    if(xGrid >= 4 || yGrid >=3  || xGrid < 0 || yGrid < 0 || x < enemyX || y < enemyY ){
        return false;
    }
    if((x - enemyX)%ENEMYSPACING > 2 || (y - enemyY)%ENEMYSPACING > 2){
      return false;  
    }
    
  
    int hitOffset = 2*xGrid;

    Serial.println(hitOffset, DEC );
    
    
    if((enemyData[yGrid]>>hitOffset & 0x3))
    {
      Serial.println(enemyData[yGrid], HEX );
      enemyData[yGrid] = (((((enemyData[yGrid]>>hitOffset & 0x3)-1)& 0x3)<<hitOffset) ) | (enemyData[yGrid] & ~(0x03<<hitOffset));
     // Serial.println((enemyData[yGrid] & ~(0x03<<hitOffset)), HEX );
    //  Serial.println(~(0x03<<hitOffset), HEX );
      Serial.println(enemyData[yGrid], HEX );
    
        return true;
    }
    return false;
}
void updateEnemies()
{
    if(!enemyData[0] &&!enemyData[1] &&! enemyData[2])
    {
        return;
    }

    char leftBoundry = 0;
    char rightBoundry = 14;

    if(enemyY%2 == 0){
        for(int i = 3; i >= 0; i-- )
        {
            if(enemyData[2]>>(i*2)&0x3 || enemyData[1]>>(i*2)&0x3 || enemyData[0]>>(i*2)&0x3)
            {
                break;
            }else{
                rightBoundry+=5;
            }
        }  
    }else if(enemyY%2 == 1){
        for(int i = 0; i <= 3; i++ )
        {
            if(enemyData[2]>>(i*2)&0x3 || enemyData[1]>>(i*2)&0x3 || enemyData[0]>>(i*2)&0x3)
            {
                break;
            }else{
                leftBoundry-=5;
            }
        }  
    }
    
    
    if(enemyX < rightBoundry && enemyY%2 == 0){
      enemyX++;
    //  enemyX &= 0x1F;
    }else if(enemyX > leftBoundry && enemyY%2 == 1){
      enemyX--;
    //  enemyX &= 0x1F;
    }else{
      enemyY++;
      enemyY &= 0x1F;
    }
  
}

void drawEnemies()
{
    for(int x =0; x < 4; x++)
    {
        for(int y = 0; y < 3; y++)
        {

         // if(hitEnemies(x, y)){
            // matrix.drawPixel( x ,     y, matrix.Color333(1,0,1));
        
          //  }
          if(enemyData[y]>>(2*(x)) & 0x3){
            uint16_t color = matrix.Color333(1,1,1);
            switch(enemyData[y]>>(2*(x)) & 0x3){
                case 1:
                  color = matrix.Color333(0,0,1);
                  break;
                case 2:
                  color = matrix.Color333(0,1,1);
                  break;
                case 3:
                  color = matrix.Color333(1,0,1);
                  break;
                     
              }
            
            matrix.drawPixel( x * ENEMYSPACING + enemyX,     y * ENEMYSPACING + enemyY, color);
            matrix.drawPixel( x * ENEMYSPACING + 2 + enemyX, y * ENEMYSPACING + enemyY, color);
            matrix.drawPixel( x * ENEMYSPACING + enemyX,     y * ENEMYSPACING +2 + enemyY, color);
            matrix.drawPixel( x * ENEMYSPACING + 2 + enemyX,  y * ENEMYSPACING +2 + enemyY, color);
            matrix.drawPixel( x * ENEMYSPACING + 1 + enemyX,  y * ENEMYSPACING +1 + enemyY, color);
          }
        }
    }
}
void onPlayerDie()
{
  playerData |= 1<<8;
  respawnTimer = timeToRespawn;
}

void respawn()
{
    enemyX=0;
    enemyY=0;

    clearMissiles();
    
    playerData &= ~(1<<8);
}

void clearMissiles()
{
   for(int i = 0; i < ENEMYMISSILECOUNT; i++){
        setMissile(enemyMissiles,i, 0);
    }
    for(int i = 0; i < MISSILECOUNT; i++){
        setMissile(playerMissiles,i, 0);
    }
}
