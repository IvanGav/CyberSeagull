#pragma once

#include <vector>
#include <string>

enum NetNodeType {
	NET_NODE_TYPE_HOME_SERVER = 0,
	NET_NODE_TYPE_IDEA_SERVER = 1,
	NET_NODE_TYPE_SERVER = 2,
	NET_NODE_TYPE_ROUTER = 3,
	NET_NODE_TYPE_FIREWALL = 4
};

enum FirewallChallenge {
	FIREWALL_CHALLENGE_ADD_ONE,
	FIREWALL_CHALLENGE_FIZZBUZZ,
	FIREWALL_CHALLENGE_NODE_SEARCH,
	FIREWALL_CHALLENGE_NODE_AND_HEX2DEC
};

struct File {
	std::string name;
	std::vector<char> data;
};

struct NetNode;

struct PortConnection {
	NetNode* dst;
	uint32_t port;
};

struct NetNode {
	NetNodeType type;
	float x;
	float y;
	// Only relevant for firewalls
	FirewallChallenge challengeType;
	uint32_t challengeData;
	bool firewallDown;

	std::vector<File> files;
	std::vector<PortConnection> outboundPorts;
	
};

std::vector<NetNode> netNodes;

void add_network_file(uint32_t idx, const char* name, const char* str) {
	NetNode& node = netNodes[idx];
	node.files.push_back(File{ std::string{ name } });
	std::vector<char>& vec = node.files.back().data;
	while (*str != '\0') {
		vec.push_back(*str++);
	}
	vec.push_back('\0');
}

void connect_nodes(uint32_t nodeA, uint32_t nodeB, uint32_t port) {
	netNodes[nodeA].outboundPorts.push_back(PortConnection{ &netNodes[nodeB], port });
}

void build_network_graph() {
	netNodes.push_back({ NET_NODE_TYPE_HOME_SERVER, 0, 0 });
	netNodes.push_back({ NET_NODE_TYPE_SERVER, 200, 0 });
	netNodes.push_back({ NET_NODE_TYPE_FIREWALL, 400, 0, FIREWALL_CHALLENGE_ADD_ONE });
	netNodes.push_back({ NET_NODE_TYPE_IDEA_SERVER, 600, 0 });

	add_network_file(1, "README", "password");
	add_network_file(3, "IDEA", "Make an idea game!");

	connect_nodes(0, 1, 5);
	connect_nodes(1, 2, 50);
	connect_nodes(2, 3, 400);
}