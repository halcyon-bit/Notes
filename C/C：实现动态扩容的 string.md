# 动态扩容的 string

string 有以下功能：

- 创建字符串
- 删除字符串
- 尾部追加字符串
- 头部插入字符串
- 从尾部删除 N 个字符串
- 从头部删除 N 个字符串
- 裁剪字符串
- 获取字符串长度
- 获取完整字符串

## 1. string 结构体

首先声明一个 string 的结构体。

```c
struct c_string_t;
typedef struct c_string_t cstring;
```

string 的实现如下：

```c
static const size_t cstring_min_size = 32;  // 字符串的初始内存大小

struct c_string_t {
	char *str;  // 字符串
	size_t alloced;  // 字符串的内存空间大小
	size_t len;  // 字符串的长度
};
```

## 2. 创建字符串

```c
cstring* cstring_create(void) {
	cstring *cstr;
	cstr = (cstring*)calloc(1, sizeof(*cstr));
	if (cstr == NULL) {
		return NULL;
	}
	cstr->str = (char*)malloc(cstring_min_size);
	*cstr->str = '\0';
	// 初始分配内存大小是 32，之后每次以 2 倍大小扩容
	cstr->alloced = cstring_min_size;
	cstr->len = 0;

	return cstr;
}
```

## 3. 销毁字符串

```c
void cstring_destroy(cstring *cstr) {
	if (cstr != NULL) {
		free(cstr->str);
		free(cstr);
	}
}
```

## 4. 内部扩容

每次以 2 倍大小进行扩容

```c
// 扩展字符串的内存空间
static void cstring_ensure_space(cstring *cstr, size_t addLen) {
	if (cstr == NULL || addLen == 0) {
		return;
	}

	size_t targetLen = cstr->len + addLen + 1;
	if (cstr->alloced >= targetLen) {
		return;
	}

	while (cstr->alloced < targetLen) {
		cstr->alloced <<= 1;  // 每次以 2 倍大小扩容
		if (cstr->alloced == 0) {
			// 左移到最后可能会变为0，由于 alloced 是无符号型，减一则会变成 UINT_MAX
			--cstr->alloced;
		}
	}
	cstr->str = (char*)realloc(cstr->str, cstr->alloced);
}
```

## 5. 尾部追加

- 在尾部追加字符串

```c
void cstring_append_str(cstring *cstr, const char *str, const size_t len) {
	if (cstr == NULL || str == NULL || *str == '\0') {
		return;
	}

	size_t size = len == 0 ? strlen(str) : len;

	cstring_ensure_space(cstr, size);

	memmove(cstr->str + cstr->len, str, size);
	cstr->len += size;
	cstr->str[cstr->len] = '\0';
}
```

- 在尾部追加字符


```c
void cstring_append_char(cstring *cstr, const char c) {
	if (cstr == NULL) {
		return;
	}

	cstring_ensure_space(cstr, 1);

	cstr->str[cstr->len] = c;
	++cstr->len;
	cstr->str[cstr->len] = '\0';
}
```

- 在尾部追加整数


```c
void cstring_append_int(cstring *cstr, const int val) {
	char str[32];

	if (cstr == NULL) {
		return;
	}
	snprintf(str, sizeof(str), "%d", val);
	cstring_append_str(cstr, str, 0);
}
```

## 6. 头部插入

- 在头部插入字符串

```c
void cstring_front_str(cstring *cstr, const char *str, const size_t len) {
	if (cstr == NULL || str == NULL || *str == '\0') {
		return;
	}

	size_t size = len == 0 ? strlen(str) : len;

	cstring_ensure_space(cstr, size);

	memmove(cstr->str + size, cstr->str, cstr->len);
	memmove(cstr->str, str, size);
	cstr->len += size;
	cstr->str[cstr->len] = '\0';
}
```

- 在头部插入字符


```c
void cstring_front_char(cstring *cstr, const char c) {
	if (cstr == NULL) {
		return;
	}

	cstring_ensure_space(cstr, 1);

	memmove(cstr->str + 1, cstr->str, cstr->len);
	cstr->str[0] = c;
	++cstr->len;
	cstr->str[cstr->len] = '\0';
}
```

- 在头部插入整数


```c
void cstring_front_int(cstring *cstr, const int val) {
	char str[32];

	if (cstr == NULL) {
		return;
	}
	snprintf(str, sizeof(str), "%d", val);
	cstring_front_str(cstr, str, 0);
}
```

## 7. 清空字符串

```c
void cstring_clear(cstring *cstr) {
	if (cstr != NULL) {
		cstr->len = 0;
		cstr->str[cstr->len] = '\0';
	}
}
```

## 8. 裁剪字符串

```c
void cstring_truncate(cstring *cstr, const size_t len) {
	if (cstr == NULL || len >= cstr->len) {
		return;
	}

	cstr->len = len;
	cstr->str[cstr->len] = '\0';
}
```

## 9. 删除头部的 N 个字符

```c
void cstring_drop_begin(cstring *cstr, const size_t len) {
	if (cstr == NULL || len == 0) {
		return;
	}

	if (len >= cstr->len) {
		cstring_clear(cstr);
		return;
	}

	cstr->len -= len;
	/* +1 是拷贝最后的 '\0' */
	memmove(cstr->str, cstr->str + len, cstr->len + 1);
}
```

## 10. 删除尾部的 N 个字符

```c
void cstring_drop_end(cstring *cstr, const size_t len) {
	if (cstr == NULL || len == 0) {
		return;
	}

	if (len >= cstr->len) {
		cstring_clear(cstr);
		return;
	}

	cstr->len -= len;
	cstr->str[cstr->len] = '\0';
}
```

## 11. 获取字符串的长度

```c
size_t cstring_len(const cstring *cstr) {
	return cstr == NULL ? 0 : cstr->len;
}
```

## 12. 返回字符串指针

使用的是字符串的原始内存。

```c
const char* cstring_peek(const cstring *cstr) {
	return cstr == NULL ? NULL : cstr->str;
}
```

## 13. 返回字符串指针

重新分配一块内存，存储字符串，并返回。malloc 分配新内存，需要外部进行 free

```c
char* cstring_dump(const cstring *cstr, size_t *len) {
	char *str;

	if (cstr == NULL) {
		return NULL;
	}

	if (len != NULL) {
		*len = cstr->len;
	}

	str = (char*)malloc(cstr->len + 1);
	memcpy(str, cstr->str, cstr->len + 1);
	return str;
}
```

全部代码见文件（cstring.h, cstring.c, cstring_test.c)