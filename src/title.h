#ifndef TITLE_H
#define TITLE_H

#include <raylib.h>

typedef struct {
    Texture2D tex;
    Shader shader;
    int locTime;
    float time;
    float hue;
} Title;

Title Title_Load(void);
void Title_Unload(Title* title);

void Title_Update(Title* title, float dt);
void Title_Draw(Title* title);

#endif