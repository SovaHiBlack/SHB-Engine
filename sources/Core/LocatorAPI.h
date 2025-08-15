// LocatorAPI.h: interface for the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#pragma warning(push)
#pragma warning(disable:4995)
#include <io.h>
#pragma warning(pop)

#include "LocatorAPI_defs.h"

class CORE_API CStreamReader;

class CORE_API CLocatorAPI
{
	friend class FS_Path;

public:
	struct	file
	{
		pcstr					name;			// low-case name
		u32						vfs;			// 0xffffffff - standart file
		u32						crc;			// contents CRC
		u32						ptr;			// pointer inside vfs
		u32						size_real;		// 
		u32						size_compressed;// if (size_real==size_compressed) - uncompressed
		u32						modif;			// for editor
	};

private:
	struct	file_pred : public 	std::binary_function<file&, file&, bool>
	{
		IC bool operator()	(const file& x, const file& y) const
		{
			return xr_strcmp(x.name, y.name) < 0;
		}
	};
	struct	archive
	{
		shared_str				path;
		pvoid hSrcFile;
		pvoid hSrcMap;
		u32						size;
	};
	DEFINE_MAP_PRED(pcstr, FS_Path*, PathMap, PathPairIt, pred_str);
	PathMap						pathes;

	DEFINE_SET_PRED(file, files_set, files_it, file_pred);
	DEFINE_VECTOR(archive, archives_vec, archives_it);

	DEFINE_VECTOR(_finddata_t, FFVec, FFIt);
	FFVec						rec_files;

	int							m_iLockRescan;
	void						rescan_path(pcstr full_path, BOOL bRecurse);
	void						check_pathes();

	files_set					files;
	archives_vec				archives;
	BOOL						bNoRecurse;

	xrCriticalSection			m_auth_lock;
	u64							m_auth_code;

	void						Register(pcstr name, u32 vfs, u32 crc, u32 ptr, u32 size_real, u32 size_compressed, u32 modif);
	void						ProcessArchive(pcstr path, pcstr base_path = NULL);
	void						ProcessOne(pcstr path, pvoid F);
	bool						Recurse(pcstr path);

	files_it					file_find_it(pcstr n);
public:
	enum
	{
		flNeedRescan = (1 << 0),
		flBuildCopy = (1 << 1),
		flReady = (1 << 2),
		flEBuildCopy = (1 << 3),
		flEventNotificator = (1 << 4),
		flTargetFolderOnly = (1 << 5),
		flCacheFiles = (1 << 6),
		flScanAppRoot = (1 << 7),
		flNeedCheck = (1 << 8),
		flDumpFileActivity = (1 << 9)
	};
	flags32						m_Flags;
	u32							dwAllocGranularity;
	u32							dwOpenCounter;

private:
	void				check_cached_files(pstr fname, const file& desc, pcstr& source_name);

	void				file_from_cache_impl(IReader*& R, pstr fname, const file& desc);
	void				file_from_cache_impl(CStreamReader*& R, pstr fname, const file& desc);
	template <typename T>
	void				file_from_cache(T*& R, pstr fname, const file& desc, pcstr& source_name);

	void				file_from_archive(IReader*& R, pcstr fname, const file& desc);
	void				file_from_archive(CStreamReader*& R, pcstr fname, const file& desc);

	void				copy_file_to_build(IWriter* W, IReader* r);
	void				copy_file_to_build(IWriter* W, CStreamReader* r);
	template <typename T>
	void				copy_file_to_build(T*& R, pcstr source_name);

	bool				check_for_file(pcstr path, pcstr _fname, string_path& fname, const file*& desc);

	template <typename T>
	IC		T* r_open_impl(pcstr path, pcstr _fname);
	void				ProcessExternalArch();
public:
	CLocatorAPI();
	~CLocatorAPI();
	void						_initialize(u32 flags, pcstr target_folder = 0, pcstr fs_name = 0);
	void						_destroy();

	CStreamReader* rs_open(pcstr initial, pcstr N);
	IReader* r_open(pcstr initial, pcstr N);
	IC IReader* r_open(pcstr N)
	{
		return r_open(0, N);
	}
	void						r_close(IReader*& S);
	void						r_close(CStreamReader*& fs);

	IWriter* w_open(pcstr initial, pcstr N);
	IC IWriter* w_open(pcstr N)
	{
		return w_open(0, N);
	}
	IWriter* w_open_ex(pcstr initial, pcstr N);
	IC IWriter* w_open_ex(pcstr N)
	{
		return w_open_ex(0, N);
	}
	void						w_close(IWriter*& S);

	const file* exist(pcstr N);
	const file* exist(pcstr path, pcstr name);
	const file* exist(string_path& fn, pcstr path, pcstr name);
	const file* exist(string_path& fn, pcstr path, pcstr name, pcstr ext);

	BOOL 						can_write_to_folder(pcstr path);
	BOOL 						can_write_to_alias(pcstr path);
	BOOL						can_modify_file(pcstr fname);
	BOOL						can_modify_file(pcstr path, pcstr name);

	BOOL 						dir_delete(pcstr path, pcstr nm, BOOL remove_files);
	BOOL 						dir_delete(pcstr full_path, BOOL remove_files)
	{
		return dir_delete(0, full_path, remove_files);
	}
	void 						file_delete(pcstr path, pcstr nm);
	void 						file_delete(pcstr full_path)
	{
		file_delete(0, full_path);
	}
	void 						file_copy(pcstr src, pcstr dest);
	void 						file_rename(pcstr src, pcstr dest, bool bOwerwrite = true);
	int							file_length(pcstr src);

	u32  						get_file_age(pcstr nm);
	void 						set_file_age(pcstr nm, u32 age);

	xr_vector<pstr>* file_list_open(pcstr initial, pcstr folder, u32 flags = FS_ListFiles);
	xr_vector<pstr>* file_list_open(pcstr path, u32 flags = FS_ListFiles);
	void						file_list_close(xr_vector<pstr>*& lst);

	bool						path_exist(pcstr path);
	FS_Path* get_path(pcstr path);
	FS_Path* append_path(pcstr path_alias, pcstr root, pcstr add, BOOL recursive);
	pcstr						update_path(string_path& dest, pcstr initial, pcstr src);

	int							file_list(FS_FileSet& dest, pcstr path, u32 flags = FS_ListFiles, pcstr mask = 0);
	//.    void						update_path			(xr_string& dest, pcstr initial, pcstr src);

		// 
	void						register_archieve(pcstr path);
	void						auth_generate(xr_vector<xr_string>& ignore, xr_vector<xr_string>& important);
	u64							auth_get();
	void						auth_runtime(pvoid);

	// editor functions
	void						rescan_pathes();
	void						lock_rescan();
	void						unlock_rescan();
};

extern CORE_API	CLocatorAPI* xr_FS;
#define FS (*xr_FS)
