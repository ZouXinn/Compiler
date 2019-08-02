#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "structDefine.h"


struct Sym {
	bool isVt;//终结符？
	string content;
};
struct Pro
{
	Sym left;
	int dotIndex = 0;//初始时为0
	vector<Sym> right;
};
struct behave
{
	int behavior = -1;//0 移进终结符  1 移进非终结符   2 归约
	string value;//移进的符号 或者 归约产生式的左部符号
	int aim = -1;//移进之后到达的状态和归约的产生式序号
};
struct I
{
	int index = -1;
	vector<Pro> pros;
	vector<behave> behaviors;
};


I createNewI();
I createNewI(vector<Pro> productions);
void derive(I& i);
int getGuiYueProductionIndex(Pro pro);
void achieve(I& i);
void createDFA();
char nextChar();
Pro getNextPro();
void readProductions(const char* path);
void getSLR1_table();
void test();
void initFollow();
int alreadyInDFA(I i);
bool theSameIs(I i1, I i2);
bool theSamePro(Pro p1, Pro p2);
void show(I i);
void readSLR1_table(const char* path);
void writeSLR1_table(const char* path);
void doErrorInSLRReading();
int strToInt(string s);
