/*
 * perfuser.c
 *
 *  Created on: Apr 13, 2015
 *      Author: francis
 */

#define _GNU_SOURCE
#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <linux/perf_event.h>
#include <signal.h>
#include <pthread.h>
#include <getopt.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>

#include "utils.h"
#include "calibrate.h"

#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "tp.h"

enum disable_mode {
    DISABLE_NONE = 0,
    DISABLE_EARLY = 1,
    DISABLE_LATE = 2,
    DISABLE_LAST = 3,
};

enum unw_type {
    UNW_TYPE_ONLINE,
    UNW_TYPE_OFFLINE,
    UNW_TYPE_LAST,
};

#define MAX_DEPTH 100

#define DEFAULT_THREAD 1
#define DEFAULT_REPEAT 1
#define DEFAULT_DRYRUN 0
#define DEFAULT_UNWIND UNW_TYPE_ONLINE
#define DEFAULT_DEPTH 25
#define DEFAULT_ITER_MAIN 1000000
#define DEFAULT_ITER_OVERHEAD 1
#define DEFAULT_DISABLE DISABLE_EARLY
#define DEFAULT_PERIOD 10000
#define DEFAULT_EXP NULL
#define progname "wk-perfuser"

static struct perf_event_attr def_attr = {
    .type = PERF_TYPE_HARDWARE,
    .size = sizeof(def_attr),
    .config = PERF_COUNT_HW_INSTRUCTIONS,
    .freq = 0,
    .disabled = 1,
    .watermark = 1,
    .wakeup_events = 1,
};

struct args;

struct exp {
    char *name;
    int (*func)(struct args *);
    void (*before)(struct args *);
    void (*after)(struct args *);
};

struct args {
    int nr_thread;
    long repeat;
    struct exp *exp;
    long iter_overhead;
    long iter_main;
    int disable;
    int period;
    int dryrun;
    int unwind_type;
    int unwind_depth;
    struct perf_event_attr attr;
    FILE *out;
    struct counter *counters;
    struct timespec t1;
    struct timespec t2;
    long iter_1ms;
};

struct counter {
    pthread_t th;
    int rank; // eq to global TLS rank
    struct args *args;
    int fd;
    int tid;
    int hits;
    gregset_t regs;
};

pthread_barrier_t barrier;
static int __thread rank;
static struct counter *g_counters;

static long sys_perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
        int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd,
            flags);
}

void hog(long iter)
{
    volatile int x = iter;
    while(x != 0) {
        x--;
    }
}

static void signal_handler(int signum, siginfo_t *info, void *arg)
{
    struct counter *cnt = &g_counters[rank];
    int disable = cnt->args->disable;

    if (disable == DISABLE_LATE)
        ioctl(cnt->fd, PERF_EVENT_IOC_DISABLE, 0);

    cnt->hits++;
    hog(cnt->args->iter_overhead);

    __sync_synchronize();
    if (disable == DISABLE_EARLY) {
        ioctl(cnt->fd, PERF_EVENT_IOC_REFRESH, 1);
    } else if (disable == DISABLE_LATE) {
        ioctl(cnt->fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    /*
    int ret;
    sigset_t set;
    sigemptyset(&set);
    ret = sigpending(&set);
    assert(ret == 0);
    ret = sigismember(&set, SIGIO);
    printf("sigismember=%d\n", ret);
    if (ret)
        cnt->hits++;

    ucontext_t *ctx = arg;
    ret = memcmp(&ctx->uc_mcontext.gregs, &cnt->regs, sizeof(gregset_t));
    if (ret == 0) {
        printf("context same\n");
    } else {
        printf("context different\n");
    }

    int depth = 10;
    void *bt[depth];
    backtrace(&bt, depth);
    backtrace_symbols_fd(&bt, depth, 1);
    if (cnt->hits < 10) {
        FILE *out = fopen("dump.data", "a");
        fwrite(&ctx->uc_mcontext.gregs, sizeof(gregset_t), 1, out);
        int pad = 512 - sizeof(gregset_t);
        int cafe = 0xCAFECAFE;
        int i;
        for(i = 0; i < (pad / sizeof(cafe)); i++)
            fwrite(&cafe, sizeof(cafe), 1, out);
        fflush(out);
        fclose(out);
    }
    assert(cnt->hits < 10);
    */
}

int install_sighand()
{
    int ret;
    struct sigaction sigact;

    sigact.sa_sigaction = signal_handler;
    sigact.sa_flags = SA_SIGINFO;
    ret = sigaction(SIGIO, &sigact, NULL);
    assert(ret == 0);
    return 0;
}

int open_counter(struct counter *counter)
{
    uint64_t val;
    int tid;
    int ret;
    int flags;
    int fd;
    struct sigaction sigact;

    tid = syscall(__NR_gettid);
    fd = sys_perf_event_open(&counter->args->attr, tid, -1, -1, 0);
    assert(fd > 0);

    // fasync setup
    struct f_owner_ex ex = {
        .type = F_OWNER_TID,
        .pid = tid,
    };
    ret = fcntl(fd, F_SETOWN_EX, &ex);
    assert(ret == 0);
    flags = fcntl(fd, F_GETFL);
    ret = fcntl(fd, F_SETFL, flags | FASYNC | O_ASYNC);
    assert(ret == 0);

    counter->fd = fd;
    counter->tid = tid;

    // 3, 2, 1... and lift-off! ;-)
    if (counter->args->disable == DISABLE_EARLY)
        ioctl(counter->fd, PERF_EVENT_IOC_REFRESH, 1);
    ioctl(counter->fd, PERF_EVENT_IOC_ENABLE, 0);
    return 0;
}

void close_counter(struct counter *counter)
{
    close(counter->fd);
}

void *do_work(void *arg)
{
    struct counter *cnt = arg;

    rank = cnt->rank;
    if (!cnt->args->dryrun)
        open_counter(cnt);

    pthread_barrier_wait(&barrier);

    hog(cnt->args->iter_main);

    if (!cnt->args->dryrun)
        close_counter(cnt);
    return NULL;
}

int do_one(struct args *args)
{
    int i;
    g_counters = calloc(args->nr_thread, sizeof(struct counter));
    args->counters = g_counters;

    if (args->exp->before)
        args->exp->before(args);
    /* spawn */
    clock_gettime(CLOCK_MONOTONIC_RAW, &args->t1);
    for (i = 0; i < args->nr_thread; i++) {
        g_counters[i].args = args;
        g_counters[i].rank = i;
        pthread_create(&g_counters[i].th, NULL, do_work, &g_counters[i]);
    }

    /* join */
    for (i = 0; i < args->nr_thread; i++) {
        pthread_join(g_counters[i].th, NULL);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &args->t2);
    if (args->exp->after)
        args->exp->after(args);
    free(g_counters);
    g_counters = NULL;
    args->counters = NULL;
    return 0;
}

void before_run(struct args *args)
{
}

void print_status(struct args *args)
{
    printf("run disable=%d iter_overhead=%-7ld hits=%d\n",
            args->disable, args->iter_overhead, args->counters[0].hits);
}

void after_run(struct args *args)
{
    int hits = 0;
    int i;
    for (i = 0; i < args->nr_thread; i++) {
        hits += args->counters[i].hits;
    }
    struct timespec ts = time_sub(&args->t2, &args->t1);
    double elapsed = timespec_to_double_ns(&ts);

    fprintf(args->out, "%d;%d;%d%ld;%ld;%d;%d;%.3f\n", args->nr_thread, args->dryrun,
            args->period, args->iter_main, args->iter_overhead, args->disable,
            hits, elapsed / 1000000000.0);
    fflush(args->out);
    print_status(args);
}

void stats_header(struct args *args)
{
    fprintf(args->out, "%s;%s;%s;%s;%s;%s;%s;%s;\n", "threads", "dryrun", "period",
            "iter_main", "iter_overhead", "disable", "hits", "elapsed");
}

int do_handler_work(struct args *args)
{
    int i;
    int disable;
    long overhead;

    for (overhead = 0; overhead < 10; overhead++) {
        args->disable = DISABLE_EARLY;
        args->iter_overhead = overhead * 100000;
        do_one(args);
    }

    return 0;
}

void do_unwind_online()
{
    unw_cursor_t cursor;
    unw_context_t uc;
    void *addr[MAX_DEPTH];
    size_t depth = 0;

    unw_getcontext(&uc);
    unw_init_local(&cursor, &uc);
    while (depth < MAX_DEPTH && unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, (unw_word_t *) &addr[depth]);
        depth++;
    }
    tracepoint(unwind, online, addr, depth);
}

void do_unwind_offline()
{
    size_t size = 4096 * 2;
    ucontext_t uc;
    getcontext(&uc);
    unsigned long sp = ((unsigned long) &uc) - size;
    tracepoint(unwind, offline, (char *) sp, size, &uc);
}

void do_unwind(int type)
{
    switch (type) {
    case UNW_TYPE_ONLINE:
        do_unwind_online();
        break;
    case UNW_TYPE_OFFLINE:
        do_unwind_offline();
        break;
    default:
        break;
    }
}

int rec1(int depth, int type);
int rec2(int depth, int type);

int rec(void *arg)
{
    struct args *args = arg;
    rec1(args->unwind_depth, args->unwind_type);
    return 0;
}

__attribute__((noinline))
int rec1(int depth, int type)
{
    if (depth > 0) {
        rec2(--depth, type);
    } else {
        do_unwind(type);
    }
    return 0;
}

__attribute__((noinline))
int rec2(int depth, int type)
{
    if (depth > 0) {
        rec1(--depth, type);
    } else {
        do_unwind(type);
    }
    return 0;
}

static int done = 0;

int do_unwind_overhead_one(struct args *args)
{
    int ret;
    int depth;

    struct profile prof = {
        .name = "unwind",
        .func = rec,
        .args = args,
        .repeat = 1000,
    };

    if (!done) {
        printf("depth;type;mean;sd;\n");
        done = 1;
    }

    depth = args->unwind_depth; // unwind_depth is updated
    profile_init(&prof);
    profile_func(&prof);
    profile_stats(&prof);
    printf("%d;%d;%.0f;%.0f\n", depth, args->unwind_type, prof.mean, prof.sd);

    return 0;
}

int do_unwind_overhead_all(struct args *args)
{
    int depth;
    int type;

    for (type = 0; type < UNW_TYPE_LAST; type++) {
        for (depth = 0; depth < MAX_DEPTH; depth++) {
            args->unwind_depth = depth;
            args->unwind_type = type;
            do_unwind_overhead_one(args);
        }
    }
    return 0;
}

int do_sampling_overhead(struct args *args)
{
    int dryrun;

    args->iter_main = args->iter_1ms * 1000;
    args->iter_overhead = 0;
    args->disable = DISABLE_EARLY;
    for (dryrun = 0; dryrun < 2; dryrun++) {
        args->dryrun = dryrun;
        do_one(args);
    }
    return 0;
}

int perf_event_open_benchmark(void *arg)
{
    struct counter *cnt = arg;
    open_counter(cnt);
    close_counter(cnt);
    return 0;
}

int do_perf_event_open_overhead(struct args *args)
{
    struct counter cnt = {
        .args = args,
    };
    struct profile prof = {
        .name = "perf_event_open",
        .func = perf_event_open_benchmark,
        .args = &cnt,
        .repeat = 10000,
    };

    profile_combo(&prof);
    profile_stats_print(&prof, stdout);
    return 0;
}

static struct exp exps[] = {
    { "handler-work-vs-hits", do_handler_work, before_run, after_run  },
    { "sampling-overhead", do_sampling_overhead, before_run, after_run  },
    { "unwind_all", do_unwind_overhead_all, before_run, after_run },
    { "unwind", do_unwind_overhead_one, before_run, after_run },
    { "perf_event_open", do_perf_event_open_overhead, before_run, after_run },
    { NULL, NULL },
};

__attribute__((noreturn))
static void usage(void) {
    struct exp *e;
    fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
    fprintf(stderr, "\nOptions:\n\n");
    fprintf(stderr, "--thread N       number of threads to be spawned (default = 1)\n");
    fprintf(stderr, "--repeat         number of repetitions\n");
    fprintf(stderr, "--main           main work (iterations)\n");
    fprintf(stderr, "--overhead       overhead work (iterations)\n");
    fprintf(stderr, "--exp            experiment to run\n");
    fprintf(stderr, "--unwind         unwind type [ online | offline ]\n");
    fprintf(stderr, "--verbose        be verbose\n");
    fprintf(stderr, "--help           print this message and exit\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "available experiments\n");
    for (e = exps; e->name != NULL; e++) {
        fprintf(stderr, "%s\n", e->name);
    }
    exit(EXIT_FAILURE);
}

static void parse_opts(int argc, char **argv, struct args *args) {
    int opt;
    int idx;

    struct option options[] = {
            { "help",       0, 0, 'h' },
            { "repeat",     1, 0, 'r' },
            { "thread",     1, 0, 't' },
            { "main",       1, 0, 'm' },
            { "overhead",   1, 0, 'x' },
            { "disable",    1, 0, 'd' },
            { "period",     1, 0, 'p' },
            { "exp",        1, 0, 'e' },
            { "unwind",     1, 0, 'u' },
            { "verbose",    0, 0, 'v' },
            { 0, 0, 0, 0 }
    };

    memset(args, 0, sizeof(*args));
    args->nr_thread = DEFAULT_THREAD;
    args->repeat = DEFAULT_REPEAT;
    args->exp = DEFAULT_EXP;
    args->iter_main = DEFAULT_ITER_MAIN;
    args->iter_overhead = DEFAULT_ITER_OVERHEAD;
    args->disable = DEFAULT_DISABLE;
    args->period = DEFAULT_PERIOD;
    args->dryrun = DEFAULT_DRYRUN;
    args->unwind_type = DEFAULT_UNWIND;
    args->unwind_depth = DEFAULT_DEPTH;
    args->attr = def_attr;

    while ((opt = getopt_long(argc, argv, "hvr:t:e:m:x:d:p:u:", options, &idx)) != -1) {
        switch (opt) {
        case 'r':
            args->repeat = atoi(optarg);
            break;
        case 't':
            args->nr_thread = atoi(optarg);
            break;
        case 'e':
        {
            struct exp *e;
            for (e = exps; e->name != NULL; e++) {
                if (strcmp(e->name, optarg) == 0) {
                    args->exp = e;
                    break;
                }
            }
            break;
        }
        case 'm':
            args->iter_main = atol(optarg);
            break;
        case 'x':
            args->iter_overhead = atol(optarg);
            break;
        case 'd':
            args->disable = atoi(optarg);
            break;
        case 'p':
            args->period = atoi(optarg);
            break;
        case 'u':
            if (strcmp("online", optarg) == 0) {
                args->unwind_type = UNW_TYPE_ONLINE;
            } else if (strcmp("offline", optarg) == 0) {
                args->unwind_type = UNW_TYPE_OFFLINE;
            }
            break;
        case 'h':
            usage();
            break;
        default:
            usage();
            break;
        }
    }

    if (args->exp == NULL) {
        usage();
    }
    args->attr.sample_period = args->period;
    args->attr.freq = 0; // make sure freq is zero to select sample_period in the union
}

int main(int argc, char **argv) {
    int ret, i;
    char *fname;
    struct args args;
    parse_opts(argc, argv, &args);
    pthread_barrier_init(&barrier, NULL, args.nr_thread);
    install_sighand();

    ret = asprintf(&fname, "%s.csv", args.exp->name);
    assert(ret > 0);
    args.out = fopen(fname, "w");
    assert(args.out != NULL);
    stats_header(&args);

    args.iter_1ms = calibrate(1000);
    for (i = 0; i < args.repeat; i++) {
        args.exp->func(&args);
    }

    fflush(args.out);
    fclose(args.out);
    return ret;
}
