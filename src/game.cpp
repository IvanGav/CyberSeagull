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
#include <vector>
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
Vector2 deltaMouse;

//holds a screen to render
void (*currentScreen)(void);

struct SeagullVirus {
	constexpr static uint32_t stackSize = 64;
	uint32_t registerFile[8];
	uint32_t stack[stackSize];
	uint32_t stackPointer = 0;
	std::vector<uint32_t> instructionStream;
	uint32_t instructionPointer;
	bool active;
	bool rshRequested;
};

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
Texture2D seagullMap;
Texture2D seagullLeave;
Texture2D terminalTex;
Texture2D mapTex;
Texture2D serverGeneric;
Texture2D serverHome;
Texture2D serverIdea;
Texture2D firewallTex;
Texture2D firewallDeadTex;
Texture2D routerTex;
Texture2D seagullVirus;
Font font;

TypingBox loginUser{ Rectangle{ 724 / 2, 536 / 2, 504 / 2, 34 / 2 }, TypingBox::cap };
TypingBox loginPass{ Rectangle{ 724 / 2, 592 / 2, 504 / 2, 34 / 2 }, TypingBox::cap };
Button loginOK{ Rectangle{ 714 / 2, 718 / 2, 162 / 2, 42 / 2 } };
Button loginExit{ Rectangle{ 1055 / 2, 719 / 2, 162 / 2, 42 / 2 } };

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
	if (activeTypingBox->pos && IsKeyPressed(KEY_BACKSPACE)) {
		activeTypingBox->data[--activeTypingBox->pos] = '\0';
	}
	char c;
	while (activeTypingBox->pos < activeTypingBox->max && (c = GetCharPressed())) {
		activeTypingBox->data[activeTypingBox->pos++] = c;
		activeTypingBox->data[activeTypingBox->pos] = '\0';
	}
}

void do_desktop();

bool termOpen;
bool mapOpen;

#include "network.h"
#include "context.h"
#include "term.h"
#include "map.h"
#include "interpreter.h"

void do_desktop() {
	DrawTextureNPatch(desktopTex, NPatchInfo{ Rectangle{0,0,1920,1080} }, Rectangle{ 0, 0, screenWidth, screenHeight }, Vector2{}, 0.0F, WHITE);
	float scale = 0.25F;
	Rectangle recycleBox{ 0, 0, 256 * scale, 300 * scale };
	Rectangle foxBox{ 0, 330 * 1 * scale, 256 * scale, 300 * scale };
	Rectangle termBox{ 0, 330 * 2 * scale, 256 * scale, 300 * scale };
	Rectangle mapBox{ 260 * 10 * scale, 330 * 4 * scale, 256 * scale, 300 * scale};
	Rectangle leaveBox{ 260 * 10 * scale, 330 * 1 * scale, 256 * scale, 300 * scale };
	DrawTextureEx(seagullRecycle, Vector2{recycleBox.x, recycleBox.y}, 0.0F, scale, CheckCollisionPointRec(mousePosition, recycleBox) ? WHITE : Color{ 220, 220, 220, 255 });
	DrawTextureEx(seagullFox, Vector2{ foxBox.x, foxBox.y }, 0.0F, scale, CheckCollisionPointRec(mousePosition, foxBox) ? WHITE : Color{ 220, 220, 220, 255 });
	DrawTextureEx(seagullTerm, Vector2{ termBox.x, termBox.y }, 0.0F, scale, CheckCollisionPointRec(mousePosition, termBox) ? WHITE : Color{ 220, 220, 220, 255 });
	DrawTextureEx(seagullMap, Vector2{ mapBox.x, mapBox.y }, 0.0F, scale, CheckCollisionPointRec(mousePosition, mapBox) ? WHITE : Color{ 220, 220, 220, 255 });
	DrawTextureEx(seagullLeave, Vector2{ leaveBox.x, leaveBox.y }, 0.0F, scale, CheckCollisionPointRec(mousePosition, leaveBox) ? WHITE : Color{ 220, 220, 220, 255 });
	if (!termOpen && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, termBox)) {
		open_terminal();
		termOpen = true;
	}
	if (!mapOpen && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, mapBox)) {
		open_map();
		mapOpen = true;
	}
	if (termOpen) {
		do_terminal();
	}
	if (mapOpen) {
		do_map();
	}
}

bool userExit = false;
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
			if (strncmp(loginPass.data, "Cgull", 5) == 0) { //make sure there's a correct password
				currentScreen = do_desktop;
				activeTypingBox = nullptr;
			}
		} else if (CheckCollisionPointRec(mousePosition, loginExit.rect)) {
			userExit = true;
		}
	}
	update_active_typing_box();
	render_typing_box(loginUser);
	render_typing_box(loginPass);
	render_active_typing_box_cursor();
	if (IsKeyPressed(KEY_TAB)) {
		if (activeTypingBox == &loginUser) {
			activeTypingBox = &loginPass;
		} else if (activeTypingBox == &loginPass || activeTypingBox == nullptr) {
			activeTypingBox = &loginUser;
		}
	}
	if ((activeTypingBox == &loginUser || activeTypingBox == &loginPass) && IsKeyPressed(KEY_ENTER) && strncmp(loginPass.data, "Cgull", 5) == 0) {
		currentScreen = do_desktop;
		activeTypingBox = nullptr;
	}
}

//int main() {
//	auto a = compileProgram(addOneS);
//	for (uint32_t i : a) {
//		int op = i & 0xFF;
//		int arg1 = (i & 0xFF00) >> 8;
//		int arg2 = (i & 0xFF0000) >> 16;
//		int arg3 = (i & 0xFF000000) >> 24;
//		op = op;
//	}
//	return 0;
//}

const char* compile = R"(
MOV R0, 10
MOV R1, 0
MOV R2, 1
MOV R3, 1
start:
CNE R4,R0,R2
JZE R4, end

ADD R1,R1,R2
ADD R2,R2,R3

JMP start
end:
)";

int main(void) {
	// Compiler test
	SeagullVirus virus{};
	virus.instructionStream = compileProgram(compile);
	virus.active = true;
	NetNode node{ NET_NODE_TYPE_SERVER };
	while (virus.active) {
		interpret_next(virus, &node);
	}
	return 0;
	currentScreen = do_login;
	InitWindow(screenWidth, screenHeight, "Cyber Seagull");
	SetWindowIcon(LoadImage("resources/icon.png"));
	loginTex = LoadTexture("resources/seagulllogin.png");
	desktopTex = LoadTexture("resources/seagulldesktop.png");
	seagullFox = LoadTexture("resources/seagullfox.png");
	seagullRecycle = LoadTexture("resources/seagullrecycle.png");
	seagullTerm = LoadTexture("resources/seagullterm.png");
	seagullMap = LoadTexture("resources/seagullmap.png");
	seagullLeave = LoadTexture("resources/seagullleave.png");
	terminalTex = LoadTexture("resources/terminal.png");
	mapTex = LoadTexture("resources/map.png");
	serverGeneric = LoadTexture("resources/server.png");
	serverHome = LoadTexture("resources/homeserver.png");
	serverIdea = LoadTexture("resources/ideaserver.png");
	firewallTex = LoadTexture("resources/firewall.png");
	firewallDeadTex = LoadTexture("resources/deadfirewall.png");
	routerTex = LoadTexture("resources/router.png");
	seagullVirus = LoadTexture("resources/seagullvirus.png");
	font = LoadFont("resources/JetBrainsMonoNL-SemiBold.ttf");

	SetTargetFPS(60);
	SetExitKey(0);

	build_network_graph();
	homeNode = &netNodes[0];
	currentConnectedNode = homeNode;

	while (!WindowShouldClose() && !userExit) {
		deltaTime = GetFrameTime();
		deltaMouse = GetMouseDelta();
		mousePosition = GetMousePosition();
		activeTypingBlinkTime += deltaTime;
		BeginDrawing();
		
		ClearBackground(RAYWHITE);
		currentScreen();

		EndDrawing();
	}
	CloseWindow();
	return EXIT_SUCCESS;
}