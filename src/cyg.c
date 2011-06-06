#include <execinfo.h>
#include <ust/marker.h>

void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
	ust_marker(entry, "func_entry %p", this_fn);
}

void __cyg_profile_func_exit  (void *this_fn, void *call_site)
{
	ust_marker(exit, "func_exit %p", this_fn);
}
