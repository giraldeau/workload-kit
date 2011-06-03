#include <execinfo.h>
#include <ust/marker.h>

void __cyg_profile_func_enter (void *this_fn,
                                         void *call_site)
{
	char *funcname = backtrace_symbols(&this_fn, 1)[0];
	ust_marker(entry, "func_entry %s", funcname);
}

void __cyg_profile_func_exit  (void *this_fn,
                                         void *call_site)
{
	char *funcname = backtrace_symbols(&this_fn, 1)[0];
	ust_marker(exit, "func_exit %s", funcname);
}



