#include <Arduino.h>
#include <M5Stack.h>
#include <M5GFX.h>
#include "Wolf3D.h"

M5GFX display;
g_map game;



void addObstacleMap(){
  bool corner = false;
  for (int i = 2; i<=game.H && game.nbObstacles<=7000; i++){
      for (int j = 0; j<=game.W; j++){
        if (i == 0 || i == game.H || j == 0 || j ==  game.W){
              game.nbObstacles++;
              game.obstacles[game.nbObstacles++] = { 
                x: j, 
                y: i, 
                color : (j % 5 && i % 2)  ||  ((j == 0 || j == game.W)  &&  i % 5) ? WHITE : BLACK };
            }
    }
  }
  for (int _b = 0; _b<=9; _b++){
    for (int _h=0; _h<=game.blockSize; _h++){
      for (int _w=0; _w<=game.blockSize; _w++){
        corner = ((_h == 0  && _w == 0) || (_h == game.blockSize && _w == 0)  ||
                  (_h == 0  && _w == game.blockSize) ||  (_h == game.blockSize && _w == game.blockSize)); 
        game.obstacles[game.nbObstacles++] = { 
          x: mapObstacle[_b].x + _w , 
          y: mapObstacle[_b].y + _h, 
          color : corner ? BLACK : WHITE };    
      } 
    }
  }
}


isObs isObstacle(int x, int y){
  isObs res  = { status: false, color:BLACK };
  for (int i = 0; i<=game.nbObstacles; i++){
    if ((x == game.obstacles[i].x && y == game.obstacles[i].y) ||
        x < 0 || x > game.W || y <= 0 || y >= game.H ){
        res.status = true;
        res.color = game.obstacles[i].color;
        return res;
    }
  }
  return res;
}

obst getCoord(int x, int y, int dist, int degree) {
  return (obst) {
      x : x + dist * cos(3.14 * degree / 180),
	    y : y + dist * sin(3.14 * degree / 180)
  };
}

void drawLine(uint16_t color){
  int nx, ny;
  obst tmpCoord;
  isObs res;

  for (int degree = game.directionDegree; degree <= game.directionDegree + game.maxDegreeLine; degree++){
    for (int distance = 0; distance<=game.maxDistanceLine; distance++){
        tmpCoord =  getCoord(game.player.x, game.player.y, distance, degree);
        res = isObstacle(tmpCoord.x, tmpCoord.y);
        if (res.status == true) {
          continue;
        }
        display.drawPixel(tmpCoord.x, tmpCoord.y, color);
     }    
  }
}

void draw3DLine(){
  int nx, ny;
  obst tmpCoord;
  isObs res;
  int wallHeightPercent, wallHeightPixel, wallBeginPixel;
  int countRay    = 1; 
  int rayWidth    = game.W  / game.maxDegreeLine;
  int _angle, _distance;

  for (int degree =  game.maxDegreeLine; degree >= 0 ; degree--){
    countRay++;
    wallHeightPixel = 0;
    wallBeginPixel  = game.H / 2;
    for (int distance = 0; distance<=game.maxDistanceLine; distance++){
        tmpCoord =  getCoord(game.player.x, game.player.y, distance, game.directionDegree + degree );
        res = isObstacle(tmpCoord.x, tmpCoord.y);
        _distance = distance;

        if (res.status == true) {
          _angle =  ((game.directionDegree + game.maxDistanceLine / 2) * 3.14 / 180) - (game.directionDegree + degree)  * 3.14 / 180;
					_distance = distance * cos(_angle);
          wallHeightPercent = ((game.maxDistanceLine - _distance) * 100) / game.maxDistanceLine;
          wallHeightPixel   = wallHeightPercent * game.H  / 100;
          wallBeginPixel    = (game.H -  wallHeightPixel) / 2;
          // WALL
          for (int h = wallBeginPixel;h <= wallBeginPixel + wallHeightPixel; h++){
            for (int w = 0; w <= rayWidth;w++ )  {
              display.drawPixel((countRay * rayWidth) + w, h,  
              res.color == BLACK ? BLACK : w % 3 == 1 && h % 2 == 0 || w % 3 == 0 && h % 2 == 1? TFT_LIGHTGREY: TFT_DARKGREY);
            }
          }
          break;
        }
     }
    if (game._3DInit == true && 
      game.historyBlocks[countRay].distance == _distance &&
      game.historyBlocks[countRay].color == res.color)  {
      continue;
    }
    // SKY
    for (int h = 0; h <= wallBeginPixel; h++){
      for (int w = 0; w <= rayWidth; w++ )  {
        if (game._3DInit == true && h < game.historyBlocks[countRay].beginWall) 
          continue;
        display.drawPixel((countRay * rayWidth) + w, h, BLACK );
      }
    }
    // GRASS
    for (int h = wallBeginPixel + wallHeightPixel; h <= game.H; h++){
      for (int w = 0; w < rayWidth;w++ )  {
        if (game._3DInit == true 
            && wallBeginPixel + wallHeightPixel >= game.historyBlocks[countRay].beginGrass 
            && h > game.historyBlocks[countRay].beginGrass) 
            continue;
        display.drawPixel((countRay * rayWidth) + w, h,  TFT_NAVY);
      }
    }   
    game.historyBlocks[countRay] = { 
      distance : _distance,  
      beginWall  : wallBeginPixel,
      beginGrass : wallBeginPixel + wallHeightPixel,
      color : res.color
    };
  }
  game._3DInit = true;
}



void setup()
{
  M5.begin();
  game.obstacles = (obst*)malloc(5000 * sizeof(obst));
  pinMode(M5_BUTTON_MENU, INPUT);
  display.begin();
  display.fillScreen(TFT_BLACK);
  game.W = display.width()  -1;
  game.H = display.height() -1;
  addObstacleMap();
}


void loop()
{
  obst tmpCoord;

  M5.update();
  for (int i = 0; i<=game.nbObstacles; i++){
     display.drawPixel(game.obstacles[i].x, game.obstacles[i].y, game.obstacles[i].color);
  }
  if (M5.BtnA.isPressed() || M5.BtnC.isPressed() || digitalRead(M5_BUTTON_MENU) == LOW){
    display.drawPixel(game.player.x, game.player.y, BLACK);
    drawLine(BLACK);
    if (M5.BtnA.isPressed()) {
        game.directionDegree=game.directionDegree-10;
    } 
    else if (M5.BtnC.isPressed()) {
      tmpCoord =  getCoord(game.player.x, game.player.y, 2, game.directionDegree + ( game.maxDegreeLine / 2) );
      game.player.x = tmpCoord.x;
      game.player.y = tmpCoord.y;
    } 
    else  if (digitalRead(M5_BUTTON_MENU) == LOW) {
      game.directionDegree=game.directionDegree+10;
      
    }
    draw3DLine();
    display.drawPixel(game.player.x, game.player.y, RED);
    drawLine(GREEN);

   }
    delay(100);
}