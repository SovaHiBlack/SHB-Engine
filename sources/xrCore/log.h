#pragma once

#define VPUSH(a)	a.x,a.y,a.z

void 	XRCORE_API	__cdecl		Msg(pcstr format, ...);
void 	XRCORE_API		Log(pcstr msg);
void 	XRCORE_API		Log(pcstr msg, pcstr			dop);
void 	XRCORE_API		Log(pcstr msg, u32			dop);
void 	XRCORE_API		Log(pcstr msg, int  			dop);
void 	XRCORE_API		Log(pcstr msg, F32			dop);
void 	XRCORE_API		Log(pcstr msg, const Fvector& dop);
void 	XRCORE_API		Log(pcstr msg, const Fmatrix& dop);
void 	XRCORE_API		LogWinErr(pcstr msg, long 			err_code);

typedef void	(*LogCallback)	(pcstr string);
void	XRCORE_API				SetLogCB(LogCallback cb);
void 							CreateLog(BOOL no_log = FALSE);
void 							InitLog();
void 							CloseLog();
void	XRCORE_API				FlushLog();

extern 	XRCORE_API	xr_vector<shared_str>* LogFile;
extern 	XRCORE_API	BOOL						LogExecCB;
