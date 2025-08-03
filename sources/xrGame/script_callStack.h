#pragma once

class CScriptDebugger;
struct SPath{
	string_path path;
};

class CScriptCallStack
{
public:
	CScriptDebugger*			m_debugger;
	void GotoStackTraceLevel	(s32 nLevel);
	void Add					(pcstr szDesc, pcstr szFile, s32 nLine);
	void Clear					();
	CScriptCallStack			(CScriptDebugger* d);
	~CScriptCallStack			();

	s32		GetLevel			()			{ return m_nCurrentLevel; };
	void	SetStackTraceLevel	(s32);

protected:
	s32							m_nCurrentLevel;
	xr_vector<u32>				m_levels;
	xr_vector<u32>				m_lines;
	xr_vector<SPath>			m_files;
};
