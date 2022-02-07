#pragma once

#include <string>

constexpr size_t maxTableLen = 1024;

typedef struct _HASHTABLE
{
    long hashA;
    long hashB;
    bool exist;
}HASHTABLE, *PHASHTABLE;

class StringHash
{
public:
    StringHash(const long tableLen = maxTableLen)
    {
        InitCryptTable();
        m_tableLen = tableLen;
        m_hashTable = new HASHTABLE[tableLen];

        for (int i = 0; i < tableLen; ++i) {
            m_hashTable[i].hashA = -1;
            m_hashTable[i].hashB = -1;
            m_hashTable[i].exist = false;
        }
    }

    ~StringHash()
    {
        if (nullptr != m_hashTable) {
            delete[]m_hashTable;
            m_hashTable = nullptr;
            m_tableLen = 0;
        }
    }

public:
    // hash一个字符串
    bool Hash(const std::string &str)
    {
        const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
        unsigned long nHash = HashString(str, HASH_OFFSET);
        unsigned long nHashA = HashString(str, HASH_A);
        unsigned long nHashB = HashString(str, HASH_B);
        unsigned long nHashStart = nHash % m_tableLen, nHashPos = nHashStart;

        while (m_hashTable[nHashPos].exist)
        {
            nHashPos = (nHashPos + 1) % m_tableLen;
            if (nHashPos == nHashStart) // 一个轮回
            {
                // hash表中没有空余的位置了,无法完成hash
                return false;
            }
        }
        m_hashTable[nHashPos].exist = true;
        m_hashTable[nHashPos].hashA = nHashA;
        m_hashTable[nHashPos].hashB = nHashB;

        return true;
    }

    // 检测str是否被hash过
    unsigned long Hashed(const std::string &str)
    {
        const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
        //不同的字符串三次hash还会碰撞的几率无限接近于不可能
        unsigned long nHash = HashString(str, HASH_OFFSET);
        unsigned long nHashA = HashString(str, HASH_A);
        unsigned long nHashB = HashString(str, HASH_B);
        unsigned long nHashStart = nHash % m_tableLen, nHashPos = nHashStart;

        while (m_hashTable[nHashPos].exist)
        {
            if (m_hashTable[nHashPos].hashA == nHashA && m_hashTable[nHashPos].hashB == nHashB)
                return nHashPos;
            else
                nHashPos = (nHashPos + 1) % m_tableLen;

            if (nHashPos == nHashStart)
                break;
        }
        return -1; // 没有找到
    }

private:
    // 对哈希索引表预处理
    void InitCryptTable()
    {
        unsigned long seed = 0x00100001;
        for (unsigned long index1 = 0; index1 < 0x100; ++index1) {
            for (unsigned long index2 = index1, i = 0; i < 5; ++i, index2 += 0x100) {
                unsigned long temp1, temp2;
                seed = (seed * 125 + 3) % 0x2AAAAB;
                temp1 = (seed & 0xFFFF) << 0x10;
                seed = (seed * 125 + 3) % 0x2AAAAB;
                temp2 = (seed & 0xFFFF);
                cryptTable[index2] = (temp1 | temp2);
            }
        }
    }

    // 求取哈希值
    unsigned long HashString(const std::string &str, unsigned long nHashType)
    {
        unsigned char *key = (unsigned char *)(const_cast<char*>(str.c_str()));
        unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;

        while (*key != 0)
        {
            int ch = toupper(*key++);
            seed1 = cryptTable[(nHashType << 8) + ch] ^ (seed1 + seed2);
            seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
        }
        return seed1;
    }

private:
    unsigned long cryptTable[0x500];
    unsigned long m_tableLen;  // 哈希索引表长度
    HASHTABLE *m_hashTable;
};
