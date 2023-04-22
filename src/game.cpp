/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#endif

int main(void) {
    const int screenWidth = 960;
    const int screenHeight = 540;
    
    InitWindow(screenWidth, screenHeight, "Cyber Seagull");
    SetWindowIcon(LoadImage("resources/icon.png"));

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        DrawText("Literally seagull.", 190, 200, 20, Color{ 0, 0, 0, 255 });

        EndDrawing();
    }
    CloseWindow();
    return 0;
}