#include "Cid.h"



Cid::Cid(int row, string value):Symbol(-1,"Cid",row)
{
	this->value = value;
	this->type = "";
}


Cid::~Cid()
{
}
