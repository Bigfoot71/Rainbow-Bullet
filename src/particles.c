#include "particles.h"

#include <stddef.h>
#include <raylib.h>
#include <raymath.h>

/* PARTICLE */

static bool Particle_Update(Particle* p, float dt)
{
    p->position.x += p->velocity.x * dt;
    p->position.y += p->velocity.y * dt;

    p->timer -= dt;

    p->color.a = Lerp(0, p->lifetime, p->timer) * 255;

    return p->timer > 0;
}

static inline void Particle_Draw(Particle* p)
{
    DrawCircleV(p->position, p->radius, p->color);
}

/* PARTICLES */

Particles Particles_New(uint16_t size)
{
    return (Particles) {
        .array = (Particle*)MemAlloc(size * sizeof(Particle)),
        .count = 0, .size = size,
    };
}

void Particles_Delete(Particles* particles)
{
    if (particles->array != NULL)
    {
        MemFree(particles->array);
        particles->array = NULL;
        particles->count = 0;
        particles->size = 0;
    }
}

void Particles_Emit(Particles* particles, Vector2 pos, Vector2 minVel, Vector2 maxVel, float radius, float lifetime, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++)
    {
        if (particles->count >= particles->size) break;

        Vector2 vel = {
            GetRandomValue(minVel.x, maxVel.x),
            GetRandomValue(minVel.y, maxVel.y)
        };

        particles->array[particles->count++] = (Particle) {
            pos, vel, radius, lifetime, lifetime,
            ColorFromHSV(GetRandomValue(0, 360), 1.0f, 1.0f)
        };
    }
}

void Particles_UpdateAndDraw(Particles* particles, float dt)
{
    BeginBlendMode(BLEND_ADDITIVE);

    for (int i = particles->count - 1; i >= 0; i--)
    {
        Particle *p = particles->array + i;

        if (!Particle_Update(p, dt))
        {
            *p = particles->array[--particles->count];
            continue;
        }

        Particle_Draw(p);
    }

    EndBlendMode();
}
