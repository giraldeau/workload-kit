/*
 * calibrate.h
 *
 *  Created on: 2011-02-18
 *      Author: francis
 */

#ifndef CALIBRATE_H_
#define CALIBRATE_H_

#include <sys/types.h>

/* calibrate function returns the average number at
 * which the CPU is able to count in USEC interval */
unsigned long calibrate(suseconds_t usec);

#endif /* CALIBRATE_H_ */
