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
#include <iostream>
#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#endif

void do_login();

const int screenWidth = 960;
const int screenHeight = 540;
const float screenWidthf = float(screenWidth);
const float screenHeightf = float(screenHeight);

void DrawRectangleNorm(float x, float y, float width, float height, Color color) {
	DrawRectangle(x * screenWidthf, y * screenHeightf, width * screenWidthf, height * screenHeightf, color);
}

float deltaTime;
Vector2 mousePosition;

struct TypingBox {
	static constexpr uint32_t cap = 31;
	Rectangle rect;
	uint32_t max;
	uint32_t pos;
	char data[cap + 1];
};

struct Button {
	Rectangle rect;
};

Texture2D loginTex;
TypingBox loginUser{ Rectangle{ 724 / 2, 536 / 2, 504 / 2, 34 / 2 }, TypingBox::cap };
TypingBox loginPass{ Rectangle{ 724 / 2, 590 / 2, 504 / 2, 34 / 2 }, TypingBox::cap };
Button loginOK{ Rectangle{ 714 / 2, 718 / 2, 162 / 2, 42 / 2 } };

float activeTypingBlinkTime;
TypingBox* activeTypingBox;

void render_typing_box(TypingBox& box) {
	DrawText(box.data, box.rect.x + 3, box.rect.y + 3, box.rect.height - 5, BLACK);
}

void render_active_typing_box_cursor() {
	if (!activeTypingBox) {
		return;
	}
	if ((uint32_t(activeTypingBlinkTime * 4) & 1) == 0) {
		int32_t len = MeasureText(activeTypingBox->data, activeTypingBox->rect.height - 5);
		DrawText("_", activeTypingBox->rect.x + 3 + len, activeTypingBox->rect.y + 3, activeTypingBox->rect.height - 5, BLACK);
	}
}

void update_active_typing_box() {
	if (!activeTypingBox) {
		return;
	}
	char c;
	while (activeTypingBox->pos < activeTypingBox->max && (c = GetCharPressed())) {
		activeTypingBox->data[activeTypingBox->pos++] = c;
		activeTypingBox->data[activeTypingBox->pos] = '\0';
	}
}

//holds a screen to render
void (*currentScreen)(void) = do_login;
#include "context.h"

//LOGIN SCREEN
void do_login() {
	DrawTextureNPatch(loginTex, NPatchInfo{ Rectangle{0,0,1920,1080} }, Rectangle{ 0, 0, screenWidth, screenHeight }, Vector2{}, 0.0F, WHITE);
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		//typing box
		if (CheckCollisionPointRec(mousePosition, loginUser.rect)) {
			activeTypingBox = &loginUser;
			activeTypingBlinkTime = 0;
		} else if (CheckCollisionPointRec(mousePosition, loginPass.rect)) {
			activeTypingBox = &loginPass;
			activeTypingBlinkTime = 0;
		} else {
			activeTypingBox = nullptr;
		}
		//button check
		if (CheckCollisionPointRec(mousePosition, loginOK.rect)) {
			if(strncmp(loginPass.data,"Cgull",5) == 0) //make sure there's a correct password
				currentScreen = do_desktop;
		}
	}
	update_active_typing_box();
	render_typing_box(loginUser);
	render_typing_box(loginPass);
	render_active_typing_box_cursor();
	//DrawRectangleRec(loginOK.rect,BLACK); //draw black on button
}

int main(void) {
	InitWindow(screenWidth, screenHeight, "Cyber Seagull");
	SetWindowIcon(LoadImage("resources/icon.png"));
	loginTex = LoadTexture("resources/seagulllogin.png");

	InitAudioDevice(); //should we?

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		deltaTime = GetFrameTime();
		mousePosition = GetMousePosition();
		activeTypingBlinkTime += deltaTime;
		BeginDrawing();
		
		ClearBackground(RAYWHITE);
		currentScreen();

		EndDrawing();
	}
	CloseWindow();
	return 0;
}