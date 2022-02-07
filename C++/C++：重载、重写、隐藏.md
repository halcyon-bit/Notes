# 重载、重写、隐藏

## 一、重载

重载（overload）是指同一可访问区内被声明的几个具有不同参数列表（参数的类型，个数，顺序不同）的同名函数，根据参数列表确定调用哪个函数，重载不关心函数的返回类型。

1. 相同的范围（在同一个作用域中）
2. 函数名字相同
3. 参数列表不同
4. virtual 关键字可有可无
5. 返回类型可以不同

```c++
int test();
int test(int a);
int test(int a, double b);
int test(double b, int a);
```

## 二、重写

重写（override）是指派生类中存在重新定义的函数，也叫覆盖。其函数名、参数列表、返回值类型，都必须同基类中被重写的函数一致，只有函数体不同（花括号内）。派生类调用时会调用派生类的重写函数，不会调用基类的函数。基类中被重写的函数必须有 virtual 修饰，即虚函数。

1. 不在同一作用域（分别位于派生类与基类）
2. 函数名称相同
3. 参数列表相同（参数个数和参数类型）
4. 基类函数必须有 virtual 关键字，不能有 static
5. 返回值类型相同（或是协变），否则报错
6. 重写函数的访问修饰符可以不同。尽管 virtual 是 private 的，派生类中重写改写为 public，protected 也是可以的

```c++
class Base {
public:
    virtual void test(int a) { 
        cout << "this is base\n"; 	 
    }
};

class Derived : public Base {
public:
    void test(int a) { 
        cout << "this is Base overwrite function\n"; 
    }
};
```

## 三、隐藏

隐藏是指派生类的函数屏蔽了与其同名的基类函数。注意只要同名函数，不管参数列表是否相同，基类函数都会被隐藏。

1. 不在同一个作用域（分别位于派生类与基类）
2. 函数名字相同
3. 返回类型可以不同
4. 参数不同，此时不论有无 virtual 关键字，基类的函数将被隐藏（注意别与重载混淆）而不是被重写
5. 参数相同，但是基类函数没有 virtual 关键字。此时，基类的函数被隐藏

```c++
class Base {
public:
    virtual void test(int a) {  // 有 virtual 关键字，参数列表不同 
        cout << "this is base there are different parameters with virtual" << endl;
    }
    void test1() {
        cout << "this is base with the same parameters with not virtual" << endl;
    }
    virtual void test2() {
        cout << "this is base with the same parameters with virtual" << endl;
    }
};

class Derived : public Base {
public:
    void test() {
        cout << "this is Ship there are different parameters with virtual cover" << endl;
    }
    void test1() {
        cout << "this is Ship with the same parameters with not virtual cover" << endl;
    }
    void test2() {
        cout << "this is Ship with the same parameters with virtual cover" << endl;
    }
};
```

## 四、区别

### 重载和重写的区别

1. 范围区别：重写和被重写的函数在不同的类中，重载和被重载的函数在同一类中。
2. 参数区别：重写与被重写的函数参数列表一定相同，重载和被重载的函数参数列表一定不同。
3. virtual 的区别：重写的基类必须要有 virtual 修饰，重载函数和被重载函数可以被 virtual 修饰，也可以没有。

### 隐藏和重写，重载的区别

1. 与重载范围不同：隐藏函数和被隐藏函数在不同类中。
2. 参数的区别：隐藏函数和被隐藏函数参数列表可以相同，也可以不同，但函数名一定同；当参数不同时，无论基类中的函数是否被 virtual 修饰，基类函数都是被隐藏，而不是被重写。