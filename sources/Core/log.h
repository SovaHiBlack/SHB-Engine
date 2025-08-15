#pragma once

#define VPUSH(a)							a.x, a.y, a.z

void CORE_API __cdecl		Msg				(pcstr format, ...);
void CORE_API				Log				(pcstr msg);
void CORE_API				Log				(pcstr msg, pcstr dop);
void CORE_API				Log				(pcstr msg, u32 dop);
void CORE_API				Log				(pcstr msg, s32 dop);
void CORE_API				Log				(pcstr msg, f32 dop);
void CORE_API				Log				(pcstr msg, const fVector3& dop);
void CORE_API				Log				(pcstr msg, const fMatrix4x4& dop);
void CORE_API				LogWinErr		(pcstr msg, long err_code);

typedef void				(*LogCallback)	(pcstr string);
void CORE_API				SetLogCB		(LogCallback cb);
void						CreateLog		(BOOL no_log = FALSE);
void						InitLog			();
void						CloseLog		();
void CORE_API				FlushLog		();

extern CORE_API xr_vector<shared_str>*		LogFile;
extern CORE_API BOOL						LogExecCB;
