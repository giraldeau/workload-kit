/*
 * Copyright (C) 2011-2012  Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 * Copyright (C) 2011-2012  Matthew Khouzam <matthew.khouzam@ericsson.com> 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */
 
#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER bossthread

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./bossthread_tp_provider.h

#ifdef __cplusplus
#extern "C"{
#endif /*__cplusplus */

#if !defined(_BOSSTHREAD_TP_PROVIDER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _BOSSTHREAD_TP_PROVIDER_H
/*
 * Add this to allow programs to call "tracepoint(...):
 */ 
#include <lttng/tracepoint.h>
#include <pthread.h>

/**/
TRACEPOINT_EVENT(
	bossthread,
	id,
	TP_ARGS(pthread_t, self),
	TP_FIELDS(
		ctf_integer(pthread_t, id, self)
	)
)

#endif /* _SAMPLE_COMPONENT_PROVIDER_H */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif /*__cplusplus */
