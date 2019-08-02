#pragma once
#include "Symbol.h"
class Cid : public Symbol
{
public:
	string type;
	string value;
	Cid(int row,string value);
	virtual ~Cid();
};

