# std::set && std::multiset

## 一、结构

set 和 multiset 会根据特定的排序原则将元素排序。两者不同之处在于，multiset 允许元素重复，而 set 不允许重复。

![img](.\Photo\set_01.png)

只要是 assignable、copyable、comparable（根据某个排序准则）的型别T，都可以成为 set 或者 multiset 的元素。如果没有特别的排序原则，采用默认的 less，已 operator < 对元素进行比较，以便完成排序。

排序准则必须遵守以下原则：

-  必须是“反对称的”，对 operator< 而言，如果 x<y 为真，y<x 为假

-  必须是“可传递的”，对 operator< 而言，如果 x<y 为真，y<z 为真，那么 x<z 也为真

-  必须是“非自反的”，对 operator< 而言，x<x 永远为假

## 二、能力

和所有的标准关联容器类似，set 和 multiset 通常以平衡二叉树完成。

![img](.\Photo\set_02.png)

自动排序的主要优点在于使二叉树搜寻元素具有良好的性能，在其搜索函数算法具有对数复杂度。但是自动排序也造成了一个限制，不能直接改变元素值，因为这样会打乱原有的顺序，要改变元素的值，必须先删除旧元素，再插入新元素。所以 set 和 multiset 具有以下特点：

- 不提供直接用来存取元素的任何操作元素
- 通过迭代器进行元素的存取

