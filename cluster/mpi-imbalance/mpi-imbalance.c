/*
 * Copyright (C) 2014  Geneviève Bastien <gbastien@versatic.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1 of
 * the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <stdio.h>
#include <mpi.h>

#include "calibrate.h"

#define TRACEPOINT_DEFINE
#include "ust_tracepoint.h"

#define CYCLES 8
#define MIN_TASK 25000
#define STEP 2000
#define TERMINATION 0

int main(int argc, char **argv) {
	int numprocs, rank, namelen;
	int i, j, num;
	MPI_Status status;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Use first process as server */
	int server = 0;

	/* Get name of processor for debug purpose */
	MPI_Get_processor_name(processor_name, &namelen);
	printf("Process %d on host %s\n", rank, processor_name);
	if (rank == server) {
		srand(time(NULL));
		int max = calibrate(MIN_TASK);
		for (i = 0; i < CYCLES; i++) {
			tracepoint(mpi_imbalance, server_start_cycle, i, numprocs);
			/* For each cycle, dispatch a workload to all worker processes */
			for (j = 1; j < numprocs; j++) {
				num = rand() % numprocs;
				num = (num + 1) * max;
				MPI_Send(&num, 1, MPI_INTEGER, j, 1, MPI_COMM_WORLD);
			}

			/* Wait for all workload to be finished before dispatching new workload */
			tracepoint(mpi_imbalance, server_wait, i);
			MPI_Barrier(MPI_COMM_WORLD);
			tracepoint(mpi_imbalance, server_awake, i);
		}
		for (j = 1; j < numprocs; j++) {
			num = TERMINATION;
			MPI_Send(&num, 1, MPI_INTEGER, j, 1, MPI_COMM_WORLD);
		}

	} else {
		/* Receive workload from server */
		MPI_Recv(&num, 1, MPI_INTEGER, server, 1, MPI_COMM_WORLD, &status);
		tracepoint(mpi_imbalance, worker_receive, rank, num);

		/* While the server does not request termination, do the work */
		while (num != TERMINATION) {
			tracepoint(mpi_imbalance, worker_start_work, rank);
			do_hog(num);

			tracepoint(mpi_imbalance, worker_wait, rank);
			MPI_Barrier(MPI_COMM_WORLD);
			tracepoint(mpi_imbalance, worker_awake, rank);
			/* Receive workload from server */
			MPI_Recv(&num, 1, MPI_INTEGER, server, 1, MPI_COMM_WORLD, &status);
			tracepoint(mpi_imbalance, worker_receive, rank, num);
		}

	}

	MPI_Finalize();
	return 0;
}
