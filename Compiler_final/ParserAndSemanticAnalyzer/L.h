#pragma once
#include "Symbol.h"
#include <vector>
class L :
	public Symbol
{
public:
	vector<int> nextlist;
	L(int row);
	virtual ~L();
};

