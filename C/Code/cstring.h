#ifndef _C_STRING_H_
#define _C_STRING_H_ 

#include <stddef.h>

struct c_string_t;
typedef struct c_string_t cstring;

/*
 * @brief		创建空字符串
 * @return		空字符串
 */
cstring* cstring_create(void);

/*
 * @brief			销毁字符串
 * @param[in\out]	待销毁的字符串
 */
void cstring_destroy(cstring *cstr);

/*
 * @brief			在尾部追加字符串
 * @param[in\out]	字符串
 * @param[in]		追加的字符串
 * @param[in]		追加的字符串的长度
 */
void cstring_append_str(cstring *cstr, const char *str, const size_t len);

/*
 * @brief			在尾部追加字符
 * @param[in\out]	字符串
 * @param[in]		追加的字符
 */
void cstring_append_char(cstring *cstr, const char c);

/*
 * @brief			在尾部追加整数
 * @param[in\out]	字符串
 * @param[in]		追加的整数
 */
void cstring_append_int(cstring *cstr, const int val);

/*
 * @brief			在头部插入字符串
 * @param[in\out]	字符串
 * @param[in]		插入的字符串
 * @param[in]		插入的字符串的长度
 */
void cstring_front_str(cstring *cstr, const char *str, const size_t len);

/*
 * @brief			在头部插入加字符
 * @param[in\out]	字符串
 * @param[in]		插入的字符
 */
void cstring_front_char(cstring *cstr, const char c);

/*
 * @brief			在头部插入整数
 * @param[in\out]	字符串
 * @param[in]		插入的整数
 */
void cstring_front_int(cstring *cstr, const int val);

/*
 * @brief			清空字符串
 * @param[in\out]	字符串
 */
void cstring_clear(cstring *cstr);

/*
 * @brief			裁剪字符串
 * @param[in\out]	字符串
 * @param[in]		字符串的保留长度
 */
void cstring_truncate(cstring *cstr, const size_t len);

/*
 * @brief			删除头部的 N 个字符
 * @param[in\out]	字符串
 * @param[in]		删除的字符长度
 */
void cstring_drop_begin(cstring *cstr, const size_t len);

/*
 * @brief			删除尾部的 N 个字符
 * @param[in\out]	字符串
 * @param[in]		删除的字符长度
 */
void cstring_drop_end(cstring *cstr, const size_t len);

/*
 * @brief			获取字符串的长度
 * @param[in]		字符串
 * @return			字符串的长度
 */
size_t cstring_len(const cstring *cstr);

/*
 * @brief			返回字符串指针，使用的是字符串的原始内存
 * @param[in\out]	字符串
 * @return			字符串指针
 */
const char* cstring_peek(const cstring *cstr);

/*
 * @brief			重新分配一块内存，存储字符串(malloc 分配新内存，需要外部进行 free)
 * @param[in]		字符串
 * @param[out]		字符串的长度
 * @return			字符串指针
 */
char* cstring_dump(const cstring *cstr, size_t *len);
#endif