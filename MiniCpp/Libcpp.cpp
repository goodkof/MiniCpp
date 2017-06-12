// ***** Internal Library Functions ***** 
// Add more of your own, here. 

#include <iostream> 
#include <cstdlib> 
#include <cstdio> 
#include "mccommon.h" 
/* 下面的几个函数声明是little c内部函数 */
using namespace std;

// Read a character from the console. 
// If your compiler supplies an unbuffered 
// character intput function, feel free to 
// substitute it for the call to cin.get(). 
int call_getchar()
{
	char ch;
	ch = getchar();
	// Advance past ()   
	get_token();
	if (*token != '(')
		throw InterpExc(PAREN_EXPECTED);

	get_token();
	if (*token != ')')
		throw InterpExc(PAREN_EXPECTED);

	return ch;
}

// Write a character to the display. 
int call_putchar()
{
	int value;
	eval_exp(value);
	putchar(value);
	return value;
}

// Return absolute value. 
int call_abs()
{
	int val;
	eval_exp(val);
	val = abs(val);
	return val;
}

// Return a randome integer. 
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