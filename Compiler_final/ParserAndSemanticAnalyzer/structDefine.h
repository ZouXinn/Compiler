#pragma once
#include <string>
#include <map>
#include <stdint.h>
class Symbol;
using namespace std;
class ID;
struct symbol//词法分析获得的符号
{
	int type;
	string content;
	int row;
};
struct DicIndex//no need
{
	int state;
	string sym;
	friend bool operator < (const struct DicIndex& d1, const struct DicIndex& d2);
};
struct VarIndex
{
	string name;
	int level;
	friend bool operator < (const struct VarIndex& v1, const struct VarIndex& v2);
};
enum Behavior { S_, r, GOTO, acc, ERROR };
struct todo
{
	Behavior behavior;
	int aim = -1;//若behavior是S或者goto,则表示目标状态；若behavior是r，则表示产生式序号
};
void getSymbolFromLexer();
void error(int row,int flag);
void error(int row, int flag, string detail);
Symbol* popSymbol();
void pushSym_State(Symbol* s, int state);
todo getWhatToDo(symbol sym);
ID* getHighestLevelVar(string name);
void clearVarTableAtLevel(int level);
