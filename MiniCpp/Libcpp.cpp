// ***** �ڲ������� ***** 
// ��Ҫ��չ��������������

#include <iostream> 
#include "mccommon.h" 
using namespace std;

void check_paren()
{
	//�������  
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
//�ӿ���̨��ȡһ���ַ�,�����ı�����֧��һ���޻�����ַ����뺯�������������ʹ�� cin.get()���������getchar()
int call_getchar()
{
	char ch;
	ch = getchar();
	check_paren();
	return ch;
}

//дһ���ַ�����ʾ�豸
int call_putchar()
{
	int value;
	eval_exp(value);
	putchar(value);
	return value;
}

// ���ؾ���ֵ
int call_abs()
{
	int val;
	eval_exp(val);
	val = abs(val);
	return val;
}

// ����һ���������
int call_rand()
{
	check_paren();
	return rand();
}

