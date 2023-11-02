#ifndef PARTICLES_H
#define PARTICLES_H

#include <stdint.h>
#include <raylib.h>

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float lifetime;
    float timer;
    Color color;
} Particle;

typedef struct {
    Particle *array;
    uint16_t count;
    uint16_t size;
} Particles;

Particles Particles_New(uint16_t size);
void Particles_Delete(Particles* particles);

void Particles_Emit(Particles* particles, Vector2 pos, Vector2 minVel, Vector2 maxVel, float radius, float lifetime, uint8_t count);
void Particles_UpdateAndDraw(Particles* particles, float dt);

#endif