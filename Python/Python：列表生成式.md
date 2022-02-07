# 列表生成式

列表生成式即 List Comprehensions，是 Python 内置的非常简单却强大的可以用来创建 list 的生成式。

```python
[ x*x for x in range(1, 11) ] # 结果 [1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
```

写列表生成式时，把要生成的元素 x * x 放到前面，后面跟 for 循环，就可以把 list 创建出来。for 循环后面还可以加上 if 判断：

```python
[ x*x for x in range(1, 11) if x%2 == 0 ]
# 两层循环
[ m+n for m in 'ABC' for n in 'XYZ' ]
```

运用列表生成式，可以写出非常简洁的代码。例如，列出当前目录下的所有文件和目录名，可以通过一行代码实现：

```python
import os
[ d for d in os.listdir('.') ]
```

for 循环可以同时使用两个甚至多个变量，比如 dict 的 items() 可以同时迭代 key 和 value，因此，列表生成式也可以使用两个变量来生成 list：

```python
d = {'x': 'A', 'y': 'B', 'z': 'C'}
[ k+'='+v for k, v in d.items() ]
# 将字符串变成小写
L = ['Hello', 'World', 'IBM', "Apple"]
[s.lower() for s in L]
```