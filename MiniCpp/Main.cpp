#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "mccommon.h" 
#include "vld.h"
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
#ifdef _DEBUG
	argc = 2;
	argv[1] = "./a.cpp";
#endif // DEBUG

	if (argc != 2) {
		cout << "Usage: minicpp <filename>\n";
		return 1;
	}

	// Ϊ��������ڴ� 
	try {
		p_buf = new char[PROG_SIZE];
	}
	catch (bad_alloc exc) {
		cout << "�޷�ΪԴ������ڴ�\n";
		return 1;
	}

	// ���س���
	if (!load_program(p_buf, argv[1]))
	{

		return 1;
	}

	// ��������ʱָ����Դ�뻺��������
	prog = p_buf;

	try {
		// Find the location of all functions 
		// and global variables in the program. 
		prescan();//Ԥɨ��

				  // Next, set up the call to main(). 

				  //�ҵ��������ڵ�main(),�������������ʱָ��ָ��main����
		prog = find_func("main");

		// Check for incorrect or missing main() function. 
		if (!prog) {
			cout << "main" << "() δ����\n";
			return 1;
		}

		// Back up to opening (. 
		prog--;

		// ���õ�һ�����Ϊmain
		strcpy(token, "main");

		// ����main()��ʼ���͹���
		call();
	}
	catch (InterpExc exc) {
		sntx_err(exc.get_err());
		return 1;
	}
	catch (bad_alloc exc) {
		cout << "Out Of Memory\n";
		return 1;
	}

	return ret_value;
}