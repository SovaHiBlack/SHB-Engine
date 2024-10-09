#pragma once

#include "uiwindow.h"
#include "uibutton.h"
#include "../script_export_space.h"

class CUIProgressBar : public CUIWindow
{
	friend class		CUIXmlInit;
	typedef CUIWindow	inherited;
protected:
	bool				m_bIsHorizontal;

	Fvector2			m_ProgressPos; //x-current y-dest
	f32				m_MinPos;
	f32				m_MaxPos;

	f32				m_CurrentLength;
	
	bool				m_bBackgroundPresent;
	Fvector2			m_BackgroundOffset;
	u32					m_last_render_frame;
	void				UpdateProgressBar();
	
public:
	bool				m_bUseColor;
	Fcolor				m_minColor;
	Fcolor				m_maxColor;
	f32				m_inertion;	//
public:
	CUIStatic			m_UIProgressItem;
	CUIStatic			m_UIBackgroundItem;

						CUIProgressBar				();
	virtual				~CUIProgressBar				();

	virtual void		Init						(f32 x, f32 y, f32 width, f32 height, bool bIsHorizontal);

	void				SetRange					(f32 _Min, f32 _Max)	{ m_MinPos = _Min;  m_MaxPos = _Max; UpdateProgressBar();}
	f32				GetRange_min				() 							{ return  m_MinPos;}
	f32				GetRange_max				() 							{ return  m_MaxPos;}

	void				SetProgressPos				(f32 _Pos);
	f32				GetProgressPos				()							{ return m_ProgressPos.y; }

	virtual void		Draw						();
	virtual void		Update						();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIProgressBar)
#undef script_type_list
#define script_type_list save_type_list(CUIProgressBar)
