#!/usr/bin/python
# do experiment for buffer size

# size power of two
subbuf_size_default=18
subbuf_size_min=12
subbuf_size_max=22

num_subbuf_default=4
num_subbuf_min=0
num_subbuf_max=6

def do_trace(subbuf_size, num_subbuf):
	print "do_trace %s %s" % (subbuf_size, num_subbuf)

def do_experiment():
	for s in range(subbuf_size_min, subbuf_size_max+1):
		byte_size = 1 << s
		for n in range(num_subbuf_min, num_subbuf_max + 1):
			num_subbuf = 1 << n
			do_trace(byte_size, num_subbuf)

if __name__=="__main__":
	do_experiment()

