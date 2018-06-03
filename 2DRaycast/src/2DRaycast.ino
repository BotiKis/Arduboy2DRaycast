#include <Arduboy2.h>
#include "Tilesheet.h"
#include "MapTile.h"

Arduboy2 arduboy;

constexpr uint8_t MAPWIDTH  = 12;
constexpr uint8_t MAPHEIGHT = 8;

uint8_t mapData[] PROGMEM =
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
        uint8_t currentTileInBuffer = mapBuffer[mapData+x+y*MAPWIDTH];
      }
    }
}

void loop() {
    if(!arduboy.nextFrame()) continue;

    // move player
    arduboy.pollButtons();
    if (arduboy.justPressed(UP_BUTTON)){
      playerPos.y--;
    }
    if (arduboy.justPressed(DOWN_BUTTON)){
      playerPos.y++;
    }
    if (arduboy.justPressed(LEFT_BUTTON)){
      playerPos.x--;
    }
    if (arduboy.justPressed(RIGHT_BUTTON)){
      playerPos.x++;
    }

    // limit player pos
    playerPos.x = min(playerPos.x, MAPWIDTH-1);
    playerPos.x = max(playerPos.x, 0);
    playerPos.y = min(playerPos.y, MAPHEIGHT-1);
    playerPos.y = max(playerPos.y, 0);

    arduboy.clear();

    arduboy.display();
}
