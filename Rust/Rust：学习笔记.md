# Rust

## 一、环境

### 1. 安装、运行相关

- 更新 Rust：rustup update
- 卸载 Rust：rustup self uninstall
- 版本：rustc --version

- 文档：rustup doc
- 编译：rustc main.rs

### 2. Cargo

cargo 是 Rust 的构建系统和包管理工具，用于构建代码、下载依赖的库、构建库...

- 版本：cargo --version
- 创建项目：cargo new 项目名称
- 构建 Cargo 项目：cargo build [--release]
- 构建和运行 Cargo 项目：cargo run
- 检查代码：cargo check

## 二、习惯

Rust 代码中的函数和变量名使用 snake case 规范风格。在 snake case 中，所有字母都是小写并使用下划线分隔单词。

Rust 常量使用全大写字母，每个单词之间用下划线分开

## 三、基础知识

### 1. 变量

**Rust 是强类型语言，但具有自动判断变量类型的能力**。

声明变量使用 let 关键字，默认情况下，**变量是不可变的**。声明时，在变量前面加上 **mut**，就可以使变量可变。

**注释**使用 "//"。

```rust
let a = 100;  // 变量 a 不可变
a = 200;  // error
let mut b = 100;  // 变量 b 可变
b = 300;  // ok
```

### 2. 常量

常量在绑定值以后也是**不可变**的，但是它与不可变的变量有很多区别：

- 不可以使用 mut，常量永远都是不可变的
- 声明常量使用 const 关键字，它的类型必须被标注
- 常量可以在任何作用域内进行声明，包括全局作用域
- **常量只可以绑定到常量表达式，无法绑定到函数的调用结果或只能在运行时才能计算出的值**

在程序运行期间，常量在其声明的作用域内一直有效。

```rust
const MAX_POINTS: u32 = 100_000;

fn main() {
    println!("The value is {}", MAX_POINTS);  // The value is 123.123
    const MAX_POINTS: f32 = 123.123;
    println!("The value is {}", MAX_POINTS);  // The value is 123.123
    // let MAX_POINTS = 123;  // error
}
```

疑问1：全局作用域中的 MAX_POINTS(u32) 并没有被使用，难道常量有效性从所在作用域的起始位置生效，不论在该作用域内何处声明该常量？小作用域的常量会覆盖大作用域的常量？(测试结果是小作用域的常量覆盖了大作用域的常量)

疑问2：若已定义常量，则不可定义同名变量？(测试结果是编译失败，报错)

### 3. 隐藏(shadow)

可以使用相同的名字声明新的变量，新的变量就会 shadow(隐藏)之前声明的同名变量，在后续的代码中这个变量名代表的就是新的变量。

shadow 和把变量标记为 mut 是不一样的：

- 如果不使用 let 关键字，那么重新给非 mut 的变量赋值会导致编译错误

使用 let 声明的同名新变量，也是不可变的，但是**类型可以与之前不同**。

```rust
fn main() {
    let name = String::from("hello variables shadow");  // 类型 字符串
    let name = name.len();  // 类型 usize(整数)
    println!("length is {}", name);  // length is 22
}
```

### 4. 数据类型

标量和复合类型

- Rust 是静态编译语言，在编译时必须知道所有变量的类型
- 基于使用的值，编译器通常能够推断出它的具体类型
- 如果可能的类型比较多，就必须添加类型的标注，否则编译会报错

#### 标量类型

一个标量类型代表一个单个的值，Rust 有四个主要的标量类型：

- 整数类型：i8, u8, i16, u16, i32, u32, i64, u64, i128, u128, isize, usize
  - 无符号整数类型以 u 开头
  - 有符号整数类型以 i 开头
  - isize 和 usize 类型的位数由程序运行的计算机的架构所决定
  - 允许使用 `_` 做为分隔符以方便读数
- 浮点类型：f32，32 位，单精度；f64 64 位，双精度
- 布尔类型：bool，有两个值 true 和 false，一个字节大小
- 字符类型：char，字面值使用单引号，占用 4 字节大小
  - 是 Unicode 标量值，可以表示比 ASCII 多得多的字符内容：拼音、中日韩文、零
    长度空白字符、emoji 表情等。

#### 复合类型

##### 元组

元组是一个将多个其他类型的值组合进一个复合类型的主要方式。元组长度固定：一旦声明，其长度不会增大或缩小。

使用包含在圆括号中的逗号分隔的值列表来创建一个元组。元组中的每一个位置都有一个类型，而且这些不同值的类型也不必是相同的。

```rust
fn main() {
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    let t = (1, 2, 3, 4, 6.6);  // 在小括号里，将值用逗号分开
	// 获取 Tuple 的元素值
    // 模式匹配
    let (x1, x2, x3, x4, x5) = t;
    println!("{}, {}, {}, {}, {}", x1, x2, x3, x4, x5);
    // 在 tuple 变量使用点标记法，后接元素的索引号
    println!("{}, {}, {}, {}, {}", t.0, t.1, t.2, t.3, t.4);
}
```

没有任何值的元组 () 是一种特殊的类型，只有一个值，也写成 ()。该类型被称为**单元类型**，而该值被称为**单元值**。如果表达式不返回任何其他值，则会隐式返回单元值。

##### 数组

数组也可以将多个值放在一个类型里，但每个元素的类型必须相同，长度也是固定的。

将数组的值写成在方括号内，用逗号分隔。

```rust
fn main() {
    let month = ["Jan.", "Feb.", "Mar.", "Apr.", "May.", "Jun.", "Jul.",
        "Aug.", "Sep.", "Oct.", "Nov.", "Dec."];
    let a: [i32; 5] = [1, 2, 3, 4, 5];
    let b = [3, 5];  // let a = [3, 3, 3, 3, 3];
    println!("{}, {}", month[0], month[1]);
}
```

### 5. 函数

声明函数使用 fn 关键字，Rust 不关心函数定义于何处，只要定义了就行。

**参数**：在函数签名里，必须声明每个参数的类型。

```rust
fn main() {
    println!("main");
    function1(1, 2);
}

fn function1(x: i32, y: i32) {
    println!("function1");
    println!("x is {}, y is {}", x, y);
}
```

**函数体中的语句与表达式**

函数体由一系列的语句和一个可选的结尾表达式构成。

Rust 是一个基于表达式的语言。

**语句**（*Statements*）是执行一些操作但不返回值的指令，所以不可以使用 let 将一个语句赋给一个变量。

**表达式**（*Expressions*）计算并产生一个值，表达式可以是语句的一部分。

表达式的结尾没有分号。如果在表达式的结尾加上分号，它就变成了语句，而语句不会返回值。

```rust
fn main() {
    let x = 5;  // 语句
    // let x = (let y = 6);  // error, 即 rust 不允许 x = y = 6
    let y = {
        let x = 3;
        x + 3
    };
    // 函数调用是一个表达式。宏调用是一个表达式。用大括号创建的一个新的块作用域也是一个表达式。
    println!("The value of y is {}", y);  // 6
}
```

**函数的返回值**

在 -> 符号后边声明函数返回值的类型，但是不可以为返回值命名。

在 Rust 里面，函数的返回值等同于函数体最后一个表达式的值。使用 `return` 关键字和指定值，可从函数中提前返回；但大部分函数隐式的返回最后的表达式。

```rust
fn main() {
    let x = function1();
    let y = function2(x);
    println!("The value of x is {}, y is {}", x, y);
}

fn function1() -> i32 {
    10
}

fn function2(x: i32) -> i32 {
    x + 5  // 加上 ';', 编译会报错
}
```

### 6. 控制流

#### if 表达式

if 表达式允许根据条件来执行不同的代码分支，**这个条件必须是 bool 值**。可选的，在后边可以加上一个 else 表达式或 else if 表达式。

```rust
fn main() {
    let number = 6;
    if number % 4 == 0 {
        println!("number is divisible by 4");
    }
    else if number % 3 == 0 {
        println!("number is divisible by 3");
    }
    else if number % 2 == 0 {
        println!("number is divisible by 2");
    }
    else {
        println!("number is not divisible by 4, 3 or 2");
    }
}
```

因为 if 是一个表达式，所以可以在 let 语句的右侧使用它。

```rust
fn main() {
    let condition = true;
    let number = if condition { 5 } else { 6 };
    println!("number val is {}", number);  // number val is 5
}
```

注意：

- 代码块的值是其最后一个表达式的值
- if 表达式返回的类型必须是一样的

#### loop 表达式

反复的执行一块代码，`break` 关键字停止循环。`continue` 关键字告诉程序跳过这个循环迭代中的任何剩余代码，并转到下一个迭代。

如果存在嵌套循环，`break` 和 `continue` 应用于此时最内层的循环。可以选择在一个循环上指定一个**循环标签**（*loop label*），然后将标签与 `break` 或 `continue` 一起使用，使这些关键字应用于已标记的循环而不是最内层的循环。

```rust
fn main() {
    let mut count = 0;
    'counting_up: loop {   // 该 loop 循环有标签
        println!("count = {}", count);
        let mut remaining = 10;

        loop {
            println!("remaining = {}", remaining);
            if remaining == 9 {
                break;  // 中止内层的 loop
            }
            if count == 2 {
                break 'counting_up;  // 中止最外层的 loop
            }
            remaining -= 1;
        }
        count += 1;
    }
    println!("End count = {}", count);
}
```

从循环返回。

```rust
fn main() {
    let mut counter = 0;
    let result = loop {
        counter += 1;
        if counter == 10 {
            break counter * 2;
        }
    };
    println!("The result is {}", result);  // The result is 20
}
```

#### while 条件循环

```rust
fn main() {
    let mut number = 5;
    while number != 0 {
        println!("{}", number);
        number -= 1;
    }
    println!("over");
}
```

#### for 遍历集合

```rust
fn main() {
    let a = [1, 2, 3, 4, 5];
    for elem in a.iter() {  // elem: &i32
        println!("{}", elem);
    }
}
```

##### Range

标准库提供的类型，用来生成从一个数字开始到另一个数字之前结束的所有数字的序列。

rev 方法可以反转 range。

```rust
fn main() {
    for number in (1..4).rev() {
        println!("{}", number);  // 3  2  1
    }
}
```

### 7. 所有权

所有权（系统）是 Rust 最为与众不同的特性，它让 Rust 无需垃圾回收（garbage collector）即可保障内存安全。

Rust 采取了一个不同的策略：内存在拥有它的变量离开作用域后就被自动释放。

#### 所有权规则

- Rust 中的每一个值都有一个被称为其**所有者**（*owner*）的变量
- 值在任一时刻有且只有一个所有者
- 当所有者（变量）离开作用域，这个值将被丢弃（调用 drop 函数）

```rust
{
    let s1 = "hello";  // 字符串字面值，即被硬编码进程序里的字符串值，不可变
    let s2 = s1;  // s1 还是有效的
    
    let mut s3 = String::from("hello");  // String，这个类型管理被分配到堆上的数据，可变
    s3.push_str(", world");  // "hello, world"
}  // s3 申请的内存会被释放
```

#### 数据交互方式

- 移动

栈上的数据就是简单的拷贝；而堆上的数据则采用移动的方式。

```rust
let s1 = String::from("test");
let s2 = s1;  // 此时 s1 已失效
println!("{}, {}", s1, s2);  // error s1
```

Rust 永远也不会自动创建数据的 “深拷贝”。因此，任何**自动**的复制可以被认为对运行时性能影响较小。

- 克隆

当需要深度复制 `String` 中堆上的数据时，使用 clone 方法。

```rust
let s1 = String::from("test");
let s2 = s1.clone();
println!("{}, {}", s1, s2);  // OK
```

##### 扩展

Rust 有一个叫做 Copy trait 的特殊注解，可以用在类似整型这样的存储在栈上的类型上。如果一个类型实现了 Copy trait，那么一个旧的变量在将其赋值给其他变量后仍然可用。Rust 不允许自身或其任何部分实现了 Drop trait 的类型使用 Copy trait。如果对其值离开作用域时需要特殊处理的类型使用 Copy 注解，将会出现一个编译时错误。(trait 详解在后面)

任何一组简单标量值的组合都可以实现 Copy，任何不需要分配内存或某种形式资源的类型都可以实现 Copy 。如下是一些 Copy 的类型：

- 所有整数类型
- 布尔类型
- 所有浮点数类型
- 字符类型
- 元组，当且仅当其包含的类型也都实现 `Copy` 的时候。比如，`(i32, i32)` 实现了 `Copy`，但 `(i32, String)` 就没有。

#### 所有权与函数

将值传递给函数在语义上与给变量赋值相似。向函数传递值可能会移动或者复制，就像赋值语句一样。

```rust
fn main() {
    let s = String::from("hello");
    takes_ownership(s);  // s 的值移动到函数里
    // s 失效
    
    let x = 5;
    makes_copy(x); 
    // x 任然有效
}

fn takes_ownership(some: String) {
    println!("{}", some);
}  // some 失效，调用 'drop' 方法，释放内存

fn makes_copy(some: i32) {
    println!("{}", some);
}  // 无任何操作
```

#### 所有权与返回值

返回值也可以转移所有权。

```rust
fn main() {
    let s1 = gives_ownership();  // gives_ownership 将返回值的所有权移给 s1

    let s2 = String::from("hello");
    let s3 = takes_and_gives_back(s2);  // s2 被移动到 takes_and_gives_back 中,
                                        // 返回值移给 s3
} // s1, s3 会释放内存，s2 已被移走，无操作

fn gives_ownership() -> String {
    let some_string = String::from("hello");
    some_string  // 返回 some_string 并移出给调用的函数
}  // 无内存的释放

fn takes_and_gives_back(a_string: String) -> String {
    a_string  // 返回 a_string 并移出给调用的函数
}  // 无内存的释放

fn calculate_length(s: String) -> (String, usize) {
    let length = s.len();
    (s, length)  // 使用元组来返回多个值
}
```

#### 总结

变量的所有权总是遵循相同的模式：将值赋给另一个变量时移动它。当持有堆中数据值的变量离开作用域时，其值将通过 `drop` 被清理掉，除非数据被移动为另一个变量所有。

### 8. 引用与借用

在上面的代码中有这样一个问题：必须将 `String` 返回给调用函数，以便在调用 `calculate_length` 后仍能使用 `String`，因为 `String` 被移动到了 `calculate_length` 内。

```rust
fn main() {
    let s1 = String::from("hello");
    let len = calculate_length(&s1);
}

fn calculate_length(s: &String) -> usize {
    s.len()
}  // 当引用停止使用时并不丢弃它指向的数据，因为没有所有权。
```

& 符号就是**引用**，它们允许使用值但不获取其所有权；将创建一个引用的行为称为**借用**。

正如变量默认是不可变的，引用也一样。（默认）不允许修改引用的值。

![img](.\Photo\rust_01.png)

#### 可变引用

可变引用的限制：在同一时间只能有一个对某一特定数据的可变引用。这样做的好处是在编译时防止数据竞争。

以下三种行为下会发生数据竞争：

- 两个或多个指针同时访问同一个数据
- 至少有一个指针用于写入数据
- 没有使用任何机制来同步对数据的访问

```rust
fn main() {
    let mut s = String::from("hello");
    change(&mut s);
    
    let r1 = &mut s;
	let r2 = &mut s;  // error
}

fn change(some_string: &mut String) {
    some_string.push_str(", world");
}
```

**不可以同时拥有一个可变引用和一个不变的引用**。多个不变的引用是可以的。

```rust
let mut s = String::from("hello");

let r1 = &s; // ok
let r2 = &s; // ok
let r3 = &mut s; // error
```

注意一个引用的作用域从声明的地方开始一直持续到最后一次使用为止。

```rust
let mut s = String::from("hello");

let r1 = &s; // 没问题
let r2 = &s; // 没问题
println!("{} and {}", r1, r2);
// 此位置之后 r1 和 r2 不再使用

let r3 = &mut s; // 没问题
println!("{}", r3);
```

编译器在作用域结束之前判断不再使用的引用的能力被称为非词法作用域生命周期（Non-Lexical Lifetimes，简称NLL）。

#### 悬空引用

悬空指针：一个指针引用了内存中的某个地址，而这块内存可能已经释放并分配给其它人使用了。

在 Rust 里，编译器可保证引用永远都不是悬空引用：如果你引用了某些数据，编译器将保证在引用离开作用域之前数据不会离开作用域。

```rust
fn main() {
    let r = dangle();
}

fn dangle() -> &String {  // error: missing lifetime specifier
    let s = String::from("hello");
    &s
}
```

编译失败

#### 引用的规则

在任何给定的时刻，只能满足下列条件之一：

- 在任意给定时间，**要么**只能有一个可变引用，**要么**只能有多个不可变引用
- 引用必须总是有效的

### 9. Slice 类型

另一个没有所有权的数据类型是 slice。slice 允许引用集合中一段连续的元素序列，而不用引用整个集合。

#### 字符串 Slice

**字符串 slice**是 `String` 中一部分值的引用。“字符串 slice” 的类型声明写作 `&str`。

```rust
let s = String::from("test for string");
let test = &s[0..4];  // &s[..4]
let string = &s[9..15];  // &s[9..]
let s2 = &s[..];  // &s[0..15]
```

可以使用一个由中括号中的 `[starting_index..ending_index]` 指定的 range 创建一个 slice，其中 `starting_index` 是 slice 的第一个位置，`ending_index` 则是 slice 最后一个位置的后一个值。编译器会确保指向 String 的引用持续有效。

```rust
// fn first_world(s: &str) -> &str {  // slice 作为参数
fn first_world(s: &String) -> &str {
    let bytes = s.as_bytes();
    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];
        }
    }
    &s[..];
}
```

字符串字面值就是 slice。

#### 其他类型的 Slice

```rust
let a = [1, 2, 3, 4, 5];
let slice = &a[1..3];
```

