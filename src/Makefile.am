bin_PROGRAMS = wk-ioburst wk-deadlock wk-dthread wk-inception wk-rpc-client \
               wk-rpc-server wk-lockfight wk-cpm1 wk-cpm2 wk-cpm3 wk-pipeline \
               wk-mutex wk-imbalance wk-reparent wk-pipette wk-pulse wk-schedfreq \
               wk-copymm wk-evfreq wk-sigbench wk-signest wk-timemalloc wk-perfuser

wk_perfuser_SOURCES = perfuser.c
wk_perfuser_CFLAGS = $(UST_CFLAGS)
wk_perfuser_LDADD = libwk.a -lm $(UNWIND_LIBS) $(UST_LIBS)
wk_perfuser_LDFLAGS = $(UST_LDFLAGS)

wk_timemalloc_SOURCES = timemalloc.c
wk_timemalloc_LDADD = libwk.a -lm

wk_signest_SOURCES = signest.c
wk_signest_CFLAGS = -fno-omit-frame-pointer -rdynamic

wk_sigbench_SOURCES = sigbench.c
wk_sigbench_CFLAGS = -fopenmp
wk_sigbench_LDADD = libwk.a -lm -lgomp

wk_schedfreq_SOURCES = schedfreq.c

wk_copymm_SOURCES = copymm.c
wk_copymm_LDADD = libwk.a -lm

wk_evfreq_SOURCES = evfreq.c
wk_evfreq_LDADD = libwk.a -lm

wk_pulse_SOURCES = pulse.c
wk_pulse_LDADD = libwk.a -lm

wk_ioburst_SOURCES = ioburst.c
wk_ioburst_LDADD = libwk.a -lm

wk_deadlock_SOURCES = deadlock.c

wk_dthread_SOURCES = dthread.c

wk_inception_SOURCES = inception.c
wk_inception_LDADD = libwk.a -lm

wk_rpc_client_SOURCES = rpc-client.c rpc.h
wk_rpc_client_LDADD = libwk.a -lm

wk_rpc_server_SOURCES = rpc-server.c rpc.h
wk_rpc_server_LDADD = libwk.a -lm

wk_lockfight_SOURCES = lockfight.c
wk_lockfight_LDADD = libwk.a -lm

wk_cpm1_SOURCES = cpm1.c
wk_cpm1_LDADD = libwk.a -lm

wk_cpm2_SOURCES = cpm2.c
wk_cpm2_LDADD = libwk.a -lm

wk_cpm3_SOURCES = cpm3.c
wk_cpm3_LDADD = libwk.a -lm

wk_pipeline_SOURCES = pipeline.c

wk_mutex_SOURCES = mutex.c

wk_imbalance_SOURCES = imbalance.c
wk_imbalance_LDADD = libwk.a

wk_reparent_SOURCES = reparent.c

wk_pipette_SOURCES = pipette.c
wk_pipette_LDADD = libwk.a -lm

noinst_LIBRARIES = libwk.a
libwk_a_SOURCES = calibrate.h calibrate.c spin.c spin.h utils.c utils.h pincpu.c pincpu.h
