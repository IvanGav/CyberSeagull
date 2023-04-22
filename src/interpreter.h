#pragma once

#include "raylib.h"
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

enum Instruction {
	ADD,SUB,REM,JMP,JZE,CEQ,CNE,CGT,CGE,CLT,CLE,MOV,AND,ORR,XOR,MOV,FLY,NXT,RSH,INN,OUT,PSH,POP
};

//convert range to a string
std::string rangeToString(char* start, char* end) {
	std::string str = "";
	while (start != end) {
		str += *start;
		start++;
	}
	return str;
}

//compare if range in char array is equal to a string
bool compare(char* start, char* end, std::string str) {
	if (end - start != str.size()) return false;
	for (int i = 0; i < str.size(); i++)
		if (str[i] != *(start + i)) return false;
	return true;
}

//return a number of the next register given a range (-1 if invalid)
int getRegister(char* start, char* end) {
	if (start - end != 2) return -1;
	if (*start != 'R') return -1;
	return *(start + 1) - '0';
}

//get next token and put into 'buffer'
char* nextToken(char* cur) {
	while (!(*cur == ' ' || *cur == ':' || *cur == '\0')) {
		cur++;
	}
	return cur;
}

//get next number (or register) and put into 'buffer'
char* nextArg(char* cur) {
	while (!(*cur == '\n' || *cur == ',' || *cur == '\0')) {
		cur++;
	}
	return cur;
}

//skip to next non space character (or :)
void skipToNext(char*& cur) {
	while (*cur != '\n' || *cur != ' ' || *cur != ':' || *cur == '\0') {
		cur++;
	}
}

//get number of instruction represented by a range (or -1 if not found)
int findInstruction(char* start, char* end) {
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

void createAndPush(int index, std::string labelName, std::unordered_map<std::string, std::vector<uint8_t>>& uninit_labels) {
	if (uninit_labels.find(labelName) == uninit_labels.end()) { //this label doesn't exist in uninit_labels yet
		uninit_labels[labelName] = std::vector<uint8_t>();
	}
	uninit_labels[labelName].push_back(index);
}

//given a just read instruction, read all of its inputs
void readInstruction(int instruction, char*& cur, 
	std::vector<uint8_t>& compiled, 
	std::unordered_map<std::string, uint8_t>& labels,
	std::unordered_map<std::string, std::vector<uint8_t>>& uninit_labels) {
	compiled.push_back(instruction);
	if (instruction == RSH) return;
	if (instruction == FLY || instruction == NXT || instruction == INN ||
		instruction == OUT || instruction == PSH || instruction == POP) {
		//read 1 reg
		char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		cur = next;
		compiled.push_back(reg);
		compiled.push_back(0); //to make all instructions 4 bytes
		compiled.push_back(0);
	} else if (instruction == JMP) {
		//label
		char* next = nextArg(cur);
		std::string label = rangeToString(cur, next);
		cur = next;
		if (labels.find(label) == labels.end()) { //it doesn't exist yet
			createAndPush(compiled.size(), label, uninit_labels);
			compiled.push_back(0); //temp
		} else {
			compiled.push_back(labels[label]);
		}
		compiled.push_back(0);
		compiled.push_back(0);
	} else if (instruction == JZE) {
		//reg
		char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		cur = next;
		compiled.push_back(reg);
		//label
		char* next = nextArg(cur);
		std::string label = rangeToString(cur, next);
		cur = next;
		if (labels.find(label) == labels.end()) { //it doesn't exist yet
			createAndPush(compiled.size(), label, uninit_labels);
			compiled.push_back(0); //temp
		}
		else {
			compiled.push_back(labels[label]);
		}
		compiled.push_back(0);
	} else if (instruction == MOV) {
		//1 reg
		char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		cur = next;
		compiled.push_back(reg);
		compiled.push_back(0); //to make all instructions 4 bytes
		compiled.push_back(0);
		//push back a number
		char* next = nextArg(cur);
		std::string numStr = rangeToString(cur, next);
		cur = next;
		int im = std::stoi(numStr);
		compiled.push_back(im);
		compiled.push_back(im >> 8);
		compiled.push_back(im >> 16);
		compiled.push_back(im >> 24);
	} else {
		//push a register, but 3 times
		char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		cur = next;
		compiled.push_back(reg);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		cur = next;
		compiled.push_back(reg);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		cur = next;
		compiled.push_back(reg);
	}
}

//compile to a vector
std::vector<uint8_t> compileProgram(char* code) {
	std::vector<uint8_t> compiled;
	char* cur = code;
	std::unordered_map<std::string, uint8_t> labels;
	std::unordered_map<std::string, std::vector<uint8_t>> uninit_labels;
	while (*cur != '\0') {
		char* next = nextToken(cur);
		int nextInstruction = findInstruction(cur, next);
		if (nextInstruction == -1) { //it's a label
			std::string label = rangeToString(cur, next);
			if (labels.find(label) == labels.end()) {
				labels[label] = cur - code;
			} else {
				return std::vector<uint8_t>(); //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			}
		} else {
			readInstruction(nextInstruction, cur, compiled, labels, uninit_labels);
		}
	}
	for (std::pair<std::string, std::vector<uint8_t>> vec : uninit_labels) {
		int labelNum = labels[vec.first];
		if (labelNum == 0) {
			return std::vector<uint8_t>(); //AAAAAAAAAAAAAAAAAAAAA and not 0, but whatever the default is
		}
		for (uint8_t i : vec.second) {
			compiled[i] = labelNum;
		}
	}
	return compiled;
}
