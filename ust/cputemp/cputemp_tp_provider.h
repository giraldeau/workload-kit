/*
 * Copyright (C) 2013  Suchakra Sharma <suchakrapani.sharma@polymtl.ca> 
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
 * cputemp tracepoint provider
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER cputemp

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./cputemp_tp_provider.h

#ifdef __cplusplus
#extern "C"{
#endif /*__cplusplus */

#if !defined(_CPUTEMP_PROVIDER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _CPUTEMP_PROVIDER_H
#include <lttng/tracepoint.h> 
TRACEPOINT_EVENT(
	cputemp,
	temp,
	TP_ARGS(double, temp),
	TP_FIELDS(ctf_float(double, sue, temp))
)

#endif /* _CPUTEMP_PROVIDER_H */

#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif /*__cplusplus */
