#include <Arduboy2.h>
#include "Tilesheet.h"
#include "MapTile.h"

Arduboy2 arduboy;
Sprites sprites;

constexpr uint8_t MAPWIDTH  = 12;
constexpr uint8_t MAPHEIGHT = 8;
constexpr uint8_t TILESIZE = 8;

const uint8_t mapData[] PROGMEM =
{
  0,0,0,0,0,0,0,0,0,1,0,0,
  0,0,0,0,0,0,0,0,1,0,0,0,
  0,0,0,0,0,0,0,1,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,1,1,1,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,1,0,0,
  0,0,0,0,0,0,1,1,0,0,0,0
};

int8_t playerSightDistance = 2;
MapTile mapBuffer[MAPWIDTH*MAPHEIGHT];
Point playerPos = {5,5};

void drawMap();
void updateMap();
void clearMap();
void removeFogForPlayer();

void setup() {
    // put your setup code here, to run once:
    arduboy.boot();
    arduboy.setFrameRate(60);
    arduboy.initRandomSeed();

    // init map
    for (uint8_t y = 0; y < MAPHEIGHT; y++) {
      for (uint8_t x = 0; x < MAPWIDTH; x++) {
        // read value from mapdata
        uint8_t currentTileData = pgm_read_byte(mapData+x+y*MAPWIDTH);

        // populate mapbuffer
        MapTile currentTileInBuffer = mapBuffer[x+y*MAPWIDTH];
        currentTileInBuffer.tileID = currentTileData;
        currentTileInBuffer.containsPlayer = 0;
        currentTileInBuffer.drawFog = 1;
        mapBuffer[x+y*MAPWIDTH] = currentTileInBuffer;
      }
    }

    // set player start
    mapBuffer[playerPos.x+playerPos.y*MAPWIDTH].containsPlayer = 1;
}

void loop() {
    if(!arduboy.nextFrame()) return;

    Point tempPlayerPos = playerPos;

    // move player
    arduboy.pollButtons();
    if (arduboy.everyXFrames(8)) {
      if (arduboy.pressed(UP_BUTTON)){
        tempPlayerPos.y--;
      }
      if (arduboy.pressed(DOWN_BUTTON)){
        tempPlayerPos.y++;
      }
      if (arduboy.pressed(LEFT_BUTTON)){
        tempPlayerPos.x--;
      }
      if (arduboy.pressed(RIGHT_BUTTON)){
        tempPlayerPos.x++;
      }
      if (arduboy.pressed(A_BUTTON)){
        playerSightDistance--;
        playerSightDistance = max(playerSightDistance, 0);
        updateMap();
      }
      if (arduboy.pressed(B_BUTTON)){
        playerSightDistance++;
        playerSightDistance = min(playerSightDistance, 8);
        updateMap();
      }

      // limit player pos
      tempPlayerPos.x = min(tempPlayerPos.x, MAPWIDTH-1);
      tempPlayerPos.x = max(tempPlayerPos.x, 0);
      tempPlayerPos.y = min(tempPlayerPos.y, MAPHEIGHT-1);
      tempPlayerPos.y = max(tempPlayerPos.y, 0);

      // check if map needs to be updated
      if (tempPlayerPos.x != playerPos.x || tempPlayerPos.y != playerPos.y) {
        playerPos = tempPlayerPos;
        updateMap();
      }
    }

    // do drawing
    arduboy.clear();

    drawMap();

    arduboy.display();
}

void drawMap(){
  static const uint8_t xOffset = 16;
  for (uint8_t y = 0; y < MAPHEIGHT; y++) {
    for (uint8_t x = 0; x < MAPWIDTH; x++) {
      MapTile currentTileInBuffer = mapBuffer[x+y*MAPWIDTH];

      // draw tiles
      sprites.drawSelfMasked(x*TILESIZE+xOffset, y*TILESIZE, tilesheet, currentTileInBuffer.tileID);

      // draw fog
      if (currentTileInBuffer.drawFog == 1) {
        sprites.drawErase(x*TILESIZE+xOffset, y*TILESIZE, tilesheet, TileFog);
      }

      // draw player
      if (currentTileInBuffer.containsPlayer == 1) {
        sprites.drawOverwrite(x*TILESIZE+xOffset, y*TILESIZE, tilesheet, TilePlayer);
      }
    }
  }

}

void updateMap(){
  // clear map
  clearMap();

  // update players pos
  mapBuffer[playerPos.x+playerPos.y*MAPWIDTH].containsPlayer = 1;

  // handle sight
  removeFogForPlayer();
}

void clearMap(){
  for (uint8_t y = 0; y < MAPHEIGHT; y++) {
    for (uint8_t x = 0; x < MAPWIDTH; x++) {
      mapBuffer[x+y*MAPWIDTH].drawFog = 1;
      mapBuffer[x+y*MAPWIDTH].containsPlayer = 0;
    }
  }
}

void removeFogForPlayer(){
  auto castRay = [](int8_t xEnd, int8_t yEnd) {

    int8_t x0 = playerPos.x;
    int8_t y0 = playerPos.y;
    int8_t dx =  abs(xEnd-x0), sx = x0<xEnd ? 1 : -1;
    int8_t dy = -abs(yEnd-y0), sy = y0<yEnd ? 1 : -1;
    int8_t err = dx+dy, e2;

    while(true){

      e2 = 2*err;
      if (e2 >= dy) { err += dy; x0 += sx; }
      if (e2 <= dx) { err += dx; y0 += sy; }

      // check for bounds
      if (x0 >= 0 && y0 >= 0 && x0 < MAPWIDTH && y0 < MAPHEIGHT) {
        mapBuffer[x0+y0*MAPWIDTH].drawFog = 0;
      }

      // check for end
      if ( x0==xEnd && y0==yEnd) break;

      // check if there is an Obstacle
      if (mapBuffer[ x0+y0*MAPWIDTH].tileID == TileObstacle) break;
    }
  };

  // bresenham circle algorithm
  int8_t r = playerSightDistance;
  int8_t x = -r;
  int8_t y = 0;
  int8_t err = 2-2*r; /* II. Quadrant */
  do {

     castRay(playerPos.x-x, playerPos.y+y); // 1st quadrant
     castRay(playerPos.x-y, playerPos.y-x); // 2nd quadrant
     castRay(playerPos.x+x, playerPos.y-y); // 3rd quadrant
     castRay(playerPos.x+y, playerPos.y+x); // 4th quadrant

     // continue with bresenham
     r = err;
     if (r <= y) err += ++y*2+1;
     if (r > x || err > y) err += ++x*2+1;
  } while (x < 0);
}
