# 暴雪的 hash 算法

暴雪公司有一个经典的字符串的 hash 公式。

所谓 hash，一般是一个整数，通过某种算法，可以把一个字符串“压缩”成一个整数，这个数称为 hash，当然，无论如何，一个32位整数是无法对应回一个字符串的，但在程序中，两个字符串计算出的 hash 值相等的可能性非常小，在 MPQ 中的 hash 算法：

```c++
unsigned long HashString(char *pStr, unsigned long nHashType)
{
    unsigned char *key = (unsigned char*)pStr;
    unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
    int ch;
    while (*key != 0) {
        ch = toupper(*key++);
        seed1 = cryptTable[(nHashType << 8) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
    }
    return seed1;
}
```

Blizzard 的这个算法是非常高效的，被称为“One-Way Hash”。

通常构造一个哈希表（Hash Table），哈希表是一个大数组，这个数组的容量根据程序的要求来定义，例如 1024，每一个 Hash 值通过取模运算（mod）对应到数组中的一个位置，这样，只要比较这个字符串的哈希值对应的位置又没有被占用，就可以得到最后的结果了，想想这是什么速度？最快的 O(1)。

```c++
int GetHashTablePos(char *pString, SOMESTRUCTURE *pTable, int nTableSize)
{
    int nHash = HashString(pString), nHashPos = nHash % nTableSize;
    if (pTable[nHashPos].bExists && !strcmp(pTable[nHashPos].pString, pString))
		return nHashPos;
    else
        return -1;
}
```

但是存在一个很严重的问题：“假如两个字符串在哈希表中对应的位置相同怎么办？”，毕竟一个数组容量是有限的，这种可能性很大。解决该问题的方法很多，首先就是用“链表”，很多算法都可以转化成链表来解决，只要在哈希表的每个入口挂一个链表，保存所有对应的字符串就 OK 了。

然而 Blizzard 的程序员使用的方法则是更精妙的方法。基本原理就是：他们在哈希表中不是用一个哈希值而是用三个哈希值来校验字符串。

中国有句古话“再一再二不能再三再四”。假如说两个不同的字符串经过一个哈希算法得到的入口点一致有可能，但用三个不同的哈希算法算出的入口点都一致，那几乎可以肯定是不可能的事了，这个几率是1:18889465931478580854784，大概是10的22.3次方分之一。

Blizzard 使用的哈希表没有使用链表，而采用“顺延”的方式来解决问题，看看这个算法：

```c++
int GetHashTablePos(char *pString, MPQHASHTABLE *pTable, int nTableSize) 
{
    const int HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
    int nHash = HashString(pString, HASH_OFFSET);
    int nHashA = HashString(pString, HASH_A);
    int nHashB = HashString(pString, HASH_B);
    int nHashStart = nHash % nTableSize, nHashPos = nHashStart;
    while (pTable[nHashPos].bExits) {
        if (pTable[nHashPos].nHashA == nHashA && pTable[nHashPos].nHashB == nHashB)
            return nHashPos;
        else
            nHashPos = (nHashPos + 1) % nTableSize;
        if (nHashPos == nHashStart)
            break;
    }
    return -1;
```

1. 计算出字符串的三个哈希值（一个用来确定位置，另外两个用来校验）
2. 查看哈希表中的这个位置，假如为空，则该字符串不存在，返回
3. 假如存在，则检查其他两个哈希值是否也匹配，假如匹配，则表示找到了该字符串，返回
4. 否则移到下一个位置，判断是不是又回到了原来的位置，假如是，则没找到
5. 否则回到3

完整算法详见 StringHash.h

# 字符串 hash 函数

常用字符串哈希函数有 BKDRHash，APHash，DJBHash，JSHash，RSHash，SDBMHash，PJWHash，ELFHash 等等。

| Hash函数 | 数据1 | 数据2 | 数据3 | 数据4 | 数据1得分 | 数据2得分 | 数据3得分 | 数据4得分 | 平均分 |
| -------- | ----- | ----- | ----- | ----- | --------- | --------- | --------- | --------- | ------ |
| BKDRHash | 2     | 0     | 4774  | 481   | 96.55     | 100       | 90.95     | 82.05     | 92.64  |
| APHash   | 2     | 3     | 4754  | 493   | 96.55     | 88.46     | 100       | 51.28     | 86.28  |
| DJBHash  | 2     | 2     | 4975  | 474   | 96.55     | 92.31     | 0         | 100       | 83.43  |
| JSHash   | 1     | 4     | 4761  | 506   | 100       | 84.62     | 96.83     | 17.95     | 81.94  |
| RSHash   | 1     | 0     | 4861  | 505   | 100       | 100       | 51.58     | 20.51     | 75.96  |
| SDBMHash | 3     | 2     | 4849  | 504   | 93.1      | 92.31     | 57.01     | 23.08     | 72.41  |
| PJWHash  | 30    | 26    | 4878  | 513   | 0         | 0         | 43.89     | 0         | 21.95  |
| ELFHash  | 30    | 26    | 4878  | 513   | 0         | 0         | 43.89     | 0         | 21.95  |

其中数据1为100000个字母和数字组成的随机串哈希冲突个数。数据2为100000个有意义的英文句子哈希冲突个数。数据3为数据1的哈希值与1000003(大素数)求模后存储到线性表中冲突的个数。数据4为数据1的哈希值与10000019(更大素数)求模后存储到线性表中冲突的个数。

经过比较，得出以上平均得分。平均数为平方平均数。可以发现，BKDRHash 无论是在实际效果还是编码实现中，效果都是最突出的。APHash 也是较为优秀的算法。DJBHash，JSHash，RSHash 与 SDBMHash 各有千秋。PJWHash 与 ELFHash 效果最差，但得分相似，其算法本质是相似的。

```c++
unsigned int SDBMHash(char *str)
{
    unsigned int hash = 0;
 
    while (*str) {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }
    return (hash & 0x7FFFFFFF);
}
 
// RS Hash Function
unsigned int RSHash(char *str)
{
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
 
    while (*str) {
        hash = hash * a + (*str++);
        a *= b;
    }
    return (hash & 0x7FFFFFFF);
}
 
// JS Hash Function
unsigned int JSHash(char *str)
{
    unsigned int hash = 1315423911;
 
    while (*str) {
        hash ^= ((hash << 5) + (*str++) + (hash >> 2));
    }
    return (hash & 0x7FFFFFFF);
}
 
// P. J. Weinberger Hash Function
unsigned int PJWHash(char *str)
{
    unsigned int BitsInUnignedInt = (unsigned int)(sizeof(unsigned int) * 8);
    unsigned int ThreeQuarters    = (unsigned int)((BitsInUnignedInt  * 3) / 4);
    unsigned int OneEighth        = (unsigned int)(BitsInUnignedInt / 8);
    unsigned int HighBits         = (unsigned int)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
    unsigned int hash             = 0;
    unsigned int test             = 0;
 
    while (*str) {
        hash = (hash << OneEighth) + (*str++);
        if ((test = hash & HighBits) != 0) {
            hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
        }
    }
    return (hash & 0x7FFFFFFF);
}
 
// ELF Hash Function
unsigned int ELFHash(char *str)
{
    unsigned int hash = 0;
    unsigned int x    = 0;
 
    while (*str) {
        hash = (hash << 4) + (*str++);
        if ((x = hash & 0xF0000000L) != 0) {
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }
    return (hash & 0x7FFFFFFF);
}
 
// BKDR Hash Function
unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
 
    while (*str) {
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}
 
// DJB Hash Function
unsigned int DJBHash(char *str)
{
    unsigned int hash = 5381;
 
    while (*str) {
        hash += (hash << 5) + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}
 
// AP Hash Function
unsigned int APHash(char *str)
{
    unsigned int hash = 0;
    int i;
 
    for (i=0; *str; i++) {
        if ((i & 1) == 0) {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        }
        else {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash & 0x7FFFFFFF);
}
```

## 参考

[各种字符串Hash函数比较](https://www.byvoid.com/zhs/blog/string-hash-compare)

