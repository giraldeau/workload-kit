/*
 * stp.c
 *
 *  Created on: May 6, 2015
 *      Author: francis
 */

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

int foo(void *arg)
{
    printf("hello\n");
    return 0;
}

int main(int argc, char **argv)
{
    struct profile p = {
        .name = "wk-stp",
        .func = foo,
        .repeat = 10000,
    };
    profile_combo(&p);
    return 0;
}
