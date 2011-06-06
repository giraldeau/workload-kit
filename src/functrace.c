/*
 * Authors Matthew Khouzam <matthew.khouzam@polymtl.ca>
 *
 * This utility tests UST automatic function instrumentation
 */

#include <stdio.h> 

int foo(int n)
{
	int i, x;

	for (i=0; i<n; i++)
		x = bar(i);
	return x;
}

int bar(int n)
{
	int i = 0;
	while(i < n)
	{
		i = i << 1; 
		baz(i++);
	}
	return i;
}

int baz(int n)
{
	n *= 3;
	n /= 4;
	return n; 
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return -1;

	int n = atoi(argv[1]);
	printf("result %d\n", foo(n));
	return 0; 
}
