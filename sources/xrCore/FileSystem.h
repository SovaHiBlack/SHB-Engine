//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------
#pragma once

class XRCORE_API EFS_Utils {
	DEFINE_MAP	(xr_string,void*,HANDLEMap,HANDLEPairIt);

    HANDLEMap 	m_LockFiles;
public:
				EFS_Utils		();
	virtual 	~EFS_Utils		();
	void 		_initialize		(){}
    void 		_destroy		(){}

	pcstr		GenerateName	(pcstr base_path, pcstr base_name, pcstr def_ext, LPSTR out_name);

	bool 		GetOpenName		(pcstr initial, string_path& buffer, int sz_buf, bool bMulti=false, pcstr offset=0, int start_flt_ext=-1 );
	bool 		GetOpenName		(pcstr initial, xr_string& buf, bool bMulti=false, pcstr offset=0, int start_flt_ext=-1 );

	bool 		GetSaveName		(pcstr initial, string_path& buffer, pcstr offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(pcstr initial, xr_string& buf, pcstr offset=0, int start_flt_ext=-1 );

	void 		MarkFile		(pcstr fn, bool bDeleteSource);

	xr_string 	AppendFolderToName(xr_string& tex_name, int depth, BOOL full_name);

	pcstr		AppendFolderToName(LPSTR tex_name, int depth, BOOL full_name);
	pcstr		AppendFolderToName(pcstr src_name, LPSTR dest_name, int depth, BOOL full_name);

	BOOL		LockFile		(pcstr fn, bool bLog=true);
	BOOL		UnlockFile		(pcstr fn, bool bLog=true);
	BOOL		CheckLocking	(pcstr fn, bool bOnlySelf, bool bMsg);//, shared_str* owner=0);
	void 		WriteAccessLog	(pcstr fn, pcstr start_msg);
//.	shared_str 	GetLockOwner	(pcstr initial, pcstr fn);

    xr_string	ChangeFileExt	(pcstr src, pcstr ext);
    xr_string	ChangeFileExt	(const xr_string& src, pcstr ext);

    xr_string	ExtractFileName		(pcstr src);
    xr_string	ExtractFilePath		(pcstr src);
    xr_string	ExtractFileExt		(pcstr src);
    xr_string	ExcludeBasePath		(pcstr full_path, pcstr excl_path);
};
extern XRCORE_API	EFS_Utils*	xr_EFS;
#define EFS (*xr_EFS)
