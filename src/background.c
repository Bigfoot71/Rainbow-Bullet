#include "./background.h"
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>

/* STAR */

static Star Star_New()
{
    return (Star) {
        .position = (Vector2) { GetRandomValue(0, GetScreenWidth()), GetRandomValue(0, GetScreenHeight()) },
        .radius = GetRandomValue(100, 250) * 0.01f,
        .speed = GetRandomValue(8, 16),
        .phase = GetRandomValue(0, 31415) * 0.0001f,
        .intensity = GetRandomValue(50, 95) * 0.01f
    };
}

static void Star_Update(Star* star, float dtSpeed, float dt)
{
    star->position.y += star->speed * dtSpeed;

    if (star->position.y - star->radius > GetScreenHeight())
    {
        star->position.x = GetRandomValue(0, GetScreenWidth());
        star->position.y = -star->radius;
    }

    star->phase += 2.0f * dt;
}

static inline void Star_Draw(const Star* star)
{
    DrawCircleV(star->position, star->radius, ColorAlpha(WHITE, star->intensity * (cosf(star->phase) + 1.0f) * 0.25f + 0.25f));
}

/* BACKGROUND */

Background Background_New(uint32_t layerNum, uint32_t starsPerLayer)
{
    Star *stars = MemAlloc(layerNum * starsPerLayer * sizeof(Star));
    for (int i = starsPerLayer * layerNum - 1; i >= 0; i--) stars[i] = Star_New();
    return (Background) { stars, layerNum, starsPerLayer };
}

void Background_Delete(Background* bg)
{
    if (bg->stars != NULL)
    {
        MemFree(bg->stars);
        bg->stars = NULL;
    }
}

void Background_UpdateAndDraw(Background* bg, float dt)
{
    for (uint32_t i = 0; i < bg->layerNum; i++)
    {
        float speed = 16.0f / i * dt;

        for (uint32_t j = 0; j < bg->starsPerLayer; j++)
        {
            Star_Update(bg->stars + i * bg->starsPerLayer + j, speed, dt);
            Star_Draw(bg->stars + i * bg->starsPerLayer + j);
        }
    }
}
