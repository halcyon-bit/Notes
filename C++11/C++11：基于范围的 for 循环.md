# 基于范围的 for 循环

## 一、C++11 前 for 循环的用法

```c++
#include <vector>
#include <iostream>
#include <algorithm>

void func(int val)
{
    std::cout << val << " ";
}

int main(int argc, char* argv[])
{
    std::vector<int> vec{ 1, 2, 3, 4, 5, 6 };
    for (std::vector<int>::iterator iter = vec.begin(); iter != vec.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;

    // for_each 方法
    std::for_each(vec.begin(), vec.end(), func);
    return 0;
}
```

std::for_each 与普通的 for 循环相比，不需要关注迭代器，只需要关心容器中的元素类型即可。不过都需要显示给出容器的开头和结尾。

## 二、C++11 的 for 循环

C++11 中改善了这种遍历方式，不再需要给出容器的两端，循环会根据容器的范围自动展开。

```c++
#include <vector>
#include <iostream>

// 语法：
// for (element : container) {
//    statement;
// }

int main()
{
    std::vector<int> vec{ 1, 2, 3, 4, 5, 6 };
    for (int n : vec) {
        std::cout << n << " ";
    }
    return 0;
}

// 编译器会转换为：
// for (auto _begin=vec.begin(), _end=vec.end(); _begin!=_end; ++_begin) {
//	  n = *_begin;
//	  statement;
// }
```

基于范围的 for 循环，对于冒号前面的局部变量声明只要求能够支持容器类型的隐式转换。

```c++
std::vector<int> vec;
for (char c : vec)  // int 会被隐式转换为 char
  // ...
```

以上都是以只读方式来遍历容器的，如果需要改变容器中的值，需要加上引用，如果是只希望遍历而不是修改，可以使用 `const auto&` 来定义 n 的类型，这样对于复制负担比较大的容器元素也可以无耗的进行遍历。

## 三、使用细节

### 1. 推导类型

使用范围的 for 循环和普通的 for 循环的区别：

```c++
#include <map>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    std::map<int, std::string> test = { {1, "1"}, {2, "2"}, {3, "3"} };
    // 普通 for 循环
    for (auto iter = test.begin(); iter != test.end(); ++iter) {
        std::cout << iter->first << " : " << iter->second << std::endl;
    }
    // 基于范围的 for 循环
    for (auto& val : test) {
        std::cout << val.first << " : " << val.second << std::endl;
    }
}
```

(1) 基于范围的 for 循环中 val 的类型是 std::pair。因此，对于 map 这种关联性容器而言，需要使用 val.first 或 val.second 来提取键值。

(2) auto 自动推导出的类型是容器中的 value_type，而不是迭代器。

### 2. 容器约束

(1) 对于 std::map 的遍历，基于范围的 for 循环中的 std::pair 引用，是不能够修改 first 的（std::set 类似）。

```c++
#include <map>
#include <set>

int main(int argc, char* argv[])
{
    std::map<int, int> testMap = { {1, 1}, {2, 2}, {3, 3} };
    std::set<int> testSet = { 1, 2, 3, 4, 5 };

    for (auto& val : testMap) {  // val: std::pair<const int, int>&
        ++val.first;
    }

    for (auto& val : testSet) {  // val: const int&
        ++val;  // error: 不能给常量赋值
    }
}
```

(2) 对于基于范围的 for 循环而言，冒号后面的表达式只会被执行一次。

```c++
#include <vector>
#include <iostream>

std::vector<int> getVec()
{
    std::cout << "getVec" << std::endl;
    return { 1, 2, 3, 4, 5, 6 };
}

int main()
{
    for (int n : getVec()) {
        std::cout << n << " ";
    }
    return 0;
}
```

程序执行结果：

![img](.\Photo\for.png)

## 四、总结

基于范围的 for 循环其实是普通 for 循环的语法糖，同普通的循环一样，在迭代时修改容器可能引起迭代器失效，导致一些意料之外的结果。

**切莫在 for 循环中修改容器的大小（添加和删除元素）**

