#pragma once

#define VPUSH(a)	a.x,a.y,a.z

void 	CORE_API	__cdecl		Msg(const char* format, ...);
void 	CORE_API		Log(const char* msg);
void 	CORE_API		Log(const char* msg, const char* dop);
void 	CORE_API		Log(const char* msg, unsigned int			dop);
void 	CORE_API		Log(const char* msg, int  			dop);
void 	CORE_API		Log(const char* msg, float			dop);
void 	CORE_API		Log(const char* msg, const Fvector3& dop);
void 	CORE_API		Log(const char* msg, const Fmatrix& dop);

typedef void	(*LogCallback)	(const char* string);
void	CORE_API				SetLogCB(LogCallback cb);
void 							CreateLog(BOOL no_log = FALSE);
void 							InitLog( );
void 							CloseLog( );
void	CORE_API				FlushLog( );

extern 	CORE_API	xr_vector<CSharedString>* LogFile;
extern 	CORE_API	BOOL						LogExecCB;
