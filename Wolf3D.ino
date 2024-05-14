#include "Wolf3D.h"

M5GFX display;
g_map game;



void addObstacleMap(){
  bool corner = false;
  for (int i = 1; i <= game.mapH && game.nbObstacles<=7000; i++){
      for (int j = 0; j <= game.mapW; j++){
        if (i == 1 || i == game.mapH || j == 0 || j ==  game.mapW){
              corner = ! ( ((j == 0 || j == game.mapW)  &&  i % 5) || j % 5 );
              game.obstacles[game.nbObstacles] = { 
                x: j, 
                y: i, 
                color : corner ? TFT_DARKGREY : BLACK,
                rgb : corner == false ? __WALL_RGB_COLOR__ : __BLACK_RGB_COLOR__
               };
               game.nbObstacles++;
            }
    }
  }
  for (int _b = 0; _b<=4; _b++){
    for (int _h=0; _h<=game.blockSize; _h++){
      for (int _w=0; _w<=game.blockSize; _w++){
        corner = ((_h == 0  && _w == 0) || (_h == game.blockSize && _w == 0)  ||
                  (_h == 0  && _w == game.blockSize) ||  (_h == game.blockSize && _w == game.blockSize)); 
        game.obstacles[game.nbObstacles] = { 
          x: mapObstacle[_b].x + _w , 
          y: mapObstacle[_b].y + _h, 
          color : corner ? BLACK : TFT_DARKGREY,
          rgb : corner == false ? __WALL_RGB_COLOR__ : __BLACK_RGB_COLOR__
        };
        game.nbObstacles++;   
      } 
    }
  }
}


isObs isObstacle(int x, int y){
  obst  emptyObst;
  isObs res  = { status: false, infos:emptyObst };
  for (int i = 0; i<=game.nbObstacles; i++){
    if ((x == game.obstacles[i].x && y == game.obstacles[i].y) ||
        x < 0 || x > game.mapW || y <= 0 || y >= game.mapH ){
        res.status = true;
        res.infos = game.obstacles[i];
        return res;
    }
  }
  return res;
}

obst getCoord(int x, int y, int dist, float degree) {
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

uint16_t getColorFromDistance(rgbColor _rgbcolor, int distance, int maxDistance, int coef) {

  int r,g,b, ratio = ( distance) * coef / maxDistance;
  uint16_t _r, _g, _b;

  if ((_rgbcolor.r == 0 && _rgbcolor.b == 0 && _rgbcolor.g == 0) == true){
    r = _rgbcolor.r;
    g = _rgbcolor.g;
    b = _rgbcolor.b;
  }
  else {
    r = _rgbcolor.r - (_rgbcolor.r - ratio > 0 ? ratio : 0);
    g = _rgbcolor.g - (_rgbcolor.g - ratio > 0 ? ratio : 0);
    b = _rgbcolor.b - (_rgbcolor.b - ratio > 0 ? ratio : 0);
  }
  
  _r = ((r >> 3) & 0x1f) << 11;
  _g = ((g >> 2) & 0x3f) << 5;
  _b = (b >> 3)  & 0x1f;

    return (uint16_t) (_r | _g | _b);
};

void draw3DLine(){
  int nx, ny;
  obst tmpCoord;
  isObs res;
  int wallHeightPercent, wallHeightPixel, wallBeginPixel;
  int countRay    = 1; 
  int rayWidth    = game.W  / game.maxDegreeLine;
  int _angle, distance, _distance;


  for (int degree =  game.maxDegreeLine; degree >= 0 ; degree--){
    countRay++;
    wallHeightPixel = 0;
    wallBeginPixel  = game.H / 2;
    for (distance = 0; distance<=game.maxDistanceLine; distance++){
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
              res.infos.color = getColorFromDistance(res.infos.rgb, distance, game.maxDistanceLine, 100);              
              display.drawPixel((countRay * rayWidth) + w, h,  res.infos.color);
              // res.infos.color == BLACK ? BLACK : w % 3 == 1 && h % 2 == 0 || w % 3 == 0 && h % 2 == 1? TFT_LIGHTGREY: TFT_DARKGREY);
            }
          }
          break;
        }
     }
    if (game._3DInit == true && 
      game.historyBlocks[countRay].distance == _distance &&
      game.historyBlocks[countRay].color == res.infos.color)  {
      continue;
    }
    // SKY
    for (int h = 0; h <= wallBeginPixel; h++){
      for (int w = 0; w <= rayWidth; w++ )  {
        if (game._3DInit == true && h < game.historyBlocks[countRay].beginWall) 
          continue;
        display.drawPixel((countRay * rayWidth) + w, h, getColorFromDistance(__SKY_RGB_COLOR__, wallBeginPixel - h, wallBeginPixel, 50) );
      }
    }
    // GRASS
    for (int h = wallBeginPixel + wallHeightPixel; h <= game.H; h++){
      for (int w = 0; w < rayWidth;w++ )  {
        if (game._3DInit == true 
            && wallBeginPixel + wallHeightPixel >= game.historyBlocks[countRay].beginGrass 
            && h > game.historyBlocks[countRay].beginGrass) 
            continue;
        display.drawPixel((countRay * rayWidth) + w, h,  getColorFromDistance(__GRASS_RGB_COLOR__, game.H - h, game.H, 50));
      }
    }   
    game.historyBlocks[countRay] = { 
      distance : _distance,  
      beginWall  : wallBeginPixel,
      beginGrass : wallBeginPixel + wallHeightPixel,
      color : res.infos.color
    };
  }
  game._3DInit = true;
}



void setup()
{
  M5.begin();
  display.init();
  display.setRotation(1);
  game.obstacles = (obst*)malloc(3000 * sizeof(obst));
  pinMode(M5_BUTTON_MENU, INPUT);
  display.fillScreen(TFT_BLACK);
  game.W = display.width();
  game.H = display.height();
  addObstacleMap();
  Serial.println("Setup OK !");
  draw3DLine();
}



void loop()
{
  obst tmpCoord;
  isObs res;
  bool leftButton  = M5.BtnA.isPressed();
  bool rightButton = digitalRead(M5_BUTTON_MENU) == LOW;
  bool frontButton = M5.BtnC.isPressed();
  bool backButton  = leftButton && rightButton;
  bool userAction  = leftButton || rightButton || frontButton;

  M5.update();
  for (int i = 0; i<=game.nbObstacles; i++){
     display.drawPixel(game.obstacles[i].x, game.obstacles[i].y, TFT_RED);//game.obstacles[i].color);
  }
  if (userAction){
    display.drawPixel(game.player.x, game.player.y, BLACK);
    drawLine(BLACK);
    if (backButton) {
      tmpCoord =  getCoord(game.player.x, game.player.y, -2, game.directionDegree + ( game.maxDegreeLine / 2) );
      res = isObstacle(tmpCoord.x, tmpCoord.y);
      game.player.x = tmpCoord.x;
      game.player.y = tmpCoord.y;
    } 
    else if (frontButton) {
      tmpCoord =  getCoord(game.player.x, game.player.y, 2, game.directionDegree + ( game.maxDegreeLine / 2) );
      game.player.x = tmpCoord.x;
      game.player.y = tmpCoord.y;
    }
    else if (leftButton) {
      game.directionDegree=game.directionDegree+10;
    }
    else  if (rightButton) {
      game.directionDegree=game.directionDegree-10;
    }
    draw3DLine();
    display.drawPixel(game.player.x, game.player.y, RED);
    drawLine(GREEN);
   }
   delay(100);
}