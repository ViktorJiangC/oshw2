#ifndef STRING_H
#define STRING_H

#include "types.h"

int memcmp(const void *, const void *, uint);
__noinline__ void *memmove(void *, const void *, uint);
void *memset(void *, int, uint);
char *safestrcpy(char *, const char *, int);
int strlen(const char *);
int strncmp(const char *, const char *, uint);
char *strncpy(char *, const char *, int);

#endif // STRING_H