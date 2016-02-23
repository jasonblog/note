// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"

/***********************************************************
 * 功能:	异常处理
 * stage:	编译阶段还是链接阶段
 * level:	错误级别
 * fmt:		参数输出的格式
 * ap:		可变参数列表
 **********************************************************/
void handle_exception(int stage,int level,char *fmt,va_list ap)
{
	char buf[1024];
	vsprintf(buf,fmt,ap);
	if(stage == STAGE_COMPILE)
	{
		if(level==LEVEL_WARNING)
			printf("%s(第%d行) : 编译警告: %s!\n",filename,line_num,buf);
		else
		{
			printf("%s(第%d行) : 编译错误: %s!\n",filename,line_num,buf);
			exit(-1);
		}
	}
	else
	{
		printf("链接错误:%s!\n",buf);
		exit(-1);
	}
}

/***********************************************************
 * 功能:	编译警告处理
 * fmt:		参数输出的格式
 **********************************************************/
void warning(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE,LEVEL_WARNING, fmt, ap);
    va_end(ap);
}

/***********************************************************
 * 功能:	编译致命错误处理
 * fmt:		参数输出的格式
 **********************************************************/
void error(char *fmt, ...)
{
	va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE,LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/***********************************************************
 * 功能:	提示错误，此处需要缺少某个语法成份
 * msg:		需要什么语法成份
 **********************************************************/
void expect(char *msg)
{
    error("缺少%s", msg);
}

/***********************************************************
 * 功能:	跳过单词v,取下一单词,如果当前单词不是v,提示错误
 * v:		单词编号
 **********************************************************/
void skip(int v)
{
    if (token != v)
       error("缺少'%s'", get_tkstr(v));
    get_token();
}

/***********************************************************
 * 功能:	链接错误处理
 * fmt:		参数输出的格式
 **********************************************************/
void link_error(char *fmt, ...)
{
	va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_LINK,LEVEL_ERROR, fmt, ap);
    va_end(ap);
}
