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
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,0,0,0,0,0,0,0,0,0,0,
  0,0,1,0,0,0,0,1,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,1,1,1,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,1,0,0,0,0,0,0,1,0,0,
  0,0,1,0,0,1,1,1,0,0,0,0
};

constexpr uint8_t playerSightDistance = 6;
MapTile mapBuffer[MAPWIDTH*MAPHEIGHT];
Point playerPos = {5,5};

void drawMap();
void updateMap();
void clearMap();

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

  // remove fog for player
  // if i would just know how...
}

void clearMap(){
  for (uint8_t y = 0; y < MAPHEIGHT; y++) {
    for (uint8_t x = 0; x < MAPWIDTH; x++) {
      mapBuffer[x+y*MAPWIDTH].drawFog = 1;
      mapBuffer[x+y*MAPWIDTH].containsPlayer = 0;
    }
  }
}
