#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool startswith(const char *s, const char *search) {
  size_t len = strlen(search);
  if (len > strlen(s))
    return false;
  return strncmp(s, search, len) == 0;
}

// Helper: decode escapes in-place (supports \040, \011, \012)
void decode_escapes(char *s) {
  char *src = s, *dst = s;
  while (*src) {
    if (*src == '\\' && src[1] == '0' && src[2] == '4' && src[3] == '0') {
      *dst++ = ' ';
      src += 4;
    } else if (*src == '\\' && src[1] == '0' && src[2] == '1' && src[3] == '1') {
      *dst++ = '\t';
      src += 4;
    } else if (*src == '\\' && src[1] == '0' && src[2] == '1' && src[3] == '2') {
      *dst++ = '\n';
      src += 4;
    } else {
      *dst++ = *src++;
    }
  }
  *dst = '\0';
}

char *truncate(const char *s, size_t len) {
  size_t slen = strlen(s);
  if (slen <= len)
    return strdup(s);

  if (len < 3)
    return strdup("...");

  char *out = malloc(len + 1);
  if (!out)
    return NULL;
  size_t i = 0;
  for (; i < len - 3 && s[i]; ++i)
    out[i] = s[i];
  out[i++] = '.';
  out[i++] = '.';
  out[i++] = '.';
  out[i] = '\0';
  return out;
}
