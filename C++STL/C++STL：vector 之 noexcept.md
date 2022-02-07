# vector 之 noexcept 应用场景

C++11 提供了关键字 `noexcept`，用来指明某个函数无法或不打算抛出异常。

```c++
void test1() noexcept;  // a function specified as will never throw
void test2() noexcept(true);  // same as test2
void test3();  // a function might throw exception
void test4() noexcept(false);  // same as test3
```

## 一、noexcept 优点

vector 有自己的 capacity，当调用 push_back 但是容量满时，vector 会申请一片更大的空间给新容器，将容器内原有的元素 copy 到新容器内。

但是如果在扩容元素时出现异常怎么办？

- 申请新空间时出现异常：旧 vector 还是保持原有状态，抛出的异常交由用户自己处理
- copy 元素时出现异常：所有已经被 copy 的元素利用元素的析构函数释放，已经分配的空间释放掉，抛出的异常交由用户自己处理

这种扩容方式比较完美，有异常时也会保持上游调用 push_back 时原有的状态。

但是扩容还是 copy 的，当 vector 内是一个类且持有资源较多时，这会很耗时。所以 C++11 推出了一个新特性：`move`，它会将资源从旧元素中“偷”给新元素。

应用到 vector 扩容的场景中：当 vector 中的元素的移动构造函数是 `noexcept` 时，vector 就不会使用 copy 方式，而是使用 move 方式将旧容器的元素放到新容器中。

利用 `move` 的交换类资源所有权的特性，使用 vector 扩容效率大大提高，但是当发生异常时怎么办：原有容器的状态已经被破坏，有部分元素的资源已经被偷走。若要恢复会极大增加代码的复杂性和不可预测性。所以只有当 vector 中元素的 move constructor 是 `noexcept` 时，vector 扩容才会采取 move 方式来提高性能。

实际上，`noexcept` 还大量应用在 `swap` 函数和 `move assignment` 中。

## 二、noexcept 使用场景

上面提到了 `noexcept` 可以使用的场景：

- move constructor
- move assignment
- swap

首先需知道以下几点：

- 函数自己不抛异常，但是不代表它们内部的调用不会抛出异常，并且编译器不会提供调用者与被调用者的 `noexcept` 一致性检查，下述代码是合法的：

```C++
void g()
{
    // some code
}

void f() noexcept
{
    // some code
    g();
}
```

- 当一个声明为 `noexcept` 的函数抛出异常时，程序会被终止并调用 std::terminate()

所以在代码内部调用复杂，链路较长，且随时有可能加入新 feature 时，过早给函数加上 `noexcept` 可能不是一个好的选择。目前主流的观点是：

加 noexcept 情况：

- 函数在 c++98 版本中已经被声明为 `throw()`
- move constructor、move assignmemt、swap。如果这些实现不抛出异常，一定要使用 `noexcept`
- leaf function。例如获取类成员变量，类成员变量的简单运算等。

```C++
# if __cplusplus >= 201103L
#  define _GLIBCXX_NOEXCEPT noexcept
# else
#  define _GLIBCXX_NOEXCEPT
reference operator*() const _GLIBCXX_NOEXCEPT
{ return *_M_current; }

pointer operator->() const _GLIBCXX_NOEXCEPT
{ return _M_current; }

__normal_iterator& operator++() _GLIBCXX_NOEXCEPT
{
    ++_M_current;
    return *this;
}

__normal_iterator operator++(int) _GLIBCXX_NOEXCEPT
{ return __normal_iterator(_M_current++); }
```

不加 noexcept 情况：

- 除了上面的要加的情况，其余的函数不要加 `noexcept` 就可以

### 如何实现利用 noexcept move constructor 扩容

由于每个平台的实现可能都不一样，就关注下 vector 是怎么判断调用 copy constructor 还是 move constructor 的。

其中利用到的核心技术有：

- type trait
- iterator trait
- move iterator
- std::forward

核心代码：

```C++
template <typename _Iterator, typename _ReturnType = typename conditional<
    __move_if_noexcept_cond<typename iterator_traits<_Iterator>::value_type>::value,
    _Iterator, move_iterator<_Iterator>>::type>
inline _GLIBCXX17_CONSTEXPR _ReturnType __make_move_if_noexcept_iterator(_Iterator __i)
{
    return _ReturnType(__i);
}

template <typename _Tp>
struct __move_if_noexcept_cond
    : public __and_<__not_<is_nothrow_move_constructible<_Tp>>, is_copy_constructible<_Tp>>::type
{};
```

这里用 type trait 和 iterator trait 联合判断：假如元素有 noexcept move constructor，那么 is_nothrow_move_constructible=1 => \_\_move_if_noexcept_cond=0 => \_\_make_move_if_noexcept_iterator 返回一个 move iterator。这里 move iterator 迭代器适配器也是一个 C++11 新特性，用来将任何对底层元素的处理转换为一个 move 操作。

```C++
std::list<std::string> s;
// make_move_iterator 返回一个 std::move_iterator
std::vector<std::string> v(make_move_iterator(s.begin()), make_move_iterator(s.end()));
```

然后上游利用生成的 move iterator 进行循环元素 move:

```C++
{
    for (; __first != __last; ++__first, (void)++__cur) 
        std::_Construct(std::__addressof(*__cur), *__first);
    return __cur;
}

template <typename _T1, typename... _Args>
inline void _Construct(_T1 *__p, _Args &&... __args) 
{
    ::new (static_cast<void *>(__p)) _T1(std::forward<_Args>(__args)...);  // 实际copy(或者move)元素
}
```

其中 \_Construct 就是实际 copy(或者 move)元素的函数。**这里很关键的一点是：对 move iterator 进行解引用操作，返回的是一个右值引用。**，这也就保证了，当 \_\_first 类型是 move iterator 时，用 \_T1(std::forward<_Args>(__args)... 进行“完美转发”才调用 \_T1 类型的 move constructor，生成的新对象被放到新 vector 的 \_\_p 地址中。

总结一下过程就是：

- 利用 type trait 和 iterator trait 生成指向旧容器的 normal iterator 或者 move iterator
- 循环将旧容器的元素搬到新容器。如果指向旧容器的是 move iterator，那么解引用会返回右值引用，会调用元素的 move constructor，否则调用 copy constructor

### noexcept move constructor 对性能的影响

这篇文章 [C++ NOEXCEPT AND MOVE CONSTRUCTORS EFFECT ON PERFORMANCE IN STL CONTAINERS](http://www.hlsl.co.uk/blog/2017/12/1/c-noexcept-and-move-constructors-effect-on-performance-in-stl-containers) 介绍了 noexcept move constructor 对耗时以及内存的影响。

![img](.\Photo\vector_noexcept_01.jpg)

