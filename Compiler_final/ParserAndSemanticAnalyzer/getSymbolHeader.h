#pragma once
#include <string>
using namespace std;
struct Text
{
	char* pC;
	int length;
};
Text readText(const char* fileName);
void init();
void end();
bool getSymbol();
bool isBlank(char c);//no need
char read();//no need
void retrail();//no need
void writeSymbolTable();
void writeBinSymbolTable(); 
