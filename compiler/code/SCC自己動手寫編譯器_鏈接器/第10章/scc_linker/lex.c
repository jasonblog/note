// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"
TkWord* tk_hashtable[MAXKEY];	// 单词哈希表
DynArray tktable;				// 单词表中放置标识符，包括变量名,函数名,结构定义名
DynString tkstr;				// 单词字符串
DynString sourcestr;			// 单词源码字符串
int tkvalue;					// 单词值(单词为整型常量)
char ch;						// 当前取到的源码字符
int token;						// 单词编码
int line_num;					// 行号


/***********************************************************
 * 功能: 将运算符、关键字、常量直接放入单词表
 * tp:	 单词指针											
 **********************************************************/
TkWord* tkword_direct_insert(TkWord* tp)
{
	int keyno;
	tp->sym_identifier = NULL;
	tp->sym_struct = NULL;
	dynarray_add(&tktable,tp);
	keyno = elf_hash(tp->spelling);
	tp->next = tk_hashtable[keyno];
	tk_hashtable[keyno] = tp;
	return tp;
}

/***********************************************************
 *	功能:	在单词表中查找单词
 *  p:		单词字符串指针
 *  keyno:	单词的哈希值
 **********************************************************/
TkWord* tkword_find(char *p, int keyno)
{
    TkWord *tp = NULL, *tp1;
    for (tp1 = tk_hashtable[keyno]; tp1; tp1 = tp1->next) 
	{
		if(!strcmp(p,tp1->spelling))
		{
			token = tp1->tkcode;
			tp = tp1;
		}
    }
	return tp;
}


/***********************************************************
 * 功能:	标识符插入单词表，先查找，查找不到再插入单词表
 * p:		单词字符串指针
 **********************************************************/
TkWord* tkword_insert(char *p)
{
    TkWord*  tp;
	int keyno;
    char   *s;
    char *end;
	int length;

    keyno = elf_hash(p);	
    tp = tkword_find(p,keyno);
	if(tp == NULL)
	{
		length = strlen(p);		
		tp = (TkWord*) mallocz (sizeof (TkWord) + length + 1);
		tp->next = tk_hashtable[keyno];	
		tk_hashtable[keyno] = tp;
		dynarray_add(&tktable,tp);
		tp->tkcode = tktable.count - 1;
		s = (char *) tp + sizeof (TkWord);
		tp->spelling = (char *) s;
		for (end = p + length; p < end;) 
		{
			*s++ = *p++;
		}
		*s = (char) '\0';
		
		tp->sym_identifier = NULL;
		tp->sym_struct = NULL;
	}
    return tp;
}

/***********************************************************
 * 功能:	判断c是否为字母(a-z,A-Z)或下划线(-)
 * c:		字符值
 **********************************************************/
int is_nodigit(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

/***********************************************************
 * 功能:	判断c是否为数字
 * c:		字符值
 **********************************************************/
int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/***********************************************************
 * 功能:	从SC源文件中读取一个字符
 **********************************************************/
void getch()
{
	ch=getc(fin); //文件尾返回EOF，其它返回实际字节值		
}

/***********************************************************
 * 功能:	解析标识符
 **********************************************************/
void parse_identifier()
{
	dynstring_reset(&tkstr);
	dynstring_chcat(&tkstr,ch);
	getch();
	while(is_nodigit(ch) || is_digit(ch))
	{
		dynstring_chcat(&tkstr,ch);
		getch();
	}
	dynstring_chcat(&tkstr,'\0');
}

/***********************************************************
 * 功能:	解析整型常量
 **********************************************************/
void parse_num()
{
	dynstring_reset(&tkstr);
	dynstring_reset(&sourcestr);		
	do{
		dynstring_chcat(&tkstr,ch);
		dynstring_chcat(&sourcestr,ch);
		getch();
	}while (is_digit(ch));
	if(ch=='.')
	{
		do{
			dynstring_chcat(&tkstr,ch);
			dynstring_chcat(&sourcestr,ch);
			getch();
		}while (is_digit(ch));	
	}
	dynstring_chcat(&tkstr,'\0');
	dynstring_chcat(&sourcestr,'\0');
	tkvalue = atoi(tkstr.data);
}

/***********************************************************
  在这我们分析一下，如果我们支持GB2312字符集的话，确实没有什么太大问题，
  因为GB2312的码表范围从A1A0-FEFF，完全兼容ASCII，不会跟ASCII区的编码产生冲突，
  但是GBK8140-FEFE,如╘ AC5C会产生误判，将后一字节当成\\,错误警告。。。
  字符串中出现GBK中XX5C的字符会出现不认识的转义字符，错把5C当成\
 **********************************************************/
/***********************************************************
 * 功能:	解析字符常量、字符串常量
 * sep:		字符常量界符标识为单引号(')
			字符串常量界符标识为双引号(")
 **********************************************************/
void parse_string(char sep)
{	
	char c;
	dynstring_reset(&tkstr);
	dynstring_reset(&sourcestr);
	dynstring_chcat(&sourcestr,sep);
	getch();
	for(;;)
	{
		if(ch == sep)
			break;
		else if(ch == '\\')
		{
			dynstring_chcat(&sourcestr,ch);
	        getch();
			switch(ch) // 解析转义字符
			{
			case '0':
				c = '\0';
                break; 
			case 'a':
				c = '\a';
                break;
            case 'b':
                c = '\b';
                break;
			case 't':
                c = '\t';
                break;
            case 'n':
                c = '\n';
                break;
            case 'v':
                c = '\v';
                break;
            case 'f':
                c = '\f';
                break;
            case 'r':
                c = '\r';
                break;
            case '\"':
				c = '\"';
                break;	
			case '\'':
				c = '\'';
				break;
            case '\\': 
				c = '\\';
				break;
           default:
				    c = ch;
                if (c >= '!' && c <= '~')
                    warning("非法转义字符: \'\\%c\'", c); // 33-126 0x21-0x7E可显示字符部分
                else
                    warning("非法转义字符: \'\\0x%x\'", c); 
                break;
			}
			dynstring_chcat(&tkstr,c);
			dynstring_chcat(&sourcestr,ch);
			getch();
		}
		else
		{
			dynstring_chcat(&tkstr,ch);
			dynstring_chcat(&sourcestr,ch);
			getch();
		}

	}
	dynstring_chcat(&tkstr,'\0');
	dynstring_chcat(&sourcestr,sep);
	dynstring_chcat(&sourcestr,'\0');
	getch();
}

/***********************************************************
 * 功能:	词法着色
 **********************************************************/
void color_token(int lex_state)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	char *p;
	switch(lex_state)
	{
	case LEX_NORMAL:
		{
			if(token >=  TK_IDENT)
				SetConsoleTextAttribute(h,FOREGROUND_INTENSITY);
			else if(token >= KW_CHAR)
				SetConsoleTextAttribute(h,FOREGROUND_GREEN|FOREGROUND_INTENSITY);
			else if(token >= TK_CINT)
				SetConsoleTextAttribute(h,FOREGROUND_RED|FOREGROUND_GREEN);
			else 
				SetConsoleTextAttribute(h,FOREGROUND_RED|FOREGROUND_INTENSITY);		
			p = get_tkstr(token);
			printf("%s",p);
			break;
		}
	case LEX_SEP:
		printf("%c",ch);
		break;
	}
}


/***********************************************************
 * 功能:	词法分析初始化
 **********************************************************/
void init_lex()
{
	TkWord* tp;
	static TkWord keywords[] = {
	{TK_PLUS,		NULL,	  "+",	NULL,	NULL},
	{TK_MINUS,		NULL,	  "-",	NULL,	NULL},
	{TK_STAR,		NULL,	  "*",	NULL,	NULL},
	{TK_DIVIDE,		NULL,	  "/",	NULL,	NULL},	
	{TK_MOD,		NULL,	  "%",	NULL,	NULL},
	{TK_EQ,			NULL,	  "==",	NULL,	NULL},
	{TK_NEQ,		NULL,	  "!=",	NULL,	NULL},
	{TK_LT,			NULL,	  "<",	NULL,	NULL},
	{TK_LEQ,		NULL,	  "<=",	NULL,	NULL},
	{TK_GT,			NULL,	  ">",	NULL,	NULL},
	{TK_GEQ,		NULL,	  ">=",	NULL,	NULL},
	{TK_ASSIGN,		NULL,	  "=",	NULL,	NULL},
	{TK_POINTSTO,	NULL,	  "->",	NULL,	NULL},
	{TK_DOT,		NULL,	  ".",	NULL,	NULL},
	{TK_AND,		NULL,	  "&",	NULL,	NULL},
	{TK_OPENPA,		NULL,	  "(",	NULL,	NULL},
	{TK_CLOSEPA,	NULL,	  ")",	NULL,	NULL},
	{TK_OPENBR,		NULL,	  "[",	NULL,	NULL},
	{TK_CLOSEBR,	NULL,	  "]",	NULL,	NULL},	
	{TK_BEGIN,		NULL,	  "{",	NULL,	NULL},
	{TK_END,		NULL,	  "}",	NULL,	NULL},
	{TK_SEMICOLON,	NULL,	  ";",	NULL,	NULL},
	{TK_COMMA,		NULL,	  ",",	NULL,	NULL},
	{TK_ELLIPSIS,	NULL,	"...",	NULL,	NULL},
	{TK_EOF,		NULL,	 "End_Of_File",	NULL,	NULL},
                     	     	     
	{TK_CINT,		NULL,	 	"整型常量",	NULL,	NULL},        
	{TK_CCHAR,		NULL,		"字符常量",	NULL,	NULL},        
	{TK_CSTR,		NULL,		"字符串常量",	NULL,	NULL},	      
                     	     	     	  	
	{KW_CHAR,		NULL,		"char",	NULL,	NULL},
	{KW_SHORT,		NULL,		"short",	NULL,	NULL},
	{KW_INT,		NULL,		"int",	NULL,	NULL},
	{KW_VOID,		NULL,		"void",	NULL,	NULL},	
	{KW_STRUCT,		NULL,		"struct",	NULL,	NULL},
                     	     	     	  	
	{KW_IF,			NULL,		"if"	,	NULL,	NULL},  
	{KW_ELSE,		NULL,		"else",	NULL,	NULL},  
	{KW_FOR,		NULL,		"for",	NULL,	NULL},  
	{KW_CONTINUE,	NULL,		"continue",	NULL,	NULL},
	{KW_BREAK,		NULL,		"break",	NULL,	NULL},    
	{KW_RETURN,		NULL,		"return",	NULL,	NULL},
	{KW_SIZEOF,		NULL,		"sizeof",	NULL,	NULL},
	{KW_ALIGN,		NULL,		"__align",	NULL,	NULL},
	{KW_CDECL,		NULL,		"__cdecl",	NULL,	NULL},
	{KW_STDCALL,	NULL,		"__stdcall",	NULL,	NULL},
	{0,				NULL,	NULL,	NULL,		NULL}
	};

	dynarray_init(&tktable,8);
    for (tp = &keywords[0]; tp->spelling != NULL; tp++)
			tkword_direct_insert(tp);	
		
}

/***********************************************************
 *  功能:	忽略空格,TAB及回车 
 **********************************************************/
void skip_white_space()
{
	while(ch==' ' || ch=='\t' || ch=='\r')      // 忽略空格,和TAB ch =='\n' ||
	{			
		if (ch == '\r') 
		{
            getch();
            if (ch != '\n')
                return;
            line_num++;	
        } 
		else
		printf("%c",ch); //这句话，决定是否打印空格，如果不输出空格，源码中空格将被去掉，所有源码挤在一起
		getch();
	}

}

/***********************************************************
 *  功能:	预处理，忽略分隔符及注释
 **********************************************************/
void preprocess()
{
	while(1)
	{
		if(ch==' ' || ch=='\t' || ch=='\r')
		  skip_white_space();
		else if(ch == '/')
		{
			// 向前多读一个字节看是否是注释开始符，猜错了把多读的字符再放回去
			getch();
			if(ch == '*')
			{
				parse_comment();
			}
			else
			{
				ungetc(ch,fin); //把一个字符退回到输入流中
				ch = '/';
				break;
			}
		}
		else
			break;
	}	
}

/***********************************************************
 *  功能:	解析注释
 **********************************************************/
void parse_comment()
{	
	getch();
	do
	{
		do
		{
			if(ch == '\n' || ch == '*' || ch == CH_EOF)
				break;
			else
				getch();
		}while(1);
		if(ch == '\n')
		{
			line_num++;
			getch();
		}
		else if(ch == '*')
		{
			getch();
			if(ch == '/')
			{
				getch();
				return;
			}
		}
		else
		{
			error("一直到文件尾未看到配对的注释结束符");
			return;
		}
	}while(1);
}

/***********************************************************
 *  功能:	取单词
 **********************************************************/
void get_token()
{
	preprocess();
	switch(ch) 
	{
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': 
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': 
    case 'o': case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': 
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': 
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': 
    case '_':
		{
			TkWord* tp;
			parse_identifier();
			tp = tkword_insert(tkstr.data);
			token = tp->tkcode;
			break;
		}
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9':
		parse_num();
		token = TK_CINT;
		break;
    case '+':
		getch();
		token = TK_PLUS;
		break;
	case '-':
		getch();
		if(ch == '>')
		{			
		    token = TK_POINTSTO;
			getch();
		}
		else
			token = TK_MINUS;
		break;
	case '/':
		token = TK_DIVIDE;
		getch();
		break;
	case '%':
		token = TK_MOD;
		getch();
		break;
	case '=':
		getch();
		if(ch == '=')
		{
			token = TK_EQ;
			getch();
		}
		else
			token = TK_ASSIGN;
		break;
	case '!':
		getch();
		if(ch == '=')
		{
			token = TK_NEQ;
			getch();
		}
		else
			error("暂不支持'!'(非操作符)");
		break;
	case '<':
		getch();
		if(ch == '=')
		{
			token = TK_LEQ;
			getch();
		}
		else
			token = TK_LT;
		break;
	case '>':
		getch();
		if(ch == '=')
		{
			token = TK_GEQ;
			getch();
		}
		else
			token = TK_GT;
		break;
	case '.':
		getch();
		if(ch == '.')
		{
			getch();
			if(ch != '.')
			  error("省略号拼写错误");	
			else
			  token = TK_ELLIPSIS;
			getch();
		}
		else
		{
			token = TK_DOT;
		}		
		break;
	case '&':
		token = TK_AND;
		getch();
		break;
	case ';':
		token = TK_SEMICOLON;
		getch();
		break;
	case ']':
		token = TK_CLOSEBR;
		getch();
		break;
	case '}':
		token = TK_END;
		getch();
		break;
	case ')':
		token = TK_CLOSEPA;
		getch();
		break;
	case '[':
		token = TK_OPENBR;
		getch();
		break;
	case '{':
		token = TK_BEGIN;
		getch();
		break;
	case ',':
		token = TK_COMMA;
		getch();
		break;
	case '(':
		token = TK_OPENPA;
		getch();
		break;
	case '*':
		token = TK_STAR;
		getch();
		break;
    case '\'':
		parse_string(ch);
		token = TK_CCHAR;
		tkvalue = *(char *)tkstr.data;
		break;
    case '\"': 
		{   
			parse_string(ch);
			token = TK_CSTR;
            break;
		}
	case EOF:
		token = TK_EOF;
		break;
    default:
        error("不认识的字符:\\x%02x", ch); //上面字符以外的字符，只允许出现在源码字符串，不允许出现的源码的其它位置
		getch();
        break;
    }
	syntax_indent();
}


/***********************************************************
 * 功能:	取得单词v所代表的源码字符串，错误提示用
 * v:		单词编号
 **********************************************************/
char *get_tkstr(int v)
{
	if(v > tktable.count)
		return NULL;
	else if(v >= TK_CINT && v <= TK_CSTR)
		return sourcestr.data;
	else
		return ((TkWord*)tktable.data[v])->spelling;
}

/***********************************************************
 *  功能:	词法分析测试
 **********************************************************/
void test_lex()
{
	do
	{
		get_token();
		color_token(LEX_NORMAL);
	}while(token != TK_EOF);
	printf("\n\n代码行数: %d行\n\n",line_num);
}