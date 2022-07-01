/**
 * @FileName: cf_interpreter.c
 * @Author: chen jixuan
 * @Date: 2022/7/1 9:55:47
 * @Version: v1.0
 * @Description: 配置文件解释器
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "cf_interpreter.h"


// 关键字
const char* cf_keyword[CF_MAX]={NULL};
// 变量
void *key_var[CF_MAX]={NULL};
// 变量的类型
char var_type[CF_MAX];

// 已经添加的变量的数量
int all_var=0;


// 当前指向的变量
void *now_var = NULL ;
// 当前指向变量的类型
char now_var_type = 0;

// 全局数据
struct _value value;

// 文件结束符
unsigned char END_OF_FILE = 255;
// 输入文件
struct input Input;
#define CURSOR 		(Input.cursor)
#define LINE		(Input.line)
#define LINEHEAD	(Input.lineHead)


// token
enum cf_token {TK_ID=0,TK_ASSIGNMENT,TK_SEMICOLON,TK_STRING,TK_NUM,TK_FILE_EOF,TK_ERROR,TK_BEGIN};

// 函数定义
void ReadSourceFile(char *filename);
void SkipWhiteSpace(void);
int ScanIdentifier(void);
void *FindKeyWord(unsigned char*start,int len);
int NextToken(void);
int DealAssignment(void);
int DealString(void);
int DealNum(void);
int DealSemicolon(void);
int DealBadChar(void);
void CloseFile(void);
void SetVar(void *var,char type,void *dat);
void StrFree(void*pt);
/**
 * @Name: ReadSourceFile
 * @Description: 读取配置文件
 * @Date: 2022/7/1 10:32:55
 * @Author: chen jixuan
 * @Param: {char*} filename
 * @Return: none
 */
void ReadSourceFile(char *filename)
{
	int len;
	Input.file = fopen(filename, "r");
	if (Input.file == NULL)
	{
		CF_PRINT("Can't open file:%s\r\n",filename);
	}

	fseek(Input.file, 0, SEEK_END);
	Input.size = ftell(Input.file);
	Input.base = malloc(Input.size + 1);
	if (Input.base == NULL)
	{
		CF_PRINT("The file %s is too big\r\n",filename);
		fclose(Input.file);
	}
	fseek(Input.file, 0, SEEK_SET);
	Input.size = fread(Input.base, 1, Input.size, Input.file);
	fclose(Input.file);

	Input.filename = filename;
	Input.base[Input.size] = END_OF_FILE;
	Input.cursor = Input.lineHead = Input.base;
	Input.line = 1;
	return;
}

/**
 * @Name: CloseFile
 * @Description: 释放文件占用内存
 * @Date: 2022/7/1 10:38:12
 * @Author: chen jixuan
 * @Param: none
 * @Return: none
 */
void CloseFile(void)
{
	free(Input.base);
}

/**
 * @Name: RunInterpreter
 * @Description: 运行配置文件解释器
 * @Date: 2022/7/1 10:38:57
 * @Author: chen jixuan
 * @Param: {char*} filename
 * @Return: none
 */
void RunInterpreter(char *filename)
{
	// 读取源文件
	ReadSourceFile(filename);
	int tk = TK_BEGIN;
	while(tk!=TK_FILE_EOF)
	{
		tk = NextToken();
		if(tk==TK_ID)
		{
			tk = NextToken();
			if(tk==TK_ASSIGNMENT)
			{
				tk = NextToken();
				if(tk==TK_NUM||tk==TK_STRING)
				{
					tk = NextToken();
					if(tk!=TK_SEMICOLON)
					{
						CF_PRINT("%s:%d:%s",Input.filename,LINE,"expect ; \n");
					}
					else 
					{
						// 运行函数
						SetVar(now_var,now_var_type,&value);
					}
				}
				else
				{
					CF_PRINT("%s:%d:%s",Input.filename,LINE,"expect num or string \n");
				}
			}
			else
			{
				CF_PRINT("%s:%d:%s",Input.filename,LINE,"expect = \n");
			}
		}
	}
	// 释放源文件内存
	CloseFile();
}

/**
 * @Name: NextToken
 * @Description: 得到token的值
 * @Date: 2022/7/1 10:39:46
 * @Author: chen jixuan
 * @Param: none
 * @Return: none
 */
int NextToken(void)
{
	int tk;
	//先跳过空白字符
	SkipWhiteSpace();
	if(IsNonDigit(*CURSOR))
	{
		tk = ScanIdentifier();
	}
	else if(*CURSOR=='=')
	{
		tk = DealAssignment();
	}
	else if(*CURSOR=='"')
	{
		tk = DealString();
	}
	else if(IsDigit(*CURSOR)||*CURSOR=='.'||*CURSOR=='-')
	{
		tk = DealNum();
	}
	else if(*CURSOR==END_OF_FILE)
	{
		tk = TK_FILE_EOF;
	}
	else if(*CURSOR==';')
	{
		tk = DealSemicolon();
	}
	else
	{
		tk = DealBadChar();
	}
	return tk;
}

/**
 * @Name: DealBadChar
 * @Description: 处理错误字符
 * @Date: 2022/7/1 10:40:26
 * @Author: chen jixuan
 * @Param: none
 * @Return: none
 */
int DealBadChar(void)
{
	CF_PRINT("%s:%d:%c:%s",Input.filename,LINE,*CURSOR,"Unrecognized character\n");
	++CURSOR;
	return NextToken();
}

/**
 * @Name: DealSemicolon
 * @Description: 处理分号
 * @Date: 2022/7/1 10:40:53
 * @Author: chen jixuan
 * @Param: none
 * @Return: int
 */
int DealSemicolon(void)
{
	++CURSOR;
	return TK_SEMICOLON;
}


/**
 * @Name: DealNum
 * @Description: 处理常量
 * @Date: 2022/7/1 10:41:17
 * @Author: chen jixuan
 * @Param: none
 * @Return: int
 */
int DealNum(void)
{
	unsigned char *start = CURSOR;
	if(*CURSOR=='-')
	{
		++CURSOR;
	}
	while(IsDigit(*CURSOR))
	{
		++CURSOR;
	}
	if(*CURSOR=='.')
	{
		++CURSOR;
		while(IsDigit(*CURSOR))
		{
			++CURSOR;
		}
	}
	value.d = strtod((char*)start,NULL);
	return TK_NUM;
}


/**
 * @Name: DealString
 * @Description: 处理字符串
 * @Date: 2022/7/1 10:41:47
 * @Author: chen jixuan
 * @Param: none
 * @Return: int
 */
int DealString(void)
{
	value.begin = CURSOR;
	while(1)
	{
		++CURSOR;
		if(*CURSOR=='\n'||*CURSOR==END_OF_FILE)
		{
			value.end = value.begin; 
			CF_PRINT("%s:%d:%s",Input.filename,LINE,"expect \" \n");
			return TK_ERROR;
		}
		else if(*CURSOR=='"')
		{
			value.end = CURSOR;
			++CURSOR;
			return TK_STRING;
		}
	}
}

/**
 * @Name: DealAssignment
 * @Description: 处理赋值符号
 * @Date: 2022/7/1 10:42:16
 * @Author: chen jixuan
 * @Param: none
 * @Return: none
 */
int DealAssignment(void)
{
	++CURSOR;
	return TK_ASSIGNMENT;
}


/**
 * @Name: SkipWhiteSpace
 * @Description: 跳过空白字符
 * @Date: 2022/7/1 10:42:39
 * @Author: chen jixuan
 * @Param: none
 * @Return: none
 */
void SkipWhiteSpace(void)
{
	char ch = *CURSOR;
	while(ch == '\t' || ch == '\v' || ch == '\f' || ch == ' ' || 
		  ch == '\r' || ch == '\n' ||ch == '#')
	{
		switch(ch)
		{
			case '#':
				while(*CURSOR!='\n'&&*CURSOR!=END_OF_FILE)
				{
					++CURSOR;
				}
			break;
			case '\n':
				++CURSOR;
				++LINE;
				LINEHEAD = CURSOR;
			break;
			default:
				++CURSOR;
			break;
		}
		ch = *CURSOR;
	}
}

/**
 * @Name: ScanIdentifier
 * @Description: 扫描标识符
 * @Date: 2022/7/1 10:42:58
 * @Author: chen jixuan
 * @Param: none
 * @Return: none
 */
int ScanIdentifier(void)
{
	unsigned char * start = CURSOR;
	while(IsLetterOrDigit(*CURSOR))
	{
		++CURSOR;
	}
	now_var = FindKeyWord(start,CURSOR-start);
	if(now_var !=NULL)
	{
		return TK_ID;
	}
	return TK_ERROR;
}

/**
 * @Name: FindKeyWord
 * @Description: 找到关键字的位置，并返回对应的处理函数
 * @Date: 2022/7/1 10:43:21
 * @Author: chen jixuan
 * @Param: {unsigned char*} start,{int} len
 * @Return: 关键字对应的指针
 */
void *FindKeyWord(unsigned char*start,int len)
{
	char _str[20];
	int i=0;
	int list_size  = CF_MAX; 
	if(all_var>0)
	{
		for(i=0;i<list_size;++i)
		{
			if(cf_keyword[i]!=NULL)
			{
				int len_key = strlen(cf_keyword[i]);
				if(len==len_key)
				{
					if(strncmp(start,cf_keyword[i],len)==0)
					{
						now_var_type = var_type[i];
						return key_var[i];
					}
				}
			}
		}
	}
	memset(_str,0,sizeof(_str));
	int limit = len<sizeof(_str)-1?len:sizeof(_str)-1;
	for(i=0;i<limit;++i)
	{
		_str[i] = *(start++);
	}
	CF_PRINT("%s:%d:%s:%s",Input.filename,LINE,_str,"keyword does not exist\n");
	return NULL;
}

/**
 * @Name: SetVar
 * @Description: 设置变量 
 * @Date: 2022/7/1 10:44:55
 * @Author: chen jixuan
 * @Param: {void*} var,{char} type, {void*}dat
 * @Return: none
 */
void SetVar(void *var,char type,void *dat)
{
	if(type == VAR_STR)
	{
		CREATE_STR(*((char**)var));
	}
	else if(type == VAR_INT)
	{
		CREATE_INT(*((int*)var));
	}
	else if(type == VAR_DOUBLE)
	{
		CREATE_DOUBLE(*((double*)var));
	}
}

/**
 * @Name: StrMalloc
 * @Description: 申请内存,并将字符串st指向的字符数据复制到申请的内存
 * @Date: 2022/7/1 10:45:45
 * @Author: chen jixuan
 * @Param: {unsigned char*}str,{int} len
 * @Return:  内存的头指针
 */
char* StrMalloc(const char*str,unsigned int len)
{
	char*pt = (char*)malloc(sizeof(char)*(len+1));
	int i=0;
	for(i=0;i<len;++i)
	{
		pt[i] = str[i];
	}
	pt[len] =0;
	return pt;
}

/**
 * @Name: StrFree
 * @Description: 释放内存
 * @Date: 2022/7/1 10:50:17
 * @Author: chen jixuan
 * @Param: {void*} pt
 * @Return: none
 */
void StrFree(void*pt)
{
	free(pt);
}

/**
 * @Name: AddVar
 * @Description: 添加变量 
 * @Date: 2022/7/1 11:06:09
 * @Author: chen jixuan
 * @Param: {const char*} key,{void*} var,{char} type
 * @Return: 成功返回0 ,失败返回1
 */
char AddVar(const char* key,void* var,char type)
{
	int empty_location=-1;
	int is_exist=0;
	if(all_var >= CF_MAX)
		return 1;
	else
	{
		int i=0;
		for(i=0;i<CF_MAX;++i)
		{
			if(cf_keyword[i] == NULL)
			{
				if(empty_location==-1)
					empty_location = i;
			}
			else
			{
				int len_key = strlen(cf_keyword[i]);
				int len = strlen(key);
				if(len==len_key)
				{
					if(strncmp(key,cf_keyword[i],len)==0)
					{
						CF_PRINT("keyword %s already exists!\n",key);
						return 1;
					}
				}
			}
		}
	}
	cf_keyword[empty_location] = key;
	key_var[empty_location] = var;
	var_type[empty_location] = type;
	all_var++;
	return 0;
}

/**
 * @Name: DelVar
 * @Description: 删除关键字对应的变量 
 * @Date: 2022/7/1 13:01:38
 * @Author: chen jixuan
 * @Param: {const char*}
 * @Return: none
 */
void DelVar(const char*key)
{
	if(all_var>0)
	{
		int i=0;
		for(i=0;i<CF_MAX;++i)
		{
			if(cf_keyword[i]!=NULL)
			{
				int len_key = strlen(cf_keyword[i]);
				int len = strlen(key);
				if(len==len_key)
				{
					if(strncmp(key,cf_keyword[i],len)==0)
					{
						cf_keyword[i] = NULL;
						break;
					}
				}
			}
		}
	}
}

























