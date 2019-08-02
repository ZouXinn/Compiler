#include "structDefine.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "Symbol.h"
#include "CodeList.h"
#include "NewTemp.h"
#include "headerForAutoDFA.h"
#include "ID.h"
#include "Cid.h"
#include "OtherSym.h"
#include "X.h"
#include "V.h"
#include "C.h"
#include "L.h"
#include "A.h"
#include "E.h"
#include "M.h"
#include "N.h"
#include "S.h"
#include "T.h"
#include "V.h"
#include "X.h"
using namespace std;

map<DicIndex, string> SLR1_table;
vector<symbol> symList;//分析时存目标字符串的vector
vector<Symbol*>* symStack = new vector<Symbol*>();//符号栈
vector<int>* stateStack = new vector<int>();//状态栈
CodeList* mainCodeList = new CodeList();
map<VarIndex, ID*>* var_table = new map<VarIndex, ID*>();
int stackTop = -1;
extern vector<Pro> productions;
const char* writeCodePath = "codes.txt";
int curLevel = 0;
Symbol* popSymbol() {
	Symbol* ret = (*symStack)[stackTop];
	symStack->pop_back();
	stateStack->pop_back();
	stackTop--;
	return ret;
}
void pushSym_State(Symbol* s, int state) {
	stackTop++;
	symStack->push_back(s);
	stateStack->push_back(state);
}
void clearVarTableAtLevel(int level) {
	for (map<VarIndex, ID*>::iterator it = var_table->begin(); it != var_table->end(); ) {
		if (it->first.level == level) {
			var_table->erase(it++);
		}
		else {
			it++;
		}
	}
}
ID* getHighestLevelVar(string name) {
	map<VarIndex, ID*>::iterator it;
	it = var_table->begin();
	int retLevel = -1;
	ID* ret = nullptr;
	while (it != var_table->end()) {
		if (it->first.name == name&& it->first.level>retLevel) {
			retLevel = it->first.level;
			ret = it->second;
		}
		it++;
	}
	return ret;
}
todo getWhatToDo(symbol sym) {
	DicIndex index;
	index.state = (*stateStack)[stackTop];
	if (sym.type == 0) {//标识符
		index.sym = "id";
	}
	else if (sym.type >= 1 && sym.type <= 4) {//常量
		index.sym = "cid";
	}
	else if (sym.type == 7 && (sym.content.compare("true") == 0 || sym.content.compare("false") == 0)) {
		index.sym = "cid";
	}
	else {//界限符，运算符，保留字   以及非终结符
		index.sym = sym.content;
	}
	todo ret;
	if (SLR1_table.count(index) == 0) {//不在表中，报错
		error(sym.row,1);
		ret.behavior = ERROR;
		return ret;
	}
	else {
		string s = SLR1_table[index];
		int length = s.length();
		char c = s[0];
		if (c == 'S') {//移进项目
			ret.behavior = S_;
			int aimState = 0;
			int valueOfI = 0;
			for (int i = 1; i < length; i++) {
				aimState *= 10;
				valueOfI = int(s[i]) - 48;
				aimState += valueOfI;
			}
			ret.aim = aimState;
			return ret;
		}
		else if (c == 'r') {//归约项目
			ret.behavior = r;
			int aimProduction = 0;
			int valueOfI = 0;
			for (int i = 1; i < length; i++) {
				aimProduction *= 10;
				valueOfI = int(s[i]) - 48;
				aimProduction += valueOfI;
			}
			ret.aim = aimProduction ;//目标规约产生式下标s
			return ret;
		}
		else if (c == 'a') {
			ret.behavior = acc;
			ret.aim = -1;
			return ret;
		}
		else {//GOTO项目
			ret.behavior = GOTO;
			int aimState = 0;
			int valueOfI = 0;
			for (int i = 0; i < length; i++) {
				aimState *= 10;
				valueOfI = int(s[i]) - 48;
				aimState += valueOfI;
			}
			ret.aim = aimState;
			return ret;
		}
	}
}
void GoTo(Symbol*& s) {
	symbol sym;
	sym.content = s->content;
	sym.type = s->sym_type;
	sym.row = s->row;
	todo toDo = getWhatToDo(sym);
	if (toDo.behavior != GOTO) {
		error(sym.row,1);
	}
	else {
		pushSym_State(s, toDo.aim);
	}
}
void ParseAndSemanticAnalyze() {
	getSymbolFromLexer();
	int endPointer = symList.size() - 1;//指向$
	int curPointer = 0;
	//初始化栈
	Symbol* s = new Symbol(3,"$",-1);//记为-1行
	pushSym_State(s, 0);
	todo action = getWhatToDo(symList[curPointer]);
	while (action.behavior != acc) {//没有接受
		if (action.behavior == S_) {//移进项目
			string content = symList[curPointer].content;
			int type = symList[curPointer].type;
			int row = symList[curPointer].row;
			if (type == 0) {//标识符   ????
				s = new ID(content,curLevel,row);
				pushSym_State(s, action.aim);//处理栈中加入
			}
			else if (type == 1) {//整形常量				
				s = new Cid(row, content);
				((Cid*)s)->type = "int";
				//加入栈中
				pushSym_State(s, action.aim);
			}
			else if (type == 2) {//浮点形常量
				s = new Cid(row, content);
				((Cid*)s)->type = "real";
				//加入栈中
				pushSym_State(s, action.aim);
			}
			else if (type == 3) {//char常量				
				//加入栈中
				//stateStack.push_back(action.aim);
				//symStack.push_back(s);//处理栈中加入
			}
			else if (type == 4) {//string常量				
				//加入栈中
				//stateStack.push_back(action.aim);
				//symStack.push_back(s);//处理栈中加入
			}
			else {//运算符，边界符，等
				if (type == 7 && (content.compare("true") == 0 || content.compare("false") == 0)) {
					s = new Cid(row, content);
					((Cid*)s)->type = "bool";
				}
				else {
					s = new OtherSym(content, row, type);
					if (content.compare("{") == 0) {
						curLevel++;
					}
					else if (content.compare("}") == 0) {
						clearVarTableAtLevel(curLevel);
						curLevel--;
					}
				}				
				//加入栈中
				pushSym_State(s, action.aim);
			}
			curPointer++;
		}
		else if (action.behavior == r) {//归约项目
			int length = productions[action.aim].right.size();
			string left = productions[action.aim ].left.content;
			if (action.aim == 1) {//X ::= V ; X
				//get X
				X* x = (X*)popSymbol();
				//get ;
				OtherSym* ds = (OtherSym*)popSymbol();
				//get V
				V* v = (V*)popSymbol();
				x->row = ds->row;
				delete ds;
				delete v;
				s = x;			
			}
			else if (action.aim == 2) {//X ::= C ; X
				//get X
				X* x = (X*)popSymbol();
				//get ;
				OtherSym* ds = (OtherSym*)popSymbol();
				//get C
				C* v = (C*)popSymbol();
				x->row = ds->row;
				delete ds;
				delete v;
				s = x;
			}
			else if (action.aim == 3) {//X ::= void main ( ) { L }
				//get }
				OtherSym* ds1 = (OtherSym*)popSymbol();
				//get L
				L* l = (L*)popSymbol();
				//get {
				OtherSym* ds2 = (OtherSym*)popSymbol();
				//get )
				OtherSym* ds3 = (OtherSym*)popSymbol();
				//get (
				OtherSym* ds4 = (OtherSym*)popSymbol();
				//get main
				OtherSym* ds5 = (OtherSym*)popSymbol();
				//get void 
				OtherSym* ds6 = (OtherSym*)popSymbol();
				//create X
				mainCodeList->backpatch(l->nextlist, mainCodeList->nextquad);
				s = new X(ds6->row);
				delete ds1;
				delete ds2;
				delete ds3;
				delete ds4;
				delete ds5;
				delete ds6;
				delete l;
			}
			else if (action.aim == 4) {//V ::= V , id
				//get id
				ID* id = (ID*)popSymbol();
				//get ,
				OtherSym* ds = (OtherSym*)popSymbol();
				//get V
				V* v = (V*)popSymbol();
				VarIndex varIndex;
				id->level = curLevel;
				id->type = v->type;
				varIndex.level = curLevel;
				varIndex.name = id->name;
				if (var_table->count(varIndex) == 1) {
					string detail = "变量" + id->name + "重定义";
					error(ds->row, 2,detail);//重定义
				}
				else {
					(*var_table)[varIndex] = id;
				}
				if (id->type.compare("int") == 0) {
					mainCodeList->gen(":=", "0", "_", id->name);
				}
				else if (id->type.compare("real") == 0) {
					mainCodeList->gen(":=", "0.0", "_", id->name);
				}
				else {
					mainCodeList->gen(":=", "true", "_", id->name);
				}
				v->row = ds->row;
				s = v;
				delete ds;
			}
			else if (action.aim == 5) {//V ::= var T id
				//get id
				ID* id = (ID*)popSymbol();
				//get T
				T* t = (T*)popSymbol();
				//get var
				OtherSym* ds = (OtherSym*)popSymbol();
				VarIndex varIndex;
				id->type = t->type;
				id->level = curLevel;
				varIndex.level = curLevel;
				varIndex.name = id->name;
				if (var_table->count(varIndex) == 1) {
					string detail = "变量" + id->name + "重定义";
					error(ds->row, 2,detail);//重定义
				}
				else {
					(*var_table)[varIndex] = id;
				}
				//create V
				s = new V(id->row);
				if (id->type.compare("int") == 0) {
					mainCodeList->gen(":=", "0", "_", id->name);
				}
				else if (id->type.compare("real") == 0) {
					mainCodeList->gen(":=", "0.0", "_", id->name);
				}
				else {
					mainCodeList->gen(":=", "true", "_", id->name);
				}
				((V*)s)->type = t->type;
				delete t;
				delete ds;
			}
			else if (action.aim == 6) {//C ::= const T id = id	
				//get id2
				ID* id2 = (ID*)popSymbol();
				//get =
				OtherSym* ds1 = (OtherSym*)popSymbol();
				//get id1
				ID* id1 = (ID*)popSymbol();
				//get T
				T* t = (T*)popSymbol();
				//get const
				OtherSym* ds2 = (OtherSym*)popSymbol();
				string name = id2->name;
				id2 = getHighestLevelVar(name);
				if (id2 == nullptr) {
					string detail = "变量" + name + "不存在";
					error(ds1->row, 2, detail);
				}
				//VarIndex varIndex;
				//varIndex.level = id2->level;
				//varIndex.name = id2->name;
				//if (var_table->count(varIndex)==0) {//id2已经被清除
				//	string detail = "变量" + id2->name + "未定义";
				//	error(id2->row, 2, detail);
				//}
				else {//id2存在
					if (id2->type.compare(t->type) != 0) {
						string detail = id2->type+"类型的值不能赋值给" + t->type + "类型变量";
						error(ds1->row, 2, detail);
					}
					else {//类型相同
						VarIndex varIndex;
						varIndex.level = curLevel;
						varIndex.name = id1->name;
						if (var_table->count(varIndex) == 1) {
							string detail = "变量" + id1->name + "重定义";
							error(ds1->row, 2, detail);
						}
						id1->changable = false;
						id1->value = id2->value;
						id1->type = t->type;
						mainCodeList->gen(":=", id2->name, "_", id1->name);
						(*var_table)[varIndex] = id1;
					}
				}
				//create C
				s = new C(ds1->row);
				delete ds1;
				delete ds2;
				delete t;
			}
			else if (action.aim == 7) {//C ::= const T id = cid
				//get cid
				Cid* cid = (Cid*)popSymbol();
				//get =
				OtherSym* ds1 = (OtherSym*)popSymbol();
				//get id
				ID* id = (ID*)popSymbol();
				//get T
				T* t = (T*)popSymbol();
				//get const
				OtherSym* ds2 = (OtherSym*)popSymbol();
				VarIndex varIndex;
				varIndex.level = curLevel;
				varIndex.name = id->name;
				if (var_table->count(varIndex) == 1) {
					string detail = "变量" + id->name + "重定义";
					error(ds1->row, 2, detail);
				}
				else {
					if (t->type.compare(cid->type) != 0) {
						string detail = cid->type + "的值不能赋值给"+t->type+"类型的变量";
						error(ds1->row, 2, detail);
					}
					else {
						id->changable = false;
						id->value = cid->value;
						id->type = t->type;
						mainCodeList->gen(":=", cid->value, "_", id->name);
						(*var_table)[varIndex] = id;
					}
				}
				//create C
				s = new C(ds1->row);
				delete ds1;
				delete ds2;
				delete t;
				delete cid;
			}
			else if (action.aim >= 8 && action.aim <= 10) {//T ::= int	T ::= real	T ::= bool
				//get int/real/bool
				OtherSym* ds = (OtherSym*)popSymbol();
				//create T
				s = new T(ds->content,ds->row);
				delete ds;
			}
			else if (action.aim == 11) {//L ::= L M S
				//get S
				S* mS = (S*)popSymbol();
				//get M
				M* m = (M*)popSymbol();
				//get L1
				L* l1 = (L*)popSymbol();
				mainCodeList->backpatch(l1->nextlist, m->quad);
				//create L
				s = new L(l1->row);
				for (int i = 0; i < mS->nextlist.size(); i++) {
					((L*)s)->nextlist.push_back(mS->nextlist[i]);
				}
				delete mS;
				delete m;
				delete l1;
			}
			else if (action.aim == 12) {//L ::= S
				//get S
				S* mS = (S*)popSymbol();
				//create L
				s = new L(mS->row);
				for (int i = 0; i < mS->nextlist.size(); i++) {
					((L*)s)->nextlist.push_back(mS->nextlist[i]);
				}
				delete mS;
			}
			else if (action.aim == 13) {//S ::= A ;
				//get ;
				OtherSym* ds = (OtherSym*)popSymbol();
				//get A
				A* a = (A*)popSymbol();
				//create S
				s = new S(ds->row);
				delete ds;
				delete a;
			}
			else if (action.aim == 14) {//S ::= V ;
				//get ;
				OtherSym* ds = (OtherSym*)popSymbol();
				//get A
				V* v = (V*)popSymbol();
				//create S
				s = new S(ds->row);
				delete ds;
				delete v;
			}
			else if (action.aim == 15) {//S ::= C ;
				//get ;
				OtherSym* ds = (OtherSym*)popSymbol();
				//get A
				C* c = (C*)popSymbol();
				//create S
				s = new S(ds->row);
				delete ds;
				delete c;
			}
			else if (action.aim == 16) {//S ::= if ( E ) M { L } N else M { L }
				//get }
				OtherSym* ds1 = (OtherSym*)popSymbol();
				//get L2
				L* l2 = (L*)popSymbol();
				//get {
				OtherSym* ds2 = (OtherSym*)popSymbol();
				//get M2
				M* m2 = (M*)popSymbol();
				//get else
				OtherSym* ds3 = (OtherSym*)popSymbol();
				//get N
				N* n = (N*)popSymbol();
				//get }
				OtherSym* ds4 = (OtherSym*)popSymbol();
				//get L1
				L* l1 = (L*)popSymbol();
				//get {
				OtherSym* ds5 = (OtherSym*)popSymbol();
				//get M1
				M* m1 = (M*)popSymbol();
				//get )
				OtherSym* ds6 = (OtherSym*)popSymbol();
				//get E
				E* e = (E*)popSymbol();
				//get (
				OtherSym* ds7 = (OtherSym*)popSymbol();
				//get if
				OtherSym* ds8 = (OtherSym*)popSymbol();
				//create S
				if (e->type.compare("bool") != 0) {
					string detail = "if的判断条件必须是bool表达式";
					error(ds7->row, 2, detail);
				}
				s = new S(ds8->row);
				//backpatch
				mainCodeList->backpatch(e->truelist, m1->quad);
				mainCodeList->backpatch(e->falselist, m2->quad);
				//merge
				for (int i = 0; i < l1->nextlist.size(); i++) {
					((S*)s)->nextlist.push_back(l1->nextlist[i]);
				}
				for (int i = 0; i < l2->nextlist.size(); i++) {
					((S*)s)->nextlist.push_back(l2->nextlist[i]);
				}
				for (int i = 0; i < n->nextlist.size(); i++) {
					((S*)s)->nextlist.push_back(n->nextlist[i]);
				}
				delete ds1;
				delete ds2;
				delete ds3;
				delete ds4;
				delete ds5;
				delete ds6;
				delete ds7;
				delete ds8;
				delete l1;
				delete l2;
				delete m1;
				delete m2;
				delete n;
				delete e;
			}
			else if (action.aim == 17) {//S ::= if ( E ) M { L }
				//get }
				OtherSym* ds1 = (OtherSym*)popSymbol();
				//get L
				L* l = (L*)popSymbol();
				//get {
				OtherSym* ds2 = (OtherSym*)popSymbol();
				//get M
				M* m = (M*)popSymbol();
				//get )
				OtherSym* ds3 = (OtherSym*)popSymbol();
				//get E
				E* e = (E*)popSymbol();
				//get (
				OtherSym* ds4 = (OtherSym*)popSymbol();
				//get if
				OtherSym* ds5 = (OtherSym*)popSymbol();
				if (e->type.compare("bool") != 0) {
					string detail = "if的判断条件必须是bool表达式";
					error(ds4->row, 2, detail);
				}
				//create S
				s = new S(ds5->row);
				//backpatch
				mainCodeList->backpatch(e->truelist, m->quad);
				//merge
				for (int i = 0; i < l->nextlist.size(); i++) {
					((S*)s)->nextlist.push_back(l->nextlist[i]);
				}
				for (int i = 0; i < e->falselist.size(); i++) {
					((S*)s)->nextlist.push_back(e->falselist[i]);
				}
				delete ds1;
				delete ds2;
				delete ds3;
				delete ds4;
				delete ds5;
				delete l;
				delete m;
				delete e;
			}
			else if (action.aim == 18) {//S ::= while M ( E ) M { L }
				//get }
				OtherSym* ds1 = (OtherSym*)popSymbol();
				//get L
				L* l = (L*)popSymbol();
				//get {
				OtherSym* ds2 = (OtherSym*)popSymbol();
				//get M2
				M* m2 = (M*)popSymbol();
				//get )
				OtherSym* ds3 = (OtherSym*)popSymbol();
				//get E
				E* e = (E*)popSymbol();
				//get (
				OtherSym* ds4 = (OtherSym*)popSymbol();
				//get M1
				M* m1 = (M*)popSymbol();
				//get while
				OtherSym* ds5 = (OtherSym*)popSymbol();
				if (e->type.compare("bool") != 0) {
					string detail = "while语句的判断条件必须是bool表达式";
					error(ds4->row, 2, detail);
				}

				//create S
				s = new S(ds5->row);
				//backpatch
				mainCodeList->backpatch(l->nextlist, m1->quad);
				mainCodeList->backpatch(e->truelist, m2->quad);
				//merge
				for (int i = 0; i < e->falselist.size(); i++) {
					((S*)s)->nextlist.push_back(e->falselist[i]);
				}
				//gen
				mainCodeList->gen("j", "_", "_", to_string(m1->quad));
				//delete
				delete ds1;
				delete ds2;
				delete ds3;
				delete ds4;
				delete ds5;
				delete l;
				delete m1;
				delete m2;
				delete e;
			}
			else if (action.aim == 19) {//M ::= 
				//create M
				s = new M(symList[curPointer].row);
				((M*)s)->quad = mainCodeList->nextquad;
			}
			else if (action.aim == 20) {//N ::= 
				//create N
				s = new N(symList[curPointer].row);
				((N*)s)->nextlist.push_back(mainCodeList->nextquad);
				//gen
				mainCodeList->gen("j", "_", "_", "-1");
			}
			//else if (action.aim == 21) {//A ::= id = cid
			//	//get Cid
			//	Cid* cid = (Cid*)popSymbol();
			//	//get =
			//	OtherSym* ds = (OtherSym*)popSymbol();
			//	//get id
			//	ID* id = (ID*)popSymbol();
			//	if (id->type.compare(cid->type) != 0) {
			//		string detail = "不能将" + cid->type + "类型的常量赋值给" + id->type + "类型的变量";
			//		error(id->row, 2, detail);
			//	}
			//	//create A
			//	s = new A(ds->row);
			//	mainCodeList->gen(":=", cid->value, "_", id->name);
			//	delete cid;
			//	delete ds;
			//}
			else if (action.aim == 21) {//A ::= id = E
				//get E
				E* e = (E*)popSymbol();
				//get =
				OtherSym* ds = (OtherSym*)popSymbol();
				//get id
				ID* id = (ID*)popSymbol();
				string name = id->name;
				id = getHighestLevelVar(name);
				if (id == nullptr) {
					string detail = "变量" + name + "不存在";
					error(ds->row, 2, detail);
				}
				else if (!id->changable) {
					string detail = "不能给常变量赋值";
					error(ds->row, 2, detail);
				}
				else if (id->type.compare(e->type) != 0) {
					string detail = "不能将结果为" + e->type + "类型的表达式赋值给" + id->type + "类型的变量";
					error(ds->row, 2, detail);
				}
				//create A
				s = new A(ds->row);
				mainCodeList->gen(":=", e->place, "_", id->name);
				delete ds;
				delete e;
			}
			else if (action.aim == 22) {//A ::= id ++
				//get ++
				OtherSym* ds = (OtherSym*)popSymbol();
				//get id
				ID* id = (ID*)popSymbol();
				string name = id->name;
				id = getHighestLevelVar(name);				
				if (id == nullptr) {
					string detail = "变量" + name + "不存在";
					error(ds->row, 2, detail);
				}
				else if (id->type.compare("int") != 0 && id->type.compare("real") != 0) {
					string detail = "只有int或者real类型的变量才可以使用++运算符";
					error(ds->row, 2, detail);
				}
				else if (!id->changable) {
					string detail = "不能改变常变量的值";
					error(ds->row, 2, detail);
				}
				//create A
				s = new A(ds->row);
				mainCodeList->gen("+", id->name, "1", id->name);
				delete ds;
			}
			else if (action.aim == 23) {//A ::= id --
				//get ++
				OtherSym* ds = (OtherSym*)popSymbol();
				//get id
				ID* id = (ID*)popSymbol();
				string name = id->name;
				id = getHighestLevelVar(name);
				if (id == nullptr) {
					string detail = "变量" + name + "不存在";
					error(ds->row, 2, detail);
				}
				else if (id->type.compare("int") != 0 && id->type.compare("real") != 0) {
					string detail = "只有int或者real类型的变量才可以使用--运算符";
					error(ds->row, 2, detail);
				}
				else if(!id->changable){
					string detail = "不能改变常变量的值";
					error(ds->row, 2, detail);
				}
				//create A
				s = new A(ds->row);
				mainCodeList->gen("-", id->name, "1", id->name);
				delete ds;
			}
			else if (action.aim >= 24 && action.aim <= 27) {//E ::= E +(-  *  /) E 
				//get E2
				E* e2 = (E*)popSymbol();
				//get +/-/*//
				OtherSym* ds = (OtherSym*)popSymbol();
				//get E1
				E* e1 = (E*)popSymbol();
				if (e1->type.compare("bool") == 0 || e2->type.compare("bool")==0) {
					string detail = "bool类型的变量不能参与" + ds->content + "运算";
					error(ds->row, 2, detail);
				}
				//create E
				s = new E(ds->row);
				if (e1->type.compare("real") == 0 || e2->type.compare("real") == 0) {
					((E*)s)->type = "real";
				}
				else {
					((E*)s)->type = "int";
				}
				((E*)s)->place = NewTemp::newtemp();
				mainCodeList->gen(ds->content, e1->place, e2->place, ((E*)s)->place);
				delete ds;
				delete e1;
				delete e2;
			}
			else if (action.aim == 28) {//E ::= E % E
				//get E2
				E* e2 = (E*)popSymbol();
				//get %
				OtherSym* ds = (OtherSym*)popSymbol();
				//get E1
				E* e1 = (E*)popSymbol();
				if (e1->type.compare("int") != 0 || e2->type.compare("int") != 0) {
					string detail = "只有两个整数才能使用%运算符";
					error(ds->row, 2, detail);
				}
				//create E
				s = new E(ds->row);
				((E*)s)->place = NewTemp::newtemp();
				((E*)s)->type = "int";
				mainCodeList->gen("%", e1->place, e2->place, ((E*)s)->place);
				delete ds;
				delete e1;
				delete e2;
			}
			else if (action.aim == 29) {//E ::= ! E
				//get E1
				E* e1 = (E*)popSymbol();
				//get !
				OtherSym* ds = (OtherSym*)popSymbol();
				if (e1->type.compare("bool") != 0) {
					string detail = "只有bool类型的值才能进行!运算";
					error(ds->row, 2, detail);
				}
				//create E
				s = new E(ds->row);
				((E*)s)->type = "bool";
				((E*)s)->place = NewTemp::newtemp();
				mainCodeList->gen("!", e1->place, "_", ((E*)s)->place);
				for (int i = 0; i < e1->truelist.size(); i++) {
					((E*)s)->falselist.push_back(e1->truelist[i]);
				}
				for (int i = 0; i < e1->falselist.size(); i++) {
					((E*)s)->truelist.push_back(e1->falselist[i]);
				}
				delete e1;
				delete ds;
			}
			else if (action.aim == 30) {//E ::= - E
				//get E1
				E* e1 = (E*)popSymbol();
				//get -
				OtherSym* ds = (OtherSym*)popSymbol();
				if (e1->type.compare("bool") == 0) {
					string detail = "bool类型的值不能进行-运算";
					error(ds->row, 2, detail);
				}
				//create E
				s = new E(ds->row);
				((E*)s)->type = e1->type;
				((E*)s)->place = NewTemp::newtemp();
				mainCodeList->gen("@", e1->place, "_", ((E*)s)->place);
				delete e1;
				delete ds;
			}
			else if (action.aim == 31) {//E ::= ( E )
				//get )
				OtherSym* ds1 = (OtherSym*)popSymbol();
				//get E1
				E* e1 = (E*)popSymbol();
				//get (
				OtherSym* ds2 = (OtherSym*)popSymbol();
				//create E
				s = e1;
				delete ds1;
				delete ds2;
			}
			else if (action.aim == 32) {//E ::= id
				//get id
				ID* id = (ID*)popSymbol();
				//create E
				int row = id->row;
				string name = id->name;
				id = getHighestLevelVar(name);
				if (id == nullptr) {
					string detail = "变量" + name + "不存在";
					error(row, 2, detail);
				}
				s = new E(id->row);
				((E*)s)->place = id->name;
				((E*)s)->type = id->type;
				if (id->type.compare("bool") == 0) {//bool类型
					((E*)s)->truelist.push_back(mainCodeList->nextquad);
					((E*)s)->falselist.push_back(mainCodeList->nextquad+1);
				}
			}
			else if (action.aim == 33) {//E ::= cid
				if (curPointer >= 16) {
					cout << "cur 17";
				}
				//get id
				Cid* cid = (Cid*)popSymbol();
				//create E
				s = new E(cid->row);
				((E*)s)->place = NewTemp::newtemp();
				((E*)s)->type = cid->type;
				mainCodeList->gen(":=", cid->value, "_", ((E*)s)->place);
				if (cid->type.compare("bool") == 0) {//bool类型
					((E*)s)->truelist.push_back(mainCodeList->nextquad);
					((E*)s)->falselist.push_back(mainCodeList->nextquad + 1);
				}
			}
			else if (action.aim == 34) {//E ::= E || M E
				//get e2
				E* e2 = (E*)popSymbol();
				//get M
				M* m = (M*)popSymbol();
				//get ||
				OtherSym* ds = (OtherSym*)popSymbol();
				//get e1
				E* e1 = (E*)popSymbol();
				if (e1->type.compare("bool") != 0 || e2->type.compare("bool") != 0) {
					string detail = "只有bool型变量才能参与||运算";
					error(ds->row, 2, detail);
				}
				//create E
				s = new E(ds->row);
				((E*)s)->place = NewTemp::newtemp();
				((E*)s)->type = "bool";
				mainCodeList->backpatch(e1->falselist, m->quad);
				for (int i = 0; i < e1->truelist.size(); i++) {
					((E*)s)->truelist.push_back(e1->truelist[i]);
				}
				for (int i = 0; i < e2->truelist.size(); i++) {
					((E*)s)->truelist.push_back(e2->truelist[i]);
				}
				for (int i = 0; i < e2->falselist.size(); i++) {
					((E*)s)->falselist.push_back(e2->falselist[i]);
				}
				delete ds;
				delete e1;
				delete e2;
			}
			else if (action.aim == 35) {//E ::= E && M E
				//get e2
				E* e2 = (E*)popSymbol();
				//get M
				M* m = (M*)popSymbol();
				//get &&
				OtherSym* ds = (OtherSym*)popSymbol();
				//get e1
				E* e1 = (E*)popSymbol();
				if (e1->type.compare("bool") != 0 || e2->type.compare("bool") != 0) {
					string detail = "只有bool型变量才能参与&&运算";
					error(ds->row, 2, detail);
				}
				//create E
				s = new E(ds->row);
				((E*)s)->place = NewTemp::newtemp();
				((E*)s)->type = "bool";
				mainCodeList->backpatch(e1->truelist, m->quad);
				for (int i = 0; i < e1->falselist.size(); i++) {
					((E*)s)->falselist.push_back(e1->falselist[i]);
				}
				for (int i = 0; i < e2->falselist.size(); i++) {
					((E*)s)->falselist.push_back(e2->falselist[i]);
				}
				for (int i = 0; i < e2->truelist.size(); i++) {
					((E*)s)->truelist.push_back(e2->truelist[i]);
				}
				delete ds;
				delete e1;
				delete e2;
			}
			else if (action.aim >= 36 && action.aim <= 39) {//E ::= E > E
				//get E2
				E* e2 = (E*)popSymbol();
				//get >
				OtherSym* ds = (OtherSym*)popSymbol();
				//get E1
				E* e1 = (E*)popSymbol();
				if (e1->type.compare("bool") == 0 || e2->type.compare("bool") == 0) {
					string detail = "bool类型的值不能参与" + ds->content + "运算";
					error(ds->row, 2, detail);
				}
				//create E
				s = new E(ds->row);
				((E*)s)->place = NewTemp::newtemp();
				((E*)s)->type = "bool";
				((E*)s)->truelist.push_back(mainCodeList->nextquad);
				((E*)s)->falselist.push_back(mainCodeList->nextquad+1);
				string op = "j" + ds->content;
				mainCodeList->gen(op, e1->place, e2->place, "-1");
				mainCodeList->gen("j", "_", "_", "-1");
				delete ds;
				delete e1;
				delete e2;

			}
			else if (action.aim == 40 || action.aim == 41) {//E ::= E < E
				//get E2
				E* e2 = (E*)popSymbol();
				//get >
				OtherSym* ds = (OtherSym*)popSymbol();
				//get E1
				E* e1 = (E*)popSymbol();
				if (e1->type.compare("bool") == 0 || e2->type.compare("bool") == 0) {
					if (e1->type.compare(e2->type) != 0) {
						string detail = e1->type+"类型的值不能与"+e2->type+"类型的值进行" + ds->content + "运算";
						error(ds->row, 2, detail);
					}					
				}
				//create E
				s = new E(ds->row);
				((E*)s)->place = NewTemp::newtemp();
				((E*)s)->type = "bool";
				((E*)s)->truelist.push_back(mainCodeList->nextquad);
				((E*)s)->falselist.push_back(mainCodeList->nextquad + 1);
				string op = "j" + ds->content;
				mainCodeList->gen(op, e1->place, e2->place, "-1");
				mainCodeList->gen("j", "_", "_", "-1");
				delete ds;
				delete e1;
				delete e2;
			}
			GoTo(s);
		}
		if (curPointer == 22) {
			cout << "cur 22" << endl;
		}
		cout << curPointer << endl;
		action = getWhatToDo(symList[curPointer]);
	}	
	mainCodeList->writeCodes(writeCodePath);
}
inline bool operator <(const struct DicIndex& d1, const struct DicIndex& d2) {
	if (d1.state != d2.state) {
		return d1.state < d2.state;
	}
	else {
		if (d1.sym.length() != d2.sym.length()) {
			return d1.sym.length() < d2.sym.length();
		}
		else {
			for (int i = 0; i < d1.sym.length(); i++) {
				if (d1.sym[i] != d2.sym[i]) {
					return d1.sym[i] < d2.sym[i];
				}
			}
			return d1.sym[0] < d2.sym[0];
		}
	}	
}
inline bool operator < (const struct VarIndex& v1, const struct VarIndex& v2) {
	if (v1.level != v2.level) {
		return v1.level < v2.level;
	}
	else {
		if (v1.name.length() != v2.name.length()) {
			return v1.name.length() < v2.name.length();
		}
		else {
			for (int i = 0; i < v1.name.length(); i++) {
				if (v1.name[i] != v2.name[i]) {
					return v1.name[i] < v2.name[i];
				}
			}
			return v1.name[0] < v2.name[0];
		}
	}
}
