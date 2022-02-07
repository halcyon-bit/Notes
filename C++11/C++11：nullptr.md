# nullptr

C++11 引入了 `nullptr`。

```c++
int *ptr = nullptr;
```

而 `NULL` 从源码中可得，在 C 中 `NULL` 是 `((void*)0)` 指针，在 C++ 中 `NULL` 却是个整数 0。

```c++
#ifndef NULL
	#ifdef __cplusplus
		#define NULL 0
	#else
		#define NULL ((void*)0)
	#endif
#endif
```

C++ 中有个很特别的规定就是 0 既表示**整数常量**也用来表示**空指针常量**。

在 C++03 标准中：

> 主要规定空指针常量需要被转化为指针类型，同时这个转化为指针类型的值还不能和其它的对象指针或者函数指针的值相同。两个空指针常量的值还需要相等

而 C99 标准中：

> C 中的空指针常量是整数 0 被强制转成了 void*，这就可以确保这个空指针的值与其他对象或函数指针值不相等。

C++ 不允许 `void*` 隐式转换成其他指针类型。

```c++
int main() 
{
    int *a = (void*)0; // error: cannot initialize a variable of type 'int *' with an rvalue of type 'void *'
}
```

## 一、为什么要引入 nullptr

一个原因是可以让整数 0 只表示一件事情，它只是一个整数类型 0，没有任何其他语义。

另一个原因是 C++ 函数重载中，传 `NULL` 会导致编译失败，所以需要引入 `nullptr`，使用 `nullptr` 可以解决函数重载中的参数匹配问题。

```c++
void func(char*) {
    cout << "char*";
}
void func(int) {
    cout << "int";
}
void main() {
    func(NULL);  // error: call of overloaded ‘func(NULL)’ is ambiguous
    func(nullptr);
}
```

## 二、nullptr 的实现

```c++
struct nullptr_t 
{
    void operator&() const = delete;
    
    template <class T>
    inline operator T*() const 
    {
        return 0;
    }
    
    template <class C, class T>
    inline operator T C::*() const 
    {
        return 0;
    }
};
```

通过实现部分运算符，`nullptr` 可以转换成 int* 等，同时，它的取址操作被 delete 修饰，所以不能对 `nullptr` 取址。

**nullptr 可以用于抛出异常**：`nullptr` 是有类型的。

```c++
typedef decltype(nullptr) nullptr_t;
```

当空指针用 `nullptr` 表示时，空指针就有类型了，当有异常需要抛出时，就可以抛出 `nullptr`。

```c++
void main() 
{
	try {
        // ...
        throw nullptr;
    } catch(nullptr_t) {
        // ...
    }
}
```

之后使用它的类型 `nullptr_t` 捕获，这里如果 throw NULL，那用什么类型去 catch 呢？用 int 类型来 catch？是不是有点别扭。所以能用 `nullptr` 就一定要用 `nullptr` 代替 `NULL`。

## 三、总结

1. 使用 `nullptr` 可以不用担心整形和指针类型的重载，不会产生二义性导致编译失败
2. 0 和空指针分别表示不同的含义，使用 `nullptr` 可以更好的支持模板编程
3. 使用 `nullptr` 使代码更安全，让用户编写代码更清晰直观

空指针特性：

1. 不能被用于算术表达式中，不能被赋值给整型，也不能用于和指针类型外的类型作比较
2. 空指针可以转化为任何指针类型，不能转换成指针类型外的任何类型



