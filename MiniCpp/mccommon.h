#pragma once
const int MAX_T_LEN = 128;   // ����ǳ���
const int MAX_ID_LEN = 31;   // ��ʶ����󳤶�
const int PROG_SIZE = 10000; // �������С
const int NUM_PARAMS = 31;   // ����������c89 ��׼  c99 127�� 


/* 
������͹����ָ������token������,
����block, ����ǲ����ǹؼ��ֵ�,
��Ҫ�Ǳ����ʲô���͵�,
������,���ַ�,�ǿ�,���ַ����ȵ�
*/
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

//˫Ŀ��������ڲ���ʽ����
//����ע���1��ʼ, �����õ�0�������.
enum double_ops 
{ 
	LT = 1, //С�ں�
	LE,		//С�ڵ��ں�
	GT,		//���ں�
	GE,		//���ڵ��ں�
	EQ,		//��ȷ���
	NE,		//���ȷ���
	LS,		//LS
	RS,		//RS
	INC,	//INC
	DEC 	//DEC
};

//������Ϣ
enum error_msg
{
	SYNTAX, 
	NO_EXP, 
	NOT_VAR,
	DUP_VAR, 
	DUP_FUNC,
	SEMI_EXPECTED, 
	UNBAL_BRACES,
	FUNC_UNDEF,
	TYPE_EXPECTED, 
	RET_NOCALL, 
	PAREN_EXPECTED,
	WHILE_EXPECTED,
	QUOTE_EXPECTED,
	DIV_BY_ZERO,
	BRACE_EXPECTED,
	COLON_EXPECTED
};

extern char *prog;						// prog������little c�ű����Եĳ�������ָ�����
extern char *p_buf;						// ����prog,��Ϊ��ʱ,�������ܻ���ֻ�˷,�������������˴�ָ�����

extern char token[MAX_T_LEN + 1];	   // �˱����Ǵ�ŵ�ǰ��� 
extern tok_types token_type;		   // �˱����Ǵ�ű�ǵ����� 
extern token_ireps tok;				   // �˱����Ǵ���ڲ��ؼ��ֵ�ֵ

extern int ret_value; // ����û�����ĺ����ķ���ֵ

extern bool breakfound; // true if break encountered 

// Mini C++ �еĽ����쳣��
class InterpExc 
{
	error_msg err;
public:
	InterpExc(error_msg e) { err = e; }
	error_msg get_err() { return err; }
};

// Interpreter prototypes. 
void prescan();									// Ԥɨ��
void decl_global();								// ����һ��ȫ�ֱ���
void call();									// ����һ������
void putback();									// �ع�һ��token
void decl_local();								// ����һ���ֲ�����
void exec_if();									// ִ��if���
void find_eob();								// Ѱ��һ������
void exec_for();								// ִ��for���
void exec_switch();								// ִ��switch���
void get_params();								// ��ú�������
void get_args();								// ����ʽ����ת��Ϊ�ֲ�����
void exec_while();								// ִ��while���
void exec_do();									// ִ��do���
void exec_cout();								// ִ�п���̨���
void exec_cin();								// ִ�п���̨����
void assign_var(char *var_name, int value);		// ����һ�������洢�ռ�
bool load_program(char *p, char *fname);		// װ�س���
int find_var(char *s);
void interp();
void func_ret();
char *find_func(char *name);					// ��һ���û�������λ�ã���ʹcall()������ȷ��������û�����ĺ���
bool is_var(char *s);
token_ireps find_var_type(char *s);

/* ���ʽ�ݹ���÷������ӳ��� */
void eval_exp(int &value);
void eval_exp0(int &value);
void eval_exp1(int &value);
void eval_exp2(int &value);
void eval_exp3(int &value);
void eval_exp4(int &value);
void eval_exp5(int &value);

void atom(int &value);				// ����atom ��ȡ�ñ��������ʽ��������������ֵ���������ȫ�ֱ���value��
void sntx_err(error_msg error);		// ����sntx_err������Ļ����ʾ�﷨����ͨ��������������error�����Ͳ�ͬ�Ĵ���
void putback();						// ����putback�����Ƿ���һ���ַ����������ڷ�����ʱ����ܻ������ǰ��һ���ַ�����������Ҫ�ڷ��ظ�ԭ�ȵ��ַ��� */
bool isdelim(char c);

token_ireps look_up(char *s);		// look_up������ͨ�������ַ����Ƿ��ǹؼ��ֱ��е�ֵ��iswhite�ǲ鿴�ַ��Ƿ��ǿհ��ַ�
int find_var(char *s);				// ����find_var�ǲ���һ��������ֵ
tok_types get_token();				// get_token�����Ǵӵ�ǰ���ַ����еõ�һ���������ڲ��ĵ���
int internal_func(char *s);			// ���ڲ������в����Ƿ��и�*s���ڲ�����
bool is_var(char *s);

// ��׼�������
int call_getchar();
int call_putchar();
int call_abs();
int call_rand();

