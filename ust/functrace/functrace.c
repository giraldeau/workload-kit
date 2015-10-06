/*
 * Authors Matthew Khouzam <matthew.khouzam@polymtl.ca>
 *
 * This utility tests UST automatic function instrumentation
 */

#include <stdio.h>

int (*foo_ptr)(int);
int (*bar_ptr)(int);
int (*baz_ptr)(int);

int baz(int n)
{
    n *= 3;
    n /= 4;
    return n;
}

int bar(int n)
{
    int i = 0;
    while(i < n)
    {
        i = i << 1;
        baz_ptr(i++);
    }
    return i;
}

int foo(int n)
{
	int i, x;

	for (i=0; i<n; i++)
		x = bar_ptr(i);
	return x;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return -1;

	foo_ptr = foo;
	bar_ptr = bar;
	baz_ptr = baz;

	int n = atoi(argv[1]);
	printf("result %d\n", foo_ptr(n));
	return 0; 
}
