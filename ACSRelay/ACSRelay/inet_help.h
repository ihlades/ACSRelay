#ifndef __INET_HELP_H
#define __INET_HELP_H

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

#ifdef __cplusplus
extern "C" {
#endif

int inet_pton(int af, const char *src, void *dst);

int inet_pton4(const char *src, void *dst);
int inet_pton6(const char *src, void *dst);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __INET_HELP_H
