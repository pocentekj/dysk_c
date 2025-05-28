#ifndef H_STR_UTILS
#define H_STR_UTILS

#include <stdbool.h>
#include <stddef.h>

bool startswith(const char *s, const char *search);
void decode_escapes(char *s);
char *truncate(const char *s, size_t len);

#endif
