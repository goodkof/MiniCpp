// Recursive descent parser for integer expressions. 
#define _CRT_SECURE_NO_WARNINGS
#include <iostream> 
#include <cstring> 
#include <cstdlib> 
#include <cctype> 
#include "mccommon.h" 

using namespace std;

/* 关键字结构 */
// Keyword lookup table. 
// Keywords must be entered lowercase. 
struct commands {
	char command[20];
	token_ireps tok;
} com_table[] = {
	"if", IF,
	"else", ELSE,
	"for", FOR,
	"do", DO,
	"while", WHILE,
	"char", CHAR,
	"int", INT,
	"return", RETURN,
	"switch", SWITCH,
	"break", BREAK,
	"case", CASE,
	"cout", COUT,
	"cin", CIN,
	"", END  // mark end of table 
};


/* 内部函数结构 */
// This structure links a library function name 
// with a pointer to that function. 
struct intern_func_type {
	char *f_name; // function name 
	int(*p)();   // pointer to the function 
} intern_func[] = {
	"getchar", call_getchar,
	"putchar", call_putchar,
	"abs", call_abs,
	"rand", call_rand,
	"", nullptr  // null terminate the list 
};

/*
Entry point into parser. 
分析部分程序开始执行
程序的入口是eval_exp()函数
*/

void eval_exp(int &value)
{
	/* 首先取得一个符号 */
	get_token();
	/* 判断是否有符号，如果没有返回错误，没有表达式 */
	if (!*token) {
		throw InterpExc(NO_EXP);
	}
	/* 如果“单词”为 ";" 的话，那么表示是一个空表达式，空表达式的值为0,表示假 */
	if (*token == ';') {
		value = 0; // empty expression 
		return;
	}
	/* 如果不是上面2种情况，那么表达式递归分析开始执行 */
	eval_exp0(value);

	putback(); // return last token read to input stream 
}

// Process an assignment expression. 
void eval_exp0(int &value)
{
	// temp holds name of var receiving the assignment. 
	char temp[MAX_ID_LEN + 1];

	tok_types temp_tok;

	if (token_type == IDENTIFIER) {
		/* 通过变量is_var查看这个“标示符”是否已经在全局或局部变量表中 */
		if (is_var(token)) { // if a var, see if assignment 
			 /* 如果在的话，那么把token复制到temp变量中 */
			strcpy(temp, token);
			//如果不在话那么将先把token内存放的值copy到temp里,因为确定是赋值语句，那么就可能是表达式，由于token是全局变量，所以要在这里保存。
			temp_tok = token_type;//缓冲标记类型设置为token_type
			get_token();//取得下一个符号，根据这个变量名字还没定义，那就说明他应该是有个赋值的语句
			if (*token == '=') { // is an assignment /如果是一个=符号的话，那就说明他是赋值语句
				get_token();//取得符号
				eval_exp0(value); // get value to assign  //递归计算表达式
				assign_var(temp, value); // assign the value   //给变量赋值
				return;
			}
			else { // not an assignment 
				putback(); // restore original token  //否则恢复token以前指针的位置
				strcpy(token, temp);//把temp的值给token
				token_type = temp_tok;//类型为temp_tok类型
			}
		}
	}
	eval_exp1(value);//递归进入下一层
}

// Process relational operators. 
void eval_exp1(int &value)
{
	int partial_value;//另一个需要计算的表达式中的值
	char op;//此变量是操作符变量
	char relops[]//操作符表
	= 
	{
		LT, LE, GT, GE, EQ, NE, 0
	};

	eval_exp2(value);//进入下一层

	op = *token;//操作符
	if (strchr(relops, op)) //如果属于操作符
	{
		get_token();//取得下一个符号
		eval_exp2(partial_value);//取得partial_value的值

		switch (op) 
		{ // perform the relational operation 
		case LT://处理LT的情况 小于号
			value = value < partial_value; // 处理的真假值放到values里，values此时只可能是1或0
			break;
		case LE://处理LE的情况 小于等于号
			value = value <= partial_value;
			break;
		case GT://处理GT的情况 大于号
			value = value > partial_value;
			break;
		case GE://处理GE的情况 大于等于号
			value = value >= partial_value;
			break;
		case EQ://处理EQ符号 测试是否相等符号
			value = value == partial_value;
			break;
		case NE://处理NE符号 测试不等符号
			value = value != partial_value;
			break;
		}
	}
}

// Add or subtract two terms. 
void eval_exp2(int &value)
{
	char  op;
	int partial_value;
	char okops[] = {
		'(', INC, DEC, '-', '+', 0
	};

	eval_exp3(value);//进入下一层递归

	while ((op = *token) == '+' || op == '-') //处理多个加号或减号
	{
		get_token();//取得一个符号

		if (token_type == DELIMITER &&
			!strchr(okops, *token))
			throw InterpExc(SYNTAX);

		eval_exp3(partial_value);//得到一个数字。这个值可能是来自乘法或除法或表达式传回来的值

		switch (op) { // add or subtract //选择一个符号
		case '-'://处理减号
			value = value - partial_value;
			break;
		case '+'://处理加号
			value = value + partial_value;
			break;
		}
	}
}

// Multiply or divide two factors. 
void eval_exp3(int &value)
{
	char  op;
	int partial_value, t;
	char okops[] = {
		'(', INC, DEC, '-', '+', 0
	};

	eval_exp4(value);//进入下一层递归

	while ((op = *token) == '*' || op == '/'//处理连乘或连除
		|| op == '%') {
		get_token();//取得下一个符号

		if (token_type == DELIMITER &&
			!strchr(okops, *token))
			throw InterpExc(SYNTAX);

		eval_exp4(partial_value);//得到一个数字

		switch (op) { // mul, div, or modulus 
		case '*'://处理乘法
			value = value * partial_value;
			break;
		case '/'://处理除法
			if (partial_value == 0)
				throw InterpExc(DIV_BY_ZERO);
			value = (value) / partial_value;
			break;
		case '%'://处理取摸运算.余数放在value里
			t = (value) / partial_value;
			value = value - (t * partial_value);
			break;
		}
	}
}

//eval_exp4处理一元加或减
// Is a unary +, -, ++, or --. 
void eval_exp4(int &value)
{
	char  op;
	char temp;

	op = '\0';
	if (*token == '+' || *token == '-' ||
		*token == INC || *token == DEC)
	{
		temp = *token;
		op = *token;
		get_token();
		if (temp == INC)
			assign_var(token, find_var(token) + 1);
		if (temp == DEC)
			assign_var(token, find_var(token) - 1);
	}

	eval_exp5(value);
	if (op == '-') value = -(value);
}

// Process parenthesized expression. 
void eval_exp5(int &value)
{

	if ((*token == '(')) {//处理(，()具有最高优先级
		get_token();

		eval_exp0(value); // get subexpression 

		if (*token != ')')//如果没有)说明语法错误
			throw InterpExc(PAREN_EXPECTED);
		get_token();
	}
	else
		atom(value);//取得一个值
}

// Find value of number, variable, or function. 
void atom(int &value)
{
	int i;
	char temp[MAX_ID_LEN + 1];

	/*  选择token类型 */
	switch (token_type) 
	{
	case IDENTIFIER:/* 如果是变量或函数 */
		i = internal_func(token);
		/* 从内部结构中查找函数名是否存在 */
		if (i != -1) 
		{
			/* 如果不是-1的话，那么表示是内部函数 */
			// Call "standard library" function. 
			value = (*intern_func[i].p)();
			/* 通过结构中的函数指针调用内部函数,返回的值放到 value指向地址里 */
		}
		else if (find_func(token)) 
		{
			/* 否则通过函数find_func查找是否是用户定义的函数，如果是的话 */
			// Call programmer-created function. 
			call();/* 通过call函数调用用户定义的函数 */
			value = ret_value;/* 函数的返回值放到value指向的地址里 */
		}
		else {
			/* 否则就认为他是一个变量的名字，通过find_var函数找到token里存放到变量值，然后放到value里 */
			value = find_var(token); // get var's value 
			strcpy(temp, token); // save variable name 

								 // Check for ++ or --. 
			get_token();
			if (*token == INC || *token == DEC) {
				if (*token == INC)
					assign_var(temp, find_var(temp) + 1);
				else
					assign_var(temp, find_var(temp) - 1);
			}
			else putback();
		}

		get_token();
		return;
	case NUMBER: // is numeric constant 
		/* 如果是一个数字的话 那么通过标准库函数中的atoi(在stdio.h中定义了此函数) 把字符转化为数字类型，以方便表达式计算 */
		value = atoi(token);
		get_token();

		return;
	case DELIMITER: // see if character constant 
		/* 如果是一个字符常量的话 */
		if (*token == '\'') 
		{
			/* 如果是'字符，那么把当前的值放到value里 */
			value = *prog;
			prog++;
			if (*prog != '\'')
				throw InterpExc(QUOTE_EXPECTED);
			/* 如果不是以'符号结尾，就抛出语法错误 */
			prog++;
			get_token();

			return;
		}
		if (*token == ')') return; // process empty expression 
		else throw InterpExc(SYNTAX);  // otherwise, syntax error 
	default:
		throw InterpExc(SYNTAX); // syntax error 
	}
}

// Display an error message. 
void sntx_err(error_msg error)
{
	char *p, *temp;
	int linecount = 0;
	/* 此字符指针数组里存放的是语法分析时的错误消息 */
	static char *e[] = {
		"Syntax error",
		"No expression present",
		"Not a variable",
		"Duplicate variable name",
		"Duplicate function name",
		"Semicolon expected",
		"Unbalanced braces",
		"Function undefined",
		"Type specifier expected",
		"Return without call",
		"Parentheses expected",
		"While expected",
		"Closing quote expected",
		"Division by zero",
		"{ expected (control statements must use blocks)",
		"Colon expected"
	};

	// Display error and line number. 
	cout << "\n" << e[error];
	p = p_buf;
	while (p != prog) { // find line number of error 
		p++;
		if (*p == '\r') {
			linecount++;
		}
	}
	cout << " in line " << linecount << endl;

	temp = p;
	while (p > p_buf && *p != '\n') p--;

	// Display offending line. 
	while (p <= temp)
		cout << *p++;

	cout << endl;
}

// Get a token. 
/* -------------------------------------------------------------------- */
/* 功能：此程序是从字符流中取得一个”单词“，把单词从字符中匹配出来     */
/* -------------------------------------------------------------------- */
/* 变量说明：                                                           */
/* token_type:这个变量是用来说明组成的关键字和的类型的类型              */
/* tok：      此为存放关键字                                            */
/* token：    此为存放字符串，数字等所有符号的值                        */
/* temp:      此为token指针的缓冲区，主要是解决了指针移动的方便的问题   */
/* -------------------------------------------------------------------- */
tok_types get_token()
{

	char *temp;

	token_type = UNDEFTT; tok = UNDEFTOK;

	temp = token; /* temp 为token的缓冲值 */
	*temp = '\0';

	// Skip over white space. 
	while (isspace(*prog) && *prog) ++prog;
	/* iswhite标准库函数在此的功能是跳过原代码中的关键字,如果是空白符号的话，那么就跳过 */
	// Skip over newline. 
	while (*prog == '\r') {
		++prog;
		++prog;
		/* 如果是换行符的话，也跳过，字符流指针prog地址加1 */
		// Again, skip over white space. 
		while (isspace(*prog) && *prog) ++prog;
	}
	/* 在新行中如果有空白符号的话，那么也是忽略掉 */

	// Check for end of program. 
	if (*prog == '\0') { /* 如果prog指针到达文件结尾，prog的值中存放的是'\0' */
		*token = '\0'; /* *token给值为'\0' */
		tok = END;
		/* tok变量为本解释器的内部关键字类型，这里是表示达到文件结尾，结束 */
		return (token_type = DELIMITER);
	}
	/* 此为程序块 */
	// Check for block delimiters. 
	if (strchr("{}", *prog)) {
		*temp = *prog;
		temp++;
		*temp = '\0';
		prog++;
		return (token_type = BLOCK);
	}

	// Look for comments. 
	/* 处理注释部分，如果是/ '+' *字符开头的，表示其是注释，然后是以* '+' /作为结尾的 */
	if (*prog == '/')
		if (*(prog + 1) == '*') { // is a /* comment 
			prog += 2;
			do { // find end of comment 
				while (*prog != '*') prog++;
				prog++;
			} while (*prog != '/');
			prog++;
			return (token_type = DELIMITER);
		}
		else if (*(prog + 1) == '/') { // is a // comment 
			prog += 2;
			// Find end of comment. 
			while (*prog != '\r' && *prog != '\0') prog++;
			if (*prog == '\r') prog += 2;
			return (token_type = DELIMITER);
		}
		/* 处理! < > =这几个运算符号 */
		// Check for double-ops. 
		if (strchr("!<>=+-", *prog)) {
			switch (*prog) {
			case '=': /* 如果=后面还是一个=符号的话，那么此单词为内部符号EQ */
				if (*(prog + 1) == '=') {
					prog++; prog++;
					*temp = EQ;
					temp++; *temp = EQ; temp++;
					*temp = '\0';
				}
				break;
			case '!':/* 如果是!加=符号的话，那么prog就是内部符号NE */
				if (*(prog + 1) == '=') {
					prog++; prog++;
					*temp = NE;
					temp++; *temp = NE; temp++;
					*temp = '\0';
				}
				break;
			case '<':/* 如果是<符号加=符号的话，那么就是内部符号LE */
				if (*(prog + 1) == '=') {
					prog++; prog++;
					*temp = LE; temp++; *temp = LE;
				}
				else if (*(prog + 1) == '<') {
					prog++; prog++;
					*temp = LS; temp++; *temp = LS;
				}
				else {
					prog++; /* 否则就是内部符号LT */
					*temp = LT;
				}
				temp++;
				*temp = '\0';
				break;
			case '>':
				if (*(prog + 1) == '=') {
					/* 如果符号是>加=符号的话，那么就是内部运算符号布尔运算符号GE */
					prog++; prog++;
					*temp = GE; temp++; *temp = GE;
				}
				else if (*(prog + 1) == '>') {
					prog++; prog++;
					*temp = RS; temp++; *temp = RS;
				}
				else {
					/* 否则就是内部布尔运算符号GT */
					prog++;
					*temp = GT;
				}
				temp++;
				*temp = '\0';
				break;
			case '+':
				if (*(prog + 1) == '+') {
					prog++; prog++;
					*temp = INC; temp++; *temp = INC;
					temp++;
					*temp = '\0';
				}
				break;
			case '-':
				if (*(prog + 1) == '-') {
					prog++; prog++;
					*temp = DEC; temp++; *temp = DEC;
					temp++;
					*temp = '\0';
				}
				break;
			}

			if (*token) return(token_type = DELIMITER);
		}
		/* 下面是测试是否是算术运算符 */
		// Check for other delimiters. 
		if (strchr("+-*^/%=;:(),'", *prog)) {
			*temp = *prog;
			prog++;
			temp++;
			*temp = '\0';
			return (token_type = DELIMITER); /* 返回类型为DELIMITER,此类型为算术运算符号 */
		}

		// Read a quoted string. 
		if (*prog == '"') { /* 如果是"符号的话，那么就表示是字符串 */
			prog++;
			/* 只要不等于"符号，那么就把所有的东西全部放到temp里 */
			while (*prog != '"' && *prog != '\r' && *prog) {
				// Check for \n escape sequence. 
				if (*prog == '\\') {
					if (*(prog + 1) == 'n') {
						prog++;
						*temp++ = '\n';
					}
				}
				else if ((temp - token) < MAX_T_LEN)
					*temp++ = *prog;

				prog++;
			}
			if (*prog == '\r' || *prog == 0)
				throw InterpExc(SYNTAX);
			prog++; *temp = '\0';
			return (token_type = STRING);  /* 返回类型为字符串 */
		}

		// Read an integer number. 
		if (isdigit(*prog)) {/* 如果是一个数字的话,把所有的数字字符连接起来 */
			while (!isdelim(*prog)) {
				if ((temp - token) < MAX_ID_LEN)
					*temp++ = *prog;
				prog++;
			}
			*temp = '\0';
			return (token_type = NUMBER);
		}

		// Read identifier or keyword. 
		if (isalpha(*prog)) {  /* 如果是一个变量或是关键字的话，那么也是把符号组成单词 */ 
			while (!isdelim(*prog)) {
				if ((temp - token) < MAX_ID_LEN)
					*temp++ = *prog;
				prog++;
			}
			token_type = TEMP;
		}

		*temp = '\0';

		// Determine if token is a keyword or identifier. 
		if (token_type == TEMP) {
			tok = look_up(token); // convert to internal form  /* 去符号表中查找是否是关键字 */ 
			if (tok) token_type = KEYWORD; // is a keyword  /* 如果tok为真的话，那么这个单词就是关键字 */ 
			else token_type = IDENTIFIER; /* 否则这个就是变量的名 */
		}

		// Check for unidentified character in file. 
		if (token_type == UNDEFTT)
			throw InterpExc(SYNTAX);

		return token_type;
}

// Return a token to input stream. 
void putback()
{
	char *t;

	t = token;
	for (; *t; t++) prog--;
}

// Look up a token's internal representation in the 
// token table. 
token_ireps look_up(char *s)
{
	int i;
	char *p;

	// Convert to lowercase. 
	p = s;
	while (*p) { *p = tolower(*p); p++; }

	// See if token is in table. 
	for (i = 0; *com_table[i].command; i++) {
		if (!strcmp(com_table[i].command, s))
			return com_table[i].tok;
	}

	return UNDEFTOK; // unknown command 
}

// Return index of internal library function or -1 if 
// not found. 
int internal_func(char *s)
{
	int i;

	for (i = 0; intern_func[i].f_name[0]; i++) {
		if (!strcmp(intern_func[i].f_name, s))  return i;
	}
	return -1;
}

// Return true if c is a delimiter. 
bool isdelim(char c)
{
	if (strchr(" !:;,+-<>'/*%^=()", c) || c == 9 ||
		c == '\r' || c == 0) return true;
	return false;
}
