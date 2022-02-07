# std::chrono

C++11 关于时间引入了 chrono 库，chrono 主要有三个点：

- `duration`
- `time_point`
- `clocks`

## 一、duration

`std::chrono::duration` 表示一段时间，常见的单位有s、ms等。

```c++
template <class _Rep, class _Period = ratio<1>>
class duration;
```

Rep 表示一种数值类型，用来表示 Period 的数量，比如 int、float、double，Period 是 ratio 类型，用来表示**用秒表示的时间单位**，比如 second。

```c++
template <intmax_t N, intmax_t D = 1> 
class ratio;
```

N 代表分子，D 代表分母，所以 ratio 表示一个分数，分母默认为1，分数值表示秒数。可以自定义 Period，比如 ratio<2, 1> 表示单位时间是2秒。

常用的 duration<Rep, Period> 已经定义好了，在 std::chrono::duration 下：

- ratio<3600, 1>：hours
- ratio<60, 1>：minutes
- ratio<1, 1>：seconds
- ratio<1, 1000>：microseconds
- ratio<1, 1000000>：microseconds
- ratio<1, 1000000000>：nanosecons

标准库还提供了 `duration_cast` 用于转换各种 duration。

```c++
void func()
{
    auto sec = std::chrono::seconds(10);
    auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(sec);
    std::cout << sec.count() << std::endl;  // 10s   
    std::cout << mill.count() << std::endl;  // 10000ms
}
```

## 二、time_point

`std::chrono::time_point` 表示一个具体时间点，如2020年5月10日10点10分10秒。

```c++
template <class _Clock, class _Duration = typename _Clock::duration>
class time_point;
```

使用方式：

```cpp
void func()
{
	auto now = std::chrono::high_resolution_clock::now();
	std::cout << now.time_since_epoch().count() << std::endl;
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::cout << "time: " << ctime(&t) << std::endl;
}
```

函数 `time_since_epoch()`，表示这个 time_point 距离元年也就是1970年1月1日所经过的 duration。

time_point 也有各种表示方式，类似于 duration，也提供了转换函数 `time_point_cast()`。

```c++
void func()
{
	auto now = std::chrono::high_resolution_clock::now();
	std::cout << now.time_since_epoch().count() << std::endl;  // ns
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	std::cout << seconds.time_since_epoch().count() << std::endl;  // s
}
```

## 三、clocks

时钟，chrono 里面提供了三种时钟：

- `steady_clock`
- `system_clock`
- `high_resolution_clock`

### 1. steady_clock

`std::steady_clock` 表示稳定的时钟，它只有一个函数，就是 now()，后一次调用 now() 肯定比上一次调用 now() 的返回值大，不受系统时间修改的影响。

源码如下：

```c++
struct steady_clock
{   // wraps QueryPerformanceCounter
    using rep = long long;
    using period = nano;
    using duration = nanoseconds;
    using time_point = chrono::time_point<steady_clock>;
    static constexpr bool is_steady = true;
    
    _NODISCARD static time_point now() noexcept
    {   // get current time
        const long long _Freq = _Query_perf_frequency(); // doesn't change after system boot
        const long long _Ctr  = _Query_perf_counter();
        static_assert(period::num == 1, "This assumes period::num == 1.");
        const long long _Whole = (_Ctr / _Freq) * period::den;
        const long long _Part  = (_Ctr % _Freq) * period::den / _Freq;
        return time_point(duration(_Whole + _Part));
    }
};
```

使用方式：

```c++
void func()
{
	auto begin = std::chrono::steady_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	auto end = std::chrono::steady_clock::now();
	auto spend = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	std::cout << "time: " << spend.count() << std::endl;
}
```

### 2. system_clock

system_clock 表示当前的系统时钟，可以用于获取当前时间。有三个函数：

```c++
now(); // 表示当前时间的 time_point
to_time_t();  // 将 time_point 转换成 time_t 秒
from_time_t();  // 将 time_t 转换成 time_point
```

使用方式：

```c++
void func()
{
	using std::chrono::system_clock;
	auto today = system_clock::now();
	std::time_t t = system_clock::to_time_t(today);
	std::cout << "today is: " << ctime(&t);
}
```

### 3. high_resolution_clock

high_resolution_clock 表示系统可用的最高精度的时钟，实际上就是 system_clock 或者 steady_clock 其中一种的定义，官方没有说明具体是哪个，不同系统可能不一样。

```c++
using high_resolution_clock = steady_clock;
```

## 四、计时器 timer

利用 high_resolution_clock 来实现一个类似于 boost.timer 的计时器，这样的 timer 在测试性能的时候经常用到。在程序日常开发的时候可以作为测试函数耗时。

```c++
#include <chrono>

using namespace std::chrono;

class Timer
{
public:
    Timer()
        : begin_(high_resolution_clock::now())
    {}

    /*
     * @brief   重置
     */
    void reset()
    {
        begin_ = high_resolution_clock::now();
    }

    /*
     * @brief   默认输出毫秒
     */
    template <typename Duration = milliseconds>
    int64_t elapsed() const
    {
        auto end = high_resolution_clock::now();
        return duration_cast<Duration>(end - begin_).count();
    }

    /*
     * @brief   微秒
     */
    int64_t elapsed_micro() const
    {
        return elapsed<microseconds>();
    }

    /*
     * @brief   纳秒
     */
    int64_t elapsed_nano() const
    {
        return elapsed<nanoseconds>();
    }

    /*
     * @brief   秒
     */
    int64_t elapsed_sconds() const
    {
        return elapsed<seconds>();
    }

    /*
     * @brief   分钟
     */
    int64_t elapsed_minutes() const
    {
        return elapsed<minutes>();
    }

    /*
     * @brief   小时
     */
    int64_t elapsed_hours() const
    {
        return elapsed<hours>();
    }

private:
    time_point<high_resolution_clock> begin_;
};

#include <iostream>
#include <thread>

void func()
{
    std::this_thread::sleep_for(seconds(10));
}

int main()
{
    Timer t;
    func();
    std::cout << t.elapsed() << std::endl;  // func 函数耗时毫秒
    std::cout << t.elapsed_micro() << std::endl;  //func 函数耗时微秒
    std::cout << t.elapsed_nano() << std::endl;  // func 函数耗时纳秒
    std::cout << t.elapsed_sconds() << std::endl;  // func 函数耗时秒
    std::cout << t.elapsed_minutes() << std::endl;  // func 函数耗时分
    std::cout << t.elapsed_hours() << std::endl;  // func 函数耗时小时
    return 0;
}
```

