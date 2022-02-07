#ifndef _C_STRING_H_
#define _C_STRING_H_ 

#include <stddef.h>

struct c_string_t;
typedef struct c_string_t cstring;

/*
 * @brief		�������ַ���
 * @return		���ַ���
 */
cstring* cstring_create(void);

/*
 * @brief			�����ַ���
 * @param[in\out]	�����ٵ��ַ���
 */
void cstring_destroy(cstring *cstr);

/*
 * @brief			��β��׷���ַ���
 * @param[in\out]	�ַ���
 * @param[in]		׷�ӵ��ַ���
 * @param[in]		׷�ӵ��ַ����ĳ���
 */
void cstring_append_str(cstring *cstr, const char *str, const size_t len);

/*
 * @brief			��β��׷���ַ�
 * @param[in\out]	�ַ���
 * @param[in]		׷�ӵ��ַ�
 */
void cstring_append_char(cstring *cstr, const char c);

/*
 * @brief			��β��׷������
 * @param[in\out]	�ַ���
 * @param[in]		׷�ӵ�����
 */
void cstring_append_int(cstring *cstr, const int val);

/*
 * @brief			��ͷ�������ַ���
 * @param[in\out]	�ַ���
 * @param[in]		������ַ���
 * @param[in]		������ַ����ĳ���
 */
void cstring_front_str(cstring *cstr, const char *str, const size_t len);

/*
 * @brief			��ͷ��������ַ�
 * @param[in\out]	�ַ���
 * @param[in]		������ַ�
 */
void cstring_front_char(cstring *cstr, const char c);

/*
 * @brief			��ͷ����������
 * @param[in\out]	�ַ���
 * @param[in]		���������
 */
void cstring_front_int(cstring *cstr, const int val);

/*
 * @brief			����ַ���
 * @param[in\out]	�ַ���
 */
void cstring_clear(cstring *cstr);

/*
 * @brief			�ü��ַ���
 * @param[in\out]	�ַ���
 * @param[in]		�ַ����ı�������
 */
void cstring_truncate(cstring *cstr, const size_t len);

/*
 * @brief			ɾ��ͷ���� N ���ַ�
 * @param[in\out]	�ַ���
 * @param[in]		ɾ�����ַ�����
 */
void cstring_drop_begin(cstring *cstr, const size_t len);

/*
 * @brief			ɾ��β���� N ���ַ�
 * @param[in\out]	�ַ���
 * @param[in]		ɾ�����ַ�����
 */
void cstring_drop_end(cstring *cstr, const size_t len);

/*
 * @brief			��ȡ�ַ����ĳ���
 * @param[in]		�ַ���
 * @return			�ַ����ĳ���
 */
size_t cstring_len(const cstring *cstr);

/*
 * @brief			�����ַ���ָ�룬ʹ�õ����ַ�����ԭʼ�ڴ�
 * @param[in\out]	�ַ���
 * @return			�ַ���ָ��
 */
const char* cstring_peek(const cstring *cstr);

/*
 * @brief			���·���һ���ڴ棬�洢�ַ���(malloc �������ڴ棬��Ҫ�ⲿ���� free)
 * @param[in]		�ַ���
 * @param[out]		�ַ����ĳ���
 * @return			�ַ���ָ��
 */
char* cstring_dump(const cstring *cstr, size_t *len);
#endif