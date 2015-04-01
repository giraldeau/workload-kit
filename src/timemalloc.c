#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

int do_empty(void *args)
{
    return 0;
}


int do_malloc(void *args)
{
    int i;
    size_t size = (long)args;
    volatile long *foo = malloc(size);
    foo[0] = 42;
    //free((void *)foo);
    return 0;
}


int do_malloc_memset(void *args)
{
    int i;
    size_t size = (long)args;
    long *foo = malloc(size);
    memset(foo, 42, size);
    free(foo);
    return 0;
}

int do_malloc_set_long(void *args)
{
    int i;
    size_t size = (long)args;
    volatile long *foo = malloc(size);
    long val = 0x2a2a2a2a2a2a2a2a;
    for (i = 0; i < size / sizeof(long); i++) {
        foo[i] = val;
    }
    free((void *)foo);
    return 0;
}

int do_malloc_set_char(void *args)
{
    int i;
    size_t size = (long)args;
    volatile char *foo = malloc(size);
    char val = 0x2a;
    for (i = 0; i < size; i++) {
        foo[i] = val;
    }
    free((void *)foo);
    return 0;
}

int main(int argc, char **argv)
{
    int repeat = 1e6;

    struct profile p[] = {
            {
                .name = "empty",
                .func = do_empty,
                .args = NULL,
                .repeat = repeat,
                .nr_thread = 1,
            },
            {
                .name = "malloc_8",
                .func = do_malloc,
                .args = (void *) (1 << 3),
                .repeat = repeat,
                .nr_thread = 1,
            },
            {
                .name = "malloc_4096",
                .func = do_malloc,
                .args = (void *) (1 << 12),
                .repeat = repeat,
                .nr_thread = 1,
            },
            {
                .name = "malloc_memset",
                .func = do_malloc_memset,
                .args = (void *) (1 << 12),
                .repeat = repeat,
                .nr_thread = 1,
            },
            {
                .name = "malloc_set_char",
                .func = do_malloc_set_char,
                .args = (void *) (1 << 12),
                .repeat = repeat,
                .nr_thread = 1,
            },
            {
                .name = "malloc_set_long",
                .func = do_malloc_set_long,
                .args = (void *) (1 << 12),
                .repeat = repeat,
                .nr_thread = 1,
            },
            { .name = NULL },
    };

    int i;
    for (i = 0; p[i].name != NULL; i++) {
        profile_combo(&p[i]);
    }

    return 0;
}
