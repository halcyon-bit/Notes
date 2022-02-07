# std::vector

## 一、头文件

`std::vector` 是标准库中的组件，使用时需要包含相关头文件。vector 是一个类模板，在实例化的时候需要指定模板参数。

```c++
#include <vector>
```

## 二、结构

vector 模塑出一个动态数组，因此，它本身是"将元素置于动态数组中加以管理"的一个抽象概念。vector 将其元素复制到内部的 dynamic array 中。元素之间总存在某种顺序，所以 vector 是一种有序群集。

vector 支持随机存取，因此只要知道位置，可以在常数时间内存取任何一个元素。根据 vector 的结构可知，在末端添加或者删除元素，性能相当好，如果在前端或者中部安插或删除元素，性能就不怎么样了，因为操作点之后的元素都需要移到另一位置，而每一次移动都需要调用 assignement（赋值）操作符。

![img](./photo/vector_01.jpg)

## 三、大小和容量

### 1. 性能

vector 的优异性能的秘诀之一就是配置比其容纳元素所需更多的内存。vector 用于操作大小的函数有 size()、empty()、max_size()，另一个有关大小的函数是 capacity()，它返回的是 vector 实际能够容纳的元素数量，如果超越这个数量，vector 就有必要重新配置内部存储器。

vector 的容量之所以很重要，有以下两个原因：

- 一旦内存重新分配，和 vector 相关的所有元素都会失效，如：references，pointers，iterators
- 内存分配很耗时间

### 2. 内存分配

#### 2.1 reserve

如果程序管理了和 vector 元素相关的 reference、pointers、iterator，或者执行效率至关重要，那么就必须考虑容器的容量问题。可以使用 reserve() 来保留适当容量，避免一再的分配内存，只要保留的容量有剩余，就不用担心 reference 失效。

```c++
std::vector<T> v;
v.reserve(n);  // 为 vector 分配容量为 n 的内存
```

如果调用 reserve() 所给的参数比当前的 vector 容量还小，不会发生任何反应，vector 不能通过 reverse 来缩减容量。

#### 2.2 构造分配

另外一种避免重新分配内存的方法是，初始化期间就向构造函数传递附加参数，构造出足够的空间。如果参数是个数值，它将成为 vector 的起始大小。

```c++
std::vector<T> v(500);  // 分配能容纳 500 个 T 元素的内存大小
```

#### 2.3 内存缩减

缩减 vector 内存的方法：两个 vector 交换了内容后，两者的容量也会交换，即保留元素，又缩减了内存。

```c++
std::vector<T>().swap(v);
```

## 四、底层实现

### 1. 基于 SGI-STL 的 vector 底层探索

**vector 底层实际是泛型的动态类型顺序表，因此其底层实际是一段连续的空间**。在 SGI-STL 的 vector 中，实际在底层使用三个指针指向该段连续空间的，如下：

![img](.\Photo\vector_03.jpg)

**start 指向空间的起始位置，finish 指向最后一个元素的下一个位置，end_of_storage 指向空间的末尾**。当向 vector 中 push_back 时，元素实际插入到 finish 位置，然后 finish 向后移动一个位置，当 finish 移动到与 end_of_storage 在同一个位置时，则需要扩容。

```c++
// vector 部分源码
template <class T, class Alloc = alloc>
class vector
{
public:
	typedef T value_type;  // vector 中元素的类型
	typedef value_type* iterator;  // 迭代器，实际就是指针的别名
	// ...

protected:
	iterator start;  // 指向底层空间的起始位置
	iterator finish;  // 指向最后一个有效元素的下一个位置，没有元素时与 start 在同一位置
	iterator end_of_storage;  // 指向空间的末尾
	// ...
	// 开辟 n 个元素的一段连续空间，并使用 value 来进行填充
	void fill_initialize(size_type n, const T& value)
    {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

	// ...
public:
	// 向 vector 中填充 n 个 value 的元素
	vector(int n, const T& value)
    {
		fill_initialize(n, value);
	}
	// ...
public:
	// vector 的迭代器
	iterator begin()
    {
		return start;
	}
	iterator end()
    {
		return finish;
	}
	// ...
};
```

因为 vector 底层是连续空间，并且 vector 重载了 `[]` 下标运算符，所以可以像使用数组的方式访问 vector 中的每一个元素，即支持随机访问，但 vector 不适宜在中间和开头进行插入和删除操作，因为要进行大量元素的搬移，比如插入：

![img](.\Photo\vector_04.jpg)

在元素3的位置插入0时，需要将3，4，5整体向后搬移一个位置，才可以插入数据0，最差情况下时间复杂度为 O(N)。

### 2. 配合 vector 的接口与底层结构关系

为了能使算法操作容器中的元素，每个容器都提供了 begin() 和 end() 的迭代器，[begin, end) 区间中包含了容器中的所有元素，vector 也是如此。

![img](.\Photo\vector_05.jpg)

size() 和 capacity() 分别表示 vector 中的元素个数以及底层的空间大小。

![img](.\Photo\vector_06.jpg)

### 3. vector 的扩容

在向 vector 中插入元素时，如果 finish 移动到与 end_of_storage 同一个位置时，即 size 等于容量，空间不足，则需要扩容。

```c++
int main()
{
    std::vector<int> foo;
    size_t sz = foo.capacity();
    std::cout << "making foo grow\n";
    for (int i = 0; i < 100; ++i) {
        foo.push_back(i);
        if (sz != foo.capacity()) {
            sz = foo.capacity();
            std::cout << "capacity changed: " << sz << std::endl;
        }
    }
}
// vs 运行结果
// making foo grow
// capacity changed: 1
// capacity changed: 2
// capacity changed: 3
// capacity changed: 4
// capacity changed: 6
// capacity changed: 9
// capacity changed: 13
// capacity changed: 19
// capacity changed: 28
// capacity changed: 42
// capacity changed: 63
// capacity changed: 94
// capacity changed: 141
    
// g++ 运行结果
// making foo grow
// capacity changed: 1
// capacity changed: 2
// capacity changed: 4
// capacity changed: 8
// capacity changed: 16
// capacity changed: 32
// capacity changed: 64
// capacity changed: 128
```

从上述结果可以得出：

- **在向 vector 中插入元素时，vector 会自动进行扩容**
- **vs 下是几乎是按照 1.5 倍方式扩容，linux 下是按照 2 倍方式扩容**
- **如果确定 vector 中大概要存储多少个元素时，尽量提前把空间申请好，否则边插入边扩容，会影响程序的效率。此时再运行以上代码，几乎就看不到扩容的过程了。**

```c++
int main()
{    
    std::vector<int> foo;
    foo.reserve(100);  // 先将 vector 底层空间扩增到100个，然后插入
    size_t sz = foo.capacity();
    std::cout << "making foo grow\n";
    for (int i=0; i<100; ++i) {        
        foo.push_back(i);        
        if (sz!=foo.capacity()) {            
            sz = foo.capacity();
            std::cout << "capacity changed: " << sz << std::endl;
        }
    }
}
```

此时再运行以上代码，几乎就看不到扩容的过程了。

虽然不同系统下 vector 扩容的方式不同，但是原理都是一样的，都需要经过以下步骤：

1. 开辟新空间
2. 拷贝元素
3. 释放旧空间
4. 使用新空间

![img](.\Photo\vector_07.jpg)
