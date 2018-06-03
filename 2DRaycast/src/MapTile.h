#ifndef MAPTILE_H
#define MAPTILE_H

#include <Arduboy2.h>

class MapTile{
public:
  uint8_t tileID:1;   // 0... Plains, 1... Obstacle
  uint8_t drawFog:1;  // 1... true, 0... false
  uint8_t containsPlayer:1;  // 1... true, 0... false
};

#endif
