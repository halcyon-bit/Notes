# 基础

## 一、书写

当 Python 解释器读取源代码时，通常在文件开头写上：

```python
#!/user/bin/env python3
# _*_ coding: utf-8 _*_
```

## 二、链式比较

Python 支持链式比较，如 3 < 4 < 5 这种就叫链式比较，相当于 3 < 4 and 4 < 5。

## 三、循环

```python
while condition:
	# 条件成立时执行
else:
	# 条件不成立时执行
    
for i in add:
    # 条件成立时执行
else:
    # 条件不成立时执行
```

与 break 结合使用

```python
ss = "abcdefghijl,asdf"
for i in ss:
    if i == ',':
        #终止循环
        break
    print(i, end="")
else:
    print("执行 else 语句中的代码")
print("\n执行循环体外的代码")

# 输出：
# abcdefghijl
# 执行循环体外的代码
```

## 四、其他

```python
def str2hex(s):
	return ''.join([hex(ord(c)).replace('0x', '') for c in s])
	
def str_to_hex(s):
	return ' '.join([hex(ord(c)).replace('0x', '') for c in s])
 
def hex_to_str(s):
	return ''.join([chr(i) for i in [int(b, 16) for b in s.split(' ')]])

def str_to_bin(s):
	return ' '.join([bin(ord(c)).replace('0b', '') for c in s])

def bin_to_str(s):
	return ''.join([chr(i) for i in [int(b, 2) for b in s.split(' ')]])

```

