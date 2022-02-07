# 装饰器

由于函数也是一个对象，而且函数对象可以被赋值给变量，通过变量也能调用该函数。函数对象有一个 \_\_name__ 属性，可以得到函数的名字。

```python
def now():
    print('2020-08-27')
```

假设要增强 now 函数的功能，比如，在函数调用前后自动打印日志，但又不希望修改 now 函数的定义，这种在代码运行期间动态增加功能的方式，称之为“装饰器”（Decorator）。本质上，decorator 是一个返回函数的高阶函数。

```python
def log(func):
    def wrapper(*args, **kw):
        print('call %s():' % func.__name__)
        return func(*args, **kw)
    return wrapper
```

观察上面的 log，因为它是一个 decorator，所以接受一个函数作为参数，并返回一个函数。借助 Python 的 @ 语法，把 decorator 置于函数的定义处：

```python
def log(func):
    # ...

@log
def now():
    print('2020-08-27')

now()  # call now():  2020-08-27
```

现在调用 now() 函数，不仅会运行 now() 函数，还会在运行 now() 函数前打印一行日志。把 @log 放到 now() 函数的定义处，相当于执行了语句：now = log(now)

由于 log() 是一个 decorator，返回一个函数，所以，原来的 now() 函数仍然存在，只是现在同名的 now 变量指向了新的函数，于是调用 now() 将执行新函数，即在 log() 函数中返回的 wrapper() 函数。

wrapper() 函数的参数定义是 (*args, **kw)，因此，wrapper() 函数可以接受任意参数的调用。如果 decorator 本身需要传入参数，那就需要编写一个返回 decorator 的高阶函数，写出来会更复杂。比如，要自定义 log 的文本：

```python
def log(text):
    def decorator(func):
        def wrapper(*args, **kw):
            print('%s %s():' % (text, func.__name__))
            return func(*args, **kw)
        return wrapper
    return decorator

@log("execute")
def now():
    print("2020-08-27")
    
now()  # execute now()   2020-08-27
now.__name__  # wrapper
```

和两层嵌套的 decorator 相比，3层嵌套是这样的：now = log('execute')(now)。

首先执行 log('execute')，返回的是 decorator 函数，再调用返回的函数，参数是 now 函数，返回值最终是 wrapper 函数。

以上两种 decorator 的定义都没有问题，但还差最后一步。因为函数有 \_\_name__ 等属性，但经过 decorator 装饰之后的函数，它们的 \_\_name__ 已经从原来的 'now' 变成了 'wrapper'。

因为返回的那个 wrapper() 函数名字就是 'wrapper'，所以，需要把原始函数的 \_\_name__ 等属性复制到 wrapper() 函数中，否则，有些依赖函数签名的代码执行就会出错。

不需要编写 wrapper.\_\_name__ = func.\_\_name__，Python 内置的 functools.wraps 就可以解决，所以，一个完整的 decorator 的写法如下：

```python
import functools
def log(text):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kw):
            print('%s %s():' %(text, func.__name__))
            return func(*args, **kw)
        return wrapper
    return decorator
```

