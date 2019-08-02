#pragma once
#include "Symbol.h"
#include <vector>
class N :
	public Symbol
{
public:
	vector<int> nextlist;
	N(int row);
	virtual ~N();
};

