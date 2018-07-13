#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "safe-ctype.h"

#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#else
extern size_t strlen(const char *s);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern void *memset(void *s, int c, size_t n);
#endif

#include <demangle.h>
#include "libiberty.h"

char *
cforall_demangle (const char *mangled, int option ATTRIBUTE_UNUSED)
{
    char* demangled = NULL;

    if (mangled == NULL || *mangled == '\0') {
        return NULL;
    }

    int len = strlen(mangled);
    demangled = XNEWVEC(char*, len);
    int index = 0;
    if (demangled) {
        // strip out the underscores
        while (mangled[index] == '_') {
            index++;
        }
        int new_len = len - index;
        strncpy(demangled, mangled + index, new_len);
        demangled[new_len + 1] = '\0';
    }
    /*
    strncpy(demangled, mangled, len);
    demangled[len + 1] = '\0';
    */

    return demangled;
}
