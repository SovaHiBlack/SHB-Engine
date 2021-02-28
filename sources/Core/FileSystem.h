//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------
#pragma once

class CORE_API EFS_Utils {
//	DEFINE_MAP	(xr_string,void*,HANDLEMap,HANDLEPairIt);
	using HANDLEMap = xr_map<xr_string, void*>;
	using HANDLEPairIt = HANDLEMap::iterator;

	HANDLEMap 	m_LockFiles;
public:
				EFS_Utils		();
	virtual 	~EFS_Utils		();
	void 		_initialize		(){}
	void 		_destroy		(){}

	const char* GenerateName	(const char* base_path, const char* base_name, const char* def_ext, char* out_name);

	bool 		GetOpenName		(const char* initial, string_path& buffer, int sz_buf, bool bMulti=false, const char* offset=0, int start_flt_ext=-1 );
	bool 		GetSaveName		(const char* initial, string_path& buffer, const char* offset=0, int start_flt_ext=-1 );

	const char* AppendFolderToName(char* tex_name, int depth, BOOL full_name);
	const char* AppendFolderToName(const char* src_name, char* dest_name, int depth, BOOL full_name);

	xr_string	ChangeFileExt	(const char* src, const char* ext);
	xr_string	ChangeFileExt	(const xr_string& src, const char* ext);

	xr_string	ExtractFileName		(const char* src);
	xr_string	ExtractFilePath		(const char* src);
	xr_string	ExtractFileExt		(const char* src);
	xr_string	ExcludeBasePath		(const char* full_path, const char* excl_path);
};

extern CORE_API	EFS_Utils*	xr_EFS;
#define EFS (*xr_EFS)
