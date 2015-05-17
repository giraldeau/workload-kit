/*
 * tp.h
 *
 *  Created on: Apr 20, 2015
 *      Author: francis
 */
#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER unwind

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./tp.h

#if !defined(_UNWIND_TP_PROVIDER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _UNWIND_TP_PROVIDER_H

#include <lttng/tracepoint.h>

#include <ucontext.h>

TRACEPOINT_EVENT(unwind, online,
    TP_ARGS(void *, addr, size_t, depth),
    TP_FIELDS(
        ctf_sequence(void *, addr, addr, size_t, depth)
    )
)

TRACEPOINT_EVENT(unwind, offline,
    TP_ARGS(char *, sp, size_t, size, ucontext_t *, uc),
    TP_FIELDS(
        ctf_sequence(char, stack, sp, size_t, size)
        ctf_sequence(char, ucontext, uc, size_t, sizeof(*uc))
    )
)

TRACEPOINT_EVENT(unwind, offline_fast,
    TP_ARGS(char *, sp, ucontext_t *, uc),
    TP_FIELDS(
        ctf_array(char, stack, sp, 8126)
        ctf_sequence(char, ucontext, uc, size_t, sizeof(*uc))
    )
)

#endif /* _UNWIND_TP_PROVIDER_H */
#include <lttng/tracepoint-event.h>
