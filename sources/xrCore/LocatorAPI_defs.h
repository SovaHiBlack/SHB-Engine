#pragma once

enum FS_List
{
	FS_ListFiles = (1 << 0),
	FS_ListFolders = (1 << 1),
	FS_ClampExt = (1 << 2),
	FS_RootOnly = (1 << 3),
	FS_forcedword = u32(-1)
};

class CORE_API FS_Path
{
public:
	enum
	{
		flRecurse = (1 << 0),
		flNotif = (1 << 1),
		flNeedRescan = (1 << 2),
	};
public:
	pstr		m_Path;
	pstr		m_Root;
	pstr		m_Add;
	pstr		m_DefExt;
	pstr		m_FilterCaption;
	flags32		m_Flags;

public:
	FS_Path(pcstr _Root, pcstr _Add, pcstr _DefExt = 0, pcstr _FilterString = 0, u32 flags = 0);
	~FS_Path();
	pcstr		_update(string_path& dest, pcstr src) const;
	//.	void		_update		(xr_string& dest, pcstr src) const;
	void		_set(pstr add);
	void		_set_root(pstr root);

	void __stdcall rescan_path_cb();
};

struct 				_finddata64i32_t;
#define _FINDDATA_T	_finddata64i32_t

struct CORE_API FS_File
{
	enum
	{
		flSubDir = (1 << 0),
		flVFS = (1 << 1),
	};
	unsigned 	attrib;
	time_t	  	time_write;
	long     	size;
	xr_string	name;			// low-case name
	void		set(xr_string nm, long sz, time_t modif, unsigned attr);

public:
	FS_File()
	{}
	FS_File(xr_string nm);
	FS_File(const _FINDDATA_T& f);
	FS_File(xr_string nm, const _FINDDATA_T& f);
	FS_File(xr_string nm, long sz, time_t modif, unsigned attr);
	bool 		operator<	(const FS_File& _X) const
	{
		return xr_strcmp(name.c_str(), _X.name.c_str()) < 0;
	}
};
DEFINE_SET(FS_File, FS_FileSet, FS_FileSetIt);

extern bool	CORE_API PatternMatch(pcstr s, pcstr mask);
