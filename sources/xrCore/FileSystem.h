//----------------------------------------------------
// file: FileSystem.h
//----------------------------------------------------
#pragma once

class XRCORE_API EFS_Utils
{
	DEFINE_MAP(xr_string, pvoid, HANDLEMap, HANDLEPairIt);

	HANDLEMap 	m_LockFiles;
public:
	EFS_Utils();
	virtual 	~EFS_Utils();
	void 		_initialize()
	{}
	void 		_destroy()
	{}

	pcstr		GenerateName(pcstr base_path, pcstr base_name, pcstr def_ext, pstr out_name);

	bool 		GetOpenName(pcstr initial, string_path& buffer, int sz_buf, bool bMulti = false, pcstr offset = 0, int start_flt_ext = -1);
	bool 		GetSaveName(pcstr initial, string_path& buffer, pcstr offset = 0, int start_flt_ext = -1);

	pcstr		AppendFolderToName(pstr tex_name, int depth, BOOL full_name);
	pcstr		AppendFolderToName(pcstr src_name, pstr dest_name, int depth, BOOL full_name);

	xr_string	ChangeFileExt(pcstr src, pcstr ext);
	xr_string	ChangeFileExt(const xr_string& src, pcstr ext);

	//xr_string	ExtractFileName(pcstr src);
	//xr_string	ExtractFilePath(pcstr src);
	//xr_string	ExtractFileExt(pcstr src);
	//xr_string	ExcludeBasePath(pcstr full_path, pcstr excl_path);
};
extern XRCORE_API	EFS_Utils* xr_EFS;
#define EFS (*xr_EFS)
