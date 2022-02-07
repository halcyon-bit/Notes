#define NEW_OVERLOAD_IMPLEMENTATION_
#include "LeakDetector.h"

#include <iostream>
#include <cstring>

size_t LeakDetector::_callCount = 0;

typedef struct tagMemoryList
{
    struct tagMemoryList *_prev;
    struct tagMemoryList *_next;
    size_t _size;  // operator new()申请的内存大小
    bool _isArray;
    char *_file;
    size_t _line;
}MemoryList;

static MemoryList memoryListHead = { &memoryListHead, &memoryListHead, 0, false, nullptr, 0 };
static size_t memoryAllocated = 0; // 保存未释放的内存大小

void* AllocateMemory(size_t size, bool array, const char *file, size_t line)
{
    size_t newsize = size + sizeof(MemoryList);
    MemoryList *newElem = (MemoryList*)malloc(newsize);
    newElem->_size = size;
    newElem->_isArray = array;
    newElem->_line = line;
    newElem->_next = memoryListHead._next;
    newElem->_prev = &memoryListHead;
    if (nullptr != file)
    {
        newElem->_file = (char*)malloc(strlen(file)+1);
        strcpy_s(newElem->_file, strlen(file)+1, file);
    }
    else
    {
        newElem->_file = nullptr;
    }
    memoryListHead._next->_prev = newElem;
    memoryListHead._next = newElem;
    memoryAllocated += size;
    return (char*)newElem + sizeof(MemoryList);
}

void DeleteMemory(void *ptr, bool array)
{
    MemoryList *curElem = (MemoryList*)((char*)ptr-sizeof(MemoryList));

    if(curElem->_isArray != array)
    {
        return;
    }

    curElem->_next->_prev = curElem->_prev;
    curElem->_prev->_next = curElem->_next;
    memoryAllocated -= curElem->_size;
    if (nullptr != curElem->_file)
    {
        free(curElem->_file);
    }
    free(curElem);
}

void* operator new(size_t size, const char *file, size_t line)
{
    return AllocateMemory(size, false, file, line);
}

void* operator new[](size_t size, const char *file, size_t line)
{
    return AllocateMemory(size, true, file, line);
}

void operator delete(void *ptr)
{
    DeleteMemory(ptr, false);
}

void operator delete[](void *ptr)
{
    DeleteMemory(ptr, true);
}

void LeakDetector::_LeakDetector()
{
    if (0 == memoryAllocated)
    {
        std::cout << "恭喜, 您的代码不存在内存泄漏!" << std::endl;
        return;
    }

    // 存在内存泄漏
    // 记录内存泄漏次数
    size_t count = 0;
    // 若不存在内存泄漏, 则双向链表中应该只剩下一个头节点
    // 若存在内存泄漏, 则双向链表中除头节点之外的结点都已泄露，个数即内存泄漏次数
    MemoryList *ptr = memoryListHead._next;
    while ((nullptr != ptr) && (&memoryListHead != ptr))
    {
        if (true == ptr->_isArray)
        {
            std::cout << "new[]空间为释放, ";
        }
        else
        {
            std::cout << "new空间为释放, ";
        }
        std::cout << "指针: " << ptr << " 大小: " << ptr->_size;
        if (nullptr != ptr->_file)
        {
            std::cout << "位于 " << ptr->_file << " 第 " << ptr->_line << " 行";
        }
        else
        {
            std::cout << "(无文件信息)";
        }
        std::cout << std::endl;
        ptr = ptr->_next;
        ++count;
    }
    std::cout << "存在" << count << "处内存泄漏, 共包括 " << memoryAllocated << " byte." << std::endl;
}    