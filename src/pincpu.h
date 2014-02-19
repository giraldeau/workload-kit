/*
 * pin.h
 *
 *  Created on: Feb 12, 2014
 *      Author: francis
 */

#ifndef PINCPU_H_
#define PINCPU_H_

struct pincpu {
    int *cpus;
    int nrcpus;
};

int pincpu_parse(struct pincpu *cpuset, const char *input);
int pincpu_do_pin(int cpu);

#endif /* PINCPU_H_ */
