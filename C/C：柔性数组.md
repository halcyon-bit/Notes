# 柔性数组

先看下面的代码，对于包含动态字符串成员的两个结构体 Test1 和 Test2 占用空间分别是多少呢？

```c
#include <stdio.h>

typedef struct _TEST_1
{
	int a;
	int b;
	char *c;
}Test1;

typedef struct _TEST_2
{
	int a;
	int b;
	char c[];
}Test2;

int main()
{
	printf("sizeof(struct _TEST_1) = %d\n", sizeof(Test1));
	printf("sizeof(struct _TEST_2) = %d\n", sizeof(Test2));
	return 0;
}
```

在64位系统上，其输出结果为：

```c
sizeof(struct _TEST_1) = 16
sizeof(struct _TEST_2) = 8
```

对于 Test1 的结果是 16，通常没有什么疑问，因为 4(int)+4(int)+8(指针)=16，但是对于 Test2 结构体占用 8 字节，可能会有疑问。

## 一、柔性数组（flexible array）

实际上这是 C99 中引入的柔性数组的特性。即结构体的最后一个成员，可以是**不完整类型(一种缺乏足够的信息去描述一个完整对象的类型)**的数组，但它使得整个结构体的大小就像没有这个成员一样。但是，当通过这个名字访问这个成员时，就像访问一个普通数组成员一样。

如果数组内一个元素都没有的话，那么访问这个数组将会是未定义行为。

```c
typedef struct _TEST_2
{
	int a;
	int b;
	char c[];
}Test2;
```

成员 c 是一个数组，但是并没有指定大小，使用 sizeof 计算 Test2，其占用空间也仅仅是 8 字节。

### 优点

#### 内存申请和释放

假设分别使用两种类型的结构体，存储 16 字节的字符数据，需要申请内存。

```c
// struct _TEST_1:
Test1* t1 = (Test1*)malloc(sizeof(struct _TEST_1));  // 为结构体申请内存
t1->c = (char*)malloc(sizeof(char) * 16);  // 为成员指向的数据申请内存
// struct _TEST_2:
Test2* t2 = (Test2*)malloc(sizeof(struct _TEST_2) + sizeof(char) * 16);
```

前者需要两次内存申请，而后者只需要一次。前者地址不连续（两次 malloc），后者地址连续。而访问成员 c 的时候，只需要：t2->c，和普通成员无异。

对于**内存释放**，前面需要单独释放成员 c 申请的内存，而后者可以一起释放。

#### 数据拷贝

由于前面的差别，导致数据拷贝时，更有区别。

```c
// struct _TEST_1：
Test1* t11;
// memcpy(t11, t1, sizeof(struct _TEST_1));  // 不可，这样导致 t11 的 c 和 t1 的 c 指向同一片内存区域。（浅拷贝）

t11->a = t1->a;
t11->b = t1->b;
memcpy(t11->c, t1->c, sizeof(char) * 16);
```

这里无法一次拷贝，因为它的成员 c 是一个指针类型，需要深拷贝，因此必须拷贝它指向的内存。

```c
// struct _TEST_2:
Test2* t22;
memcpy(t22, t2, sizeof(struct _TEST_2) + sizeof(char) * 16);
```

在这里，由于柔性数组的内存，它的数据内容和结构体数据成员的地址是连续的，因此可以直接拷贝。

#### 减少内存碎片

由于结构体的柔性数组和结构体成员的地址是连续的，即可一同申请内存，因此更大程度地避免了内存碎片。另外由于该成员本身不占结构体空间，因此，整体而言，比普通的数组成员占用空间要会稍微小点。

## 二、零长数组

与柔性数组功能类似，还有一个 0 长数组，不过它并不是标准中的，但是它可以实现类似的功能，使用方式如下：

```c
typedef struct _TEST_3
{
	int a;
	int b;
	char c[0];
}Test3;
```

差别在于使得数组长度为 0。但是由于它并非 C 标准中的，因此从可移植性考虑，不建议使用这种方式，除非还无法使用 C99。

## 三、总结

柔性数组的使用：

- 位于结构体最后一个位置
- 不完整数组类型
- 不是唯一成员

普通和柔性数组内存情况：

![img](Photo\柔性数组.png)

