#pragma once
#include "Symbol.h"
#include <vector>
class S :
	public Symbol
{
public:
	vector<int> nextlist;
	S(int row);
	virtual ~S();
};

