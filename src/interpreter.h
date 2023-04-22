#pragma once

#include "raylib.h"
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

enum Instruction {
	ADD,SUB,REM,JMP,JZE,CEQ,CNE,CGT,CGE,CLT,CLE,MOV,AND,ORR,XOR,FLY,NXT,RSH,INN,OUT,PSH,POP
};

void interpret_next(SeagullVirus& virus, NetNode* node) {
	if (!virus.active || virus.rshRequested || virus.instructionPointer >= virus.instructionStream.size()) {
		return;
	}
	uint32_t data = virus.instructionStream[virus.instructionPointer++];
	Instruction op = static_cast<Instruction>(data & 0xFF);
	data >>= 8;
	if (op == MOV && virus.instructionPointer >= virus.instructionStream.size()) {
		virus.active = false;
		return;
	}
	switch (op) {
	case ADD: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] + virus.registerFile[(data >> 16) & 7]; break;
	case SUB: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] - virus.registerFile[(data >> 16) & 7]; break;
	case REM: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] % virus.registerFile[(data >> 16) & 7]; break;
	case JMP: virus.instructionPointer = data; break;
	case JZE: virus.instructionPointer = virus.registerFile[data & 7] ? virus.instructionPointer : data >> 8; break;
	case CEQ: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] == virus.registerFile[(data >> 16) & 7]; break;
	case CNE: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] != virus.registerFile[(data >> 16) & 7]; break;
	case CGT: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] > virus.registerFile[(data >> 16) & 7]; break;
	case CGE: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] >= virus.registerFile[(data >> 16) & 7]; break;
	case CLT: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] < virus.registerFile[(data >> 16) & 7]; break;
	case CLE: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] <= virus.registerFile[(data >> 16) & 7]; break;
	case MOV: virus.registerFile[data & 7] = virus.instructionStream[virus.instructionPointer++]; break;
	case AND: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] & virus.registerFile[(data >> 16) & 7]; break;
	case ORR: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] | virus.registerFile[(data >> 16) & 7]; break;
	case XOR: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] ^ virus.registerFile[(data >> 16) & 7]; break;
	case FLY:
	{
		uint32_t port = data;
		for (PortConnection& con : node->outboundPorts) {
			if (con.port == port && !con.dst->virus.active && (node->type != NET_NODE_TYPE_FIREWALL || node->firewallDown == true)) {
				con.dst->virus = virus;
				node->virus.active = false;
				break;
			}
		}
		break;
	}
	case NXT:
		virus.registerFile[0] = node->outboundPorts[virus.registerFile[data & 7] % node->outboundPorts.size()].port;
		if (virus.registerFile[1] == node->outboundPorts.size()) {
			virus.registerFile[1] = 0xFFFFFFFF;
		} else {
			virus.registerFile[1]++;
		}
		break;
	case RSH:
		rshConnectQueue.push_back(node);
		virus.rshRequested = true;
		break;
	case INN:
		if (node->type != NET_NODE_TYPE_FIREWALL) {
			virus.registerFile[data & 7] = 0;
			break;
		}
		switch (node->challengeType) {
		case FIREWALL_CHALLENGE_ADD_ONE: virus.registerFile[data & 7] = node->challengeData; break;
		case FIREWALL_CHALLENGE_FIZZBUZZ: virus.registerFile[data & 7] = 100; node->challengeData = 0; break;
		case FIREWALL_CHALLENGE_NODE_SEARCH: break;
		case FIREWALL_CHALLENGE_NODE_AND_HEX2DEC: break;
		}
		break;
	case OUT:
		if (node->type != NET_NODE_TYPE_FIREWALL) {
			break;
		}
		switch (node->challengeType) {
		case FIREWALL_CHALLENGE_ADD_ONE: node->firewallDown |= node->addOne(virus.registerFile[data & 7]); break;
		case FIREWALL_CHALLENGE_FIZZBUZZ: 
			if (node->fizzBuzz(virus.registerFile[data & 7])) {
				if (node->challengeData == 100) {
					node->firewallDown = true;
				}
			} else {
				node->challengeData = 0;
			}
			break;
		case FIREWALL_CHALLENGE_NODE_SEARCH: node->firewallDown |= node->search(virus.registerFile[data & 7]); break;
		case FIREWALL_CHALLENGE_NODE_AND_HEX2DEC: node->firewallDown |= node->hex2Dec(virus.registerFile[data & 7]); break;
		}
		break;
	case PSH:
		if (virus.stackPointer >= SeagullVirus::stackSize) {
			virus.active = false;
			return;
		}
		virus.stack[virus.stackPointer++] = virus.registerFile[data & 7];
		break;
	case POP:
		if (virus.stackPointer == 0) {
			virus.active = false;
			return;
		}
		virus.registerFile[data & 7] = virus.stack[--virus.stackPointer];
		break;
	default: virus.active = false; break;
	}
	if (virus.instructionPointer >= virus.instructionStream.size()) {
		virus.active = false;
	}
}

//skip to next non space character (or :)
void skipToNext(const char*& cur) {
	while (*cur == '\n' || *cur == ' ' || *cur == ':' || *cur == ',') {
		cur++;
	}
}

//convert range to a string
std::string rangeToString(const char* start, const char* end) {
	std::string str = "";
	while (start != end) {
		str += *start;
		start++;
	}
	return str;
}

//compare if range in char array is equal to a string
bool compare(const char* start, const char* end, std::string str) {
	if (end - start != str.size()) return false;
	for (int i = 0; i < str.size(); i++)
		if (str[i] != toupper(*(start + i))) return false;
	return true;
}

//return a number of the next register given a range (-1 if invalid)
int getRegister(const char* start, const char* end) {
	if (end - start != 2) return -1;
	if (*start != 'R' && *start != 'r') return -1;
	int rid = *(start + 1) - '0';
	if (rid > 7 || rid < 0) return -1;
	return rid;
}

//get next token and put into 'buffer'
const char* nextToken(const char* cur) {
	while (!(*cur == ' ' || *cur == ':' || *cur == '\0')) {
		cur++;
	}
	return cur;
}

//get next number (or register) and put into 'buffer'
const char* nextArg(const char* cur) {
	while (!(*cur == '\n' || *cur == ',' || *cur == '\0')) {
		cur++;
	}
	return cur;
}

//get number of instruction represented by a range (or -1 if not found)
int findInstruction(const char* start, const char* end) {
	if (compare(start, end, "ADD")) return ADD;
	if (compare(start, end, "SUB")) return SUB;
	if (compare(start, end, "REM")) return REM;
	if (compare(start, end, "JMP")) return JMP;
	if (compare(start, end, "JZE")) return JZE;
	if (compare(start, end, "CEQ")) return CEQ;
	if (compare(start, end, "CNE")) return CNE;
	if (compare(start, end, "CGT")) return CGT;
	if (compare(start, end, "CGE")) return CGE;
	if (compare(start, end, "CLT")) return CLT;
	if (compare(start, end, "CLE")) return CLE;
	if (compare(start, end, "AND")) return AND;
	if (compare(start, end, "ORR")) return ORR;
	if (compare(start, end, "XOR")) return XOR;
	if (compare(start, end, "MOV")) return MOV;
	if (compare(start, end, "FLY")) return FLY;
	if (compare(start, end, "NXT")) return NXT;
	if (compare(start, end, "RSH")) return RSH;
	if (compare(start, end, "INN")) return INN;
	if (compare(start, end, "OUT")) return OUT;
	if (compare(start, end, "PSH")) return PSH;
	if (compare(start, end, "POP")) return POP;
	return -1;
}

void createAndPush(int index, std::string labelName, std::unordered_map<std::string, std::vector<uint32_t>>& uninit_labels) {
	if (uninit_labels.find(labelName) == uninit_labels.end()) { //this label doesn't exist in uninit_labels yet
		uninit_labels[labelName] = std::vector<uint32_t>();
	}
	uninit_labels[labelName].push_back(index);
}

//given a just read instruction, read all of its inputs; return flase if there's an error occured
bool readInstruction(int instruction, const char*& cur, 
	std::vector<uint32_t>& compiled, 
	std::unordered_map<std::string, uint32_t>& labels,
	std::unordered_map<std::string, std::vector<uint32_t>>& uninit_labels) {
	uint32_t fullInstruction = instruction; //a "full" instruction with instruction, registers, etc
	if (instruction == RSH) {
		compiled.push_back(instruction);
		return true;
	}
	if (instruction == FLY || instruction == NXT || instruction == INN ||
		instruction == OUT || instruction == PSH || instruction == POP) {
		//read 1 reg
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
	} else if (instruction == JMP) {
		//label
		skipToNext(cur);
		const char* next = nextArg(cur);
		std::string label = rangeToString(cur, next);
		cur = next;
		if (labels.find(label) == labels.end()) { //it doesn't exist yet
			createAndPush(compiled.size(), label, uninit_labels);
			//temporarily, this is still 0
		} else {
			fullInstruction |= (labels[label] << 8);
		}
	} else if (instruction == JZE) {
		//reg
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
		//label
		skipToNext(cur);
		next = nextArg(cur);
		std::string label = rangeToString(cur, next);
		cur = next;
		if (labels.find(label) == labels.end()) { //it doesn't exist yet
			createAndPush(compiled.size(), label, uninit_labels);
			//temporarily, this is still 0
		}
		else {
			fullInstruction |= (labels[label] << 16);
		}
	} else if (instruction == MOV) {
		//1 reg
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
		//push back a number
		skipToNext(cur);
		next = nextArg(cur);
		std::string numStr = rangeToString(cur, next);
		cur = next;
		skipToNext(cur);
		//the only instruction in 2 32 memory cells (instruction, immidiate)
		uint32_t im = std::stoi(numStr);
		compiled.push_back(fullInstruction);
		compiled.push_back(im);
		return true;
	} else {
		//push a register, but 3 times
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
		skipToNext(cur);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 16);
		skipToNext(cur);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 24);
	}
	skipToNext(cur);
	compiled.push_back(fullInstruction);
	return true;
}

//first 8 bytes (int & 0xFF) is a command (unless immediate for MOV), next bytes (int & 0xFF00; 0xFF0000; 0xFF000000) are params

//compile to a vector
std::vector<uint32_t> compileProgram(const char* code) {
	std::vector<uint32_t> compiled;
	const char* cur = code;
	std::unordered_map<std::string, uint32_t> labels;
	std::unordered_map<std::string, std::vector<uint32_t>> uninit_labels;
	while (*cur != '\0') {
		skipToNext(cur);
		const char* next = nextToken(cur);
		int nextInstruction = findInstruction(cur, next);
		if (nextInstruction == -1) { //it's a label
			std::string label = rangeToString(cur, next);
			if (labels.find(label) == labels.end()) {
				labels[label] = compiled.size();
				cur = next;
			} else {
				return std::vector<uint32_t>(); //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			}
		} else {
			cur = next;
			bool noError = readInstruction(nextInstruction, cur, compiled, labels, uninit_labels);
			if(!noError) return std::vector<uint32_t>(); //AAAAA
		}
	}
	for (std::pair<std::string, std::vector<uint32_t>> vec : uninit_labels) {
		if (labels.find(vec.first) == labels.end()) {
			return std::vector<uint32_t>(); //AAAAAAAAAAAAAAAAAAAAA
		}
		int jumpTo = labels[vec.first];
		for (uint32_t i : vec.second) {
			if((compiled[i] & 0xFF) == 3) //it it's a regular jump, not conditional
				compiled[i] |= (jumpTo << 8);
			else
				compiled[i] |= (jumpTo << 16);
		}
	}
	return compiled;
}
