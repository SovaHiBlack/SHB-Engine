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
		const char* name;			// low-case name
		U32						vfs;			// 0xffffffff - standart file
		U32						crc;			// contents CRC
		U32						ptr;			// pointer inside vfs
		U32						size_real;		// 
		U32						size_compressed;// if (size_real==size_compressed) - uncompressed
		U32						modif;			// for editor
	};

private:
	struct	file_pred : public 	std::binary_function<file&, file&, bool>
	{
		inline bool operator()	(const file& x, const file& y) const
		{
			return xr_strcmp(x.name, y.name) < 0;
		}
	};
	struct	archive
	{
		shared_str				path;
		void* hSrcFile;
		void* hSrcMap;
		U32						size;
	};
//	DEFINE_MAP_PRED				(const char*,FS_Path*,PathMap,PathPairIt,pred_str);
	using PathMap = xr_map<const char*, FS_Path*, pred_str>;
	using PathPairIt = PathMap::iterator;

	PathMap						pathes;

//	DEFINE_SET_PRED				(file,files_set,files_it,file_pred);
	using files_set = xr_set<file, file_pred>;
	using files_it = files_set::iterator;

//	DEFINE_VECTOR				(archive,archives_vec,archives_it);
	using archives_vec = xr_vector<archive>;
	using archives_it = archives_vec::iterator;

//	DEFINE_VECTOR				(_finddata_t,FFVec,FFIt);
//	FFVec						rec_files;

	int							m_iLockRescan;
	void						rescan_path(const char* full_path, BOOL bRecurse);
	void						check_pathes( );

	files_set					files;
	archives_vec				archives;
	BOOL						bNoRecurse;

	xrCriticalSection			m_auth_lock;
	U64							m_auth_code;

	void						Register(const char* name, U32 vfs, U32 crc, U32 ptr, U32 size_real, U32 size_compressed, U32 modif);
	void						ProcessArchive(const char* path, const char* base_path = 0);
	void						ProcessOne(const char* path, const _finddata_t& F);
	bool						Recurse(const char* path);
//	bool						CheckExistance	(const char* path);

	files_it					file_find_it(const char* n);

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
	Flags32						m_Flags;
	U32							dwAllocGranularity;
	U32							dwOpenCounter;

private:
	void				check_cached_files(char* fname, const file& desc, const char*& source_name);

	void				file_from_cache_impl(IReader*& R, char* fname, const file& desc);
	void				file_from_cache_impl(CStreamReader*& R, char* fname, const file& desc);
	template <typename T>
	void				file_from_cache(T*& R, char* fname, const file& desc, const char*& source_name);

	void				file_from_archive(IReader*& R, const char* fname, const file& desc);
	void				file_from_archive(CStreamReader*& R, const char* fname, const file& desc);

	void				copy_file_to_build(IWriter* W, IReader* r);
	void				copy_file_to_build(IWriter* W, CStreamReader* r);
	template <typename T>
	void				copy_file_to_build(T*& R, const char* source_name);

	bool				check_for_file(const char* path, const char* _fname, string_path& fname, const file*& desc);

	template <typename T>
	inline		T* r_open_impl(const char* path, const char* _fname);
	void				ProcessExternalArch( );
public:
	CLocatorAPI( );
	~CLocatorAPI( );
	void						_initialize(U32 flags, const char* target_folder = 0, const char* fs_name = 0);
	void						_destroy( );

	CStreamReader* rs_open(const char* initial, const char* N);
	IReader* r_open(const char* initial, const char* N);
	inline IReader* r_open(const char* N)
	{
		return r_open(0, N);
	}
	void						r_close(IReader*& S);
	void						r_close(CStreamReader*& fs);

	IWriter* w_open(const char* initial, const char* N);
	inline IWriter* w_open(const char* N)
	{
		return w_open(0, N);
	}
	IWriter* w_open_ex(const char* initial, const char* N);
	inline IWriter* w_open_ex(const char* N)
	{
		return w_open_ex(0, N);
	}
	void						w_close(IWriter*& S);

	const file* exist(const char* N);
	const file* exist(const char* path, const char* name);
	const file* exist(string_path& fn, const char* path, const char* name);
	const file* exist(string_path& fn, const char* path, const char* name, const char* ext);

	BOOL 						can_write_to_folder(const char* path);
	BOOL 						can_write_to_alias(const char* path);
	BOOL						can_modify_file(const char* fname);
	BOOL						can_modify_file(const char* path, const char* name);

	BOOL 						dir_delete(const char* path, const char* nm, BOOL remove_files);
	BOOL 						dir_delete(const char* full_path, BOOL remove_files)
	{
		return dir_delete(0, full_path, remove_files);
	}
	void 						file_delete(const char* path, const char* nm);
	void 						file_delete(const char* full_path)
	{
		file_delete(0, full_path);
	}
	void 						file_copy(const char* src, const char* dest);
	void 						file_rename(const char* src, const char* dest, bool bOwerwrite = true);
	int							file_length(const char* src);

	U32  						get_file_age(const char* nm);
	void 						set_file_age(const char* nm, U32 age);

	xr_vector<char*>* file_list_open(const char* initial, const char* folder, U32 flags = FS_ListFiles);
	xr_vector<char*>* file_list_open(const char* path, U32 flags = FS_ListFiles);
	void						file_list_close(xr_vector<char*>*& lst);

	bool						path_exist(const char* path);
	FS_Path* get_path(const char* path);
	FS_Path* append_path(const char* path_alias, const char* root, const char* add, BOOL recursive);
	const char* update_path(string_path& dest, const char* initial, const char* src);

	int							file_list(FS_FileSet& dest, const char* path, U32 flags = FS_ListFiles, const char* mask = 0);
//.    void						update_path			(xr_string& dest, const char* initial, const char* src);

	// 
	void						register_archieve(const char* path);
	void						auth_generate(xr_vector<xr_string>& ignore, xr_vector<xr_string>& important);
	U64							auth_get( );
	void						auth_runtime(void*);

	// editor functions
	void						rescan_pathes( );
	void						lock_rescan( );
	void						unlock_rescan( );
};

extern CORE_API CLocatorAPI* xr_FS;
#define FS (*xr_FS)
