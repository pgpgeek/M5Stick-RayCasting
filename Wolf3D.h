#ifndef __WORLD3D_H_

#define M5_BUTTON_MENU 35
#define M5_BUTTON_HOME 37
#define M5_BUTTON_RST 39

struct isObs{
  bool status;
  uint16_t color;
};

typedef struct{
  int x;
  int y;
  uint16_t color;
} obst;


struct hblock {
  int      distance;
  int      beginWall;
  int      beginGrass;
  uint16_t color; 
};


struct g_map{
  uint16_t W;
  uint16_t H;
  int directionDegree = 30;
  int maxDegreeLine   = 60;
  int maxDistanceLine = 20;
  int blockSize = 5;
  hblock historyBlocks[380];
  obst player = { x: 30, y: 3};
  int nbObstacles = 0;
  obst* obstacles;
  bool _3DInit = false;
};

obst mapObstacle[9] = {
  { x : 15, y : 15},
  { x : 30, y : 15},
  { x : 45, y : 15}, 
  { x : 15, y : 30},
  { x : 30, y : 30},
  { x : 45, y : 30},
  { x : 15, y : 45},
  { x : 30, y : 45},
  { x : 45, y : 45}
};


void draw3DLine();
void drawLine(uint16_t color);
obst getCoord(int x, int y, int dist, int degree);
isObs isObstacle(int x, int y);
void addObstacleMap();

#endif