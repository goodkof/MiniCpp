#pragma once
// Common declarations used by parser.cpp, minicpp.cpp, 
// or libcpp.cpp, or by other files that you might add. 
// 
const int MAX_T_LEN = 128;   // 最大标记长度
const int MAX_ID_LEN = 31;   // max identifier length
const int PROG_SIZE = 10000; // 最大代码大小
const int NUM_PARAMS = 31;   // 参数组数量c89 标准  c99 127个 


//这两个枚举的区别是上面的比较广义,指代各种token的类型, 比如block, 这个是不会是关键字的,
//下面的可以理解成就是关键字
// Enumeration of token types. /* 标记类型 主要是标记是什么类型的,是数字,是字符,是块,是字符串等等.*/
enum tok_types 
{
	UNDEFTT,	   //未定义的标记类型
	DELIMITER,	   //分隔符
	IDENTIFIER,	   //标识符
	NUMBER,		   //数值
	KEYWORD,	   //关键字
	TEMP, 		   //临时类型(右值或者实参)
	STRING,		   //字符串
	BLOCK,		   //代码块(变量的作用域)
	//TYPE		   //内建类型
};				   


/* 内部关键字值定义 */
// Enumeration of internal representation of tokens. 
enum token_ireps 
{
	UNDEFTOK, 		   //未定义的保留关
	ARG, 			   //
	CHAR, 			   //字符类型
	//BOOL, 			   //布尔类型
	//SHORT,				   //短整型
	INT,				   //整形
	//LONG,				   //长整形
	//FLOAT,		           //单精度浮点型
	//DOUBLE,		           //双精度浮点型
	//STRUCT,		           //结构体
	//FALSE,		           //假
	//TRUE,		           //真
	SWITCH,					   //switch 分支
	CASE, 					   //case 分支项
	IF, 					   //if 分支
	ELSE,					   //else 分支
	FOR, 					   //for 循环
	DO, 					   //do while 循环
	WHILE, 					   //while 循环
	BREAK,					   //破坏
	RETURN,					   //返回
	COUT, 					   //控制台输出
	CIN, 					   //控制台输入
	END,						   //结束符
	//ENDL,				   //结束符外加换行
	//DEFAULT,			   //缺省
	//CONTINUE,			   //继续
	//NONE				   //不存在
};

/* 操作符定义 大于,等于,小于等算术操作符的内部形式 */
// Enumeration of two-character operators, such as <=. 
//这里注意从1开始, 以免用到0出现误解.
enum double_ops { LT = 1, LE, GT, GE, EQ, NE, LS, RS, INC, DEC };

// These are the constants used when throwing a 
// syntax error exception. 
// 
// NOTE: SYNTAX is a generic error message used when 
// nothing else seems appropriate. 
/* 错误消息的定义 */
enum error_msg
{
	SYNTAX, NO_EXP, NOT_VAR, DUP_VAR, DUP_FUNC,
	SEMI_EXPECTED, UNBAL_BRACES, FUNC_UNDEF,
	TYPE_EXPECTED, RET_NOCALL, PAREN_EXPECTED,
	WHILE_EXPECTED, QUOTE_EXPECTED, DIV_BY_ZERO,
	BRACE_EXPECTED, COLON_EXPECTED
};

extern char *prog;  // current location in source code /* prog函数是little c脚本语言的程序代码的指针变量 */
extern char *p_buf; // points to start of program buffer /* 缓冲prog,因为有时,分析可能会出现回朔,所以作者设置了此指针变量 */

extern char token[MAX_T_LEN + 1]; // string version of token 
extern tok_types token_type; // contains type of token 
extern token_ireps tok; // internal representation of token 

extern int ret_value; // function return value 

extern bool breakfound; // true if break encountered 

// Mini C++ 中的解析异常类
class InterpExc {
	error_msg err;
public:
	InterpExc(error_msg e) { err = e; }
	error_msg get_err() { return err; }
};

// Interpreter prototypes. 
void prescan();
void decl_global();					//定义一个全局变量
void call();
void putback();
void decl_local();					//定义一个局部变量
void exec_if();						//执行if语句
void find_eob();
void exec_for();					// 执行for语句
void exec_switch();					// 执行switch语句
void get_params();
void get_args();
void exec_while();					 // 执行while语句
void exec_do();						 // 执行do语句
void exec_cout();					 // 执行控制台输出
void exec_cin();					 // 执行控制台输入
void assign_var(char *var_name, int value); /* assign_var函数是分配一个变量存储空间 */
bool load_program(char *p, char *fname);
int find_var(char *s);
void interp();
void func_ret();
char *find_func(char *name);/* 找一个用户函数的位置，好使call()函数正确调用这个用户定义的函数 */
bool is_var(char *s);
token_ireps find_var_type(char *s);

/* start 下面函数是表达式递归调用分析的子程序  */
// Parser prototypes. 
void eval_exp(int &value);
void eval_exp0(int &value);
void eval_exp1(int &value);
void eval_exp2(int &value);
void eval_exp3(int &value);
void eval_exp4(int &value);
void eval_exp5(int &value);

void atom(int &value);  /* 函数atom 是取得变量，表达式，函数等运算后的值，并存放在全局变量value里 */
void sntx_err(error_msg error);/* 函数sntx_err是向屏幕上显示语法错误，通过整数参数变量error来解释不同的错误 */
void putback();/* 函数putback函数是返回一个字符流，就是在分析的时候可能会出现向前看一个字符，当看后需要在返回给原先的字符流 */
bool isdelim(char c);
/* look_up函数是通过查找字符看是否是关键字表中的值，iswhite是查看字符是否是空白字符 */
token_ireps look_up(char *s);
int find_var(char *s);/* 函数find_var是查找一个变量的值 */
tok_types get_token();/* get_token函数是从当前的字符流中得到一个解释器内部的单词 */
int internal_func(char *s);/* 从内部函数中查找是否有个*s的内部函数 */
bool is_var(char *s);

// 标准库代理函数
int call_getchar();
int call_putchar();
int call_abs();
int call_rand();

