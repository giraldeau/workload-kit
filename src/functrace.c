/*
 * Authors Matthew Khouzam <matthew.khouzam@polymtl.ca>
 *
 * This utility tests UST automatic function instrumentation
 */

#include <stdio.h> 

int foo(int n)
{
	n--; 
	if(n > 0 ) 
		return bar( n);
	else
		return 0;
}

int bar( int n)
{
	int i =0;
	while( i < n ) 
	{
		i = i << 1; 
		baz(i++);
	}
	return foo(n);
}

int baz( int n) 
{
	n *= 3; 
	n /= 4; 
	return n; 
}



int main()
{
	int n = 100;
	foo(n);
	return 0; 
}
