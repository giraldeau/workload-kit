#include <execinfo.h>

#define TRACEPOINT_DEFINE
#include "cyg_tp_provider.h"

void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
	tracepoint(cyg, entry, this_fn);
}

void __cyg_profile_func_exit  (void *this_fn, void *call_site)
{
	tracepoint(cyg, exit);
}
