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
	SeagullVirus virus;
	bool compromised;

	bool fizzBuzz(int i) {
		bool val = false;
		if (challengeData % 3 && challengeData % 5)
			val = (i == 9);
		else if (challengeData % 3)
			val = (i == 1);
		else if (challengeData % 5)
			val = (i == 8);
		else
			val = (i == challengeData);
		challengeData++;
		return val;
	}

	bool addOne(int i) {
		return (i == (challengeData + 1));
	}

	bool search(int i) {
		return (i == challengeData);
	}

	bool hex2Dec(int i) {
		return (i == challengeData);
	}
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
	netNodes.push_back({ NET_NODE_TYPE_SERVER, 600, 0 });
	netNodes.push_back({ NET_NODE_TYPE_SERVER, 700, 100 });
	netNodes.push_back({ NET_NODE_TYPE_FIREWALL, 800, 0, FIREWALL_CHALLENGE_FIZZBUZZ });
	netNodes.push_back({ NET_NODE_TYPE_SERVER, 1000, 0 });
	netNodes.push_back({ NET_NODE_TYPE_ROUTER, 1100, 100 });
	netNodes.push_back({ NET_NODE_TYPE_FIREWALL, 1200, 0, FIREWALL_CHALLENGE_NODE_SEARCH });
	netNodes.push_back({ NET_NODE_TYPE_SERVER, 1400, 0 });
	netNodes.push_back({ NET_NODE_TYPE_ROUTER, 1500, 100 });
	netNodes.push_back({ NET_NODE_TYPE_SERVER, 1450, 200 });
	netNodes.push_back({ NET_NODE_TYPE_FIREWALL, 1600, 0, FIREWALL_CHALLENGE_NODE_AND_HEX2DEC });
	netNodes.push_back({ NET_NODE_TYPE_SERVER, 1800, 0 });
	netNodes.push_back({ NET_NODE_TYPE_IDEA_SERVER, 2000, 0 });

	add_network_file(1, "README", "password");
	add_network_file(3, "IDEA", "Make an idea game!");

	connect_nodes(0, 1, 5);
	connect_nodes(1, 2, 50);
	connect_nodes(2, 3, 400);
	connect_nodes(3, 4, 235);
	connect_nodes(3, 5, 670);
	connect_nodes(5, 6, 320);
	connect_nodes(6, 7, 1104);
	connect_nodes(6, 8, 1113);
	connect_nodes(8, 9, 1216);
	connect_nodes(9, 10, 1218);
	connect_nodes(10, 11, 913);
	connect_nodes(10, 12, 278);
	connect_nodes(12, 13, 920);
	connect_nodes(13, 14, 148);
}