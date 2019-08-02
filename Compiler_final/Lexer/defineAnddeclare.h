#pragma once
#include <string>
using namespace std;
struct Text
{
	char * pC;
	int length;
};
struct symbol
{
	int type;
	string content;
	int row;
};
Text readText(const char * fileName);
void init();
void end();
bool getSymbol();
bool isBlank(char c);//no need
char read();//no need
void retrail();//no need
void writeSymbolTable();
void writeBinSymbolTable();