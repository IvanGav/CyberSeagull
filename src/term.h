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
bool isReverseShell;

std::vector<NetNode*> rshConnectQueue;
NetNode* currentConnectedNode;
NetNode* homeNode;

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
					std::string name{ termCommand.l + 5 };
					for (File& file : currentConnectedNode->files) {
						if (file.name == name) {
							char* ptr = file.data.data();
							while (*ptr != '\0') {
								push_terminal_line();
								uint32_t size = 0;
								while (*ptr != '\n' && *ptr != '\0') {
									if (size < terminalWidth) {
										terminalLines.back().l[size++] = *ptr;
									}
									ptr++;
								}
								if (*ptr == '\0') {
									break;
								}
								ptr++;
							}
							break;
						}
					}
				}
			} else if (strncmp(termCommand.l + 1, "ls", 2) == 0) {
				if (currentConnectedNode) {
					for (File& file : currentConnectedNode->files) {
						write_terminal_line(file.name.c_str());
					}
				}
			} else if (strncmp(termCommand.l + 1, "rsh", 3) == 0) {
				if (isReverseShell) {
					isReverseShell = false;
					currentConnectedNode->virus.rshRequested = false;
					currentConnectedNode = homeNode;
					editorMode = false;
				}
				if (rshConnectQueue.empty()) {
					write_terminal_line("No pending reverse shell connections!");
				} else {
					currentConnectedNode = rshConnectQueue.back();
					rshConnectQueue.pop_back();
					isReverseShell = true;
				}
			} else if (strncmp(termCommand.l + 1, "vim", 3) == 0) {
				if (cmdLen <= 4) {
					write_terminal_line("\tProvide filename argument");
				} else {
					std::string name{ termCommand.l + 5 };
					const char* fileData = "";
					for (File& file : currentConnectedNode->files) {
						if (file.name == name) {
							fileData = file.data.data();
							break;
						}
					}
					open_editor(name, fileData);
				}
			} else if (strncmp(termCommand.l + 1, "fly", 3) == 0) {
				if (cmdLen <= 4) {
					write_terminal_line("\tProvide filename argument");
				} else {
					if (currentConnectedNode->virus.active) {
						write_terminal_line("\tVirus already active on this machine!");
					} else {
						std::string name{ termCommand.l + 5 };
						const char* fileData = nullptr;
						for (File& file : currentConnectedNode->files) {
							if (file.name == name) {
								fileData = file.data.data();
								break;
							}
						}
						if (!fileData) {
							write_terminal_line("\tFile not found!");
						} else {
							currentConnectedNode->virus = SeagullVirus{};
							currentConnectedNode->virus.instructionStream = compileProgram(fileData);
							if (currentConnectedNode->virus.instructionStream.empty()) {
								write_terminal_line("\tCompile error!");
							} else {
								currentConnectedNode->virus.active = true;
								write_terminal_line("\tProgram activated.");
							}
						}
					}

				}
			} else if (strncmp(termCommand.l + 1, "quit", 4) == 0) {
				if (isReverseShell) {
					isReverseShell = false;
					currentConnectedNode->virus.rshRequested = false;
					currentConnectedNode = homeNode;
					editorMode = false;
				} else {
					write_terminal_line("\tNo reverse shell active");
				}
			} else if (strncmp(termCommand.l + 1, "cls", 3) == 0 || strncmp(termCommand.l + 1, "clear", 5) == 0) {
				terminalLines.clear();
			} else if (strncmp(termCommand.l + 1, "taskkill", 8) == 0) {
				for (NetNode& node : netNodes) {
					node.virus.active = false;
				}
				write_terminal_line("\tAll programms terminated.");
			} else {
				write_terminal_line("\tCommand not recognized");
			}
			termCmdSize = 0;
			termCommand.l[termCmdSize++] = '>';
			if (isReverseShell) {
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