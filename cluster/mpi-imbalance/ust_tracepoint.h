#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER mpi_imbalance

#if !defined(UST_TRACEPOINT_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define UST_TRACEPOINT_H

/*
 * Copyright (C) 2014  Geneviève Bastien <gbastien@versatic.net>
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

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(mpi_imbalance, server_start_cycle,
	TP_ARGS(int, cycleno, int, numproc),
	TP_FIELDS(
		ctf_integer(int, cycleno, cycleno)
		ctf_integer(int, numworker, numproc)
	)
)

TRACEPOINT_EVENT(mpi_imbalance, server_wait,
	TP_ARGS(int, cycleno),
	TP_FIELDS(
		ctf_integer(int, cycleno, cycleno)
	)
)

TRACEPOINT_EVENT(mpi_imbalance, server_awake,
	TP_ARGS(int, cycleno),
	TP_FIELDS(
		ctf_integer(int, cycleno, cycleno)
	)
)

TRACEPOINT_EVENT(mpi_imbalance, worker_receive,
	TP_ARGS(int, rank, int, data),
	TP_FIELDS(
		ctf_integer(int, workerno, rank)
		ctf_integer(int, payload, data)
	)
)

TRACEPOINT_EVENT(mpi_imbalance, worker_start_work,
	TP_ARGS(int, rank),
	TP_FIELDS(
		ctf_integer(int, workerno, rank)
	)
)

TRACEPOINT_EVENT(mpi_imbalance, worker_wait,
	TP_ARGS(int, rank),
	TP_FIELDS(
		ctf_integer(int, workerno, rank)
	)
)

TRACEPOINT_EVENT(mpi_imbalance, worker_awake,
	TP_ARGS(int, rank),
	TP_FIELDS(
		ctf_integer(int, workerno, rank)
	)
)
#endif /* UST_TRACEPOINT_H */

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./ust_tracepoint.h

#include <lttng/tracepoint-event.h>
