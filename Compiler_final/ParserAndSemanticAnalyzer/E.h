#pragma once
#include "Symbol.h"
#include <vector>
class E :
	public Symbol
{
public:
	string place;
	string type;
	vector<int> truelist;
	vector<int> falselist;
	E(int row);
	virtual ~E();
};

