# 强制类型转换

在 C++ 基本的数据类型中，可以分为四类：整型，浮点型，字符型，布尔型。

1. 将浮点型数据赋值给整型变量时，舍弃其小数部分。
2. 将整型数据赋值给浮点型变量时，数值不变，但是以指数形式存储。
3. 将 double 型数据赋值给 float 型变量时，注意数值范围溢出。
4. 字符型数据可以赋值给整型变量，此时存入的是字符的 ASCII 码。
5. 将一个 int，short 或 long 型数据赋值给一个 char 型变量，只将低8位原封不动的送到 char 型变量中。
6. 将有符号型数据赋值给长度相同的无符号型变量，连同原来的符号位一起传送。

在 C++ 语言中新增了四个关键字 **static_cast**、**const_cast**、**reinterpret_cast** 和 **dynamic_cast**。这四个关键字都是用于强制类型转换。

## 一、static_cast

```c++
static_cast<type>(expression)
```

在 C++ 语言中 static_cast 用于数据类型的强制转换，强制将一种数据类型转换为另一种数据类型。例如将整型数据转换为浮点型数据。

```c++
// C 语言所采用的类型转换
int a = 10, b = 3;
double result = (double)a/(double)b;

// static_cast 关键字的使用
int a = 10, b = 3;
double result = static_cast<double>(a) / static_cast<double>(b);
```

它主要有以下几种用法：

- **用于类层次结构中基类和派生类之间指针或引用的转换**
  - **进行上行转换（把派生类的指针或引用转换成基类表示）是安全的**
  - **进行下行转换（把基类的指针或引用转换为派生类表示），由于没有动态类型检查，所以是不安全的**

- **用于基本数据类型之间的转换，如把 int 转换成 char。安全性由开发人员保证**
- **把空指针转换成目标类型的空指针**
- **把任何类型的表达式转换为 void 类型**

注意：static_cast 不能转换掉 expression 的 const、volitale 或者 __unaligned 属性。

static_cast：实现 C++ 中内置基本数据类型之间的相互转换。如果涉及到类的话，static_cast 只能在有相互联系的类型中进行相互转换，不一定包含虚函数。

## 二、const_cast

```c++
const_cast<type>(expression)
```

用于强制去掉不能被修改的常量特性（去掉 const 属性），但需要特别注意的是 const_cast 不是用于去除变量的常量性，而是去除指向常数对象的指针或引用的常量性，其**去除常量性的对象必须为指针或引用**。

该运算符用来修改类型的 const 或 volatile 属性。除了 const 或 volatile 修饰之外，type 和 expression 的类型是一样的。

**常量指针被转化成非常量指针，并且仍然指向原来的对象；常量引用被转换成非常量引用，并且仍然指向原来的对象。**

```c++
// 一个错误的例子
const int a = 10;
const int *p = &a;
*p = 20;  // compile error
int b = const_cast<int>(a);  // compile error
```

有两个编译错误，第一个编译错误是 *p 具有常量性，其值是不能被修改的；另一处错误是 const_cast 强制转换对象必须为指针或引用，而代码中为一个变量，这是不允许的！

```c++
// const_cast关键字的使用
const int a = 10;
const int *p = &a;
int *q = const_cast<int*>(p);
*q = 20;
```

变量 a 为常量变量，同时声明了一个 const 指针指向该变量。之后定义了一个普通的指针 *q。将 p 指针通过 const_cast 去掉其常量性，并赋给 q 指针。之后再修改 q 指针所指地址的值时，这是不会有问题的。

指针 p 和指针 q 都是指向 a 变量的地址，p 和 q 的值都是20，而 a 的值也是20。

```c++
const int* Search(const int* a, int n, int val) {
    for (int i = 0; i < n; ++i) {
        if (a[i] == val)
            return &a[i];
    }
    return nullptr;
}

void main() {
    int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int *p = const_cast<int*>(Search(a, 10, 5));
}
```

定义了一个函数，用于在 a 数组中寻找 val 值，如果找到了就返回该值的地址，如果没有找到则返回 nullptr。函数 Search 返回值是 const 指针，当 a 数组中找到了 val 值的时候，返回 val 的地址，最关键的是 a 数组在 main 函数中并不是 const，因此即使去掉返回值的常量性有可能会造成 a 数组被修改，但是这也依然是安全的。

```c++
const int& Search(const int* a, int n, int val) {
    for (int i = 0; i < n; ++i) {
        if (a[i] == val)
            return a[i];
    }
    return nullptr;
}  

void main() {
    int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int &p = const_cast<int&>(Search(a, 10, 5));
}            
```

了解了 const_cast 的使用场景后，可以知道使用 const_cast 通常是一种无奈之举，同时也建议在 C++ 程序设计过程中一定不要利用 const_cast 去掉指针或引用的常量性并且去修改原始变量的数值，这是一种非常不好的行为。

## 三、reinterpret_cast

```c++
reinterpret_cast<type>(expression)
```

在 C++ 语言中，主要用于三种强制转换：

1. **改变指针或引用的类型**
2. **将指针或引用转换为一个足够长度的整形**
3. **将整型转换为指针或引用类型**

type 必须是一个指针、引用、算术类型、函数指针或者成员指针。

它可以把一个指针转换成一个整数，也可以把一个整数转换成一个指针（先把一个指针转换成一个整数，在把该整数转换成原类型的指针，还可以得到原先的指针值）。

在使用 reinterpret_cast 强制转换过程仅仅只是比特位的拷贝，因此在使用过程中需要特别谨慎！

```c++
int *a = new int;
double *d = reinterpret_cast<double*>(a);
```

reinterpret_cast 可以将指针或引用转换为一个足够长度的整形，此中的足够长度具体长度需要多少则取决于操作系统，如果是32位的操作系统，就需要4个字节及以上的整型，如果是64位的操作系统则需要8个字节及以上的整型。

## 四、dynamic_cast

```c++
dynamic_cast<type>(expression)
```

1. 其他三种都是编译时完成的，dynamic_cast 是运行时处理的，运行时要进行类型检查。
2. 不能用于内置的基本数据类型的强制转换。
3. dynamic_cast 转换如果成功的话返回的是指向类的指针或引用，转换失败的话则会返回 nullptr。
4. 使用 dynamic_cast 进行转换的，**基类中一定要有虚函数**，否则编译不通过。
   - 需要检测有虚函数的原因：类中存在虚函数，就说明它有基类指针或引用指向派生类对象的情况，此时转换才有意义
   - 这是由于运行时类型检查需要运行时类型信息，而这个信息存储在类的虚函数表（关于虚函数表的概念，详细可见<Inside c++ object model>）中
   - 只有定义了虚函数的类才有虚函数表
5. 在类的转换时，在类层次间进行上行转换时，dynamic_cast 和 static_cast 的效果是一样的。在进行下行转换时，dynamic_cast 具有类型检查的功能，比 static_cast 更安全
   - 向上转换，即为子类指针指向父类指针（一般不会出问题）；向下转换，即将父类指针转化子类指针。
   - 向下转换的成功与否还与将要转换的类型有关，即要转换的指针指向的对象的实际类型与转换以后的对象类型一定要相同，否则转换失败。

在 C++ 中，编译期的类型转换有可能会在运行时出现错误，特别是涉及到类对象的指针或引用操作时，更容易产生错误。dynamic_cast 操作符则可以在运行期对可能产生问题的类型转换进行测试。

```c++
class Base {
public:
    void m() { cout << "m" << endl; }
};

class Derived : public Base {
public:
    void f() { cout << "f" << endl; }
};

int main() {
    Derived *p = new Base;
    p = staic_cast<Derived*>(new Base);
    p->m();
    p->f();
}    
```

定义了两个类：base 类和 derived 类，这两个类构成继承关系。在 base 类中定义了 m 函数，derived 类中定义了 f 函数。

主函数中定义的是一个派生类指针，当将其指向一个基类对象时，这是错误的，会导致编译错误。但是通过强制类型转换可以将派生类指针指向一个基类对象，p = static_cast<derived*>(new base); 语句实现的就是这样一个功能，这样的一种强制类型转换时合乎 C++ 语法规定的，但是是非常不明智的，它会带来一定的危险。

在程序中 p 是一个派生类对象，我们将其强制指向一个基类对象，首先通过 p 指针调用 m 函数，因为基类中包含有 m 函数，这一句没有问题，之后通过 p 指针调用 f 函数。一般来讲，因为 p 指针是一个派生类类型的指针，而派生类中拥有 f 函数，因此 p->f(); 这一语句不会有问题，但是本例中 p 指针指向的是基类的对象，而基类中并没有声明 f 函数，虽然 p->f(); 这一语句虽然仍没有语法错误，但是它却产生了一个运行时的错误。换言之，p 指针是派生类指针，这表明程序设计人员可以通过 p 指针调用派生类的成员函数 f，但是在实际的程序设计过程中却误将 p 指针指向了一个基类对象，这就导致了一个运行期错误。

产生这种运行期的错误原因在于 static_cast 强制类型转换时并不具有保证类型安全的功能，而 C++ 提供的 dynamic_cast 却能解决这一问题，dynamic_cast 可以在程序运行时检测类型转换是否类型安全。

当然 dynamic_cast 使用起来也是有条件的，它要求所转换的操作数必须包含多态类类型(即至少包含一个虚函数的类)。

```c++
class Base {
public:
    void m() { cout << "m" << endl; }
};

class Derived : public Base {
public:
    void f() { cout << "f" << endl; }
};

void main() {
    Derived *p = new Base;
    p = dynamic_cast<Derived*>(new Base);
    p->m();
    p->f();
}
```

利用 dynamic_cast 进行强制类型转换，但是因为 base 类中并不存在虚函数，因此会产生编译错误。为了解决本例中的语法错误，可以将 base 类中的函数 m 声明为虚函数，virtual void m() { cout<<"m"<<endl; }。

dynamic_cast 还要求 <> 内部所描述的目标类型必须为指针或引用。