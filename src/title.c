#include "title.h"
#include <raylib.h>

static const char fragWave[] =
#ifdef PLATFORM_DESKTOP
    "#version 330\n"
    "in vec2 fragTexCoord;"
    "in vec4 fragColor;"
    "uniform sampler2D texture0;"
    "uniform float time;"
    "out vec4 finalColor;"
    "void main()"
    "{"
        "float amp = 0.05;"
        "float freq = 8.0;"
        "float wave = amp * sin((time+fragTexCoord.x)*freq);"
        "vec2 uv = vec2(fragTexCoord.x, fragTexCoord.y + wave);"
        "uv = clamp(uv, 0.0, 1.0);"
        "vec4 texelColor = texture(texture0, uv);"
        "finalColor = texelColor * fragColor;"
    "}";
#else
    "#version 100\n"
    "precision mediump float;"
    "varying vec2 fragTexCoord;"
    "varying vec4 fragColor;"
    "uniform sampler2D texture0;"
    "uniform float time;"
    "void main()"
    "{"
        "float amp = 0.05;"
        "float freq = 8.0;"
        "float wave = amp * sin((time+fragTexCoord.x)*freq);"
        "vec2 uv = vec2(fragTexCoord.x, fragTexCoord.y + wave);"
        "uv = clamp(uv, 0.0, 1.0);"
        "vec4 texelColor = texture2D(texture0, uv);"
        "gl_FragColor = texelColor * fragColor;"
    "}";
#endif

Title Title_Load(void)
{
    Image text = ImageText("Rainbow Bullet", 42, WHITE);
    Texture2D texture = LoadTextureFromImage(text);
    UnloadImage(text);

    Shader shader = LoadShaderFromMemory(0, fragWave);
    int locTime = GetShaderLocation(shader, "time");

    return (Title) {
        .tex = texture,
        .shader = shader,
        .locTime = locTime,
        .time = 0.0f,
        .hue = 0.0f
    };
}

void Title_Unload(Title* title)
{
    UnloadTexture(title->tex);
    title->tex = (Texture2D) {0};

    UnloadShader(title->shader);
    title->shader = (Shader) {0};
}

void Title_Update(Title* title, float dt)
{
    title->time += dt;
    title->hue += 180.0f * dt;
    SetShaderValue(title->shader, title->locTime, &title->time, SHADER_UNIFORM_FLOAT);
}

void Title_Draw(Title* title)
{
    BeginShaderMode(title->shader);
        DrawTexture(title->tex, (GetScreenWidth() - title->tex.width) / 2, GetScreenHeight() / 6, ColorFromHSV(title->hue, 1.0f, 1.0f));
    EndShaderMode();
}
