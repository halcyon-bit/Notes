# std::map && std::multimap

## 一、结构

`std::map` 和 `std::multimap` 将 key/value pair 当作元素，进行管理。根据 key 的排序准则将元素排序。multimap 允许重复元素，map 不允许。

元素要求：

- key/value 必须具有 assigned（可赋值）和 copyable（可复制）性质
- 对于排序而言，key 必须具是 comparable（可比较的）

![img](.\Photo\map_01.png)

## 二、能力

set、multiset、map、multimap 使用相同的内部结构，因此可以将 set 和 multiset 当成特殊的 map 和 multimap，只不过 set 的 value 和 key 指向的是同一元素。

map 和 multimap 会根据 key 对元素进行自动排序，所以根据 key 值搜寻某个元素具有良好的性能，但是如果根据 value 来搜寻效率就很低了。

同样，自动排序使得不能直接改变元素的 key，因为这样会破坏正确次序，要修改元素的 key，必须先移除拥有 key 的元素，然后插入拥有新的 key/value 的元素，从迭代器的观点来看，元素的 key 是常数，元素的 value 是可以直接修改的。

![img](.\Photo\map_02.png)

## 三、map 视为关联数组

通常，关联数组不提供直接存取，必须依靠迭代器，不过 map 是个例外，map 提供下标操作符，可以直接存取元素。不过下标操作符的索引不是元素整数位置，而是元素的 key。也就是说，索引可以是任何型别，而非局限的整数型别。

| 操作   | 效果                                                         |
| ------ | ------------------------------------------------------------ |
| m[key] | 返回一个 reference，指向键值为 key 的元素，如果该元素尚未存在，插入该元素。 |

和一般数组的区别不仅仅是索引型别，不能使用一个错误的索引。如果用某个 key 为索引，容器尚未存在该元素，会自动安插该元素，新元素由 default 构造，所有基本型别都提供 default 构造函数，以零为初始值。

关联数组的行为方式有优点，也有缺点：

**优点**是可以透过方便的接口向 map 安插新元素。

```C++
std::map<std::string, float> coll;  // empty map
coll["1"] = 7.7;
```

coll["1"] = 7.7; 处理如下：

1. 处理 coll["1"]
   - 如果存在键值为“1”的元素，以上式子返回该元素的 reference
   - 如果没有任何元素的键值为“1”，以上式子便为 map 自动安插一个新元素，键值 key 为“1”，value 通过 default 构造函数完成，并返回一个 reference 指向新元素。
2. 将 7.7 赋值给刚才返回的元素

这样，map 之内就包含了一个键值（key）为“1”的元素，其值（value）为7.7。

**缺点**就是可能不小心误置新元素。

```c++
std::map<std::string, float> coll;  // empty map
std::cout << coll["1"] << std::endl;
```

它会安插一个键值为“1”的元素，然后打印其值，缺省情况下是0。它并不会告诉错误，并且插入一个你可能不需要的元素。

这种插入方式比一般的 map 安插方式来得慢，因为在生成新元素的时候，需要使用 default 构造函数将新元素初始化，而这个值马上又会被真正的 value 覆盖。
