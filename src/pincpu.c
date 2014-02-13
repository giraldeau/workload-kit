/*
 * pincpu.c
 *
 *  Created on: Feb 12, 2014
 *      Author: francis
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

#include "pincpu.h"
#include "utils.h"

int pincpu_parse(struct pincpu *cpuset, const char *input) {
	int cpu;
	int i = 0;
	int ret = 0;
	const char sep = ',';
	char *tok;
	char *str;
	char *end;
	int cpus = sysconf(_SC_NPROCESSORS_ONLN);

	// count number of CPUs
	str = strdup(input);
	tok = strtok(str, &sep);
	while (tok) {
		cpuset->nrcpus++;
		tok = strtok(0, &sep);
	}
	free(str);

	cpuset->cpus = calloc(cpuset->nrcpus, sizeof(int));
	str = strdup(input);
	tok = strtok(str, &sep);
	while (tok) {
		cpu = strtol(tok, &end, 10);
		if (cpu >= cpus) {
			printf("Error: CPU %d is greater or equal to max online cpus %d\n",
					cpu, cpuset->nrcpus);
			ret = -1;
			goto out;
		}
		if (*end != '\0') {
			printf("Error: invalid CPU %s\n", tok);
			ret = -1;
			goto out;
		}
		cpuset->cpus[i++] = cpu;
		tok = strtok(0, &sep);
	}
out:
	free(str);
	return ret;
}

void dump_cpuset(struct pincpu *cpuset) {
	int i;
	printf("cpuset: [");
	for (i = 0; i < cpuset->nrcpus; i++) {
		printf("%d", cpuset->cpus[i]);
		if (i != cpuset->nrcpus - 1)
			printf(",");
	}
	printf("]\n");
}

int pincpu_do_pin(int cpu) {
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(cpu, &set);
    sched_setaffinity(gettid(), sizeof(set), &set);
    if (cpu != sched_getcpu())
        return -1;
    return 0;
}
