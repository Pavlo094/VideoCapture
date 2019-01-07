#include "log.h"

static hP7_Trace InitTrace()
{
	static hP7_Client hClient = P7_Client_Create(TM("/P7.Sink=FileTxt"));
    //using the client create telemetry & trace channels
    return P7_Trace_Create(hClient, TM("TraceChannel"), NULL);
}

hP7_Trace getGlobalTraceInstance() 
{
	static hP7_Trace g_hTrace = InitTrace();
	return g_hTrace;
}