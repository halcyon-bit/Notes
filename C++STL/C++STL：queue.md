# std::queue

## 一、头文件

`std::queue` 是标准库中的组件，使用时需要包含相关头文件。queue 是一个类模板，在实例化的时候需要指定模板参数。

```c++
#include <queue>
```

## 二、定义

`class queue<>` 实作为一个 queue（FIFO，先进先出)，可以使用 `push()` 将任意数量的元素置入 queue 中，也可以使用 `pop()` 将元素以其插入序从容器中移除（即先进先出）。

![img](.\Photo\queue_01.png)

在 \<queue> 中，class queue 定义如下：

```c++
template <class T, class Container = deque<T>>
class queue;
```

第一个模板参数代表元素类型，带有默认值的第二个模板参数用来定义 queue 内部存放元素用的实际容器，缺省采用 deque。

```c++
std::queue<std::string> buffer;  // string queue
```

实际上，queue 只是将各项操作转化为内部容器的各项调用，可以使用任何形式的容器来支持 queue，只要它支持 front()、back()、push_back()、pop_front() 等操作。

例如使用 list 来容纳元素：

```c++
std::queue<std::string, std::list<std::string>> buffer;
```

![img](.\Photo\queue_02.png)

## 三、接口

- `push()`：将元素置入 queue 中
- `front()`：返回 queue 内的下一个元素（也就是第一个被置入的元素）
- `back()`：返回 queue 的最后一个元素（也就是最后一个被插入的元素）
- `pop()`：从 queue 中移除一个元素
- `size()`：返回 stack 长度
- `empty()`：返回 stack 是否为空