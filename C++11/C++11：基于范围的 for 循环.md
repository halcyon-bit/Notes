# 基于范围的 for 循环

## 一、for 循环的新用法

```c++
#include <iostream>
#include <vector>
#include <algorithm>

void funcFor(int val) {
	std::cout << val << " ";
}

int main() {
	// C++98/03
	std::vector<int> vec;
	for (std::vector<int>::iterator iter = vec.begin(); iter != vec.end(); ++iter) {
		std::cout << *iter << " ";
	}

	// for_each 方法
	std::for_each(vec.begin(), vec.end(), funcFor);
	return 0;
}
```

for_each 相比一般的 for 循环，只需关注容器中元素的类型，但是不管是哪一种遍历方法，必须显示的给出容器的开始（begin）和结束（end）。

C++11 中改善了这种遍历方式，不再需要给出容器的两端，循环会根据容器的范围自动展开，在循环中屏蔽了迭代器的遍历细节，直接抽取容器中的元素进行运算。

```c++
#include <iostream>
#include <vector>

// 语法：
// for (decl : coll) {
//	  statement;
// }

// 编译器会转换为：
// for (auto _pos = coll.begin(), _end=col.end(); _pos!=_end; ++_pos) {
//    decl = *_pos;
//	  statement;
// }

int main() {
	std::vector<int> vec;
	for (auto n : vec) {
		std::cout << n << " ";
	}
}
```

n 表示 vec 中的一个元素，同时，这种循环中，冒号前面的变量支持隐式转换的，因此在使用时需要注意。

```c++
std::vector<int> vec;
for (char c : vec)  // int 会被隐式转换为 char
```

在这种循环中，都是以只读方式来遍历容器的，如果需要改变容器中的值，需要加上引用，如果是只希望遍历而不是修改，可以使用 `const auto&` 来定义 n 的类型，这样对于复制负担比较大的容器元素也可以无耗的进行遍历。

```c++
std::vector<int> vec;
for (auto n : vec) {
	std::cout << n << " ";  // 输出，并把元素的值+1
}
```

## 二、使用细节

### 1. 推导类型

使用范围的 for 循环和普通的 for 循环的区别：

```c++
std::map<int, std::string> test = { {1, "1"}, {1, "2"}, {3, "3"} };
// 一般情况的 for 循环
for (auto iter = test.begin(); iter != test.end(); ++iter) {
	std::cout << iter->first << "->" << iter->second << std::endl;
}

// 基于范围的 for 循环
for (auto val : test) {
	std::cout << val.first << "->" << val.second << std::endl;
}
```

## 2. 容器约束

如果要改变某些容器元素的值，通过 auto& 可以解决大多数问题，但是某些特殊容器并不能达到预期的结果。比如希望在循环中对 set 的值进行修改，但是 set 内部元素的值是只读的——由 set 容器的特性决定的，因此 for 循环中的 auto& 会被推导为 const xx&。同样**基于范围的 map 遍历**，for 循环得到的 std::pair 引用，是**不能修改 key** 的。

```c++
#include <iostream>
#include <set>

int main() {
	std::set<int> s = { 1, 2, 3 };
	for (auto &val : s) {  // val : const int&
		std::cout << val++ << " ";  // error val不可更改
	}
	return 0;
}
```

### 3. 访问频率

测试 C++11 中循环对于容器的访问频率。

```c++
#include <iostream>
#include <vector>

std::vector<int> vec = { 1,2,3,4,5 };

std::vector<int>& func() {
	std::cout << "get range->" << std::endl;
	return vec;
}

int main() {
	for (auto val : func()) {
		std::cout << val << std::endl;
	}
	return 0;
}
```

程序执行结果：

![img](.\Photo\for_01.png)

从结果中可以看出，不论基于范围的 for 循环迭代了多少次，func() 只在第一次迭代之前被调用，在循环之前就确定好迭代的范围，而不是在每次迭代之前都去调用一次 end()。所以可以得出结论：**基于范围的 for 循环，冒号后面的表达式只会被执行一次**。

那么如果在基于范围的 for 循环中修改容器会出现什么情况：

```c++
#include <iostream>
#include <vector>

int main() {
	std::vector<int> vec = { 1,2,3,4,5 };
	for (auto val : vec) {
		std::cout << val << std::endl;
		vec.push_back(100);
	}
	return 0;
}
```

程序执行结果：

![img](.\Photo\for_02.png)

从结果看出，这并不是我们需要的结果，如果把 vector 换成 list，结果又不一样。

因为基于范围的 for 循环其实是普通 for 循环的语法糖，同普通的循环一样，在迭代时修改容器可能引起迭代器失效，导致一些意料之外的结果。由于我们这里是没法看到迭代器的，所以在基于范围的 for 循环中修改容器到底会造成什么样的影响非常困难。

**切莫在 for 循环中修改容器的大小(添加和删除元素)**