#include "cstring.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const size_t cstring_min_size = 32;  // �ַ����ĳ�ʼ�ڴ��С

struct c_string_t {
	char *str;  // �ַ���
	size_t alloced;  // �ַ������ڴ�ռ��С
	size_t len;  // �ַ����ĳ���
};

cstring* cstring_create(void) {
	cstring *cstr;
	cstr = (cstring*)calloc(1, sizeof(*cstr));
	if (cstr == NULL) {
		return NULL;
	}
	cstr->str = (char*)malloc(cstring_min_size);
	*cstr->str = '\0';
	// ��ʼ�����ڴ��С�� 32��֮��ÿ���� 2 ����С����
	cstr->alloced = cstring_min_size;
	cstr->len = 0;

	return cstr;
}

void cstring_destroy(cstring *cstr) {
	if (cstr != NULL) {
		free(cstr->str);
		free(cstr);
	}
}

// ��չ�ַ������ڴ�ռ�
static void cstring_ensure_space(cstring *cstr, size_t addLen) {
	if (cstr == NULL || addLen == 0) {
		return;
	}

	size_t targetLen = cstr->len + addLen + 1;
	if (cstr->alloced >= targetLen) {
		return;
	}

	while (cstr->alloced < targetLen) {
		cstr->alloced <<= 1;  // ÿ���� 2 ����С����
		if (cstr->alloced == 0) {
			// ���Ƶ������ܻ��Ϊ0������ alloced ���޷����ͣ���һ����� UINT_MAX
			--cstr->alloced;
		}
	}
	cstr->str = (char*)realloc(cstr->str, cstr->alloced);
}

void cstring_append_str(cstring *cstr, const char *str, const size_t len) {
	if (cstr == NULL || str == NULL || *str == '\0') {
		return;
	}

	size_t size = len == 0 ? strlen(str) : len;

	cstring_ensure_space(cstr, size);

	memmove(cstr->str + cstr->len, str, size);
	cstr->len += size;
	cstr->str[cstr->len] = '\0';
}

void cstring_append_char(cstring *cstr, const char c) {
	if (cstr == NULL) {
		return;
	}

	cstring_ensure_space(cstr, 1);

	cstr->str[cstr->len] = c;
	++cstr->len;
	cstr->str[cstr->len] = '\0';
}

void cstring_append_int(cstring *cstr, const int val) {
	char str[32];

	if (cstr == NULL) {
		return;
	}
	snprintf(str, sizeof(str), "%d", val);
	cstring_append_str(cstr, str, 0);
}

void cstring_front_str(cstring *cstr, const char *str, const size_t len) {
	if (cstr == NULL || str == NULL || *str == '\0') {
		return;
	}

	size_t size = len == 0 ? strlen(str) : len;

	cstring_ensure_space(cstr, size);

	memmove(cstr->str + size, cstr->str, cstr->len);
	memmove(cstr->str, str, size);
	cstr->len += size;
	cstr->str[cstr->len] = '\0';
}

void cstring_front_char(cstring *cstr, const char c) {
	if (cstr == NULL) {
		return;
	}

	cstring_ensure_space(cstr, 1);

	memmove(cstr->str + 1, cstr->str, cstr->len);
	cstr->str[0] = c;
	++cstr->len;
	cstr->str[cstr->len] = '\0';
}

void cstring_front_int(cstring *cstr, const int val) {
	char str[32];

	if (cstr == NULL) {
		return;
	}
	snprintf(str, sizeof(str), "%d", val);
	cstring_front_str(cstr, str, 0);
}

void cstring_truncate(cstring *cstr, const size_t len) {
	if (cstr == NULL || len >= cstr->len) {
		return;
	}

	cstr->len = len;
	cstr->str[cstr->len] = '\0';
}

void cstring_clear(cstring *cstr) {
	if (cstr != NULL) {
		cstr->len = 0;
		cstr->str[cstr->len] = '\0';
	}
}

void cstring_drop_begin(cstring *cstr, const size_t len) {
	if (cstr == NULL || len == 0) {
		return;
	}

	if (len >= cstr->len) {
		cstring_clear(cstr);
		return;
	}

	cstr->len -= len;
	/* +1 �ǿ������� '\0' */
	memmove(cstr->str, cstr->str + len, cstr->len + 1);
}

void cstring_drop_end(cstring *cstr, const size_t len) {
	if (cstr == NULL || len == 0) {
		return;
	}

	if (len >= cstr->len) {
		cstring_clear(cstr);
		return;
	}

	cstr->len -= len;
	cstr->str[cstr->len] = '\0';
}

size_t cstring_len(const cstring *cstr) {
	return cstr == NULL ? 0 : cstr->len;
}

const char* cstring_peek(const cstring *cstr) {
	return cstr == NULL ? NULL : cstr->str;
}

char* cstring_dump(const cstring *cstr, size_t *len) {
	char *str;

	if (cstr == NULL) {
		return NULL;
	}

	if (len != NULL) {
		*len = cstr->len;
	}

	str = (char*)malloc(cstr->len + 1);
	memcpy(str, cstr->str, cstr->len + 1);
	return str;
}