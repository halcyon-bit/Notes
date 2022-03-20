# 时间函数

## 一、clock

通过 clock 拿到程序执行时处理器所使用的时钟数来计时：

```c
clock_t clock(void);
// clock_t == long
```

该函数返回程序执行起（一般为程序的开头），处理器时钟所使用的时间。也可获取 CPU 所使用的秒数，除以 CLOCKS_PER_SEC 即可。

```c
void main()
{
    clock_t start_t = clock();
    printf("%d 个时钟\n", start_t);
    for (int i = 0; i < 100000000; i++) { }
    clock_t end_t = clock();
    printf("%d 个时钟\n", end_t);
    printf("循环的秒数：%f\n", ((double)(end_t - start_t) / CLOCKS_PER_SEC));
}
```

## 二、time

```c
time_t time(time_t* time);
// time_t == int64_t
```

该函数返回系统的当前时间，返回的是自1970年1月1日以来所经过的秒数。参数一般传空即可。

```c
void main() 
{
    // 获取当前时间戳，单位为秒
    time_t now = time(NULL);
    printf("%d s\n", now);
}
```

### 获取当前时间戳

```c
void main()
{
    // 获取当前时间戳，单位为秒
    struct timeval t;
    gettimeofday(&t, NULL);
    printf("%d s\n", t.tv_sec);
    // 获取当前时间戳，单位为毫秒
    printf("%d ms\n", (t.tv_sec * 1000 + t.tv_usec / 1000));
}
```

可以使用 ctime 显示当前时间，该函数返回一个表示当地时间的字符串指针：

```c
char* ctime(const time_t* time);
// day month year hours:minutes:seconds year\n\0。
```

示例代码如下：

```c
void main() 
{
    time_t now = time(NULL);
    char* dt = ctime(&now); 
    printf("cur time is %s\n", dt);  // 输出：Tue Sep 22 22:01:40 2020
}
```

可以使用 tm 结构自定义显示当前时间的格式：

```c
struct tm* localtime(const time_t* time);
```

将日历时间转换为本地时间，从 1970 年起始的时间戳转换为 1900 年起始的时间数据结构。

另一个类似的函数是 gmtime 函数：

```c
struct tm* gmtime(const time_t* time);
```

只是该函数返回的是 UTC 时间，协调世界时（UTC）也被称为格林尼治标准时间（GMT）。

tm 结构如下：

```c
struct tm
{
    int tm_sec;   // 秒，正常范围从 0 到 59，但允许至 61  
    int tm_min;   // 分，范围从 0 到 59  
    int tm_hour;  // 小时，范围从 0 到 23
    int tm_mday;  // 一月中的第几天，范围从 1 到 31
    int tm_mon;   // 月，范围从 0 到 11 
    int tm_year;  // 自 1900 年起的年数 
    int tm_wday;  // 一周中的第几天，范围从 0 到 6，从星期日算起 
    int tm_yday;  // 一年中的第几天，范围从 0 到 365，从 1 月 1 日算起 
    int tm_isdst; // 夏令时
};
```

tm_sec 在 C89 的范围是[0-61]，在 C99 更正为[0-60]。通常范围是[0-59]，貌似有些系统会出现 60 秒的跳跃。

tm_mon 是从零开始的，所以一月份为0，十二月份为11。

tm_year 是从1900年开始计算，所以显示年份的时候需要加上1900。

```c
void main() 
{
    time_t rawtime = time(NULL);
    struct tm* ptminfo = localtime(&rawtime);
    printf("cur time is: %02d-%02d-%02d %02d:%02d:%02d\n", ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday, ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
    ptminfo = gmtime(&rawtime);
    printf("cur time is: %02d-%02d-%02d %02d:%02d:%02d\n", ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday, ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
}
// 输出：cur time is: 2020-09-23 21:27:37   cur time is: 2020-09-23 13:27:37
```

可以通过 asctime 显示 tm 结构的时间：

```c
char* asctime(const struct tm* time);
```

和 ctime 类似，返回的都是一个固定时间格式的字符串，只是传入的参数不同。

```c
void main() 
{    
    time_t rawtime = time(NULL);
    struct tm* info = localtime(&rawtime);
    printf("正常 日期和时间：%s\n", asctime(info));  // 正常 日期和时间：Wed Sep 23 21:47:44 2020
    info = gmtime(&rawtime);
    printf("UTC 日期和时间：%s\n", asctime(info));  // UTC 日期和时间：Wed Sep 23 13:47:44 2020
}
```

也可以使用 strftime() 函数，该函数可用于格式化日期和时间为指定的格式，如果产生的 C 字符串小于 size 个字符（包括空结束字符），则会返回复制到 str 中的字符总数（不包括空结束字符），否则返回零。

```c
size_t strftime(
    char* str, // 指向目标数组的指针，用来复制产生的C字符串
    size_t maxsize, // 最多传出字符数量
    const char* format, // 格式化方式
    const struct tm* timeptr // tm指针
);
```

format 格式如下：

```
%a 星期几的缩写
%A 星期几的全称
%b 月份的缩写
%B 月份的全称
%c 标准的日期的时间串
%C 年份的前两位数字
%d 十进制表示的每月的第几天（值从1到31）
%D 月/天/年
%e 在两字符域中，十进制表示的每月的第几天
%F 年-月-日
%g 年份的后两位数字，使用基于周的年
%G 年份，使用基于周的年
%h 简写的月份名
%H 24小时制的小时（值从0到23）
%I 12小时制的小时（值从1到12）
%j 十进制表示的每年的第几天（值从1到366）
%m 十进制表示的月份（值从1到12）
%M 十时制表示的分钟数（值从0到59）
%n 换行符
%p 本地的AM或PM的等价显示
%r 12小时的时间
%R 显示小时和分钟：hh:mm
%S 十进制的秒数（值从0到61）
%t 水平制表符
%T 显示时分秒：hh:mm:ss
%u 每周的第几天，星期一为第一天 （值从1到7，星期一为1）
%U 第年的第几周，把星期日作为第一天（值从0到53）
%V 每年的第几周，使用基于周的年
%w 十进制表示的星期几（值从0到6，星期天为0）
%W 每年的第几周，把星期一做为第一天（值从0到53）
%x 标准的日期串
%X 标准的时间串
%y 不带世纪的十进制年份（值从0到99）
%Y 带世纪部分的十制年份
%Z 时区名称，如果不能得到时区名称则返回空字符。
%% 一个%符号
```

示例代码如下：

```c
void main() 
{
    time_t rawtime = time(NULL);
    char buf[256];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&rawtime));
    printf("%s", buf);
}
```