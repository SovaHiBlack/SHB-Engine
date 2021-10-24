//	Module 		: ScriptBinder.h
//	Description : Script objects binder

#pragma once

class CSE_Abstract;
class CScriptBinderObject;
class CNetPacket;

class CScriptBinder
{
protected:
	CScriptBinderObject* m_object;

public:
	CScriptBinder( );
	virtual						~CScriptBinder( );
	void				init( );
	void				clear( );
	virtual void				reinit( );
	virtual void				Load(const char* section);
	virtual void				reload(const char* section);
	virtual BOOL				net_Spawn(CSE_Abstract* DC);
	virtual void				net_Destroy( );
	virtual void				shedule_Update(u32 time_delta);
	virtual void				save(CNetPacket& output_packet);
	virtual void				load(IReader& input_packet);
	virtual BOOL				net_SaveRelevant( );
	virtual void				net_Relcase(CObject* object);
	void				set_object(CScriptBinderObject* object);
	inline CScriptBinderObject* object( );
};

#include "ScriptBinder_inline.h"
