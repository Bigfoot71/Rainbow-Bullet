#include <raylib.h>
#include "./title.h"
#include "./player.h"
#include "./enemies.h"
#include "./background.h"
#include "./particles.h"

#ifdef PLATFORM_WEB
#   include <emscripten/emscripten.h>
#endif

static int score;
static int highScore;
static int displayedScore;
static float timerIncScore;
static bool inGame;

static Title title;
static Background bg;
static Player player;
static Enemies enemies;

static void UpdateAndDraw(void);

int main(void)
{
    InitWindow(800, 450, "Rainbow Bullet");

    score = 0;
    highScore = 0;
    displayedScore = 0;
    timerIncScore = 0;
    inGame = false;

    title = Title_Load();
    bg = Background_New(8, 24);
    player = Player_New(400, 400);
    enemies = Enemies_New();

#   ifdef PLATFORM_WEB
        emscripten_set_main_loop(UpdateAndDraw, 60, 1);
#   else
        SetTargetFPS(60);
        while (!WindowShouldClose())
        {
            UpdateAndDraw();
        }
#   endif

    Player_Delete(&player);
    Background_Delete(&bg);
    Title_Unload(&title);

    CloseWindow();

    return 0;
}

#ifdef PLATFORM_WEB
EMSCRIPTEN_KEEPALIVE void Pause(void) { emscripten_pause_main_loop(); }
EMSCRIPTEN_KEEPALIVE void Resume(void) { emscripten_resume_main_loop(); }
#endif

static void UpdateAndDraw(void)
{
    float dt = GetFrameTime();

    if (inGame)
    {
        Enemies_Update(&enemies, dt);

        if (!Player_Update(&player, &enemies, &score, dt))
        {
            Player_Reset(&player, 400, 400);
            if (score > highScore) highScore = score;
            score = 0, displayedScore = 0;
            inGame = false;
        }

        if (score > displayedScore)
        {
            if ((timerIncScore += dt) > 0.032f)
            {
                timerIncScore -= 0.032f;
                displayedScore++;
            }
        }
    }
    else
    {
        Title_Update(&title, dt);
        if (IsKeyPressed(KEY_SPACE)) inGame = true;
    }

    BeginDrawing();

    ClearBackground(BLACK);
    Background_UpdateAndDraw(&bg, dt);

    if (inGame)
    {
        Player_Draw(&player);
        Enemies_Draw(&enemies);
        Particles_UpdateAndDraw(&player.particles, dt);
        DrawText(TextFormat("SCORE: %i", displayedScore), 10, 10, 22, LIME);
    }
    else
    {
#   ifdef PLATFORM_DESKTOP
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color) { 0, 0, 0, 127 });
#   endif
        Title_Draw(&title);

        const char* startMessage = "Press SPACE to start!";
        float startMessageWidth = MeasureText(startMessage, 24);
        DrawText(startMessage, (GetScreenWidth() - startMessageWidth) / 2, GetScreenHeight() / 1.25f, 24, WHITE);

        if (highScore > 0)
        {
            const char* highScoreMessage = TextFormat("HIGH SCORE: %i", highScore);
            float highScoreMessageWidth = MeasureText(highScoreMessage, 20);
            DrawText(highScoreMessage, (GetScreenWidth() - highScoreMessageWidth) / 2, (GetScreenHeight() - 20) / 2, 20, LIME);
        }
    }

    EndDrawing();
}
