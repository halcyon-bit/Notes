# std::stack

## 一、头文件

`std::stack` 是标准库中的组件，使用时需要包含相关头文件。stack 是一个类模板，在实例化的时候需要指定模板参数。

```c++
#include <stack>
```

## 二、定义

`class stack<>` 实作出一个 stack（LIFO，后进先出），可以使用 `push()` 将任意数量的元素置入 stack 中，也可以使用 `pop()` 将元素依次插入序的反序从容器中移除（即后进先出）。

![img](.\Photo\stack_01.png)

在 \<stack> 中，class stack 定义如下：

```c++
template <class T, class Container = deque<T>>
class stack;
```

第一个模板参数代表元素类别。带有默认值的第二个模板参数用来定义 stack 内部存放元素的实际容器，缺省采用 deque。之所以采用 deque 是因为 deque 移除元素会释放内存，并且不用重新分配内存时复制所有元素。

```c++
std::stack<int> s;  // integer stack
```

实际上，stack 只是将各项操作转化为内部容器的各项调用，可以使用任何序列式容器来支持 stack，只要它支持 back()、push_back()、pop_back() 等操作。

例如使用 vector 来容纳元素：

```c++
std::stack<int, std::vector<int>> s;  // integer stack that uses vector
```

![img](.\Photo\stack_02.png)

## 三、接口

- `push()`：将一个元素放入 stack 中
- `top()`：返回 stack 中的栈顶元素，返回的是 reference，可以就地修改值
- `pop()`：移除栈顶元素，无返回值
- `size()`：返回 stack 长度
- `empty()`：返回 stack 是否为空