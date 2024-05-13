#ifndef __WORLD3D_H_
#include <Arduino.h>
#include <M5Stack.h>
#include <M5GFX.h>

#define M5_BUTTON_MENU 35
#define M5_BUTTON_HOME 37
#define M5_BUTTON_RST 39


typedef struct {
  int r = 0;
  int g = 0;
  int b = 0;
} rgbColor;



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
  obst player = (obst){ x: 30, y: 5};
  int nbObstacles = 0;
  obst* obstacles;
  bool _3DInit = false;
};

obst mapObstacle[9] = {
  { x : 10, y : 10},
// { x : 25, y : 10},
  { x : 40, y : 10}, 
//  { x : 10, y : 25},
  { x : 25, y : 25},
// { x : 40, y : 25},
  { x : 10, y : 40},
//  { x : 25, y : 40},
  { x : 40, y : 40}
};




void draw3DLine();
void drawLine(uint16_t color);
obst getCoord(int x, int y, int dist, float degree);
isObs isObstacle(int x, int y);
void addObstacleMap();

#endif