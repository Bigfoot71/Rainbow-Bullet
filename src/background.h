#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

typedef struct {
    Vector2 position;
    float radius;
    float speed;
    float phase;
    float intensity;
} Star;

typedef struct {
    Star *stars;
    uint32_t layerNum;
    uint32_t starsPerLayer;
} Background;

Background Background_New(uint32_t layerNum, uint32_t starsPerLayer);
void Background_Delete(Background* bg);

void Background_UpdateAndDraw(Background* bg, float dt);

#endif