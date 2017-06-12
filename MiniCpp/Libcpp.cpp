// ***** 内部函数库 ***** 
// 需要扩展功能在这里添加喔

#include <iostream> 
#include <cstdlib> 
#include <cstdio> 
#include "mccommon.h" 
using namespace std;

// Read a character from the console. 
// If your compiler supplies an unbuffered 
// character intput function, feel free to 
// substitute it for the call to cin.get(). 
int call_getchar()
{
	char ch;
	ch = getchar();
	//Advance past ()   
	get_token();
	if (*token != '(')
		throw InterpExc(PAREN_EXPECTED);

	get_token();
	if (*token != ')')
		throw InterpExc(PAREN_EXPECTED);

	return ch;
}

//写一个字符到显示设备
int call_putchar()
{
	int value;
	eval_exp(value);
	putchar(value);
	return value;
}

// 返回绝对值
int call_abs()
{
	int val;
	eval_exp(val);
	val = abs(val);
	return val;
}

// 返回一个随机整数
int call_rand()
{
	// Advance past ()   
	get_token();
	if (*token != '(')
	{
		throw InterpExc(PAREN_EXPECTED);
	}
	get_token();
	if (*token != ')')
	{
		throw InterpExc(PAREN_EXPECTED);
	}
	return rand();
}