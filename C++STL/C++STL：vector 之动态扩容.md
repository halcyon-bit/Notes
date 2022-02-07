# vector 的动态扩容机制

## 一、高效使用 vector，避免频繁扩容

### 1. 扩容机制回顾

当向 vector 中插入元素时，如果元素有效个数 size 与空间容量 capacity 相等时，vector 内部会触发扩容机制：

1. 开辟新空间
2. 拷贝元素
3. 释放旧空间

注意：每次扩容的新空间不能太大，也不能太小，太大容易造成空间浪费，太小则会导致频繁扩容而影响程序效率。

### 2. 如何避免扩容导致效率低

如果要避免扩容而导致程序效率过低问题，其实非常简单：**如果在插入之前，可以预估 vector 存储元素的个数，提前将底层容量开辟好即可**。如果插入之前进行 reserve，只要空间给足，则插入时不会扩容，如果没有 reserve，则会边插入边扩容，效率极其低下。

```c++
int main()
{
    std::vector<int> foo;
    size_t sz = foo.capacity();
    // foo.reserve(100);  // 先将 vector 底层空间扩增到100个，然后插入
    std::cout << "making foo grow\n";
    for (int i = 0; i < 100; ++i) {
        foo.push_back(i);
        if (sz != foo.capacity()) {
            sz = foo.capacity();
            std::cout << "capacity changed: " << sz << std::endl;
        }
    }
}
// vs 运行结果
// making foo grow
// capacity changed: 1
// capacity changed: 2
// capacity changed: 3
// capacity changed: 4
// capacity changed: 6
// capacity changed: 9
// capacity changed: 13
// capacity changed: 19
// capacity changed: 28
// capacity changed: 42
// capacity changed: 63
// capacity changed: 94
// capacity changed: 141
    
// g++ 运行结果
// making foo grow
// capacity changed: 1
// capacity changed: 2
// capacity changed: 4
// capacity changed: 8
// capacity changed: 16
// capacity changed: 32
// capacity changed: 64
// capacity changed: 128
```

可以试试，将上述程序第5行屏蔽掉和放开分别运行下，看看会有什么不同。

从结果可以看出：vs 下是几乎是按照 1.5 倍方式扩容，linux 下是按照 2 倍方式扩容。

## 二、为什么选择以倍数方式扩容

### 1. 以等长个数进行扩容

等长个数方式扩容，新空间大小就是将**原空间大小扩增到 capacity+K 个空间(capacity 为旧空间大小)**。假设**需要向 vector 中插入100个元素，K为10，那么就需要扩容10次**；每次扩容都需要将旧空间元素搬移到新空间，第i次扩容拷贝的元素数量为：ki(第1次扩容，新空间大小为20，旧空间中有10个元素，需要搬移到新空间中；第2次扩容，新空间大小为30，旧空间中有20个元素，需要全部搬移到新空间中)，假设元素插入与元素搬移为1个单位操作，则n个元素 push_back() 的总操作次数为：

![img](.\Photo\vector_expand_01.jpg)

###  2. 以倍数方式进行扩容

假设有 n 个元素需要向 vector 中插入，倍增因子为 m，则完成 n 个元素向 vector 中 push_back 操作，需要扩容的次数为 $\log_mn$ 次。

如：以2倍方式扩容，当向 vector 中插入 1000 个元素时，需要扩容 $\log_{2}{1000}$ 向上取整即 10 次，第 i 次扩容将会有 $m^i$ 个元素搬移到新空间，n 次 push_back() 的总操作次数为：

![img](.\Photo\vector_expand_02.png)

**可以看到以倍数的方式扩容比以等长个数的扩容方式效率高**。

### 3. 为什么选择1.5倍或者2倍方式扩容，而不是3倍、4倍？

扩容原理为：申请新空间，拷贝元素，释放旧空间，**理想的分配方案是在第 N 次扩容时如果能复用之前 N-1 次释放的空间就太好了**，如果按照2倍方式扩容，第i次扩容空间大小如下：

1  2  4  8  16  32 ... $2^i$

可以看到，**每次扩容时，前面释放的空间都不能使用**。比如：第 4 次扩容时，前 2 次空间已经释放，第 3 次空间还没有释放(开辟新空间、拷贝元素、释放旧空间)，即前面释放的空间只有 3，假设第 3 次空间已经释放，才只有 7，而第四次需要 8 个空间，因此无法使用之前已释放的空间，但是按照小于2倍方式扩容，多次扩容之后就可以复用之前释放的空间了。如果倍数超过2倍(包含2倍)方式扩容会存在：

1. **空间浪费可能会比较高**，比如：扩容后申请了64个空间，但只存了33个元素，有接近一半的空间没有使用
2. **无法使用到前面已释放的内存**

因为 STL 标准并没有严格说明需要按何种方式进行扩容，因此不同的实现厂商都是按照自己的方式扩容，即：**linux 下以2倍的方式扩容，而 vs 下以1.5倍的方式扩容**。

## 三、vs 为什么选择1.5倍方式扩容

在 windows 中，**已释放的或未被申请的堆空间使用堆表进行管理**，最重要的堆表有两种：**空闲双向链表 FreeList**(以下简称空表)和**快速单向链表 Lookaside**(以下简称快表)。

### 1. 空表

**按照内存块大小的不同，空表总共被分为128条，**free[1]~free[127] 每个空表中管理的都是8的整数倍的堆内存块，逐次递增，将空闲堆块按照大小不同，链入不同的空表中，来方便堆管理、高效检索指定大小的堆空间。free[0] 管理大于 1024byte 小于 512K 的堆块，这些堆块按照大小在 free[0] 中依次排列下去。

![img](.\Photo\vector_expand_03.png)

### 2. 快表

快表是 windows 用来加速堆块分配而采用的一种堆表，称为快表是因为：**该单向链表中永远不会发生堆块合并**，快表也有128条，**与空表不同的是每条快表最多只有4个节点**。

![img](.\Photo\vector_expand_04.png)

经过反复的申请与释放操作后，堆区很可能变得"千疮百孔"，产生很多碎片，为了合理有效的利用内存，堆管理系统还要能够进行堆块合并。

**当堆管理系统发现两个空闲堆块批次相邻的时候，就会进行堆块合并操作**。

![img](.\Photo\vector_expand_05.png)

windows 中堆管理系统会时时对释放的堆块进行合并，因此：**vs 下的 vector 扩容机制选择使用1.5倍方式进行扩容，这样多次扩容之后，就可以使用之前已经释放的空间了**。

## 四、linux 为什么选择2倍方式扩容

linux 下主要使用 glibc 中 ptmalloc 来进行空间申请的。如果 malloc 的空间小于 128KB，其内部通过 brk() 来扩张，如果大于 128KB 且 arena 中没有足够的空间时，通过 mmap 将内存空间映射到进程地址空间。

**每次申请出来的小块内存，对应一个chunk**，根据作用不同，chunk(内存块的意思)总共分为4类：

**Allocated chunk**：即分配给用户且未释放的内存块

**Free chunk**：即用户已经释放的内存块

**Top chunk**：一个 arena 顶部的 chunk 叫做 Top chunk，它不属于 bin，当所有 bin 中都没有可用的 chunk 时，切割 Top chunk 来满足用户内存申请。

**Last Remainder chunk**：当需要分配一个较小的k字节的 chunk 但是 small bins 中找不到满足要求的，且 Last Remainder chunk 的大小N能满足要求，那么 Last Remainder chunk 将被切割。

![图片](.\Photo\vector_expand_06.jpg)

![img](.\Photo\vector_expand_07.jpg)

![img](.\Photo\vector_expand_08.jpg)

**free() 之后的内存不会立即返回给系统，而是被表示为一个 Free Chunk，Free Chunk 之间通过双向链表链接，被管理在 bin 中，目的就是为了提高下一次分配的速度**，当用户进行下一次分配时，会首先试图在 bins 管理的空闲 chunk 中挑选一块给用户，这样就避免了频繁的系统调用，降低内存开销。

bin 使用空闲链表的方式来管理空闲的 chunk，可分为：

**10个 fast bins**，存储在 fastbinsY 中

**10个 LIFO 的单链表，相邻空闲 chunk 不会被合并，主要用于提高小块内存分配和释放效率**

![img](.\Photo\vector_expand_09.jpg)

**1个 unsorted bin**，存储在 bin[1] 中。

大小超过 fast bins 阈值的 chunk 被释放时会添加到这里，目的复用最近释放的 chunk，从而消除寻找合适 bin 的时间开销，从而提高内存分配和释放的效率。

![img](.\Photo\vector_expand_10.jpg)

**62个 small bins**，存储在 bin[2] 到 bin[63]

小于 0x200(0x400-64位系统)Byte 的 chunk 叫做 small chunk，chunk 的大小从 16Byte 开始，每次递增8字节，small bin 是就存储这些小 chunk，**small bin 是62个双向循环链表**，与 fast bin 是不同的是：small bins 是 FILO 的，**且相邻空闲的 chunk 会被合并**。

chunk 大小：0x10\~0x1f0Byte 范围(0x20~0x3f0---64位系统)，相邻 bin 存放的 chunk 大小相差 0x8(0x10)Byte。

**63个 large bins**，存储在 bin[64] 到 bin[126]。

大于等于 0x200(0x400---64为系统)Byte 的 chunk 叫做 large chunk，存放在 large bins 中。**large bins 是63个双向循环链表，插入和删除可以发生在任意位置，相邻空闲 chunk 也会被合并**。chunk 大小比较复杂：

1. 前32个 bins：从 0x200Byte 开始每次加 0x40Byte
2. 接下来16个 bins：每次加 0x200Byte
3. 接下来8个 bins：每次加 0x1000Byte
4. 接下来4个 bins：每次加 0x8000Byte
5. 接下来2个 bins：每次加 0x4000Byte
6. 最后一个 bin：只有一个 chunk，大小和 large bins 剩余的大小相同

注意：**同一个 bin 中的 chunks 不是相同大小的，按大小降序排列，在分配取出时遵循 best-fit 原则，即取出满足大小最小的 chunk**。

以上为 **linux 中管理释放内存块的方式，即在对 chunk 进行管理时，也会进行合并**，既然会合并按道理讲应该按照1.5倍方式扩容才比较合适，多次申请释放之后可以利用之前被释放的空间，但为什么 SGI-STL 的 vector 选择2倍方式扩容？

猜想：**Linux 中引入伙伴系统**为内核提供了一种**用于分配一些连续的页**而建立的一种高效的分配策略，对固定分区和动态分区方式的折中，固定分区存在内部碎片，动态分区存在外部碎片，而且动态分区回收时的合并以及分配是的切分是比较耗时的。

伙伴系统是将整个内存区域构建成基本大小 basicSize 的1倍、2倍、4倍、8倍、16倍等，即要求**内存空间分区链均对应2的整次幂倍大小的空间**，整齐划一，有规律的而不是乱糟糟的。

在分配和释放空间时，可以通过 log2(request/basicSize) 向上取整的哈希算法快速找到对应内存块。

![图片](.\Photo\vector_expand_11.png)

![图片](.\Photo\vector_expand_12.jpg)

![图片](.\Photo\vector_expand_13.png)

![图片](.\Photo\vector_expand_14.png)

![图片](.\Photo\vector_expand_15.jpg)

![图片](.\Photo\vector_expand_16.png)

![图片](.\Photo\vector_expand_17.jpg)

![图片](.\Photo\vector_expand_18.jpg)

![图片](.\Photo\vector_expand_19.png)

通过伙伴系统管理空闲分区的了解，可以看到**在伙伴系统中的每条空闲分区链中挂的都是**2i**的页面大小，通过哈希思想进行空间分配与合并，非常高效**。估计可能是这个原因 SGI-STL 选择以2倍方式进行扩容。