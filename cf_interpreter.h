/**
 * @FileName: cf_interpreter.h
 * @Author: chen jixuan
 * @Date: 2022/7/1 10:06:11
 * @Version: v1.0
 * @Description: 配置文件解释器
 */
#ifndef _CF_INTERPRETER_H_
#define _CF_INTERPRETER_H_

#include "cf_interface.h"

// 输入文件结构体
struct input
{
	char *filename;
	unsigned char *base;
	unsigned char *cursor;
	unsigned char *lineHead;
	int line;
	void* file;
	unsigned long size;
};

// 读取的变量结构体
struct _value
{
	double d;
	unsigned char* begin;
	unsigned char* end;
};

// CF错误信息和警告信息的输出
#ifndef CF_MESSAGE
#define CF_PRINT(...)
#else
#define CF_PRINT(...) printf(__VA_ARGS__)
#endif

#define IsDigit(c) (c>='0' && c<='9')
#define IsNonDigit(c) ( (c >= 'a' && c <= 'z') || (c == '_') || (c >= 'A' && c <= 'Z') )
#define IsLetterOrDigit(c) ( IsDigit(c) || IsNonDigit(c) )

#define CREATE_STR(var) \
		do \
		{ \
			struct _value *temp = (struct _value*)dat;\
			if((temp->begin)+1==(temp->end)) \
			{\
				return;\
			}\
			if(var!=NULL)\
			{ \
				StrFree(var);\
			} \
			++(temp->begin);\
			var=StrMalloc(temp->begin,temp->end-temp->begin); \
		}while(0);
	
#define CREATE_DOUBLE(var) \
		do \
		{ \
 			struct _value *temp= (struct _value*)dat; \
			var = temp->d; \
		}while(0);

#define CREATE_INT(var) \
		do \
		{ \
 			struct _value *temp= (struct _value*)dat; \
			var = (int)(temp->d); \
		}while(0); \


#endif
