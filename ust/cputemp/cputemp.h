/*
 * Copyright (C) 2013  Suchakra Sharma <suchakrapani.sharma@polymtl.ca>
 *
 * Header file for cputemp.c
 * This should have come with Workload Kit stuff
 *
 */

#ifndef _CPUTEMP_H
#define _CPUTEMP_H

static void print_short_help(void);
static void print_version();
static void print_long_help(void);
static const char *sprintf_chip_name(const sensors_chip_name *name);
static void do_a_print(const sensors_chip_name *name);
static int do_the_real_work(const sensors_chip_name *match, int *err);
static void print_label(const char *label, int space);
static int get_input_value(const sensors_chip_name *name, const sensors_subfeature *sub, double *val);
static void print_chip_temp(const sensors_chip_name *name,const sensors_feature *feature, int label_size);
static int get_label_size(const sensors_chip_name *name);
void print_chip(const sensors_chip_name *name);
static double get_value(const sensors_chip_name *name, const sensors_subfeature *sub);i

#endif /* _CPUTEMP_H */
