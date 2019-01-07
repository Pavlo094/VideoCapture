#include "GTypes.h"
#include "P7_Cproxy.h"

#define P7_CRITICAL(i_pFormat, ...)\
    P7_TRACE_ADD(getGlobalTraceInstance(), 0, P7_TRACE_LEVEL_CRITICAL, 0, i_pFormat, __VA_ARGS__)

hP7_Trace getGlobalTraceInstance();