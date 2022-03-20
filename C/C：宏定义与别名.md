# 宏定义与别名

## 一、别名

1. 掩饰符合类型，如指针和数组

```c
// 数组
char text1[81];
char text1[81];

typedef char Line[81];

Line text1, text2;  // 等同于上面的两个数组

// 指针
typedef char *pStr;  // typedef char* pStr;
pStr p1;
```

2. 与结构结合使用

```c
typedef struct tagMyStruct
{
    int nNum;
    long lLength;
} MyStruct;

// 等价于
struct tagMyStruct
{
    int nNum;
    long lLength;
};

typedef struct tagMyStruct MyStruct; 
```

分析：tagMyStruct 称为“tag”，即“标签”，实际上是一个临时名字，struct 关键字和 tagMyStruct 一起，构成了这个结构类型，不论是否有 typedef，这个结构都存在。

所以，若没有 typedef，则使用 `struct tagMyStruct varName` 来定义变量，但要注意，使用 `tagMyStruct varName` 来定义变量是不对的，因为 struct 和 tagMyStruct 合在一起才能表示一个结构类型。

## 二、宏定义

1. 宏的单行定义

```c
#define A(x)  T_##x
#define B(x)  #@x
#define C(x)  #x

A(1);   // 结果: T_1 直接把 1 黏贴上去
B(1);   // 结果: '1' 把 1 带上单引号后黏贴上去
C(1);   // 结果: "1" 把 1 带上双引号后黏贴上去
```

2. 条件编译

在跨平台和系统的软件里，条件编译就很重要了。

```c
#ifdef WINDOWS
    // 如果 WINDOWS 宏被定义，就执行这里的内容
#endif

#ifdef LINUX
    // 如果 LINUX 宏被定义，执行这里的内容
#else
    // 如果 LINUX 宏没有被定义，执行这里的内容
#endif
```

3. 防止头文件被重复编译

```c
#ifndef _SOMEFILE_H_
#define _SOMEFILE_H_

#endif
// 或
#pragma once
```

4. 得到指定地址上的一个字节或字

```c
#define MEM_B(x)    (*((byte*)(x)))
#define MEM_W(x)    (*((word*)(x)))
```

