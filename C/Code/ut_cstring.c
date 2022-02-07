#include "cstring.h"
#include <stdio.h>

int main() {
	cstring *cs = cstring_create();
	cstring_append_str(cs, "but we still want to experience", 0);
	cstring_append_char(cs, '.');
	cstring_append_int(cs, 66);
	printf("%s\n", cstring_peek(cs));
	cstring_front_str(cs, "start Life had a lot of things is futile, ", 0);
	printf("%s\n", cstring_peek(cs));
	cstring_drop_begin(cs, 6);
	printf("%s\n", cstring_peek(cs));
	cstring_drop_end(cs, 2);
	printf("%s\n", cstring_peek(cs));
	cstring_destroy(cs);
	return 0;
}