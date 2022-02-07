# new 与 delete

operator new 和 operator delete 函数有两个重载版本：

```c++
void* operator new(size_t);         // allocate an object
void* operator new[](size_t);       // allocate an array
void* operator delete(void*);       // free an object
void* operator delete[](void*);     // free an array
```

## 一、new

new 操作针对数据类型的处理，分为两种情况：

### 1. 简单数据类型

包括基本数据类型和不需要构造函数的类型

```c++
int *p = new int;
```

汇编代码如下：

![img](.\Photo\new_01.png)

operator new 的源码如下：

```c++
void* __CRTDECL operator new(size_t size) _THROW1(_STD bad_alloc) {
    void *p;
    while ((p = malloc(size)) == 0) {
        if (_callnewh(size) == 0) {
            _THROW_NCEE(_XSTD bad_alloc);
        }
    }
	return (p);
}
```

分析：调用 malloc 失败后会调用 _callnewh。如果 _callnewh 返回 0 则抛出 bac_alloc 异常，返回非零则继续分配内存。

这个 _callnewh 是一个 new handler，通俗来讲就是 new 失败的时候调用的回调函数。可以通过 _set_new_handler 来设置。例如：

```c++
#include <stdio.h>
#include <new.h>
int MyNewHandler(size_t size) {
    printf("Allocation failed. Try again");
    return 1;       // continue to allocate
    // return 0;    // stop allocating, throw bad_alloc
}       
int main() {
    // Set the failure handler for new to be MyNewHandler.
    _set_new_handler(MyNewHandler);
    while(1) {
        int *p = new int[10000000];
    }
    return 0;
}                    
```

在 new 基本数据类型的时候还可以指定初始化值，比如：

```c++
int *p = new int(4);
```

#### 总结

- 简单类型直接调用 operator new 分配内存；
- 可以通过 new_handler 来处理 new 失败的情况；
- new 分配失败的时候不像 malloc 那样返回 NULL，它直接抛出异常。要判断是否分配成功应该用异常捕获的机制。

### 2. 复杂数据类型

需要由构造函数初始化对象

```c++
class Object {
public:
    Object() : _val(1) {}
    ~Object() {}
private:
    int _val;
};

void main() {
    Object* p = new Object();
}    
```

汇编码如下：

![img](.\Photo\new_02.png)

#### 总结

- new 复杂数据类型的时候先调用 operator new，然后在分配的内存上调用构造函数。

## 二、delete

### 1. 简单数据类型

包括基本数据类型和不需要析构函数的类型

```c++
int *p = new int(1);
delete p;
```

汇编码如下：

![img](.\Photo\delete.png)

分析：传入参数 p 之后调用 operator delete，其源码如下：

```c++
void operator delete(void *p) {
    RTCCALLBACK(_RTC_Free_hook, (p, 0));
	free(p);
}
```

RTCCALLBACK 默认是空的宏定义，所以这个函数默认情况下就是简单的调用 free 函数。

#### 总结

- delete 简单数据类型默认只是调用 free 函数。

### 2. 复杂数据类型

需要由析构函数销毁对象

```c++
class Object {
public:
    Object() : _val(1) {}
    ~Object() { cout << "destroy object" << endl; }
private:
    int _val;
};

void main() {
    Object *p = new Object;
    delete p;
}        
```

部分汇编码如下：

```assembly
012241F0  mov         dword ptr [this],ecx
012241F3  mov         ecx,dword ptr [this]
012241F6  call        Object::~Object (0122111Dh)         //先调用析构函数
012241FB  mov         eax,dword ptr [ebp+8]  
012241FE  and         eax,1
01224201  je          Object::`scalar deleting destructor'+3Fh (0122420Fh)
01224203  mov         eax,dword ptr [this]
01224206  push        eax
01224207  call        operator delete (01221145h)
0122420C  add         esp,4          
```

#### 总结

- delete 复杂数据类型先调用析构函数再调用 operator delete。

## 三、new 数组

### 1. 简单数据类型

包括基本数据类型和不需要构造函数的类型

new[] 调用的是 operator new[]，计算出数组总大小之后调用 operator new。可以通过()初始化数组为零值，如：

```c++
char *p = new char[32]();
// 等价于
char *p = new char[32];
memset(p, 32, 0);
```

#### 总结

- 针对简单类型，new[] 计算好大小后调用 operator new。

### 2. 复杂数据类型

需要由构造函数初始化对象

```c++
class Object {
public:
    Object() : _val(1) {}
    ~Object() { cout << "destroy object" << endl; }
private:
    int _val;
};

void main() {
    Object *p = new Object[3];
} 
```

new[] 先调用 operator new[] 分配内存，然后在 p 的前四个字节写入数组大小，最后调用三次构造函数。

实际分配的内存块如下：

![img](.\Photo\new_03.png)

这里为什么要写入数组大小呢？因为对象析构时不得不用这个值，举个例子：

```c++
class Object {
public:
    Object() : _val(1) {}
    virtual ~Object() { cout << "destroy object" << endl; }
private:
    int _val;
};

class MyObject : public Object {
public:    
    ~MyObject() { cout << "destroy MyObject" << endl; }
private:
    int _foo;
};

void main() {
    Object *p = new MyObject[3];
    delete[] p;
}        
```

释放内存之前会调用每个对象的析构函数。但是编译器并不知道 p 实际所指对象的大小。如果没有储存数组大小，编译器如何知道该把 p 所指的内存分为几次来调用析构函数呢？

#### 总结

- 针对复杂类型，new[] 会额外存储数组大小。

## 四、delete 数组

### 1. 简单数据类型

包括基本数据类型和不需要析构函数的类型

delete 和 delete[] 效果一样，比如下面的代码：

```c++
int *pint = new int[32];
delete pint;

char *pch = new char[32];
delete pch;
```

运行后不会有什么问题，内存也能完成的被释放。看下汇编码就知道 operator delete[] 就是简单的调用 operator delete。

#### 总结

- 针对简单类型，delete 和 delete[] 等同。

### 2. 复杂数据类型

需要由析构函数销毁对象

释放内存之前会先调用每个对象的析构函数。

new[] 分配的内存只能由 delete[] 释放。如果由 delete 释放会崩溃。

**假设指针 p 指向 new[] 分配的内存。因为要4字节存储数组大小，实际分配的内存地址为 [p-4]，系统记录的也是这个地址。delete[] 实际释放的就是 p-4 指向的内存。而 delete 会直接释放 p 指向的内存，这个内存根本没有被系统记录，所以会崩溃。**

#### 总结

- 针对复杂类型，new[] 出来的内存只能由 delete[] 释放。

## 五、内存泄漏检查器

详见 LeakDetector(h, cpp, test) 文件

