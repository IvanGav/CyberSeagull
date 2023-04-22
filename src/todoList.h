#pragma once

#include <vector>

void open_todoList() {
	float textureWidth = todoList.width;
	float textureHeight = todoList.height;
	Rectangle todoListBox{ 0, 0, 1920 / 4, 1080 / 2 };
	DrawTextureNPatch(todoList, NPatchInfo{ Rectangle{0, 0, textureWidth, textureHeight} }, Rectangle{ 390/2, 250/2, screenWidth - 390, screenHeight - 250 }, Vector2{}, 0.0F, WHITE);
	
}

void do_todoList() {
	float textureWidth = todoList.width;
	float textureHeight = todoList.height;
	Rectangle todoListBox{ 0, 0, 1920 / 4, 1080 / 2 };
	DrawTextureNPatch(todoList, NPatchInfo{ Rectangle{0, 0, textureWidth, textureHeight} }, Rectangle{ 390 / 2, 250 / 2, screenWidth - 390, screenHeight - 250 }, Vector2{}, 0.0F, WHITE);
	Rectangle xBox{ (390/2) + 545 , (250/2) , 25, 14 };
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(mousePosition, xBox)) {
			todoListOpen = false;
			return;
		}
	}
}


