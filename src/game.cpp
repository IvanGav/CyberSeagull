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

const int screenWidth = 960;
const int screenHeight = 540;
const float screenWidthf = float(screenWidth);
const float screenHeightf = float(screenHeight);

void DrawRectangleNorm(float x, float y, float width, float height, Color color) {
	DrawRectangle(x * screenWidthf, y * screenHeightf, width * screenWidthf, height * screenHeightf, color);
}

float deltaTime;
Vector2 mousePosition;

//holds a screen to render
void (*currentScreen)(void);

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
Texture2D desktopTex;
Texture2D seagullFox;
Texture2D seagullRecycle;
Texture2D seagullTerm;
Font font;

TypingBox loginUser{ Rectangle{ 724 / 2, 536 / 2, 504 / 2, 34 / 2 }, TypingBox::cap };
TypingBox loginPass{ Rectangle{ 724 / 2, 592 / 2, 504 / 2, 34 / 2 }, TypingBox::cap };
Button loginOK{ Rectangle{ 400, 300, 100, 100 } };

float activeTypingBlinkTime;
TypingBox* activeTypingBox;

void render_typing_box(TypingBox& box) {
	DrawTextEx(font, box.data, Vector2{ box.rect.x + 3, box.rect.y }, box.rect.height, 0, BLACK);
}

void render_active_typing_box_cursor() {
	if (!activeTypingBox) {
		return;
	}
	if ((uint32_t(activeTypingBlinkTime * 4) & 1) == 0) {
		Vector2 len = MeasureTextEx(font, activeTypingBox->data, activeTypingBox->rect.height, 0);
		DrawTextEx(font, "_", Vector2{ activeTypingBox->rect.x + 3 + len.x, activeTypingBox->rect.y }, activeTypingBox->rect.height, 0, BLACK);
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

#include "context.h"

void do_desktop() {
	DrawTextureNPatch(desktopTex, NPatchInfo{ Rectangle{0,0,1920,1080} }, Rectangle{ 0, 0, screenWidth, screenHeight }, Vector2{}, 0.0F, WHITE);
	float scale = 0.25F;
	Rectangle recycleBox{ 0, 0, 256 * scale, 300 * scale };
	Rectangle foxBox{ 0, 330 * scale, 256 * scale, 300 * scale };
	Rectangle termBox{ 0, 660 * scale, 256 * scale, 300 * scale };
	DrawTextureEx(seagullRecycle, Vector2{recycleBox.x, recycleBox.y}, 0.0F, scale, CheckCollisionPointRec(mousePosition, recycleBox) ? WHITE : Color{ 220, 220, 220, 255 });
	DrawTextureEx(seagullFox, Vector2{ foxBox.x, foxBox.y }, 0.0F, scale, CheckCollisionPointRec(mousePosition, foxBox) ? WHITE : Color{ 220, 220, 220, 255 });
	DrawTextureEx(seagullTerm, Vector2{ termBox.x, termBox.y }, 0.0F, scale, CheckCollisionPointRec(mousePosition, termBox) ? WHITE : Color{ 220, 220, 220, 255 });
}

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
			currentScreen = do_login;
		}
	}
	update_active_typing_box();
	render_typing_box(loginUser);
	render_typing_box(loginPass);
	render_active_typing_box_cursor();
	if (strncmp(loginPass.data, "Duck", TypingBox::cap) != 0) {
		currentScreen = do_desktop;
		activeTypingBox = nullptr;
	}
}

int main(void) {
	currentScreen = do_login;
	InitWindow(screenWidth, screenHeight, "Cyber Seagull");
	SetWindowIcon(LoadImage("resources/icon.png"));
	loginTex = LoadTexture("resources/seagulllogin.png");
	desktopTex = LoadTexture("resources/seagulldesktop.png");
	seagullFox = LoadTexture("resources/seagullfox.png");
	seagullRecycle = LoadTexture("resources/seagullrecycle.png");
	seagullTerm = LoadTexture("resources/seagullterm.png");
	font = LoadFont("resources/JetBrainsMonoNL-SemiBold.ttf");

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