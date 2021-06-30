#pragma once

#define VPUSH(a)	a.x,a.y,a.z

void 	CORE_API	__cdecl		Msg(Pcstr format, ...);
void 	CORE_API		Log(Pcstr msg);
void 	CORE_API		Log(Pcstr msg, Pcstr dop);
void 	CORE_API		Log(Pcstr msg, U32			dop);
void 	CORE_API		Log(Pcstr msg, int  			dop);
void 	CORE_API		Log(Pcstr msg, float			dop);
void 	CORE_API		Log(Pcstr msg, const Fvector3& dop);
void 	CORE_API		Log(Pcstr msg, const Fmatrix& dop);
void 	CORE_API		LogWinErr(Pcstr msg, long 			err_code);

typedef void	(*LogCallback)	(Pcstr string);
void	CORE_API				SetLogCB(LogCallback cb);
void 							CreateLog(BOOL no_log = FALSE);
void 							InitLog( );
void 							CloseLog( );
void	CORE_API				FlushLog( );

extern 	CORE_API	xr_vector<shared_str>* LogFile;
extern 	CORE_API	BOOL						LogExecCB;
