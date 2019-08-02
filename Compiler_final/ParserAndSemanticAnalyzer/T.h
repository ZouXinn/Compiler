#pragma once
#include "Symbol.h"
class T :
	public Symbol
{
public:
	string type;
	T(string type,int row);
	virtual ~T();
};

