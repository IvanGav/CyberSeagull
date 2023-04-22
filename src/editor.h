#pragma once

std::vector<TermLine> fileLines;

float editorTime;
uint32_t lineOffset;
uint32_t cursorX;
uint32_t cursorY;

void open_editor(const char* file) {
	while (*file != '\0') {
		fileLines.push_back({});
		uint32_t size = 0;
		while (*file != '\n' && *file != '\0') {
			if (size < terminalWidth) {
				fileLines.back().l[size++] = *file;
			}
			file++;
		}
		file++;
	}
	if (fileLines.size() == 0) {
		fileLines.push_back({});
	}
	editorTime = 0;
	lineOffset = 0;
	cursorX = 0;
	cursorY = 0;
	editorMode = true;
}

void serialize_edited_file(std::vector<char>& file) {
	file.clear();
	for (TermLine& line : fileLines) {
		for (uint32_t i = 0; line.l[i] != '\0'; i++) {
			file.push_back(line.l[i]);
		}
		file.push_back('\n');
	}
	file.back() = '\0';
}

void do_editor() {
	editorTime += deltaTime;
	if (termSelected) {
		if (IsKeyPressed(KEY_LEFT) && cursorX > 0) {
			cursorX--;
		}
		if (IsKeyPressed(KEY_RIGHT) && cursorX < strlen(fileLines[cursorY].l)) {
			cursorX++;
		}
		if (IsKeyPressed(KEY_UP) && cursorY > 0) {
			cursorY--;
			if (cursorY < lineOffset) {
				lineOffset--;
			}
			cursorX = std::min<uint32_t>(cursorX, strlen(fileLines[cursorY].l));
		}
		if (IsKeyPressed(KEY_DOWN) && cursorY < fileLines.size() - 1) {
			cursorY++;
			if (cursorY - lineOffset >= terminalHeight) {
				lineOffset++;
			}
			cursorX = std::min<uint32_t>(cursorX, strlen(fileLines[cursorY].l));
		}
		if (IsKeyPressed(KEY_BACKSPACE)) {
			if (cursorX == 0) {
				if (cursorY > 0) {
					uint32_t currentSize = strlen(fileLines[cursorY].l);
					uint32_t prevSize = strlen(fileLines[cursorY - 1].l);
					cursorX = prevSize;
					uint32_t amountToTransfer = std::min<uint32_t>(terminalWidth - prevSize, currentSize);
					for (uint32_t i = 0; i < amountToTransfer; i++) {
						fileLines[cursorY - 1].l[prevSize++] = fileLines[cursorY].l[i];
						fileLines[cursorY].l[i] = i + amountToTransfer < currentSize ? fileLines[cursorY].l[i + amountToTransfer] : '\0';
					}
					fileLines[cursorY - 1].l[prevSize] = '\0';
					if (amountToTransfer == currentSize) {
						fileLines.erase(fileLines.begin() + cursorY);
					}
					cursorY--;
					if (cursorY < lineOffset) {
						lineOffset--;
					}
				}
			} else {
				for (uint32_t i = cursorX - 1; fileLines[cursorY].l[i] != '\0'; i++) {
					fileLines[cursorY].l[i] = fileLines[cursorY].l[i + 1];
				}
				cursorX--;
			}
		}
		if (IsKeyPressed(KEY_ENTER)) {
			fileLines.insert(fileLines.begin() + cursorY + 1, TermLine{});
			uint32_t amountToTransfer = strlen(fileLines[cursorY].l) - cursorX;
			for (uint32_t i = 0; i < amountToTransfer; i++) {
				fileLines[cursorY + 1].l[i] = fileLines[cursorY].l[cursorX + i];
				fileLines[cursorY].l[cursorX + i] = '\0';
			}
			cursorX = 0;
			cursorY++;
			if (cursorY - lineOffset >= terminalHeight) {
				lineOffset++;
			}
		}
		char c;
		while (strlen(fileLines[cursorY].l) < terminalWidth && (c = GetCharPressed())) {
			char next = c;
			uint32_t i = cursorX;
			for (; next != '\0'; i++) {
				char prev = next;
				next = fileLines[cursorY].l[i];
				fileLines[cursorY].l[i] = prev;
			}
			fileLines[cursorY].l[i] = '\0';
			cursorX++;
		}
		if (IsKeyPressed(KEY_ESCAPE)) {
			std::vector<char> vec;
			serialize_edited_file(vec);
			editorMode = false;
		}
	}
	Vector2 offset{ 2, 20 };
	for (uint32_t i = lineOffset; i < std::min<uint32_t>(lineOffset + terminalHeight, fileLines.size()); i++) {
		DrawTextEx(font, &fileLines[i].l[0], offset, 20, 0.0F, LIGHTGRAY);
		offset.y += 20;
	}
	DrawTextEx(font, "|", Vector2{ float(cursorX) * 8.75F - 2.0F, float((cursorY - lineOffset) * 20) + 20 }, 20, 0.0F, LIGHTGRAY);
}