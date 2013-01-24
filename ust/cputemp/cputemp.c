/*
 * Copyright (C) 2013  Suchakra Sharma <suchakrapani.sharma@polymtl.ca>
 * 
 * This is a nice small tool to see the CPU temperature as your systems
 * starts a burn. Its useful if you have workload-kit.
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


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <langinfo.h>

#include "sensors/sensors.h"
#include "sensors/error.h"
#include "cputemp.h"

#define TRACEPOINT_DEFINE
#include "cputemp_tp_provider.h"

#define PROGRAM "wk-cputemp"
#define VERSION "0.0.1"

static void print_short_help(void)
{
	printf("Try `%s -h' for more information\n", PROGRAM);
}

static void print_version(){
  printf("%s version is %s\n", PROGRAM, VERSION);

}

static void print_long_help(void)
{
	printf("Usage: %s [OPTION]... [CHIP]...\n", PROGRAM);
	puts("  -s, --repeat-seconds  Specify how many CPU temp samples are to be taken\n"
	     "  -h, --help            Display this help text\n"
	     "  -v, --version         Display the program version\n"
	     );
}

static const char *sprintf_chip_name(const sensors_chip_name *name)
{
#define BUF_SIZE 200
    static char buf[BUF_SIZE];

      if (sensors_snprintf_chip_name(buf, BUF_SIZE, name) < 0)
            return NULL;
        return buf;
}

static void do_a_print(const sensors_chip_name *name)
{
	printf("The Chip I've found is : %s\n", sprintf_chip_name(name));
	print_chip(name);
	printf("\n");
}

static int do_the_real_work(const sensors_chip_name *match, int *err)
{

  //printf("In %s\n", __PRETTY_FUNCTION__);
	const sensors_chip_name *chip;
	int chip_nr;
	int cnt = 0;

	chip_nr = 0;
	while ((chip = sensors_get_detected_chips(match, &chip_nr))) {
			printf("Chip is %s\n", chip->prefix);
      if (strstr(chip->prefix, "core" ) != NULL){			//Bad Idea : seeing if the chip found is CPU core chip?
				do_a_print(chip);
			}else
			{
				printf("The chip I have found is not a CPU core. Trying next maybe..\n");
			}

		cnt++;
	}
	return cnt;
}



static void print_label(const char *label, int space)
{
	int len = strlen(label)+1;
	printf("%s:%*s", label, space - len, "");
}

/* A variant for input values, where we want to handle errors gracefully */
static int get_input_value(const sensors_chip_name *name,
             const sensors_subfeature *sub,
                     double *val)
{
    int err;

      err = sensors_get_value(name, sub->number, val);
        if (err && err != -SENSORS_ERR_ACCESS_R) {
              fprintf(stderr, "ERROR: Can't get value of subfeature %s: %s\n",
                        sub->name, sensors_strerror(err));
                }
          return err;
}

static void print_chip_temp(const sensors_chip_name *name,
			    const sensors_feature *feature,
			    int label_size)
{

	int sensor_count, alarm_count;
	const sensors_subfeature *sf;
	double val;
	char *label;
	int i;

	if (!(label = sensors_get_label(name, feature))) {
		fprintf(stderr, "ERROR: Can't get label of feature %s!\n",
			feature->name);
		return;
	}
	print_label(label, label_size);
	free(label);

//	sf = sensors_get_subfeature(name, feature,
//				    SENSORS_SUBFEATURE_TEMP_FAULT);
//	if (sf && get_value(name, sf)) {
//		printf("   FAULT  ");
//	} else {
		sf = sensors_get_subfeature(name, feature,
					    SENSORS_SUBFEATURE_TEMP_INPUT);
		if (sf && get_input_value(name, sf, &val) == 0) {
			get_input_value(name, sf, &val);
			printf("%+6.1f%s  ", val, " deg C");
		} else
			printf("     N/A  ");
//	}
}

static int get_label_size(const sensors_chip_name *name)
{
  int i;
  const sensors_feature *iter;
  char *label;
  unsigned int max_size = 11; /* 11 as minumum label width */

  i = 0;
  while ((iter = sensors_get_features(name, &i))) {
    if ((label = sensors_get_label(name, iter)) &&
        strlen(label) > max_size)
          max_size = strlen(label);
    free(label);
    }

    /* One more for the colon, and one more to guarantee at least one
     *     space between that colon and the value */
    return max_size + 2;
}

void print_chip(const sensors_chip_name *name)
{
	const sensors_feature *feature;
	int i, label_size;

	label_size = get_label_size(name);
	feature = sensors_get_features(name, &i);
	print_chip_temp(name, feature, label_size);

}


static double get_value(const sensors_chip_name *name,
			const sensors_subfeature *sub)
{
	double val;
	int err;

	err = sensors_get_value(name, sub->number, &val);
	if (err) {
		fprintf(stderr, "ERROR: Can't get value of subfeature %s: %s\n",
			sub->name, sensors_strerror(err));
		val = 0;
	}
	return val;
}



int main(int argc, char *argv[]){

	int c, i, err, repeat_seconds_val;
		
    const char *repeat_seconds;
  	struct option long_opts[] =  {
			{ "help", no_argument, NULL, 'h' },
			{ "version", no_argument, NULL, 'v'},
			{ "repeat-seconds", required_argument, 's'},
			{ 0, 0, 0, 0 }
		};

		while (1) {
			c = getopt_long(argc, argv, "hvfs:", long_opts, NULL);
			if (c == EOF)
				break;
			switch(c) {
			case ':':
			case '?':
				print_short_help();
				exit(1);
			case 'h':
				print_long_help();
				exit(0);
			case 'v':
				print_version();
				exit(0);
			case 's':
				repeat_seconds = optarg;
        repeat_seconds_val = atoi(optarg);
				break;

			default:
				fprintf(stderr,
					"Internal error while parsing options!\n");
				exit(1);
			}
		}

	//Sensor inits with lib sensors default
	err = sensors_init(NULL);
	if (err) {
		fprintf(stderr, "sensors_init: %s\n", sensors_strerror(err));
		printf("Sensor Init Error!!");
    return 1;
	}
  else {
	  for (i=0; i<repeat_seconds_val; i++){
    err=do_the_real_work(NULL, &err);
      if(!err){
      printf("Error with do_the_real_work() in %s\n", __PRETTY_FUNCTION__);
      }
    sleep(1);
    }
      //Start taking readings
        if (!do_the_real_work(NULL, &err)) {
					fprintf(stderr,
							"No sensors found!\n"
							"Make sure you loaded all the kernel drivers you need.\n"
							"Try sensors-detect to find out which these are.\n");
					err = 1;
		}
	}

exit:
		sensors_cleanup();
		exit(err);
}


