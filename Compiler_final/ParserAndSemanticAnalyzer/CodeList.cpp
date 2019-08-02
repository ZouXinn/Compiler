#include "CodeList.h"
#include <iostream>
#include <fstream>
using namespace std;

CodeList::CodeList() {
	nextquad = 0;
	//this->m_codeList = new vector
}
CodeList::CodeList(int nextquad) {
	this->nextquad = nextquad;
	//this->m_codeList = new vector
}
void CodeList::gen(string s1, string s2, string s3, string s4) {
	Code newCode;
	newCode.s1 = s1;
	newCode.s2 = s2;
	newCode.s3 = s3;
	newCode.s4 = s4;
	m_codeList.push_back(newCode);
	nextquad++;
}
void CodeList::gen(Code code) {
	m_codeList.push_back(code);
	nextquad++;
}
void CodeList::connect(CodeList* list) {//在外创建list的时候一定要注意吧curNextquad改成当前nextquad
										//还要注意递归调用
	for (int i = 0; i < list->m_codeList.size(); i++) {
		Code code = list->m_codeList[i];
		char c = code.s4[0];
		if (c >= '0' && c <= '9') {
			int index = 0;
			for (int j = 0; j < code.s4.length(); j++) {
				int tN = code.s4[j] - '0';
				index *= 10;
				index += tN;
			}
			int aimIndex = index + this->nextquad;
			code.s4 = to_string(aimIndex);
		}
		gen(code);
	}
}
void CodeList::printCodes() {
	for (int i = 0; i < m_codeList.size(); i++) {
		cout <<'('<<i<<')' <<'(' << m_codeList[i].s1 << ',' << m_codeList[i].s2 << ',' << 
			m_codeList[i].s3 << ',' << m_codeList[i].s4 << ')' << endl;
	}
}
void CodeList::writeCodes(const char* path) {
	ofstream out;
	out.open(path, ios::out);
	for (int i = 0; i < m_codeList.size(); i++) {
		out<< '(' << i << ')' << '(' << m_codeList[i].s1 << ',' << m_codeList[i].s2 << ',' <<
			m_codeList[i].s3 << ',' << m_codeList[i].s4 << ')' << endl;
	}
	out.close();
}
void CodeList::backpatch(vector<int> list, int index) {
	for (int i = 0; i < list.size(); i++) {
		m_codeList[list[i]].s4 = to_string(index);
	}
}
