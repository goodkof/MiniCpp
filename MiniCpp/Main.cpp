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

	// 为程序分配内存 
	try {
		p_buf = new char[PROG_SIZE];
	}
	catch (bad_alloc exc) {
		cout << "无法为源码分配内存\n";
		return 1;
	}

	// 加载程序
	if (!load_program(p_buf, argv[1]))
	{

		return 1;
	}

	// 设置运行时指针与源码缓冲区对齐
	prog = p_buf;

	try {
		// Find the location of all functions 
		// and global variables in the program. 
		prescan();//预扫描

				  // Next, set up the call to main(). 

				  //找到程序的入口点main(),并将程序的运行时指针指向main函数
		prog = find_func("main");

		// Check for incorrect or missing main() function. 
		if (!prog) {
			cout << "main" << "() 未定义\n";
			return 1;
		}

		// Back up to opening (. 
		prog--;

		// 设置第一个标记为main
		strcpy(token, "main");

		// 调用main()开始解释过程
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