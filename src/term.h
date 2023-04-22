#pragma once

#include <vector>

const uint32_t terminalWidth = 54;
const uint32_t terminalHeight = 25;

struct TermLine {
	char l[terminalWidth + 1];
};

std::vector<TermLine> terminalLines;
uint32_t termCmdSize;
TermLine termCommand;
bool termSelected;
bool editorMode;

#include "editor.h"

void do_terminal();

void open_terminal() {
	terminalLines.reserve(terminalHeight);
	if (termCmdSize == 0) {
		termCmdSize = 1;
		termCommand.l[0] = '>';
	}
}

void push_terminal_line() {
	if (terminalLines.size() >= terminalHeight) {
		terminalLines.erase(terminalLines.begin());
	}
	terminalLines.push_back({});
}

void write_terminal_line(const char* msg) {
	push_terminal_line();
	memcpy(terminalLines.back().l, msg, strlen(msg));
}

void do_terminal() {
	Rectangle termBox{ 0, 0, 1920 / 4, 1080 / 2 };
	Rectangle xBox{ 882 / 2, 4 / 2, 71 / 2, 41 / 2 };
	DrawTextureNPatch(terminalTex, NPatchInfo{ Rectangle{0,0,960,1080} }, Rectangle{ 0, 0, screenWidth / 2, screenHeight }, Vector2{}, 0.0F, WHITE);
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(mousePosition, xBox)) {
			termOpen = false;
			return;
		}
		termSelected = CheckCollisionPointRec(mousePosition, termBox);
	}
	if (editorMode) {
		do_editor();
		return;
	}
	if (termSelected) {
		if (termCmdSize > 1 && IsKeyPressed(KEY_BACKSPACE)) {
			termCommand.l[--termCmdSize] = '\0';
		}
		char c;
		while (termCmdSize < terminalWidth && (c = GetCharPressed())) {
			termCommand.l[termCmdSize++] = c;
			termCommand.l[termCmdSize] = '\0';
		}
		if (IsKeyPressed(KEY_ENTER)) {
			push_terminal_line();
			memcpy(&terminalLines.back().l[0], termCommand.l, termCmdSize);

			// Process command
			uint32_t cmdLen = strlen(termCommand.l + 1);
			if (strncmp(termCommand.l + 1, "cat", 3) == 0) {
				if (cmdLen <= 4) {
					write_terminal_line("\tProvide filename argument");
				} else {

				}
			} else if (strncmp(termCommand.l + 1, "ls", 2) == 0) {
				// ls current server
			} else if (strncmp(termCommand.l + 1, "rsh", 3) == 0) {
				// do reverse shell here
			} else if (strncmp(termCommand.l + 1, "vim", 3) == 0) {
				if (cmdLen <= 4) {
					write_terminal_line("\tProvide filename argument");
				} else {
					open_editor("I\nlike\nducks");
				}
			} else if (strncmp(termCommand.l + 1, "fly", 3) == 0) {
				if (cmdLen <= 4) {
					write_terminal_line("\tProvide filename argument");
				} else {
					
				}
			} else if (strncmp(termCommand.l + 1, "cls", 3) == 0 || strncmp(termCommand.l + 1, "clear", 5) == 0) {
				terminalLines.clear();
			} else {
				write_terminal_line("\tCommand not recognized");
			}
			termCmdSize = 0;
			termCommand.l[termCmdSize++] = '>';
			bool rsh = false;
			if (rsh) {
				termCommand.l[termCmdSize++] = '>';
			}
			termCommand.l[termCmdSize] = '\0';
		}
	}
	Vector2 offset{ 0, 20 };
	for (uint32_t i = 0; i < terminalLines.size(); i++) {
		DrawTextEx(font, &terminalLines[i].l[0], offset, 20, 0.0F, LIGHTGRAY);
		offset.y += 20;
	}
	DrawTextEx(font, termCommand.l, offset, 20, 0.0F, LIGHTGRAY);
}