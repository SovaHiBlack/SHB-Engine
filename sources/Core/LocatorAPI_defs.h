#pragma once

enum FS_List
{
	FS_ListFiles = (1 << 0),
	FS_ListFolders = (1 << 1),
	FS_ClampExt = (1 << 2),
	FS_RootOnly = (1 << 3),
	FS_forcedword = U32(-1)
};

class CORE_API FS_Path
{
public:
	enum
	{
		flRecurse = (1 << 0),
		flNotif = (1 << 1),
		flNeedRescan = (1 << 2)
	};

	char* m_Path;
	char* m_Root;
	char* m_Add;
	char* m_DefExt;
	char* m_FilterCaption;
	Flags32							m_Flags;

	FS_Path(const char* _Root, const char* _Add, const char* _DefExt = 0, const char* _FilterString = 0, U32 flags = 0);
	~FS_Path( );
	const char* _update(string_path& dest, const char* src) const;
	void			_set(char* add);
	void			_set_root(char* root);

	void __stdcall	rescan_path_cb( );
};

struct _finddata64i32_t;
#define _FINDDATA_T	_finddata64i32_t

struct CORE_API FS_File
{
	enum
	{
		flSubDir = (1 << 0),
		flVFS = (1 << 1)
	};
	unsigned				attrib;
	time_t					time_write;
	long					size;
	xr_string				name;			// low-case name
	void		set(const xr_string& nm, long sz, time_t modif, unsigned attr);

public:
	FS_File( )
	{ }
	FS_File(const xr_string& nm);
	FS_File(const _FINDDATA_T& f);
	FS_File(const xr_string& nm, const _FINDDATA_T& f);
	FS_File(const xr_string& nm, long sz, time_t modif, unsigned attr);
	bool		operator <	(const FS_File& _X) const
	{
		return xr_strcmp(name.c_str( ), _X.name.c_str( )) < 0;
	}
};

//DEFINE_SET		(FS_File,FS_FileSet,FS_FileSetIt);
using FS_FileSet = xr_set<FS_File>;
using FS_FileSetIt = FS_FileSet::iterator;

extern bool CORE_API PatternMatch(const char* s, const char* mask);
