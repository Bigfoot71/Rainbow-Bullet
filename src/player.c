#include "player.h"
#include "particles.h"
#include <raylib.h>
#include <stddef.h>
#include <rlgl.h>

/* BULLET */

typedef enum {
    BULLET_MOVING           = 0,
    BULLET_OUT_OF_SCREEN    = 1,
    BULLET_EXPLODE          = 2
} Bullet_Ret;

static Bullet Bullet_New(Vector2 position, Vector2 velocity, float radius, float speed, Color color)
{
    return (Bullet) {
        .position = position,
        .velocity = velocity,
        .radius = radius,
        .speed = speed,
        .hues = {
            GetRandomValue(0, 360),
            GetRandomValue(0, 360)
        }
    };
}

static int Bullet_Update(Bullet* bullet, Enemies* enemies, float dt)
{
    bullet->position = Vector2Add(bullet->position, Vector2Scale(bullet->velocity, bullet->speed * dt));

    if (bullet->position.y + bullet->radius < 0 || bullet->position.y - bullet->radius > GetScreenHeight())
    {
        return BULLET_OUT_OF_SCREEN;
    }
    else if (bullet->position.x - bullet->radius < 0)
    {
        bullet->position.x = bullet->radius;
        bullet->velocity = Vector2Reflect(bullet->velocity, (Vector2) { 1, 0 });
    }
    else if (bullet->position.x + bullet->radius > GetScreenWidth())
    {
        bullet->position.x = GetScreenWidth() - bullet->radius;
        bullet->velocity = Vector2Reflect(bullet->velocity, (Vector2) { -1, 0 });
    }

    for (uint8_t i = 0; i < enemies->count; i++)
    {
        Enemy *e = enemies->array + i;
        if (!e->alive) continue;

        float dx = e->position.x - bullet->position.x;
        float dy = e->position.y - bullet->position.y;

        float sqDist = dx*dx + dy*dy;
        float sqRadius = (e->radius + bullet->radius) * (e->radius + bullet->radius);

        if (sqDist <= sqRadius)
        {
            e->alive = false;
            return BULLET_EXPLODE;
        }
    }

    float colSpeed = 360.0f * dt;
    for (uint8_t i = 0; i < 2; i++)
    {
        bullet->hues[i] += colSpeed;
    }

    return BULLET_MOVING;
}

static void Bullet_Draw(const Bullet* bullet)
{
    int segments = 8;
    float stepLength = 360.0f/segments;
    float angle = 360.0f;

    Vector4 c1 = ColorNormalize(ColorFromHSV(bullet->hues[0], 1.0f, 1.0f));
    Vector4 c2 = ColorNormalize(ColorFromHSV(bullet->hues[1], 1.0f, 1.0f));

    rlSetTexture(rlGetTextureIdDefault());

    rlBegin(RL_QUADS);
        for (int i = 0; i < segments/2; i++)
        {
            rlColor4f(c1.x, c1.y, c1.z, c1.w);

            rlTexCoord2f(0, 0);
            rlVertex2f(bullet->position.x, bullet->position.y);

            rlColor4f(c2.x, c2.y, c2.z, c2.w);

            rlTexCoord2f(1.0f, 0);
            rlVertex2f(bullet->position.x + cosf(DEG2RAD*(angle + stepLength*2.0f))*bullet->radius, bullet->position.y + sinf(DEG2RAD*(angle + stepLength*2.0f))*bullet->radius);

            rlTexCoord2f(1.0f, 1.0f);
            rlVertex2f(bullet->position.x + cosf(DEG2RAD*(angle + stepLength))*bullet->radius, bullet->position.y + sinf(DEG2RAD*(angle + stepLength))*bullet->radius);

            rlTexCoord2f(0, 1.0f);
            rlVertex2f(bullet->position.x + cosf(DEG2RAD*angle)*bullet->radius, bullet->position.y + sinf(DEG2RAD*angle)*bullet->radius);

            angle += (stepLength*2.0f);
        }
    rlEnd();

    rlSetTexture(0);
}

/* BULLET ARRAY */

static Bullets Bullets_New(uint32_t size, float fireRate)
{
    Bullet *array = (Bullet*) MemAlloc(size * sizeof(Bullet));

    return (Bullets) {
        .array = array,
        .fireRate = fireRate,
        .timeCount = 0.f,
        .count = 0,
        .size = size
    };
}

static void Bullets_Delete(Bullets* bullets)
{
    if (bullets->array != NULL)
    {
        MemFree(bullets->array);
        bullets->array = NULL;
    }
}

static void Bullets_Add(Bullets* bullets, Vector2 position, Vector2 velocity, float radius, float speed, Color color)
{
    if (bullets->count < bullets->size && bullets->timeCount == 0)
    {
        bullets->array[bullets->count++] = Bullet_New(position, velocity, radius, speed, color);
        bullets->timeCount = 1.0f / bullets->fireRate;
    }
}

static void Bullets_Update(Bullets* bullets, Enemies* enemies, Particles* particles, int* score, float dt)
{
    if (bullets->timeCount > 0)
    {
        bullets->timeCount = fmaxf(bullets->timeCount - dt, 0);
    }

    Bullet* array = bullets->array;

    for (int i = bullets->count - 1; i >= 0; i--)
    {
        Bullet *b = array + i;

        switch (Bullet_Update(b, enemies, dt))
        {
            case BULLET_OUT_OF_SCREEN:
                *b = array[--bullets->count];
                break;

            case BULLET_EXPLODE:
                Particles_Emit(particles, b->position, (Vector2) { -200, -200 }, (Vector2) { 200, 200 }, b->radius * 0.5f, 1.0f, 8);
                *b = array[--bullets->count];
                *score += 10;
                break;

            default:
                break;
        }
    }
}

static void Bullets_Draw(const Bullets* bullets)
{
    for (uint32_t i = 0; i < bullets->count; i++)
    {
        Bullet_Draw(bullets->array + i);
    }
}

/* PLAYER */

Player Player_New(int x, int y)
{
    float baseHue = GetRandomValue(0, 360);

    return (Player) {
        .bullets = Bullets_New(32, 8),
        .particles = Particles_New(128),
        .position = (Vector2) { x, y },
        .rotation = -90.0f,
        .radius = 24.0f,
        .speed = 256.0f,
        .hues = {
            baseHue,
            baseHue + 90,
            baseHue + 180
        },
        .alive = true
    };
}

void Player_Reset(Player* player, int x, int y)
{
    player->position.x = x;
    player->position.y = y;
    player->radius = 24.0f;

    float baseHue = GetRandomValue(0, 360);
    for (int i = 0; i < 3; i++) player->hues[i] = baseHue + i * 90;

    player->alive = true;
    player->rotation = 90.0f;
    player->bullets.count = 0;
    player->particles.count = 0;
}

void Player_Delete(Player* player)
{
    Particles_Delete(&player->particles);
    Bullets_Delete(&player->bullets);
}

bool Player_Update(Player* player, Enemies* enemies, int* score, float dt)
{
    if (!player->alive)
    {
        if (player->radius > 0.1f)
        {
            player->radius *= 0.9f;
            goto end_update;
        }
        return false;
    }

    int8_t vx = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);

    player->position.x = Clamp(player->position.x + vx * player->speed * dt,
        player->radius, GetScreenWidth() - player->radius);

    if (vx) player->rotation = Clamp(player->rotation + vx * 180.0f * dt, -135.0f, -45.0f);
    else player->rotation += (-90.0f - player->rotation) * 0.2f;

    if (IsKeyDown(KEY_SPACE))
    {
        float angle = player->rotation * DEG2RAD;
        Vector2 dir = { cos(angle), sin(angle) };
        Vector2 pos = { player->position.x + dir.x * player->radius, player->position.y + dir.y * player->radius };
        Bullets_Add(&player->bullets, pos, dir, 4.0f, 512.0f, WHITE);
    }

    float colSpeed = 180.0f * dt;
    for (uint8_t i = 0; i < 3; i++)
    {
        player->hues[i] += colSpeed;
    }

    for (uint8_t i = 0; i < enemies->count; i++)
    {
        Enemy *e = enemies->array + i;
        if (!e->alive) continue;

        float dx = e->position.x - player->position.x;
        float dy = e->position.y - player->position.y;

        float sqDist = dx*dx + dy*dy;
        float sqRadius = (e->radius + player->radius) * (e->radius + player->radius);

        if (sqDist <= sqRadius)
        {
            Particles_Emit(&player->particles, player->position, (Vector2) { -200, -200 }, (Vector2) { 200, 200 }, player->radius * 0.1f, 0.75f, 32);
            player->alive = false;
        }
    }

  end_update:
    Bullets_Update(&player->bullets, enemies, &player->particles, score, dt);
    return true;
}

void Player_Draw(const Player* player)
{
#   define SIDES        3
#   define LINE_THICK   5

    Bullets_Draw(&player->bullets);

    Vector4 colors[3];
    for (uint8_t i = 0; i < 3; i++)
    {
        colors[i] = ColorNormalize(ColorFromHSV(player->hues[i], 1.0f, 1.0f));
    }

    DrawPoly(player->position, 3, player->radius, player->rotation, BLACK);

    float centralAngle = player->rotation*DEG2RAD;
    float exteriorAngle = 360.0f/(float)SIDES*DEG2RAD;
    float innerRadius = player->radius - (LINE_THICK*cosf(DEG2RAD*exteriorAngle/2.0f));

    rlSetTexture(rlGetTextureIdDefault());

    rlBegin(RL_QUADS);
        for (int i = 0; i < SIDES; i++)
        {
            Vector4 c1 = colors[i], c2 = colors[(i+1)%3];
            float nextAngle = centralAngle + exteriorAngle;

            rlColor4f(c1.x, c1.y, c1.z, c1.w);

            rlTexCoord2f(0, 1.0f);
            rlVertex2f(player->position.x + cosf(centralAngle)*player->radius, player->position.y + sinf(centralAngle)*player->radius);

            rlTexCoord2f(0, 0);
            rlVertex2f(player->position.x + cosf(centralAngle)*innerRadius, player->position.y + sinf(centralAngle)*innerRadius);

            rlColor4f(c2.x, c2.y, c2.z, c2.w);

            rlTexCoord2f(1.0f, 1.0f);
            rlVertex2f(player->position.x + cosf(nextAngle)*innerRadius, player->position.y + sinf(nextAngle)*innerRadius);

            rlTexCoord2f(1.0f, 0);
            rlVertex2f(player->position.x + cosf(nextAngle)*player->radius, player->position.y + sinf(nextAngle)*player->radius);

            centralAngle = nextAngle;
        }
    rlEnd();
    rlSetTexture(0);
}
