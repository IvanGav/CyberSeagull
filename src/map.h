#pragma once

float mapScale;
float mapX;
float mapY;


void open_map() {
	mapScale = 1.0F;
	mapX = (float(screenWidth) * 3.0F) * 0.25F;
	mapY = float(screenHeight) * 0.5F;
}

void render_nodes() {
	float halfwidth = 50.0F;
	float halfheight = 50.0F;
	for (const NetNode& node : netNodes) {
		for (const PortConnection& con : node.outboundPorts) {
			DrawLineEx(Vector2{ node.x * mapScale + mapX, node.y * mapScale + mapY }, Vector2{ con.dst->x * mapScale + mapX, con.dst->y * mapScale + mapY }, 5.0F, RED);
		}
	}
	for (const NetNode& node : netNodes) {
		Texture2D texture = firewallDeadTex;
		switch (node.type) {
		case NET_NODE_TYPE_HOME_SERVER: texture = serverHome; break;
		case NET_NODE_TYPE_IDEA_SERVER: texture = serverIdea; break;
		case NET_NODE_TYPE_SERVER: texture = serverGeneric; break;
		case NET_NODE_TYPE_ROUTER: texture = routerTex; break;
		case NET_NODE_TYPE_FIREWALL: texture = node.firewallDown ? firewallDeadTex : firewallTex; break;
		}
		DrawTextureNPatch(texture, NPatchInfo{ Rectangle{ 0, 0, float(texture.width), float(texture.height) } }, Rectangle{ (node.x - halfwidth) * mapScale + mapX, (node.y - halfheight) * mapScale + mapY, (halfwidth * 2) * mapScale, (halfheight * 2) * mapScale }, Vector2{}, 0.0F, WHITE);
	}
}

void do_map() {
	Rectangle xBox{ 882 / 2 + screenWidth / 2, 4 / 2, 71 / 2, 41 / 2 };
	DrawTextureNPatch(mapTex, NPatchInfo{ Rectangle{0,0,960,1080} }, Rectangle{ screenWidth / 2, 0, screenWidth / 2, screenHeight }, Vector2{}, 0.0F, WHITE);
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, xBox)) {
		mapOpen = false;
		return;
	}
	Rectangle nodeDrawArea{ screenWidth / 2 + 3, 24, screenWidth / 2 - 6, screenHeight - 27 };
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		mapX += deltaMouse.x;
		mapY += deltaMouse.y;
	}
	BeginScissorMode(nodeDrawArea.x, nodeDrawArea.y, nodeDrawArea.width, nodeDrawArea.height);
	render_nodes();
	EndScissorMode();
}