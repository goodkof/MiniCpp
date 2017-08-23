// ***** 内部函数库 ***** 
// 需要扩展功能在这里添加喔

#include <iostream> 
#include "mccommon.h" 
using namespace std;

void check_paren()
{
	//检查括弧  
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
}
//从控制台读取一个字符,如果你的编译器支持一个无缓冲的字符输入函数，可以随意的使用 cin.get()来代替这个getchar()
int call_getchar()
{
	char ch;
	ch = getchar();
	check_paren();
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
	check_paren();
	return rand();
}

