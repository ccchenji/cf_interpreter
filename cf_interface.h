/**
 * @FileName: cf_interface.h
 * @Author: chen jixuan
 * @Date: 2022/7/1 10:25:15
 * @Version: v1.0
 * @Description: 配置文件解释器接口
 */
#ifndef _CF_INTERFACE_H_
#define _CF_INTERFACE_H_
#include <string.h>

// 开启警告信息
#define CF_MESSAGE

// 可以配置变量的最大数量
#define CF_MAX 30

//变量类型
enum cf_type{VAR_DOUBLE=0,VAR_INT,VAR_STR};

// 运行解释器
void RunInterpreter(char *filename);
// 添加变量
char AddVar(const char*,void*,char);
// 删除关键字对应的变量
void DelVar(const char*key);
// 给字符串类型申请内存
char* StrMalloc(const char*str,unsigned int len);


#endif
