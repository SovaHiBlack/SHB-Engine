#pragma once
#include "UIOptionsManager.h"//

class CUIOptionsItem
{
	friend class CUIOptionsManager;

public:
	virtual					~CUIOptionsItem( );
	virtual void			Register(pcstr entry, pcstr group);
	static CUIOptionsManager* GetOptionsManager( )
	{
		return &m_optionsManager;
	}

protected:
	virtual void			SetCurrentValue( ) = 0;
	virtual void			SaveValue( );

	virtual bool			IsChanged( ) = 0;
	virtual void			SeveBackUpValue( )
	{ }
	virtual void			Undo( )
	{
		SetCurrentValue( );
	}

	void			SendMessage2Group(pcstr group, pcstr message);
	virtual	void			OnMessage(pcstr message);

			// string
	pcstr			GetOptStringValue( );
	void			SaveOptStringValue(pcstr val);
	// integer
	void			GetOptIntegerValue(s32& val, s32& min, s32& max);
	void			SaveOptIntegerValue(s32 val);
	// float
	void			GetOptFloatValue(f32& val, f32& min, f32& max);
	void			SaveOptFloatValue(f32 val);
	// bool
	bool			GetOptBoolValue( );
	void			SaveOptBoolValue(bool val);
	// token
	pstr			GetOptTokenValue( );
	xr_token* GetOptToken( );
	void			SaveOptTokenValue(pcstr val);

	xr_string		m_entry;

	static CUIOptionsManager m_optionsManager;
};
