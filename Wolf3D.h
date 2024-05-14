#ifndef __WORLD3D_H_
#include <Arduino.h>
#include <M5Stack.h>
#include <M5GFX.h>

#define M5_BUTTON_MENU 35


struct rgbColor{
  int r;
  int g;
  int b;
};

typedef struct{
  int x;
  int y;
  uint16_t color;
  rgbColor rgb;
} obst;


struct isObs{
  bool status;
  obst infos;
};


struct hblock {
  int      distance;
  int      beginWall;
  int      beginGrass;
  uint16_t color; 
};


struct g_map{
  uint16_t W;
  uint16_t H;
  int mapW = 55;
  int mapH = 55;
  int directionDegree = 30;
  int maxDegreeLine   = 60;
  int maxDistanceLine = 20;
  int blockSize = 5;
  hblock historyBlocks[380];
  obst player = { x: 30, y: 5};
  int nbObstacles = 0;
  obst* obstacles;
  bool _3DInit = false;
};

obst mapObstacle[5] = {
  { x : 10, y : 10},
  { x : 40, y : 10}, 
  { x : 25, y : 25},
  { x : 10, y : 40},
  { x : 40, y : 40}
};


rgbColor __BLACK_RGB_COLOR__   = { r:0,   g:0,   b:0 };
rgbColor __WALL_RGB_COLOR__    = { r:128, g:128, b:128 };
rgbColor __SKY_RGB_COLOR__     = { r:65,  g:105, b:225 };
rgbColor __GRASS_RGB_COLOR__   = { r:85,  g:107, b:47 };

void draw3DLine();
void drawLine(uint16_t color);
obst getCoord(int x, int y, int dist, float degree);
isObs isObstacle(int x, int y);
void addObstacleMap();

#endif