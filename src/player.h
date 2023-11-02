#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

#include "enemies.h"
#include "particles.h"

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float speed;
    float hues[2];
} Bullet;

typedef struct {
    Bullet* array;
    float fireRate;
    float timeCount;
    uint32_t count;
    uint32_t size;
} Bullets;

typedef struct {
    Bullets bullets;
    Particles particles;
    Vector2 position;
    float rotation;
    float radius;
    float speed;
    float hues[3];
    bool alive;
} Player;

Player Player_New(int x, int y);
void Player_Reset(Player* player, int x, int y);
void Player_Delete(Player* player);

bool Player_Update(Player* player, Enemies* enemies, int* score, float dt);
void Player_Draw(const Player* player);

#endif