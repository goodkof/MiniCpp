#pragma once
// Common declarations used by parser.cpp, minicpp.cpp, 
// or libcpp.cpp, or by other files that you might add. 
// 
const int MAX_T_LEN = 128;   // ����ǳ���
const int MAX_ID_LEN = 31;   // max identifier length
const int PROG_SIZE = 10000; // �������С
const int NUM_PARAMS = 31;   // ����������c89 ��׼  c99 127�� 


//������ö�ٵ�����������ıȽϹ���,ָ������token������, ����block, ����ǲ����ǹؼ��ֵ�,
//����Ŀ������ɾ��ǹؼ���
// Enumeration of token types. /* ������� ��Ҫ�Ǳ����ʲô���͵�,������,���ַ�,�ǿ�,���ַ����ȵ�.*/
enum tok_types 
{
	UNDEFTT,	   //δ����ı������
	DELIMITER,	   //�ָ���
	IDENTIFIER,	   //��ʶ��
	NUMBER,		   //��ֵ
	KEYWORD,	   //�ؼ���
	TEMP, 		   //��ʱ����(��ֵ����ʵ��)
	STRING,		   //�ַ���
	BLOCK,		   //�����(������������)
	//TYPE		   //�ڽ�����
};				   


/* �ڲ��ؼ���ֵ���� */
// Enumeration of internal representation of tokens. 
enum token_ireps 
{
	UNDEFTOK, 		   //δ����ı�����
	ARG, 			   //
	CHAR, 			   //�ַ�����
	//BOOL, 			   //��������
	//SHORT,				   //������
	INT,				   //����
	//LONG,				   //������
	//FLOAT,		           //�����ȸ�����
	//DOUBLE,		           //˫���ȸ�����
	//STRUCT,		           //�ṹ��
	//FALSE,		           //��
	//TRUE,		           //��
	SWITCH,					   //switch ��֧
	CASE, 					   //case ��֧��
	IF, 					   //if ��֧
	ELSE,					   //else ��֧
	FOR, 					   //for ѭ��
	DO, 					   //do while ѭ��
	WHILE, 					   //while ѭ��
	BREAK,					   //�ƻ�
	RETURN,					   //����
	COUT, 					   //����̨���
	CIN, 					   //����̨����
	END,						   //������
	//ENDL,				   //��������ӻ���
	//DEFAULT,			   //ȱʡ
	//CONTINUE,			   //����
	//NONE				   //������
};

/* ���������� ����,����,С�ڵ��������������ڲ���ʽ */
// Enumeration of two-character operators, such as <=. 
//����ע���1��ʼ, �����õ�0�������.
enum double_ops { LT = 1, LE, GT, GE, EQ, NE, LS, RS, INC, DEC };

// These are the constants used when throwing a 
// syntax error exception. 
// 
// NOTE: SYNTAX is a generic error message used when 
// nothing else seems appropriate. 
/* ������Ϣ�Ķ��� */
enum error_msg
{
	SYNTAX, NO_EXP, NOT_VAR, DUP_VAR, DUP_FUNC,
	SEMI_EXPECTED, UNBAL_BRACES, FUNC_UNDEF,
	TYPE_EXPECTED, RET_NOCALL, PAREN_EXPECTED,
	WHILE_EXPECTED, QUOTE_EXPECTED, DIV_BY_ZERO,
	BRACE_EXPECTED, COLON_EXPECTED
};

extern char *prog;  // current location in source code /* prog������little c�ű����Եĳ�������ָ����� */
extern char *p_buf; // points to start of program buffer /* ����prog,��Ϊ��ʱ,�������ܻ���ֻ�˷,�������������˴�ָ����� */

extern char token[MAX_T_LEN + 1]; // string version of token 
extern tok_types token_type; // contains type of token 
extern token_ireps tok; // internal representation of token 

extern int ret_value; // function return value 

extern bool breakfound; // true if break encountered 

// Mini C++ �еĽ����쳣��
class InterpExc {
	error_msg err;
public:
	InterpExc(error_msg e) { err = e; }
	error_msg get_err() { return err; }
};

// Interpreter prototypes. 
void prescan();
void decl_global();					//����һ��ȫ�ֱ���
void call();
void putback();
void decl_local();					//����һ���ֲ�����
void exec_if();						//ִ��if���
void find_eob();
void exec_for();					// ִ��for���
void exec_switch();					// ִ��switch���
void get_params();
void get_args();
void exec_while();					 // ִ��while���
void exec_do();						 // ִ��do���
void exec_cout();					 // ִ�п���̨���
void exec_cin();					 // ִ�п���̨����
void assign_var(char *var_name, int value); /* assign_var�����Ƿ���һ�������洢�ռ� */
bool load_program(char *p, char *fname);
int find_var(char *s);
void interp();
void func_ret();
char *find_func(char *name);/* ��һ���û�������λ�ã���ʹcall()������ȷ��������û�����ĺ��� */
bool is_var(char *s);
token_ireps find_var_type(char *s);

/* start ���溯���Ǳ��ʽ�ݹ���÷������ӳ���  */
// Parser prototypes. 
void eval_exp(int &value);
void eval_exp0(int &value);
void eval_exp1(int &value);
void eval_exp2(int &value);
void eval_exp3(int &value);
void eval_exp4(int &value);
void eval_exp5(int &value);

void atom(int &value);  /* ����atom ��ȡ�ñ��������ʽ��������������ֵ���������ȫ�ֱ���value�� */
void sntx_err(error_msg error);/* ����sntx_err������Ļ����ʾ�﷨����ͨ��������������error�����Ͳ�ͬ�Ĵ��� */
void putback();/* ����putback�����Ƿ���һ���ַ����������ڷ�����ʱ����ܻ������ǰ��һ���ַ�����������Ҫ�ڷ��ظ�ԭ�ȵ��ַ��� */
bool isdelim(char c);
/* look_up������ͨ�������ַ����Ƿ��ǹؼ��ֱ��е�ֵ��iswhite�ǲ鿴�ַ��Ƿ��ǿհ��ַ� */
token_ireps look_up(char *s);
int find_var(char *s);/* ����find_var�ǲ���һ��������ֵ */
tok_types get_token();/* get_token�����Ǵӵ�ǰ���ַ����еõ�һ���������ڲ��ĵ��� */
int internal_func(char *s);/* ���ڲ������в����Ƿ��и�*s���ڲ����� */
bool is_var(char *s);

// ��׼�������
int call_getchar();
int call_putchar();
int call_abs();
int call_rand();

