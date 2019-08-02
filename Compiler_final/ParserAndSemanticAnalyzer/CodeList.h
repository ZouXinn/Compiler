#pragma once
#include "structDefine.h"
#include <vector>
using namespace std;
struct Code {
	string s1;
	string s2;
	string s3;
	string s4;
};
class CodeList
{
	//varieties
public:
	vector<Code> m_codeList;
	int nextquad;
	void gen(string s1, string s2, string s3, string s4);
	void gen(Code code);
	void printCodes();
	CodeList();
	CodeList(int nextquad);
	void writeCodes(const char* path);
	void backpatch(vector<int> list, int index);
	void connect(CodeList* list);
};

