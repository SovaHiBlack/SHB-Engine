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
		Pcstr name;			// low-case name
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
//	DEFINE_MAP_PRED				(Pcstr,FS_Path*,PathMap,PathPairIt,pred_str);
	using PathMap = xr_map<Pcstr, FS_Path*, pred_str>;
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
	void						rescan_path(Pcstr full_path, BOOL bRecurse);
	void						check_pathes( );

	files_set					files;
	archives_vec				archives;
	BOOL						bNoRecurse;

	xrCriticalSection			m_auth_lock;
	U64							m_auth_code;

	void						Register(Pcstr name, U32 vfs, U32 crc, U32 ptr, U32 size_real, U32 size_compressed, U32 modif);
	void						ProcessArchive(Pcstr path, Pcstr base_path = 0);
	void						ProcessOne(Pcstr path, const _finddata_t& F);
	bool						Recurse(Pcstr path);
//	bool						CheckExistance	(Pcstr path);

	files_it					file_find_it(Pcstr n);
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
	void				check_cached_files(char* fname, const file& desc, Pcstr& source_name);

	void				file_from_cache_impl(IReader*& R, char* fname, const file& desc);
	void				file_from_cache_impl(CStreamReader*& R, char* fname, const file& desc);
	template <typename T>
	void				file_from_cache(T*& R, char* fname, const file& desc, Pcstr& source_name);

	void				file_from_archive(IReader*& R, Pcstr fname, const file& desc);
	void				file_from_archive(CStreamReader*& R, Pcstr fname, const file& desc);

	void				copy_file_to_build(IWriter* W, IReader* r);
	void				copy_file_to_build(IWriter* W, CStreamReader* r);
	template <typename T>
	void				copy_file_to_build(T*& R, Pcstr source_name);

	bool				check_for_file(Pcstr path, Pcstr _fname, string_path& fname, const file*& desc);

	template <typename T>
	inline		T* r_open_impl(Pcstr path, Pcstr _fname);
	void				ProcessExternalArch( );
public:
	CLocatorAPI( );
	~CLocatorAPI( );
	void						_initialize(U32 flags, Pcstr target_folder = 0, Pcstr fs_name = 0);
	void						_destroy( );

	CStreamReader* rs_open(Pcstr initial, Pcstr N);
	IReader* r_open(Pcstr initial, Pcstr N);
	inline IReader* r_open(Pcstr N)
	{
		return r_open(0, N);
	}
	void						r_close(IReader*& S);
	void						r_close(CStreamReader*& fs);

	IWriter* w_open(Pcstr initial, Pcstr N);
	inline IWriter* w_open(Pcstr N)
	{
		return w_open(0, N);
	}
	IWriter* w_open_ex(Pcstr initial, Pcstr N);
	inline IWriter* w_open_ex(Pcstr N)
	{
		return w_open_ex(0, N);
	}
	void						w_close(IWriter*& S);

	const file* exist(Pcstr N);
	const file* exist(Pcstr path, Pcstr name);
	const file* exist(string_path& fn, Pcstr path, Pcstr name);
	const file* exist(string_path& fn, Pcstr path, Pcstr name, Pcstr ext);

	BOOL 						can_write_to_folder(Pcstr path);
	BOOL 						can_write_to_alias(Pcstr path);
	BOOL						can_modify_file(Pcstr fname);
	BOOL						can_modify_file(Pcstr path, Pcstr name);

	BOOL 						dir_delete(Pcstr path, Pcstr nm, BOOL remove_files);
	BOOL 						dir_delete(Pcstr full_path, BOOL remove_files)
	{
		return dir_delete(0, full_path, remove_files);
	}
	void 						file_delete(Pcstr path, Pcstr nm);
	void 						file_delete(Pcstr full_path)
	{
		file_delete(0, full_path);
	}
	void 						file_copy(Pcstr src, Pcstr dest);
	void 						file_rename(Pcstr src, Pcstr dest, bool bOwerwrite = true);
	int							file_length(Pcstr src);

	U32  						get_file_age(Pcstr nm);
	void 						set_file_age(Pcstr nm, U32 age);

	xr_vector<char*>* file_list_open(Pcstr initial, Pcstr folder, U32 flags = FS_ListFiles);
	xr_vector<char*>* file_list_open(Pcstr path, U32 flags = FS_ListFiles);
	void						file_list_close(xr_vector<char*>*& lst);

	bool						path_exist(Pcstr path);
	FS_Path* get_path(Pcstr path);
	FS_Path* append_path(Pcstr path_alias, Pcstr root, Pcstr add, BOOL recursive);
	const char* update_path(string_path& dest, Pcstr initial, Pcstr src);

	int							file_list(FS_FileSet& dest, Pcstr path, U32 flags = FS_ListFiles, Pcstr mask = 0);
//.    void						update_path			(xr_string& dest, Pcstr initial, Pcstr src);

	// 
	void						register_archieve(Pcstr path);
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
