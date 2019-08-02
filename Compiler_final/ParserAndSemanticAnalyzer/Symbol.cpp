#include "Symbol.h"
Symbol::Symbol(int sym_type,string content,int row)
{
	this->sym_type = sym_type;
	this->content = content;
	this->row = row;
}
Symbol::~Symbol()
{
}
