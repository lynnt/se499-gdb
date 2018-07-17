#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "safe-ctype.h"

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#else
extern size_t strlen(const char *s);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern void *memset(void *s, int c, size_t n);
#endif

#include <demangle.h>
#include "libiberty.h"

enum Type {FUNCTION, CHAR, INT, DOUBLE, VOID};

enum Type
find_type(int curr, int newlen, const char* mangled)
{
    if (curr < newlen && mangled[curr] != '_') {
        switch (mangled[curr]) {
            case 'F':
                return FUNCTION;
            case 'd':
                return DOUBLE;
            case 'i':
                return INT;
            case 'c':
                return CHAR;
            default:
                return VOID;
        }
    }

    return VOID;
}

char
get_type(enum Type type)
{
    switch (type) {
        case FUNCTION:
            return 'f';
        case DOUBLE:
            return 'd';
        case INT:
            return 'i';
        case CHAR:
            return 'c';
        default:
            return 'v';
    }

    return 'v';
}
char *
cforall_demangle (const char *mangled, int option ATTRIBUTE_UNUSED)
{
    char* demangled = NULL;

    if (mangled == NULL || *mangled == '\0') {
        return NULL;
    }

    int len = strlen(mangled);
    int index = 0;
    // figure out new beginning without underscores
    while (mangled[index] == '_') {
        index++;
    }
    int new_len = len - index;
    /*
    int curr = index;

    // figure out if it's function or variable
    while (curr < len && mangled[curr] != '_') {
        curr++;
    }

    // skip all the whitespace again
    while (curr < len && mangled[curr] == '_') {
        curr++;
    }

    // figure out its return type
    enum Type type = find_type(curr, len, mangled);
    curr++;

    enum Type return_type = find_type(curr, len, mangled);
    */

    demangled = XNEWVEC(char*, new_len + 1);
    if (demangled) {
        /*
        demangled[0] = get_type(return_type);

        strncpy(demangled + 1, mangled + index, new_len);
        */
        strncpy(demangled, mangled + index, new_len);
        demangled[new_len] = '\0';
    }

    return demangled;
}
