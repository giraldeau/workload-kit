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
 
/*
 * Threadtree tracepoint provider
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER threadtree

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./threadtree_tp_provider.h

#ifdef __cplusplus
#extern "C"{
#endif /*__cplusplus */

#if !defined(_THREADTREE_PROVIDER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _THREADTREE_PROVIDER_H
#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
	threadtree,
	start,
	TP_ARGS(long, _id),
	TP_FIELDS(ctf_integer_hex(int, id, _id))
)

TRACEPOINT_EVENT(
	threadtree,
	fork,
	TP_ARGS(long, _id, long, _child),
	TP_FIELDS(ctf_integer_hex(int, id, _id)
		  ctf_integer_hex(int, child, _child))
)

TRACEPOINT_EVENT(
	threadtree,
	exit,
	TP_ARGS(long, _id),
	TP_FIELDS(ctf_integer_hex(int, id, _id))
)

#endif /* _THREADTREE_PROVIDER_H */

#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif /*__cplusplus */
