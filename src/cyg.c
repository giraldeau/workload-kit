#include <stdio.h>
#include <execinfo.h>
//#include <ust/marker.h>

// will add a cache for the function names later. 

void __cyg_profile_func_enter (void *this_fn,
                                         void *call_site)
{
	char *funcname = backtrace_symbols(&this_fn, 1)[0];
	//ust_marker(entry, "func_entry %s", funcname);
	printf("entry %p\n", this_fn);
}

void __cyg_profile_func_exit  (void *this_fn,
                                         void *call_site)
{
	char *funcname = backtrace_symbols(&this_fn, 1)[0];
	//ust_marker(exit, "func_exit %s", funcname);
	printf("exit %p\n", this_fn);
}



