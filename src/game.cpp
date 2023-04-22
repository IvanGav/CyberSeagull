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
#include <stdint.h>
#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#endif

const int screenWidth = 960;
const int screenHeight = 540;
const float screenWidthf = float(screenWidth);
const float screenHeightf = float(screenHeight);

void DrawRectangleNorm(float x, float y, float width, float height, Color color) {
	DrawRectangle(x * screenWidthf, y * screenHeightf, width * screenWidthf, height * screenHeightf, color);
}


struct TypingBox {
	static constexpr uint32_t cap = 31;
	char data[cap + 1];
	uint32_t pos;
};

Texture2D loginTex;
TypingBox loginUser;
TypingBox loginPass;

void do_login() {
	DrawTextureNPatch(loginTex, NPatchInfo{ Rectangle{0,0,1920,1080} }, Rectangle{ 0, 0, screenWidth, screenHeight }, Vector2{}, 0.0F, WHITE);
	DrawRectangleNorm(0.3F, 0.4F, 0.1F, 0.1F, BLACK);
}

int main(void) {
	InitWindow(screenWidth, screenHeight, "Cyber Seagull");
	SetWindowIcon(LoadImage("resources/icon.png"));
	loginTex = LoadTexture("resources/seagulllogin.png");

	SetTargetFPS(60);

	void (*currentScreen)(void) = do_login;

	while (!WindowShouldClose()) {
		BeginDrawing();
		
		ClearBackground(RAYWHITE);
		currentScreen();
		//DrawText("More Seagulls.", 190, 200, 12, Color{ 0, 0, 0, 255 });

		EndDrawing();
	}
	CloseWindow();
	return 0;
}