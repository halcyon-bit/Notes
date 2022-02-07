# std::function、std::bind、lambda 表达式

C++11 新增 `std::function`、`std::bind`、lambda 表达式等封装，使函数调用更加方便。

满足以下条件之一就可称为**可调用对象**：

- 函数指针
- 具有 operator() 成员函数的类对象(传说中的仿函数)，lambda 表达式
- 是一个可被转换为函数指针的类对象
- 是一个类成员(函数)指针
- bind 表达式或其它函数对象

## 一、lambda 表达式

lambda 表达式定义了一个匿名函数，可以捕获一定范围的变量在函数内部使用，一般有如下语法形式：

```C++
auto func = [capture] (params) opt -> ret { func_body; };
```

其中 func 可以说是 lambda 表达式的名字，作为一个函数使用，capture 是捕获列表，params 是参数表，opt 是函数选项(mutable 之类)，ret 是返回值类型，func_body 是函数体。

```c++
auto func1 = [](int a) -> int { return a + 1; };
auto func2 = [](int a) { return a + 2; };
auto func3 = [] {return 1};  // 省略空参数列表
std::cout << func1(1) << " " << func2(2) << std::endl;
```

很多时候 lambda 表达式返回值是很明显的，C++11 允许省略表达式的返回值定义。在没有参数列表的时候，参数列表可以省略。

lambda 表达式允许捕获一定范围内的变量：

- [] 不捕获任何变量
- [&] 引用捕获，捕获外部作用域所有变量，在函数体内当作引用使用
- [=] 值捕获，捕获外部作用域所有变量，在函数体内有个副本
- [=, &a] 值捕获外部作用域所有变量，按引用捕获 a 变量
- [a] 只值捕获 a 变量，不捕获其它变量，捕获的 a 是副本
- [this] 捕获当前类中的this指针，引用捕获

```c++
#include <iostream>

int main() {

	int a = 0;
	auto func1 = [=]() { return a; }; // 值捕获 a
	a = 100;
	std::cout << "func1.a: " << func1() << std::endl;
	std::cout << "a: " << a << std::endl;

	auto func2 = [=]() { return a++; }; // error 修改按值捕获的外部变量
	auto func3 = [=]() mutable { return ++a; };
	std::cout << "func3.a: " << func3() << std::endl;
	std::cout << "a: " << a << std::endl;
    
    auto func4 = [&a]() { return ++a; };
	std::cout << "func4.a: " << func4() << std::endl;
	std::cout << "a: " << a << std::endl;
	a = 500;
	std::cout << "func4.a: " << func4() << std::endl;
	std::cout << "a: " << a << std::endl;
}
```

代码中的 func2 是编译不过的，因为修改了按值捕获的外部变量，其实 lambda 表达式就相当于是一个仿函数，仿函数是一个有 operator() 成员函数的类对象，这个 operator() 默认是 const 的，所以不能修改成员变量，而加了 mutable，就是去掉 const 属性。

运行结果：

![img](./Photo/lambda.png)

从结果发现，func1() 的结果为 0，因为 lambda 表达式对与值捕获的情况，在 lambda 表达式创建之时，就已经获取了相关变量的值，而不是在调用时获取。

对于没有捕获任何变量的 lambda 表达式，还可以被转换成一个普通的函数指针。

```c++
using func_t = int(*)(int);
func_t f = [](int a){ return a; };
f(123);
```

## 二、std::bind

使用 `std::bind` 可以将可调用对象和参数一起绑定，绑定后的结果使用 `std::function` 进行保存，并延迟到任何需要调用的时候。

`std::bind` 通常有两大作用：

- 将可调用对象与参数一起绑定为另一个 `std::function` 供调用
- 将 n 个参数的可调用对象转成 m(m < n)个参数的可调用对象，绑定一部分参数，这里需要使用 `std::placeholders`

```c++
#include <functional>
#include <iostream>
#include <memory>

void func1(int arg1, int arg2, int arg3, const int& arg4, int arg5) {
	std::cout << arg1 << ' ' << arg2 << ' ' << arg3 << ' ' << arg4 << ' ' << arg5 << std::endl;
}

int func2(int arg) {
	return arg; 
}

class Test {
public:
	void add(int arg1, int arg2) { 
		std::cout << arg1 + arg2 << std::endl; 
	}

	int m_data = 10;
};

int main() {
	using namespace std::placeholders;  // 针对 _1, _2, _3...

	// 演示参数重排序和按引用传递
	int n = 7;
	// （_1 与 _2 来自 std::placeholders，表示将来会传递给 f1 的参数）
	// （std::cref 创建常量引用）
	auto f1 = std::bind(func1, _2, 42, _1, std::cref(n), n);
	n = 10;
	std::cout << "f1: ";
	f1(1, 2, 1001);  // 1 为 _1 所绑定， 2 为 _2 所绑定，不使用 1001
					 // 进行到 func1(2, 42, 1, n, 7) 的调用


	// 嵌套 bind 子表达式共享占位符
	auto f2 = std::bind(func1, _3, std::bind(func2, _3), _3, 4, 5);
	std::cout << "f2: ";
	f2(10, 11, 12);  // 进行到 func1(12, func2(12), 12, 4, 5); 的调用

	// 绑定指向成员函数指针
	Test t;
	auto f3 = std::bind(&Test::add, &t, 95, _1);
	std::cout << "f3: ";
	f3(5);

	// 绑定指向数据成员指针  输出数据成员的值
	auto f4 = std::bind(&Test::data, _1);
	std::cout << "f4: " << f4(t) << std::endl;

	// 智能指针亦能用于调用被引用对象的成员
	std::cout << "f4: " << f4(std::make_shared<Test>(t)) << std::endl;

    std::vector<int> vec;

    // 查找元素值大于10的元素个数
    int count = std::count_if(vec.begin(), vec.end(), std::bind(less<int>(), 10, _1));

    // 查找元素值小于10的元素个数
    count = std::count_if(vec.begin(), vec.end(), std::bind(less<int>(), _1, 10));

    return 0;
}
```

## 三、std::function

std::function 就是上面这种可调用对象的封装器，可以把 std::function 看做一个函数对象，用于表示函数这个抽象概念。std::function 的实例可以存储、复制和调用任何可调用对象，存储的可调用对象称为 std::function 的目标，若 std::function 不含目标，则称它为空，调用空的 std::function 的目标会抛出 std::bad_function_call 异常。

```c++
#include <functional>
#include <iostream>

class Test1 {
public:
	Test1(int num) : m_num(num) {}

	void add(int i) const { 
		std::cout << m_num + i << std::endl;
	}

	int m_num;
};

// 仿函数
class Test2 {
public:
	void operator()(int i) const { 
		std::cout << i << std::endl;
	}
};

void print(int i) { 
	std::cout << i << std::endl; 
}

int main() {
	// 存储普通函数
	std::function<void(int)> func1 = print;
	std::cout << "func1: ";
	func1(10);

	// 存储 lambda
	std::function<void()> func2 = []() { print(20); };
	std::cout << "func2: ";
	func2();

	// 存储 std::bind 的结果
	std::function<void()> func3 = std::bind(print, 30);
	std::cout << "func3: ";
	func3();

	// 存储成员函数的调用
	std::function<void(const Test1&, int)> func4 = &Test1::add;
	const Test1 t(40);
	std::cout << "func4: ";
	func4(t, 20);
	std::cout << "func4: ";
	func4(50, 30);  // 会先构造 Test1(50)，再调用 add

	// 存储数据成员访问器的调用
	std::function<int(const Test1&)> func5 = &Test1::m_num;
	std::cout << "func5: num: " << func5(t) << std::endl;

	// 存储成员函数及对象的调用
	using std::placeholders::_1;
	std::function<void(int)> func6 = std::bind(&Test1::add, t, _1);
	std::cout << "func6: ";
	func6(50);

	// 存储成员函数和对象指针的调用
	std::function<void(int)> func7 = std::bind(&Test1::add, &t, _1);
	std::cout << "func7: ";
	func7(50);

	// 存储函数对象的调用
	std::function<void(int)> func8 = Test2();
	std::cout << "func8: ";
	func8(60);
}
```

运行结果：

![img](./photo/function.png)

从上面可以看到 std::function 的使用方法，当给 std::function 填入合适的参数表和返回值后，它就变成了可以容纳所有这一类调用方式的函数封装器。std::function 还可以用作回调函数，或者在 C++ 里如果需要使用回调那就一定要使用 std::function，特别方便。

