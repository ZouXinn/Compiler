#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "structDefine.h"
#include "headerForAutoDFA.h"
using namespace std;
vector<Pro> productions;
int I_index = 0;
extern map<DicIndex, string> SLR1_table;
vector<I> DFA;
map<string, vector<string>> follow;
char* str;
int curProIndex = 0;
const char* proPath = "productions.txt";
const char* SLR1_path = "SLR1.txt";
int testIndex = 0;
int DFA_SIZE = 0;

I createNewI() {
	I ret;
	ret.index = I_index;
	I_index++;
	return ret;
}
I createNewI(vector<Pro> productions) {
	I ret;
	ret.index = I_index;
	I_index++;
	for (int i = 0; i < productions.size(); i++) {
		ret.pros.push_back(productions[i]);
	}
	return ret;
}
void derive(I &i) {//传入里面已经有产生式的I ，进行派生
	for (int k = 0; k < i.pros.size(); k++) {
		if (i.pros[k].dotIndex < i.pros[k].right.size()) {//不是归约项目 当为空时，0<0不满足，故不会做
			bool isVt = i.pros[k].right[i.pros[k].dotIndex].isVt;
			if (!isVt) {//是非终结符，要加入新的
				string aim = i.pros[k].right[i.pros[k].dotIndex].content;
				for (int j = 0; j < productions.size(); j++) {
					if (productions[j].left.content.compare(aim) == 0) {
						bool canAdd = true;
						for (int m = 0; m < i.pros.size(); m++) {
							if (theSamePro(i.pros[m], productions[j])) {
								canAdd = false;
								break;
							}
						}
						if (canAdd) {
							i.pros.push_back(productions[j]);
						}						
					}
				}
			}
		}		
	}
}
bool theSamePro(Pro p1,Pro p2) {
	if (p1.left.content.compare(p2.left.content) == 0 && p1.dotIndex == p2.dotIndex && p1.right.size() == p2.right.size()) {
		for (int i = 0; i < p1.right.size(); i++) {
			if (p1.right[i].content.compare(p2.right[i].content) != 0) {//不管是不是Vt了，只要content相同isVt就一定相同
				return false;
			}
		}
		return true;
	}
	return false;
}
bool theSameIs(I i1,I i2) {//判断两个I是否相同
	if (i1.pros.size() != i2.pros.size()) {
		return false;
	}
	for (int i = 0; i < i1.pros.size(); i++) {
		bool canMatch = false;
		for (int j = 0; j < i2.pros.size(); j++) {
			if (theSamePro(i1.pros[i], i2.pros[j])) {
				canMatch = true;
				break;
			}
		}
		if (!canMatch) {
			return false;
		}
	}
	return true;
}
int alreadyInDFA(I i) {//看一个派生后的I是否在DFA中，如果在，返回其DFA中的下标，否则返回-1
	for (int j = 0; j < DFA_SIZE ; j++) {
		if (theSameIs(i, DFA[j])) {
			return j;
		}
	}
	return -1;
}
void achieve(I& i) {//获得i的行为，如移进规约等等
	int DFAIndex = i.index;
	const int account = i.pros.size();
	map<int, bool> m_map;
	for (int j = 0; j < account; j++) {//先处理归约项目
		if (DFA[DFAIndex].pros[j].dotIndex == DFA[DFAIndex].pros[j].right.size()) {
			behave b;
			b.aim = getGuiYueProductionIndex(DFA[DFAIndex].pros[j]);;
			if (b.aim == -1) {
				cout << "something wrong with DFA!" << endl;
				system("pause");
				exit(1);
			}
			b.behavior = 2;
			b.value = DFA[DFAIndex].pros[j].left.content;//代表归约项目
			m_map[j] = true;
			DFA[DFAIndex].behaviors.push_back(b);
		}
	}
	for (int j = 0; j < account; j++) {//处理移进项目
		int k = j;
		while (m_map.count(k) == 1 && k < DFA[DFAIndex].pros.size()) {
			k++;//找到第一个没有移进过的
		}
		if (k < DFA[DFAIndex].pros.size()&& DFA[DFAIndex].pros[k].dotIndex < DFA[DFAIndex].pros[k].right.size()) {//如果是移进项目，则处理他
			vector<Pro> v;//记录了移进当前符号后的产生式 如A->a.b 则记录的是A->ab.  当然，不一定只有一条记录
			Sym s = DFA[DFAIndex].pros[k].right[DFA[DFAIndex].pros[k].dotIndex];//需要的Sym
			Pro p = DFA[DFAIndex].pros[k];//第k条产生式——即将移进它
			p.dotIndex++;
			v.push_back(p);
			m_map[k] = true;
			k++;
			while (k < account) {//后面的如果sym的content和s的content一样，则一定没有加入m_map,daans
				if (m_map.count(k) == 0 && DFA[DFAIndex].pros[k].right[DFA[DFAIndex].pros[k].dotIndex].content.compare(s.content) == 0) {
					Pro pt = DFA[DFAIndex].pros[k];
					pt.dotIndex++;
					v.push_back(pt);
					m_map[k] = true;
				}
				k++;
			}
			if (v.size() > 0) {
				I i1 = createNewI(v);
				int index = DFA[DFAIndex].index;
				derive(i1);//先将i1派生了
				int indexOfDFA = alreadyInDFA(i1);
				if (indexOfDFA == -1) {//不在DFA中
					DFA.push_back(i1);//一会儿如果判断出i1已经在DFA中，要记得将I_index--
					DFA_SIZE++;
					behave b;
					b.behavior = s.isVt ? 0 : 1;
					b.aim = i1.index;
					b.value = s.content;
					DFA[index].behaviors.push_back(b);
				}
				else {//在DFA中
					behave b;
					I_index--;
					b.behavior = s.isVt ? 0 : 1;
					b.aim = indexOfDFA;
					b.value = s.content;
					DFA[index].behaviors.push_back(b);
				}				
			}
		}		
	}
}
void createDFA() {
	I i0 = createNewI();
	if (productions.size() <= 0) {
		cout << "no productions!" << endl;
		system("pause");
		exit(1);
	}
	i0.pros.push_back(productions[0]);
	derive(i0);
	DFA.push_back(i0);
	DFA_SIZE++;
	achieve(i0);
	for (int j = 1; j < DFA_SIZE; j++) {
		//derive(DFA[j]);//派生		一会儿将achieve改进完成后，这一句应该会删掉
		achieve(DFA[j]);//获得行为并加入新的
	}
}
void test() {
	
	readProductions(proPath);
	readSLR1_table(SLR1_path);


	//readProductions("createDFATest.txt");

	/*createDFA();
	initFollow();
	getSLR1_table();
	writeSLR1_table(SLR1_path);*/
}
int getGuiYueProductionIndex(Pro pro) {//根据产生式获得其规约项目的下标
	for (int i = 0; i < productions.size(); i++) {
		if ((productions[i].left.content.compare(pro.left.content) != 0) ||
			(productions[i].right.size() != pro.right.size())) {
			continue;
		}
		else {
			bool theSame = true;
			for (int j = 0; j < productions[i].right.size(); j++) {
				if (productions[i].right[j].content.compare(pro.right[j].content) != 0) {
					theSame = false;
					break;
				}
			}
			if (theSame) {
				return i;
			}
		}
	}
	return -1;
}
void writeSLR1_table(const char* path) {
	fstream out;
	out.open(path, ios::out);
	if (out.is_open()) {
		map<DicIndex, string>::iterator it;
		it = SLR1_table.begin();
		while (it != SLR1_table.end()) {
			out << '[' << it->first.state << '#' << it->first.sym << '#' << it->second << "]\n";
			it++;
		}
		out << '$';
		out.close();
	}	
}
void readSLR1_table(const char* path) {
	fstream in;
	in.open(path, ios::in);
	in.seekg(0, ios::end);//移动到末尾
	int length = in.tellg();//获得长度
	in.seekg(0, ios::beg);//移动到开头
	char* buffer = new char[length];
	in.read(buffer, length);
	in.close();
	int num = 0;
	char c = *buffer;
	while (c != '$') {
		if (c != '[') {
			cout << "[ worng" << endl;
			doErrorInSLRReading();
		}
		buffer++;
		num++;
		c = *buffer;
		string s = "";
		while (c != '#') {
			s += c;
			buffer++;
			num++;
			c = *buffer;
		}
		DicIndex dicIndex;
		dicIndex.state = strToInt(s);
		s = "";
		buffer++;
		num++;
		c = *buffer;
		while (c != '#') {
			s += c;
			buffer++;
			num++;
			c = *buffer;
		}
		dicIndex.sym = s;
		s = "";
		buffer++;
		num++;
		c = *buffer;
		while (c != ']') {
			s += c;
			buffer++;
			num++;
			c = *buffer;
		}
		buffer++;
		num++;
		if (*buffer != '\n') {
			cout << "enter wrong" << endl;
			doErrorInSLRReading();
		}
		else {
			SLR1_table[dicIndex] = s;
			buffer++;
			num++;
			c = *buffer;
		}
	}
}
int strToInt(string s) {
	int ret = 0;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] > '9' || s[i] < '0') {
			doErrorInSLRReading();
		}
		else {
			int t = s[i] - '0';
			ret *= 10;
			ret += t;
		}
	}
	return ret;
}
void doErrorInSLRReading() {
	cout << "something wrong while reading SLR" << endl;
	system("pause");
	exit(0);
}
void show(I i) {
	for (int j = 0; j < i.pros.size(); j++) {
		cout << i.pros[j].left.content << " ::= ";
		for (int k = 0; k < i.pros[j].right.size(); k++) {
			if (k == i.pros[j].dotIndex) {
				cout << ".";
			}
			cout << i.pros[j].right[k].content<<" ";
		}
		if (i.pros[j].dotIndex == i.pros[j].right.size()) {
			cout << ".";
		}
		cout << endl;
	}
}
void getSLR1_table() {
	//里面如果有项目冲突，要根据自己的文法要求来做
	for (int i = 0; i < DFA_SIZE ; i++) {
		for (int j = 0; j < DFA[i].behaviors.size(); j++) {			
			if (DFA[i].behaviors[j].behavior == 0) {//移进终结符
				DicIndex dicIndex;
				dicIndex.state = DFA[i].index;
				dicIndex.sym = DFA[i].behaviors[j].value;
				int aimState = DFA[i].behaviors[j].aim;
				string s = "S" + to_string(aimState);
				if (SLR1_table.count(dicIndex) == 1) {//已存在，需解决冲突
					cout << "the table at(" << dicIndex.state << "," << dicIndex.sym << ") is already " <<
						SLR1_table[dicIndex] << endl;
					cout << "but we are going to insert " << s << " at that place" << endl;
					cout << "the state is:" << endl;
					show(DFA[i]);
					cout << "press \"y\" to insert new and \"n\" to keep old" << endl;
					char c = '\0';
					cin >> c;
					while (true) {
						if (c == 'y') {//可以写进文本，之后就不用再输入
							SLR1_table[dicIndex] = s;
							break;
						}
						else if (c == 'n') {
							break;
						}
						else {
							cout << "please give the right input!" << endl;
						}
					}
				}
				else {//不存在，直接加入
					SLR1_table[dicIndex] = s;
				}
			}
			else if (DFA[i].behaviors[j].behavior == 1) {//移进非终结符
				DicIndex dicIndex;
				dicIndex.state = DFA[i].index;
				dicIndex.sym = DFA[i].behaviors[j].value;
				int aimState = DFA[i].behaviors[j].aim;
				string s = to_string(aimState);
				if (SLR1_table.count(dicIndex) == 1) {//已存在，需解决冲突,应该是不会存在的
					cout << "the table at(" << dicIndex.state << "," << dicIndex.sym << ") is already " <<
						SLR1_table[dicIndex] << endl;
					cout << "but we are going to insert " << s << " at that place" << endl;
					cout << "the state is:" << endl;
					show(DFA[i]);
					cout << "press \"y\" to insert new and \"n\" to keep old" << endl;
					char c = '\0';
					cin >> c;
					while (true) {
						if (c == 'y') {//可以写进文本，之后就不用再输入
							SLR1_table[dicIndex] = s;
							break;
						}
						else if (c == 'n') {
							break;
						}
						else {
							cout << "please give the right input!" << endl;
						}
					}
				}
				else {//不存在，直接加入
					SLR1_table[dicIndex] = s;
				}
			}
			else if (DFA[i].behaviors[j].behavior == 2) {//归约
				//要单独提出acc状态
				DicIndex dicIndex;
				dicIndex.state = DFA[i].index;
				if (dicIndex.state == 1) {//acc
					dicIndex.sym = "$";
					string s = "acc";
					SLR1_table[dicIndex] = s;
				}
				else {//其他归约项目
					int aimProduction = DFA[i].behaviors[j].aim;
					string s = "r" + to_string(aimProduction);
					//要归约的左部符号
					string left = DFA[i].behaviors[j].value;
					//要先求左部符号follow集
					vector<string> follows = follow[left];
					for (int k = 0; k < follows.size(); k++) {
						dicIndex.sym = follows[k];
						if (SLR1_table.count(dicIndex) == 1) {//已存在，需解决冲突,应该是不会存在的
							//分情况讨论，要根据文法讨论
							cout << "the table at(" << dicIndex.state << "," << dicIndex.sym << ") is already " <<
								SLR1_table[dicIndex] << endl;
							cout << "but we are going to insert " << s << " at that place" << endl;
							cout << "the state is:" << endl;
							show(DFA[i]);
							cout << "press \"y\" to insert new and \"n\" to keep old" << endl;
							char c = '\0';
							cin >> c;
							while (true) {
								if (c == 'y') {//可以写进文本，之后就不用再输入
									SLR1_table[dicIndex] = s;
									break;
								}
								else if (c == 'n') {
									break;
								}
								else {
									cout << "please give the right input!" << endl;
								}
							}
						}
						else {
							SLR1_table[dicIndex] = s;
						}
					}
				}
			}
		}
	}
}

void initFollow() {
	vector<string> v;
	//F
	v.push_back("$");
	follow["F"] = v;
	//X
	follow["X"] = v;
	//V
	v.clear();
	v.push_back(",");
	v.push_back(";");
	follow["V"] = v;
	//C
	v.clear();
	v.push_back(";");
	follow["C"] = v;
	//T
	v.clear();
	v.push_back("id");
	follow["T"] = v;
	//L
	v.clear();
	v.push_back("}");
	v.push_back("id");
	v.push_back("var");
	v.push_back("const");
	v.push_back("if");
	v.push_back("while");
	follow["L"] = v;
	//S
	v.clear();
	v.push_back("}");
	v.push_back("id");
	v.push_back("var");
	v.push_back("const");
	v.push_back("if");
	v.push_back("while");
	follow["S"] = v;
	//M
	v.clear();
	v.push_back("{");
	v.push_back("(");
	v.push_back("id");
	v.push_back("cid");
	v.push_back("var");
	v.push_back("const");
	v.push_back("if");
	v.push_back("while");
	v.push_back("-");
	v.push_back("!");
	follow["M"] = v;
	//N
	v.clear();
	v.push_back("else");
	follow["N"] = v;
	//A
	v.clear();
	v.push_back(";");
	follow["A"] = v;
	//E
	v.clear();
	v.push_back(")");
	v.push_back(";");
	v.push_back("+");
	v.push_back("-");
	v.push_back("*");
	v.push_back("/");
	v.push_back("%");
	v.push_back("||");
	v.push_back("&&");
	v.push_back(">");
	v.push_back("<");
	v.push_back(">=");
	v.push_back("<=");
	v.push_back("==");
	v.push_back("!=");
	follow["E"] = v;
}
void readProductions(const char* path) {
	fstream in;
	in.open(path, ios::in);
	in.seekg(0, ios::end);//移动到末尾
	int length = in.tellg();//获得长度
	in.seekg(0, ios::beg);//移动到开头
	char* buffer = new char[length];
	in.read(buffer, length);
	in.close();
	str = buffer;
	Pro p = getNextPro();
	while (p.dotIndex != -1) {
		productions.push_back(p);
		p = getNextPro();
	}
}
char nextChar() {
	if (*(str - 1) == '$') {
		return '$';
	}
	char ret = *str;
	str++;
	return ret;
}
Pro getNextPro() {
	string s = "";
	char c = nextChar();
	if (c == '$') {
		Pro p;
		p.dotIndex = -1;//-1表示结束
		return p;
	}
	else if (c < 'A' || c > 'Z') {
		curProIndex++;
		cout << "第" << curProIndex << "行文法定义出错" << endl;
		system("pause");
		exit(0);
	}
	curProIndex++;
	s += c;
	Pro p;
	p.left.content = s;
	p.left.isVt = false;
	c = nextChar();
	if (c != ' ') {
		cout << "第" << curProIndex << "行文法定义出错" << endl;
		system("pause");
		exit(0);
	}
	s = "";
	for (int i = 0; i < 3; i++) {
		c = nextChar();
		s += c;
	}
	if (s.compare("::=") != 0) {
		cout << "第" << curProIndex << "行文法定义出错" << endl;
		system("pause");
		exit(0);
	}
	c = nextChar();
	if (c != ' ') {
		cout << "第" << curProIndex << "行文法定义出错" << endl;
		system("pause");
		exit(0);
	}
	s = "";
	c = nextChar();
	while (c != '\n') {
		while (c != ' ' && c != '\n') {
			s += c;
			c = nextChar();
		}
		if (s.length() == 0) {
			cout << "第" << curProIndex << "行文法定义出错" << endl;
			system("pause");
			exit(0);
		}
		else {
			Sym sym;
			sym.content = s;
			if (s.length() == 1 && s[0] >= 'A' && s[0] <= 'Z') {
				sym.isVt = false;
			}
			else {
				sym.isVt = true;
			}
			p.right.push_back(sym);
		}
		if (c == ' ') {
			c = nextChar();
			if (c == '\n') {
				cout << "第" << curProIndex << "行文法定义出错" << endl;
				system("pause");
				exit(0);
			}
			s = "";
		}
	}
	return p;
}