#pragma once
#include "Symbol.h"
#include <string>
using namespace std;
class ID : public Symbol
{
public:
	string name;
	string type;
	bool changable;
	string value;
	int level;
	ID(string name,int level,int row);
	virtual ~ID();
};

