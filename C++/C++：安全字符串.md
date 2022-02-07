# 安全字符串（window)

## 一、strsafe 功能的优点

1. 始终向函数提供目标缓冲区的大小，以确保函数不会写入缓冲区的末尾。
2. 即使操作截断预期结果，也保证缓冲区以空值终止。
3. 所有函数都返回一个 HRESULT 值，只有一个可能的成功代码 (S_OK)。
4. 每个函数都有相应的字符数("cch")或字节数("cb")版本。
5. 大多数功能都具有扩展("Ex")版本，可用于高级功能。

## 二、字符计数功能

以下函数使用字符计数而不是字节计数。

| Function                            | Replaces                                                     |
| ----------------------------------- | ------------------------------------------------------------ |
| StringCchCat StringCchCatEx         | strcat, wcscat, _tcsat lstrcat StrCat StrCatBuff             |
| StringCchCatN StringCchCatNEx       | strncatStrNCat                                               |
| StringCchCopy StringCchCopyEx       | strcpy, wcscpy, _tcscpylstrcpyStrCpy                         |
| StringCchCopyN StringCchCopyNEx     | strncpy, wcsncpy, _tcsncpy                                   |
| StringCchGets StringCchGetsEx       | gets, _getws, _getts                                         |
| StringCchPrintf StringCchPrintfEx   | sprintf, swprintf, _stprintf wsprintf wnsprintf _snprintf, _snwprintf, _sntprintf |
| StringCchVPrintf StringCchVPrintfEx | vsprintf, vswprintf, _vstprintf vsnprintf, _vsnwprintf, _vsntprintf wvsprintf wvnsprintf |
| StringCchLength                     | strlen, wcslen, _tcslen                                      |

## 三、字节计数功能

以下函数使用字节计数而不是字符计数

| Function                          | Replaces                                                     |
| --------------------------------- | ------------------------------------------------------------ |
| StringCbCat StringCbCatEx         | strcat, wcscat, _tcsat lstrcat StrCat StrCatBuff             |
| StringCbCatN StringCbCatNEx       | strncat StrNCat                                              |
| StringCbCopy StringCbCopyEx       | strcpy, wcscpy, _tcscpy lstrcpy StrCpy                       |
| StringCbCopyN StringCbCopyNEx     | strncpy, wcsncpy, _tcsncpy                                   |
| StringCbGets StringCbGetsEx       | gets, _getws, _getts                                         |
| StringCbPrintf StringCbPrintfEx   | sprintf, swprintf, _stprintf wsprintf wnsprintf _snprintf, _snwprintf, _sntprintf |
| StringCbVPrintf StringCbVPrintfEx | vsprintf, vswprintf, _vstprintf vsnprintf, _vsnwprintf, _vsntprintf wvsprintf wvnsprintf |
| StringCbLength                    | strlen, wcslen, _tcslen                                      |

## 四、使用 strsafe.h

1. 头文件

```c++
#include <strsafe.h>
```

2. 在文件中包含 strsafe.h 时，将用 strsafe.h 函数替换旧函数。尝试使用旧函数将导致编译错误，需使用较新的函数。如果要避免此错误，请在包含 strsafe.h 之前包含以下语句

```c++
#define STRSAFE_NO_DEPRECATE
```

要仅允许**字符计数**功能，请在包含 strsafe.h 之前包含以下语句。

```c++
#define STRSAFE_NO_CB_FUNCTIONS
```

要仅允许**字节计数**功能，请在包含 strsafe.h 之前包含以下语句。

```c++
#define STRSAFE_NO_CCH_FUNCTIONS
```

注：可以单独定义 STRSAFE_NO_CB_FUNCTIONS 或 STRSAFE_NO_CCH_FUNCTIONS，但不能同时定义两者。

3. 某些 strsafe 函数具有区域设置感知版本。默认情况下，标头不会声明这些功能。要启用这些声明，请在包含 strsafe.h 之前包含以下宏语句。

```c++
#define STRSAFE_LOCALE_FUNCTIONS
```

支持的最大字符串长度为2,147,483,647（STRSAFE_MAX_CCH）个字符，ANSI 或 Unicode。

## 五、参考

[About Strsafe.h](https://docs.microsoft.com/zh-cn/windows/desktop/menurc/strsafe-ovw)