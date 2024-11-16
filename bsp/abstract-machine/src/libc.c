#include <klib.h>
#include <rtthread.h>
#include <string.h>

char *strchr(const char *s, int c) {
    const char *str = s;
    while (*str != '\0') {
        if (*str == (char)c) return (char *)str;
        str++;
    }
    return NULL;
}

char *strrchr(const char *s, int c) {
    const char *str = s;
    char *last_occurrence = NULL;
    while (*str != '\0') {
        if (*str == (char)c)  last_occurrence = (char *)str;
        str++;
    }
    return last_occurrence;
}

char *strstr(const char *haystack, const char *needle) {
  return rt_strstr(haystack, needle);
}

long strtol(const char *restrict nptr, char **restrict endptr, int base) {
    assert(base >= 2 && base <= 36);
    const char *p = nptr;
    long acc = 0;
    int sign = 1;
    if (*p == '+' || *p == '-') {
        sign = (*p == '+') ? 1 : -1;
        p++;
    }
    while (*p) {
        int digit = (*p >= '0' && *p <= '9') ? *p - '0' :
                    (*p >= 'a' && *p <= 'z') ? *p - 'a' + 10 :
                    (*p >= 'A' && *p <= 'Z') ? *p - 'A' + 10 : -1;
        if (digit < 0 || digit >= base) break;
        acc = acc * base + digit;
        p++;
    }
    if (endptr) *endptr = (char *)p;
    return sign * acc;
}

char *strncat(char *restrict dst, const char *restrict src, size_t sz) {
    size_t dst_len = strlen(dst);
    size_t i;
    for (i = 0; i < sz && src[i] != '\0'; i++) {
        dst[dst_len + i] = src[i];
    }
    dst[dst_len + i] = '\0';
    return dst;
}


