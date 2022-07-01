# cf_interpreter c语言写的配置文件解释器

## 文件说明

cf_interpreter.c : 解释器配置源文件

cf_interpreter.h : 解释器.h文件

cf_interface.h : 解释器api接口文件

cf.txt: 示例工程对应的配置文件

main.c: 示例工程对应的main文件

Makefile: 示例工程对应的编译文件

## 配置文件语法说明

### 配置文件例程

参考cf.txt
```

TEST_DOUBLE = 4.3;
TEST_INT= 4;
TEST_STR = "world";

```

### 语法

关键字 = 值 ;

## 解释器api说明

```c

// 是否开启解释器的警告和错误信息,如果不需要开启直接注释掉
#define CF_MESSAGE

// 解释器支持的变量类型
// 当前只支持三种类型 int ,double,字符串
enum cf_type{VAR_DOUBLE=0,VAR_INT,VAR_STR};

// 运行解释器
// 入口参数为配置文件的路径
void RunInterpreter(char *filename);

// 添加要配置的变量到解释器
// 第一个参数：变量对应的关键字
// 第二个参数：要配置的变量,需要变量的地址
// 第三个参数：变量的类型->cf_type里的变量类型
char AddVar(const char*,void*,char);

// 从解释器中删除变量
// 参数变量对应的关键字
void DelVar(const char*key);

// 给字符串类型申请内存
// 字符串类型的变量需要用该函数先初始化
// 第一个参数：字符串内容
// 第二个参数：字符串长度
char* StrMalloc(const char*str,unsigned int len);


```



## 快速开始

1. 将 cf_interpreter.c cf_interpreter.h cf_interface.h 这三个文件包含在你的工程里
2. 调用api开始使用解释器

### 添加double类型的变量到解释器

```c

#include <stdio.h>
#include "cf_interface.h"
int main()
{
	double testd = 1.1;

	// 调用AddVar添加变量
	// 变量testd对应的关键字设置为 TEST_DOUBLE
	AddVar("TEST_DOUBLE",&testd,VAR_DOUBLE);
	printf("%f\r\n",testd);

	// 运行解释器,解释器输入为配置文件路径
	RunInterpreter("./cf.txt");
	printf("%f\r\n",testd);
}

```

### 添加int类型的变量到解释器

```c

#include <stdio.h>
#include "cf_interface.h"
int main()
{
	int testi = 1;

	// 调用AddVar添加变量
	// 变量testd对应的关键字设置为 TEST_INT
	AddVar("TEST_INT",&testi,VAR_INT);
	printf("%d\r\n",testi);

	// 运行解释器,解释器输入为配置文件路径
	RunInterpreter("./cf.txt");
	printf("%d\r\n",testi);
}

```
### 添加str类型的变量到解释器

```c
#include <stdio.h>
#include <string.h>
#include "cf_interface.h"
int main()
{
	// 使用StrMalloc为字符串添申请内存
	char *tests = StrMalloc("hello",strlen("hello"));

	// 变量tests对应的关键字设置为 TEST_STR
	AddVar("TEST_STR",&tests,VAR_STR);
	printf("%s\r\n",tests);
	
	// 运行解释器，解释器输入为配置文件路径
	RunInterpreter("./cf.txt");
	printf("%s\r\n",tests);
}

```























