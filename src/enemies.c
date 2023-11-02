#include "./enemies.h"
#include <stdint.h>

Enemies Enemies_New(void)
{
    return (Enemies) {
        .count = 0,
        .maxTime = 0.25f,
        .timer = 0
    };
}

void Enemies_Update(Enemies* enemies, float dt)
{
    if (enemies->count < MAX_ENEMIES)
    {
        enemies->timer += dt;

        if (enemies->timer >= enemies->maxTime)
        {
            enemies->timer -= enemies->maxTime;

            float radius = GetRandomValue(12, 24);

            enemies->array[enemies->count++] = (Enemy) {
                .position = { GetRandomValue(radius, GetScreenWidth()-radius), -radius },
                .radius = radius,
                .speed = GetRandomValue(128, 512),
                .alive = true
            };
        }
    }

    for (int16_t i = enemies->count - 1; i >= 0; i--)
    {
        Enemy *e = enemies->array + i;

        if (!e->alive) e->radius *= 0.9f;
        else e->position.y += e->speed * dt;

        if (e->position.y - e->radius > GetScreenHeight() || e->radius < 0.1f)
        {
            *e = enemies->array[--enemies->count];
        }
    }
}

void Enemies_Draw(Enemies* enemies)
{
    for (uint8_t i = 0; i < enemies->count; i++)
    {
        Enemy *e = enemies->array + i;
        DrawPoly(e->position, 3, e->radius, 90.0f, BLACK);
        DrawPolyLinesEx(e->position, 3, e->radius, 90.0f, 3, (Color) { 255, 0, 0, 255 });
    }
}
