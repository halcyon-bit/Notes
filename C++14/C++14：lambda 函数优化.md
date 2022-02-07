# Lambda 优化

## 一、泛型 Lambda

C++11 要求 lambda 表达式参数需要使用具体的类型声明。

```c++
auto lambda = [](int x, int y) { return x + y; };
```

C++14 进行了优化，lambda 表达式参数类型可以是 `auto`。

```c++
auto lambda = [](auto x, auto y) { return x + y; }; 
int res = lambda(1, 3); // 4
std::string str = lambda("hello", " world"); // hello world
```

## 二、Lambda 初始化捕获

C++14 允许使用任意表达式初始化捕获列表中的变量。被初始化的变量，不需要与外部的任何变量进行绑定，同时其初始值是在创建时捕获的，而非调用时捕获。

```c++
#include <iostream>

int test(int i) {
    return i * 100;
}

int main() {
    auto f1 = [x = test(6)]{ return x; };
    std::cout << f1() << std::endl;  // 600

    int val = 1000;
    auto f2 = [x = val]{ return x; };

    val = 2000;
    std::cout << f2() << std::endl;  // 输出：1000 而不是 2000

    auto f3 = [x = 0]() mutable { return x++; };
    // this would not compile without 'mutable' as we are modifying x on each call

    auto res = f3();  // 输出：0
    res = f3();  // 输出：1
    res = f3(); // 输出：2
    return 0;
}
```

C++14 中除了通过拷贝或者引用捕获变量，还允许捕获只能移动的变量（如：`unique_ptr`）。

```c++
int main() {
    auto p = std::make_unique<int>(100);

    auto f1 = [=] { *p = 200; };  // error: std::unique_ptr cannot be copied

    auto f2 = [p = std::move(p)]{ *p = 200; };
    // 在 f2 创建完成后，外部的 p 变成 nullptr 了
    return 0;
}
```

相对于引用变量，通过引用捕获初始化，可以重新定义变量名称。

```c++
#include <iostream>

int main() {
    auto val = 100;

    auto f = [&r = val, val = val * 6]{
        ++r;
        return r + val;
    };

    std::cout << f() << std::endl;  // 输出：701
    std::cout << val << std::endl;  // 输出：101
    return 0;
}
```

