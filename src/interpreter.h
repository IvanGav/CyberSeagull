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
		if (str[i] != *(start + i)) return false;
	return true;
}

//return a number of the next register given a range (-1 if invalid)
int getRegister(const char* start, const char* end) {
	if (end - start != 2) return -1;
	if (*start != 'R') return -1;
	return *(start + 1) - '0';
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

//given a just read instruction, read all of its inputs
void readInstruction(int instruction, const char*& cur, 
	std::vector<uint32_t>& compiled, 
	std::unordered_map<std::string, uint32_t>& labels,
	std::unordered_map<std::string, std::vector<uint32_t>>& uninit_labels) {
	uint32_t fullInstruction = instruction; //a "full" instruction with instruction, registers, etc
	if (instruction == RSH) {
		compiled.push_back(instruction);
		return;
	}
	if (instruction == FLY || instruction == NXT || instruction == INN ||
		instruction == OUT || instruction == PSH || instruction == POP) {
		//read 1 reg
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
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
		return;
	} else {
		//push a register, but 3 times
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		cur = next;
		fullInstruction |= (reg << 8);
		skipToNext(cur);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		cur = next;
		fullInstruction |= (reg << 16);
		skipToNext(cur);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		cur = next;
		fullInstruction |= (reg << 24);
	}
	skipToNext(cur);
	compiled.push_back(fullInstruction);
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
				labels[label] = cur - code;
				cur = next;
			} else {
				return std::vector<uint32_t>(); //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			}
		} else {
			cur = next;
			readInstruction(nextInstruction, cur, compiled, labels, uninit_labels);
		}
	}
	for (std::pair<std::string, std::vector<uint32_t>> vec : uninit_labels) {
		int labelNum = labels[vec.first];
		if (labelNum == 0) {
			return std::vector<uint32_t>(); //AAAAAAAAAAAAAAAAAAAAA and not 0, but whatever the default is
		}
		for (uint32_t i : vec.second) {
			compiled[i] = labelNum;
		}
	}
	return compiled;
}
