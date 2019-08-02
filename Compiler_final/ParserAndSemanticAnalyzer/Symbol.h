#pragma once
#include "structDefine.h"
using namespace std;
class Symbol
{
public:
	int sym_type;
	string content;
	int row;//如果是非终结符，则row等于规约产生它的项目中的随便一个
	Symbol(int type,string content,int row);
	virtual ~Symbol();
};

