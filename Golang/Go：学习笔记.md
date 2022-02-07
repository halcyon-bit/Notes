# 基础

注意'{'，'}'所在的位置。

## 常量和变量

const 用来声明常量，常量的值不可以改变。var 用来声明变量

```go
package main

import "fmt"

func main() {  // ‘{’ 必须写在 main() 后面，否则编译报错
    var distance = 56000000
    var speed = 100800
    
    var (
        distance = 56000000
        speed = 100800
    )
    
    var distance, speed = 56000000, 100800
    const hoursPerDay, minutesPerHour = 24, 60
    
    distance = 56100000
    distance++  // 无 ++distance
}
```

## if 语句

```go
var test = "1"
if test == "1" {
    //...
} else if test == "2" {
    // ...
} else {   // else 必须写在 } 之后
    // ...
}
```

## switch 语句

```go
var test = "1"
switch {
case test == "1":
    // ...
case test == "2":
    // ...
    fallthrough
case test == "3":
    // ...
}
```

**fallthrough**：一个 case 执行完成后，switch 语句就结束了，不会执行下一个 case。fallthrough 会使该 case 执行完成后，继续执行下一个 case 语句。

## for 语句

```go
// 1
var count = 10
for count > 0 {
	// ...	
}

// 2
sum := 0
for i := 0; i < 10; i++ {
    sum += i
}

// 3
for {
    // 无线循环，可用 break 结束循环
}
```

