#ifndef ENEMIES_H
#define ENEMIES_H

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

#define MAX_ENEMIES 32

typedef struct {
    Vector2 position;
    float radius;
    float speed;
    bool alive;
} Enemy;

typedef struct {
    Enemy array[MAX_ENEMIES];
    uint8_t count;
    float maxTime;
    float timer;
} Enemies;

Enemies Enemies_New(void);

void Enemies_Update(Enemies* enemies, float dt);
void Enemies_Draw(Enemies* enemies);

#endif