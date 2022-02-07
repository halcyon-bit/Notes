# auto 推导规则

auto 的推导规则跟模板参数的推导基本上一样的，都是推导参数。

```c++
template<typename T>
void f(ParamType param);

f(expr);
```

编译器使用 expr 来推断 ParamType 和 T。

那么对应在 auto 推导中，auto 就对应了 T，变量的 type specifier 就是 ParamType：

```c++
auto x = 27;
const auto cx = x;
const auto& rx = x;
```

cx 的 type specifier 就是 `const auto`，rx 的 type specifier 就是 `const auto&`。

可以想象成在推导 auto 类型时，编译器就是在做一次模板参数类型推断：

```c++
// 推断 x
template <typename T>
void func_for_x(T param);

func_for_x(27);

// 推断 cx
template <typename T>
void func_for_cx(const T param);

func_for_cx(x);

// 推断 rx
template <typename T>
void func_for_rx(const T& param);

func_for_rx(x);
```

在模板参数推导中，根据 ParamType 将情况分成了三类。在 auto 推导中，同样可以根据 type specifier 来分成三种情况：

- type specifier 是一个指针或者引用，但不是 universal reference(或者叫 forwarding references)
- type specifier 是一个 universal reference
- type specifier 既不是指针也不是引用

![img](.\Photo\template_06_01.jpg)

## 一、type specifier 是一个指针或者引用

```c++
const auto& rx = x;
```

![img](.\Photo\template_06_02.jpg)

## 二、type specifier 是一个 universal reference

```c++
auto&& uref1 = x;  // x is int and lvalue, so uref1's type is int&
auto&& uref2 = cx;  // cx is const int and lvalue, so uref2's type is const int&
auto&& uref3 = 27;  // 27 is int and rvalue, so uref3's type is int&&
```

![img](.\Photo\template_06_03.jpg)

## 三、type specifier 既不是指针也不是引用

```c++
auto x = 27;
const auto cx = x;
```

![img](.\Photo\template_06_04.jpg)

注意：**假如有个函数返回引用，使用 auto 接收返回值，如果想改变函数的返回值，那么必须用 auto&，而不是 auto，因为这里和函数模板参数推断规则一样，是 pass-by-value，会忽略引用、const 和 volatile**。

```c++
int x = 50;
int& f()
{ 
    return x;
}

int main()
{
    auto a1 = f();
    a1 = 10;  // x = 50

    auto& a2 = f();
    a2 = 20;  // x = 20
    return 0;
}
```

## 四、数组和函数类型推断

在模板参数推导规则中讨论了数组和函数作为模板参数的推断情况，在 auto 类型推断中情况也是相同的。

```c++
const char name[] = "ege";

auto arr1 = name;  // arr1's type is const char*
auto& arr2 = name;  // arr2's type is const char(&)[4]

void someFunc(int, double);

auto func1 = someFunc;  // func1's type is void (*)(int, double)
auto& func2 = someFunc;  // func2's type is void (&)(int, double)
```

## 五、auto 与函数模板参数推断的区别

从 C++11 起有 4 种方式可以定义一个变量：

```c++
auto x1 = 27;
auto x2(27);
auto x3 = { 27 };
auto x4{ 27 };
```

最后两种是 C++11 的新特性：统一初始化（uniform initialization），就是这个造成了模板参数推导和 auto 类型推导的最大区别：

```c++
auto x1 = 27;  // type is int
auto x2(27);  // type is int
auto x3 = { 27 };  // type is std::initializer_list<int>
auto x4{ 27 };  // type is std::initializer_list<int>
```

特殊点在于：**如果使用统一初始化来进行 auto 类型推断，那么最终 auto 推断出的结果是 `std::initializer_list<int>`**。

注：2014 年 C++ 标准委员会通过了 N3922 提案，上述的 `auto x4{27}`，x4 的类型为 int，不是 `std::initializer_list<int>`。

所以下面的代码会编译报错：

```c++
template <typename T>
void f(T param);

f({ 11, 23, 9 });
```

如果指定 ParamType 为 `std::initializer_list<T>`，则可以正确的推断 T 的类型：

```c++
template <typename T>
void f(std::initializer_list<T> initList);

f({ 11, 23, 9 });  // T deduced as int, and initList's type is std::initializer_list<int>
```

### C++14 中更特殊的情况

在 C++14 中，允许将函数返回值和 lambda 参数声明为 auto，**但是在这种情况下，auto 的类型推断使用的是模板参数类型推断规则**。

```c++
auto createInitList()
{
    return { 1, 2, 3 };  // error: can't deduce type for { 1, 2, 3 }
}

std::vector<int> v;
auto resetV = [&v](const auto& newValue) {
    v = newValue; 
};

resetV({ 1, 2, 3 });  // error: can't deduce type for { 1, 2, 3 }
```

