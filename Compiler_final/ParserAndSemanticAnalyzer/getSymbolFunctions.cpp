#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include "structDefine.h"
#include "getSymbolHeader.h"
using namespace std;
const char* symTabPath = "symbolTable.txt";
const string reservedCharacter[] = { "var","const","int","real","bool","main","if","else","while","void","true","false" };//保留字
const int reservedCharacterCount = 12;//保留字数目
extern vector<symbol> symList;
Text text;
int row = 1;
Text readText(const char* fileName) {
	fstream in;
	in.open(fileName, ios::in);
	in.seekg(0, ios::end);//移动到末尾
	int length = in.tellg();//获得长度
	in.seekg(0, ios::beg);//移动到开头
	char* buffer = new char[length];
	in.read(buffer, length);
	in.close();
	Text ret;
	ret.length = length;
	ret.pC = buffer;
	return ret;
}
void init() {
	text = readText("test.txt");
	text.pC--;
	//......
}
void end() {

}
bool isBlank(char c) {//   \n加不加进去？？？？
	return c == ' ' || c == '\t' || c == '\n';
}
void error(int row,int flag) {//0 词法   1 语法   2或以上 语义
	if (flag == 0) {
		cout << "词法分析出错," << "行号：" << row << endl;
	}
	else if (flag == 1) {
		cout << "语法分析出错," << "行号：" << row << endl;
	}
	else {
		cout << "语义分析出错," << "行号：" << row << endl;
	}
	system("pause");
	exit(1);
}
void error(int row, int flag,string detail) {//0 词法   1 语法   2或以上 语义
	if (flag == 0) {
		cout << "词法分析出错," << "行号：" << row << '\t' <<detail<<endl;
	}
	else if (flag == 1) {
		cout << "语法分析出错," << "行号：" << row << '\t' << detail << endl;
	}
	else {
		cout << "语义分析出错," << "行号：" << row << '\t' << detail << endl;
	}
	system("pause");
	exit(1);
}
bool inReservedCharacter(string s) {
	for (int i = 0; i < reservedCharacterCount; i++) {
		if (reservedCharacter[i].compare(s) == 0) {
			return true;
		}
	}
	return false;
}
char read() {
	text.pC++;
	return *text.pC;
}
void retrail() {
	text.pC--;
}
bool getSymbol() {//返回是否到达底部
	char c = read();
	string s = "";
	char c1;
	while (isBlank(c)) {//c是空格或table或\n
		if (c == '\n') {
			row++;
		}
		c = read();
	}
	symbol sym;
	switch (c)
	{
	case ';'://         ;
		sym.type = 5;
		sym.content = ";";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '{':
		sym.type = 5;
		sym.content = "{";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '}':
		sym.type = 5;
		sym.content = "}";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case ':':
		error(row, 0, "不允许的符号:");
		sym.type = 5;
		sym.content = ":";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '+'://                              运算符
		c = read();
		if (c == '+') {//++
			sym.type = 6;
			sym.content = "++";
			sym.row = row;
			symList.push_back(sym);
		}
		else {//+
			retrail();
			sym.type = 6;
			sym.content = "+";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '-'://    -     --
		c = read();
		if (c == '-') {//--
			sym.type = 6;
			sym.content = "--";
			sym.row = row;
			symList.push_back(sym);
		}
		else if (c == '0')
		{
			c = read();
			if (c != '.') {//整数0
				if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'||c == '_') {
					error(row, 0);
				}
				retrail();
				sym.type = 1;
				sym.content = "0";
				sym.row = row;
				symList.push_back(sym);
			}
			else {
				s += "-0.";
				c = read();
				if (c < '0' || c > '9') {//0.x
					error(row,0);
				}
				else
				{
					while (c >= '0' && c <= '9') {
						s += c;
						c = read();
					}
					if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_') {
						error(row, 0);
					}
					retrail();
					sym.type = 2;
					sym.content = s;
					sym.row = row;
					symList.push_back(sym);
				}
			}
		}
		else if (c >= '1' && c <= '9')
		{
			s += '-';
			while (c >= '0' && c <= '9')
			{
				s += c;
				c = read();
			}
			if (c == '.') {//浮点数
				s += '.';
				c = read();
				if (c < '0' || c > '9') {//number.char
					error(row,0);
				}
				else {
					while (c >= '0' && c <= '9') {
						s += c;
						c = read();
					}
					if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'|| c == '_') {
						error(row, 0);
					}
					retrail();
					sym.type = 2;
					sym.content = s;
					sym.row = row;
					symList.push_back(sym);
				}
			}
			else {//非0整数
				if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'|| c == '_') {
					error(row, 0);
				}
				retrail();
				sym.type = 1;
				sym.content = s;
				sym.row = row;
				symList.push_back(sym);
			}
		}
		else {//-
			retrail();
			sym.type = 6;
			sym.content = "-";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '*':
		sym.type = 6;
		sym.content = "*";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '/'://  /   //    /*...*/
		c = read();
		if (c == '/') {//
			while (c != '\n') {
				c = read();
			}
			row++;
		}
		else if (c == '*') {//     /*...*/
			bool ok = false;
			while (!ok) {
				while (c != '*') {
					if (c == '\n') {
						row++;
					}
					c = read();
				}
				c = read();
				if (c == '/') {
					ok = true;
				}
			}
		}
		else {//   /
			retrail();
			sym.type = 6;
			sym.content = "/";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '%':
		sym.type = 6;
		sym.content = "%";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '^':
		error(row, 0, "不允许的符号^");
		sym.type = 6;
		sym.content = "^";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '&':// &   &&
		c = read();
		if (c == '&') {//&&
			sym.type = 6;
			sym.content = "&&";
			sym.row = row;
			symList.push_back(sym);
		}
		else {// &
			error(row, 0, "不允许的符号&");
			retrail();
			sym.type = 6;
			sym.content = "&";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '|'://   |    ||
		c = read();
		if (c == '|') {//||
			sym.type = 6;
			sym.content = "||";
			sym.row = row;
			symList.push_back(sym);
		}
		else {// |
			error(row, 0, "不允许的符号|");
			retrail();
			sym.type = 6;
			sym.content = "|";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '!'://  !=
		c = read();
		if (c == '=') {//!=
			sym.type = 6;
			sym.content = "!=";
			sym.row = row;
			symList.push_back(sym);
		}
		else {// =
			retrail();
			sym.type = 6;
			sym.content = "=";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '<'://  <   <=  <<
		c = read();
		if (c == '=') {//<=
			sym.type = 6;
			sym.content = "<=";
			sym.row = row;
			symList.push_back(sym);
		}
		else if (c == '<') {// <<
			error(row, 0, "不允许的符号<<");
			sym.type = 6;
			sym.content = "<<";
			sym.row = row;
			symList.push_back(sym);
		}
		else {//  <
			retrail();
			sym.type = 6;
			sym.content = "<";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '>'://  >   >=   >>
		c = read();
		if (c == '=') {//>=
			sym.type = 6;
			sym.content = ">=";
			sym.row = row;
			symList.push_back(sym);
		}
		else if (c == '>') {// >>
			error(row, 0, "不允许的符号>>");
			sym.type = 6;
			sym.content = ">>";
			sym.row = row;
			symList.push_back(sym);
		}
		else {//  >
			retrail();
			sym.type = 6;
			sym.content = ">";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '='://   =    ==
		c = read();
		if (c == '=') {//==
			sym.type = 6;
			sym.content = "==";
			sym.row = row;
			symList.push_back(sym);
		}
		else {//=
			retrail();
			sym.type = 6;
			sym.content = "=";
			sym.row = row;
			symList.push_back(sym);
		}
		return false;
	case '[':
		error(row, 0, "不允许的符号[");
		sym.type = 6;
		sym.content = "[";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case ']':
		error(row, 0, "不允许的符号]");
		sym.type = 6;
		sym.content = "]";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '(':
		sym.type = 6;
		sym.content = "(";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case ')':
		sym.type = 6;
		sym.content = ")";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case ',':
		sym.type = 6;
		sym.content = ",";
		sym.row = row;
		symList.push_back(sym);
		return false;
	case '\''://字符常量		
		error(row, 0, "不允许的符号\'");
		c1 = read();//row要++吗
		c = read();
		if (c == '\'') {//正确的字符常量
			s += c1;
			sym.type = 3;
			sym.content = s;
			sym.row = row;
			symList.push_back(sym);
		}
		else {
			error(row,0);//出错
		}
		return false;
	case '\"'://  字符串常量
		error(row, 0, "不允许的符号\"");
		c = read();
		while (c != '\"' && c != '$') {
			s += c;
			if (c == '\n') {
				row++;
			}
			c = read();
		}
		if (c == '\"') {//正确的字符串
			sym.type = 4;
			sym.content = s;
			sym.row = row;
			symList.push_back(sym);
		}
		else {//出错
			error(row,0);
		}
		return false;
	default://    标识符  整数常量  浮点数常量  
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {//标识符或保留字
			s += c;
			c = read();
			while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9')) {
				s += c;
				c = read();
			}
			retrail();
			sym.content = s;
			sym.row = row;
			if (inReservedCharacter(s)) {//保留字
				sym.type = 7;
			}
			else {//标识符
				sym.type = 0;
			}
			symList.push_back(sym);
		}
		else if (c == '0') {
			c = read();
			if (c != '.') {//整数0
				if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'|| c == '_') {
					error(row, 0);
				}
				retrail();
				sym.type = 1;
				sym.content = "0";
				sym.row = row;
				symList.push_back(sym);
			}
			else {
				s += "0.";
				c = read();
				if (c < '0' || c > '9') {//0.x
					error(row,0);
				}
				else
				{
					while (c >= '0' && c <= '9') {
						s += c;
						c = read();
					}
					if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'|| c == '_') {
						error(row, 0);
					}
					retrail();
					sym.type = 2;
					sym.content = s;
					sym.row = row;
					symList.push_back(sym);
				}
			}
		}
		else if (c >= '1' && c <= '9')
		{
			while (c >= '0' && c <= '9')
			{
				s += c;
				c = read();
			}
			if (c == '.') {//浮点数
				s += '.';
				c = read();
				if (c < '0' || c > '9') {//number.char
					error(row,0);
				}
				else {
					while (c >= '0' && c <= '9') {
						s += c;
						c = read();
					}
					if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'|| c == '_') {
						error(row, 0);
					}
					retrail();
					sym.type = 2;
					sym.content = s;
					sym.row = row;
					symList.push_back(sym);
				}
			}
			else {//非0整数
				if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'|| c == '_') {
					error(row, 0);
				}
				retrail();
				sym.type = 1;
				sym.content = s;
				sym.row = row;
				symList.push_back(sym);
			}
		}
		else if (c == '$') {
			sym.type = -1;
			sym.content = "$";
			sym.row = row;
			symList.push_back(sym);
			return true;
		}
		else if (c == '\0') {//读完了
			error(row,0);
		}
		return false;
	}
	return false;
}
void writeSymbolTable() {
	ofstream out;
	out.open(symTabPath, ios::out);
	for (int i = 0; i < symList.size(); i++) {
		out << "code:" << symList[i].type << "\tcontent:" << symList[i].content
			<< "\trow:" << symList[i].row << '\n';
	}
	out.close();
}
void getSymbolFromLexer() {
	init();//初始化输入、输出流等
	while (!getSymbol()) {

	}
	writeSymbolTable();
	end();
}