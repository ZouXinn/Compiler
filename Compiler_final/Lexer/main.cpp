#include <iostream>
#include "defineAnddeclare.h"
using namespace std;

int main()
{
	init();//初始化输入、输出流等
	//cout << text.pC;
	//Text t= readText("test.txt");
	while (!getSymbol()) {

	}
	writeSymbolTable();
	writeBinSymbolTable();
	end();//关闭数据流等
	system("pause");
}