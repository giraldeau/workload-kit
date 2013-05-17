#define TRACEPOINT_DEFINE
#include "tpw_tp_provider.h"

void tpw_integer1(int value) {
	tracepoint(tpw, integer1, value);
}
