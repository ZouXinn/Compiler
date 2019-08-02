#include "NewTemp.h"

int NewTemp::s_index = 1;
string NewTemp::newtemp() {
	string s = "T";
	s += to_string(s_index);
	s_index++;
	return s;
}
