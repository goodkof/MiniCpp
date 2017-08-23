// Recursive descent parser for integer expressions. 
#define _CRT_SECURE_NO_WARNINGS
#include <iostream> 
#include <cstring> 
#include <cstdlib> 
#include <cctype> 
#include "mccommon.h" 

using namespace std;

/* �ؼ��ֽṹ */
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


/* �ڲ������ṹ */
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
�������ֳ���ʼִ��
����������eval_exp()����
*/

void eval_exp(int &value)
{
	/* ����ȡ��һ������ */
	get_token();
	/* �ж��Ƿ��з��ţ����û�з��ش���û�б��ʽ */
	if (!*token) {
		throw InterpExc(NO_EXP);
	}
	/* ��������ʡ�Ϊ ";" �Ļ�����ô��ʾ��һ���ձ��ʽ���ձ��ʽ��ֵΪ0,��ʾ�� */
	if (*token == ';') {
		value = 0; // empty expression 
		return;
	}
	/* �����������2���������ô���ʽ�ݹ������ʼִ�� */
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
		/* ͨ������is_var�鿴�������ʾ�����Ƿ��Ѿ���ȫ�ֻ�ֲ��������� */
		if (is_var(token)) { // if a var, see if assignment 
			 /* ����ڵĻ�����ô��token���Ƶ�temp������ */
			strcpy(temp, token);
			//������ڻ���ô���Ȱ�token�ڴ�ŵ�ֵcopy��temp��,��Ϊȷ���Ǹ�ֵ��䣬��ô�Ϳ����Ǳ��ʽ������token��ȫ�ֱ���������Ҫ�����ﱣ�档
			temp_tok = token_type;//��������������Ϊtoken_type
			get_token();//ȡ����һ�����ţ���������������ֻ�û���壬�Ǿ�˵����Ӧ�����и���ֵ�����
			if (*token == '=') { // is an assignment /�����һ��=���ŵĻ����Ǿ�˵�����Ǹ�ֵ���
				get_token();//ȡ�÷���
				eval_exp0(value); // get value to assign  //�ݹ������ʽ
				assign_var(temp, value); // assign the value   //��������ֵ
				return;
			}
			else { // not an assignment 
				putback(); // restore original token  //����ָ�token��ǰָ���λ��
				strcpy(token, temp);//��temp��ֵ��token
				token_type = temp_tok;//����Ϊtemp_tok����
			}
		}
	}
	eval_exp1(value);//�ݹ������һ��
}

// Process relational operators. 
void eval_exp1(int &value)
{
	int partial_value;//��һ����Ҫ����ı��ʽ�е�ֵ
	char op;//�˱����ǲ���������
	char relops[]//��������
	= 
	{
		LT, LE, GT, GE, EQ, NE, 0
	};

	eval_exp2(value);//������һ��

	op = *token;//������
	if (strchr(relops, op)) //������ڲ�����
	{
		get_token();//ȡ����һ������
		eval_exp2(partial_value);//ȡ��partial_value��ֵ

		switch (op) 
		{ // perform the relational operation 
		case LT://����LT����� С�ں�
			value = value < partial_value; // ��������ֵ�ŵ�values�values��ʱֻ������1��0
			break;
		case LE://����LE����� С�ڵ��ں�
			value = value <= partial_value;
			break;
		case GT://����GT����� ���ں�
			value = value > partial_value;
			break;
		case GE://����GE����� ���ڵ��ں�
			value = value >= partial_value;
			break;
		case EQ://����EQ���� �����Ƿ���ȷ���
			value = value == partial_value;
			break;
		case NE://����NE���� ���Բ��ȷ���
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

	eval_exp3(value);//������һ��ݹ�

	while ((op = *token) == '+' || op == '-') //�������ӺŻ����
	{
		get_token();//ȡ��һ������

		if (token_type == DELIMITER &&
			!strchr(okops, *token))
			throw InterpExc(SYNTAX);

		eval_exp3(partial_value);//�õ�һ�����֡����ֵ���������Գ˷����������ʽ��������ֵ

		switch (op) { // add or subtract //ѡ��һ������
		case '-'://�������
			value = value - partial_value;
			break;
		case '+'://����Ӻ�
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

	eval_exp4(value);//������һ��ݹ�

	while ((op = *token) == '*' || op == '/'//�������˻�����
		|| op == '%') {
		get_token();//ȡ����һ������

		if (token_type == DELIMITER &&
			!strchr(okops, *token))
			throw InterpExc(SYNTAX);

		eval_exp4(partial_value);//�õ�һ������

		switch (op) { // mul, div, or modulus 
		case '*'://����˷�
			value = value * partial_value;
			break;
		case '/'://�������
			if (partial_value == 0)
				throw InterpExc(DIV_BY_ZERO);
			value = (value) / partial_value;
			break;
		case '%'://����ȡ������.��������value��
			t = (value) / partial_value;
			value = value - (t * partial_value);
			break;
		}
	}
}

//eval_exp4����һԪ�ӻ��
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

	if ((*token == '(')) {//����(��()����������ȼ�
		get_token();

		eval_exp0(value); // get subexpression 

		if (*token != ')')//���û��)˵���﷨����
			throw InterpExc(PAREN_EXPECTED);
		get_token();
	}
	else
		atom(value);//ȡ��һ��ֵ
}

// Find value of number, variable, or function. 
void atom(int &value)
{
	int i;
	char temp[MAX_ID_LEN + 1];

	/*  ѡ��token���� */
	switch (token_type) 
	{
	case IDENTIFIER:/* ����Ǳ������� */
		i = internal_func(token);
		/* ���ڲ��ṹ�в��Һ������Ƿ���� */
		if (i != -1) 
		{
			/* �������-1�Ļ�����ô��ʾ���ڲ����� */
			// Call "standard library" function. 
			value = (*intern_func[i].p)();
			/* ͨ���ṹ�еĺ���ָ������ڲ�����,���ص�ֵ�ŵ� valueָ���ַ�� */
		}
		else if (find_func(token)) 
		{
			/* ����ͨ������find_func�����Ƿ����û�����ĺ���������ǵĻ� */
			// Call programmer-created function. 
			call();/* ͨ��call���������û�����ĺ��� */
			value = ret_value;/* �����ķ���ֵ�ŵ�valueָ��ĵ�ַ�� */
		}
		else {
			/* �������Ϊ����һ�����������֣�ͨ��find_var�����ҵ�token���ŵ�����ֵ��Ȼ��ŵ�value�� */
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
		/* �����һ�����ֵĻ� ��ôͨ����׼�⺯���е�atoi(��stdio.h�ж����˴˺���) ���ַ�ת��Ϊ�������ͣ��Է�����ʽ���� */
		value = atoi(token);
		get_token();

		return;
	case DELIMITER: // see if character constant 
		/* �����һ���ַ������Ļ� */
		if (*token == '\'') 
		{
			/* �����'�ַ�����ô�ѵ�ǰ��ֵ�ŵ�value�� */
			value = *prog;
			prog++;
			if (*prog != '\'')
				throw InterpExc(QUOTE_EXPECTED);
			/* ���������'���Ž�β�����׳��﷨���� */
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
	/* ���ַ�ָ���������ŵ����﷨����ʱ�Ĵ�����Ϣ */
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
/* ���ܣ��˳����Ǵ��ַ�����ȡ��һ�������ʡ����ѵ��ʴ��ַ���ƥ�����     */
/* -------------------------------------------------------------------- */
/* ����˵����                                                           */
/* token_type:�������������˵����ɵĹؼ��ֺ͵����͵�����              */
/* tok��      ��Ϊ��Źؼ���                                            */
/* token��    ��Ϊ����ַ��������ֵ����з��ŵ�ֵ                        */
/* temp:      ��Ϊtokenָ��Ļ���������Ҫ�ǽ����ָ���ƶ��ķ��������   */
/* -------------------------------------------------------------------- */
tok_types get_token()
{

	char *temp;

	token_type = UNDEFTT; tok = UNDEFTOK;

	temp = token; /* temp Ϊtoken�Ļ���ֵ */
	*temp = '\0';

	// Skip over white space. 
	while (isspace(*prog) && *prog) ++prog;
	/* iswhite��׼�⺯���ڴ˵Ĺ���������ԭ�����еĹؼ���,����ǿհ׷��ŵĻ�����ô������ */
	// Skip over newline. 
	while (*prog == '\r') {
		++prog;
		++prog;
		/* ����ǻ��з��Ļ���Ҳ�������ַ���ָ��prog��ַ��1 */
		// Again, skip over white space. 
		while (isspace(*prog) && *prog) ++prog;
	}
	/* ������������пհ׷��ŵĻ�����ôҲ�Ǻ��Ե� */

	// Check for end of program. 
	if (*prog == '\0') { /* ���progָ�뵽���ļ���β��prog��ֵ�д�ŵ���'\0' */
		*token = '\0'; /* *token��ֵΪ'\0' */
		tok = END;
		/* tok����Ϊ�����������ڲ��ؼ������ͣ������Ǳ�ʾ�ﵽ�ļ���β������ */
		return (token_type = DELIMITER);
	}
	/* ��Ϊ����� */
	// Check for block delimiters. 
	if (strchr("{}", *prog)) {
		*temp = *prog;
		temp++;
		*temp = '\0';
		prog++;
		return (token_type = BLOCK);
	}

	// Look for comments. 
	/* ����ע�Ͳ��֣������/ '+' *�ַ���ͷ�ģ���ʾ����ע�ͣ�Ȼ������* '+' /��Ϊ��β�� */
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
		/* ����! < > =�⼸��������� */
		// Check for double-ops. 
		if (strchr("!<>=+-", *prog)) {
			switch (*prog) {
			case '=': /* ���=���滹��һ��=���ŵĻ�����ô�˵���Ϊ�ڲ�����EQ */
				if (*(prog + 1) == '=') {
					prog++; prog++;
					*temp = EQ;
					temp++; *temp = EQ; temp++;
					*temp = '\0';
				}
				break;
			case '!':/* �����!��=���ŵĻ�����ôprog�����ڲ�����NE */
				if (*(prog + 1) == '=') {
					prog++; prog++;
					*temp = NE;
					temp++; *temp = NE; temp++;
					*temp = '\0';
				}
				break;
			case '<':/* �����<���ż�=���ŵĻ�����ô�����ڲ�����LE */
				if (*(prog + 1) == '=') {
					prog++; prog++;
					*temp = LE; temp++; *temp = LE;
				}
				else if (*(prog + 1) == '<') {
					prog++; prog++;
					*temp = LS; temp++; *temp = LS;
				}
				else {
					prog++; /* ��������ڲ�����LT */
					*temp = LT;
				}
				temp++;
				*temp = '\0';
				break;
			case '>':
				if (*(prog + 1) == '=') {
					/* ���������>��=���ŵĻ�����ô�����ڲ�������Ų����������GE */
					prog++; prog++;
					*temp = GE; temp++; *temp = GE;
				}
				else if (*(prog + 1) == '>') {
					prog++; prog++;
					*temp = RS; temp++; *temp = RS;
				}
				else {
					/* ��������ڲ������������GT */
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
		/* �����ǲ����Ƿ������������ */
		// Check for other delimiters. 
		if (strchr("+-*^/%=;:(),'", *prog)) {
			*temp = *prog;
			prog++;
			temp++;
			*temp = '\0';
			return (token_type = DELIMITER); /* ��������ΪDELIMITER,������Ϊ����������� */
		}

		// Read a quoted string. 
		if (*prog == '"') { /* �����"���ŵĻ�����ô�ͱ�ʾ���ַ��� */
			prog++;
			/* ֻҪ������"���ţ���ô�Ͱ����еĶ���ȫ���ŵ�temp�� */
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
			return (token_type = STRING);  /* ��������Ϊ�ַ��� */
		}

		// Read an integer number. 
		if (isdigit(*prog)) {/* �����һ�����ֵĻ�,�����е������ַ��������� */
			while (!isdelim(*prog)) {
				if ((temp - token) < MAX_ID_LEN)
					*temp++ = *prog;
				prog++;
			}
			*temp = '\0';
			return (token_type = NUMBER);
		}

		// Read identifier or keyword. 
		if (isalpha(*prog)) {  /* �����һ���������ǹؼ��ֵĻ�����ôҲ�ǰѷ�����ɵ��� */ 
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
			tok = look_up(token); // convert to internal form  /* ȥ���ű��в����Ƿ��ǹؼ��� */ 
			if (tok) token_type = KEYWORD; // is a keyword  /* ���tokΪ��Ļ�����ô������ʾ��ǹؼ��� */ 
			else token_type = IDENTIFIER; /* ����������Ǳ������� */
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
