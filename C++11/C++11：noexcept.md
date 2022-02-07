## noexcept

`noexcept` 表示其修饰的函数不会抛出异常，如果 `noexcept` 修饰的函数抛出了异常，编译器可以选择直接调用 `std::terminate()` 函数来终止程序的运行，这比基于异常机制的 `throw()` 在效率上会高一些，这是因为异常机制会带来一些额外的开销，比如函数抛出异常，会导致函数栈被依次的展开，并依帧调用在本帧中以构造的自动变量的析构函数等。

**noexcept** 有两种形式，一种就是简单地在函数声明后加上 noexcept 关键字

```c++
void func() noexcept;
// 等价于
void func() noexcept(true);
```

另一种则可以接受一个常量表达式作为参数，如：

```c++
void func() noexcept(常量表达式);
```

常量表达式的结果会被转换成一个 bool 类型的值，该值为 true，表示函数不会抛出异常，反之，则有可能抛出异常。这里，不带常量表达式的 noexcept 相当于声明了 noexcept(true)，即不会抛出异常。

在 C++11 中使用 noexcept 可以有效地阻止异常的传播与扩散。

```c++
void Throw() {
    throw 1
}

void NoBlockThrow() {
    Throw(); 
}

void BlockThrow() noexcept {
    Throw();
}

int main() {
    try {
        Throw();
    }
    catch(...) {
        cout << "found throw." << endl;
    }
    
    try {
        NoBlockThrow();
    }  // 会在catch中捕获异常
    catch(...) {
        cout << "throw is not blocked." << endl;
    }
    
    try { 
        BlockThrow();
    }  // 直接调用std::terminate中断程序的执行，从而阻止了异常的继续传播
    catch(...) {
        cout << "found throw 1." << endl;
    }
}
```

noexcept 作为一个操作符时，通常可以用于模板

```c++
template<typename T>
void fun() noexcept(noexcept(T())) {}
```

这里，fun 函数是否是一个 noexcept 的函数，将由 T() 表达式是否会抛出异常所决定。这里的第二个 noexcept 就是一个 noexcept 操作符。当其参数是一个有可能抛出异常的表达式的时候，其返回值为 false，反之为 true。 这样一来，就可以使模板函数根据条件实现 noexcept 修饰的版本或无 noexcept 修饰的版本。 从泛型编程的角度看来，这样的设计保证了关于“函数是否抛出异常”这样的问题可以通过表达式进行推导。因此这也可以视作 C++11 为了更好地支持泛型编程而引入的特性。

比如在 C++98 中，存在着使用 throw() 来声明不抛出异常的函数。

```c++
template<typename T>
class A
{
public:
    static constexpr T min() throw() {
        return T(); 
    }
    static constexpr T max() throw() {
        return T(); 
    }
    static constexpr T lowest() throw() {
        return T(); 
    }
// ...
}
```

而在 C++11 中，则使用 noexcept 来替换 throw()

```c++
template<typename T>
class A
{
public:
    static constexpr T min() noexcept {
        return T();
    }
    static constexpr T max() noexcept {
        return T(); 
    }
    static constexpr T lowest() noexcept {
        return T();
    }
// ...
}
```

又比如在 C++98 中，new 可能会包含一些抛出的 std::bad_alloc 异常。

```c++
void* operator new(std::size_t) throw(std::bad_alloc);
void* operator new[](std::size_t) throw(std::bad_alloc);
```

而在 C++11 中，则使用 noexcept(false) 来进行替代。

```c++
void* operator new(std::size_t) noexcept(false);
void* operator new[](std::size_t) noexcept(false);
```

C++11 标准中类的析构函数默认也是 noexcept(true)

```c++
class MyString {
private:
    char* _data;
    size_t _len;
// ...
public:
    // move constructor
    MyString(MyString&& str) noexcept : _data(str._data), _len(str._len) {
        // ...
    }
    // move assignment
    MyString& operator=(MyString&& str) noexcept {
        // ...
        return *this;
    }
// ...
};
```

**vector 使用移动语义时，需要确保与移动相关的函数是不会抛出异常的，否则 vector 不会调用。**

> You need to inform C++ (specifically std::vector) that your move constructor and destructor does not throw. Then the move constructor will be called when the vector grows. If the constructor is not noexcept, std::vector can't use it, since then it can't ensure the exception guarantees demanded by the standard.

