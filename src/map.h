#pragma once

void open_map() {

}

void do_map() {
	Rectangle xBox{ 882 / 2 + screenWidth / 2, 4 / 2, 71 / 2, 41 / 2 };
	DrawTextureNPatch(mapTex, NPatchInfo{ Rectangle{0,0,960,1080} }, Rectangle{ screenWidth / 2, 0, screenWidth / 2, screenHeight }, Vector2{}, 0.0F, WHITE);
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, xBox)) {
		mapOpen = false;
		return;
	}
}