#include "stdafx.h"
#include "uiprogressbar.h"

CUIProgressBar::CUIProgressBar(void)
{
	m_MinPos				= 1.0f;
	m_MaxPos				= 1.0f+ EPSILON_5;

	Enable					(false);

	m_bBackgroundPresent	= false;
	m_bUseColor				= false;

	AttachChild				(&m_UIBackgroundItem);
	AttachChild				(&m_UIProgressItem);
	m_ProgressPos.x			= 0.0f;
	m_ProgressPos.y			= 0.0f;
	m_inertion				= 0.0f;
	m_last_render_frame		= u32(-1);
}

CUIProgressBar::~CUIProgressBar(void)
{
}

void CUIProgressBar::Init(f32 x, f32 y, f32 width, f32 height, bool bIsHorizontal)
{
	m_bIsHorizontal			= bIsHorizontal;
	CUIWindow::Init			(x,y, width, height);
	UpdateProgressBar		();
}

void CUIProgressBar::UpdateProgressBar()
{
	if( fsimilar(m_MaxPos,m_MinPos) ) m_MaxPos	+= EPSILON_5;

	f32 progressbar_unit = 1/(m_MaxPos-m_MinPos);

	f32 fCurrentLength = m_ProgressPos.x*progressbar_unit;

	if(m_bIsHorizontal)	m_CurrentLength			= GetWidth()*fCurrentLength; 	
	else				m_CurrentLength			= GetHeight()*fCurrentLength; 	

	if(m_bUseColor){
		Fcolor curr;
		curr.lerp							(m_minColor,m_maxColor,fCurrentLength);
		m_UIProgressItem.GetStaticItem		()->SetColor			(curr);
	}
}

void CUIProgressBar::SetProgressPos(f32 _Pos)
{ 
	m_ProgressPos.y		= _Pos; 
	clamp(m_ProgressPos.y,m_MinPos,m_MaxPos);

	if(m_last_render_frame+1 != Device.dwFrame)
		m_ProgressPos.x = m_ProgressPos.y;

	UpdateProgressBar	();
}

f32 _sign(const f32& v)
{
	return (v>0.0f)?+1.0f:-1.0f;
}
void CUIProgressBar::Update()
{
	inherited::Update();
	if(!fsimilar(m_ProgressPos.x, m_ProgressPos.y))
	{
		if( fsimilar(m_MaxPos,m_MinPos) ) m_MaxPos	+= EPSILON_5;	//hack ^(
		f32 _diff				= m_ProgressPos.y - m_ProgressPos.x;
		
		f32 _length			= (m_MaxPos-m_MinPos);
		f32 _val				= _length*(1.0f-m_inertion)*Device.fTimeDelta;

		_val					= _min(_abs(_val), _abs(_diff) );
		_val					*= _sign(_diff);
		m_ProgressPos.x			+= _val;
		UpdateProgressBar		();
	}
}

void CUIProgressBar::Draw()
{
	Frect					rect;
	GetAbsoluteRect			(rect);

	if(m_bBackgroundPresent){
		UI()->PushScissor	(rect);		
		m_UIBackgroundItem.Draw();
		UI()->PopScissor	();
	}

	Frect progress_rect;

	if(m_bIsHorizontal){
		progress_rect.set	(0, 0, m_CurrentLength, GetHeight());
	}else{
		progress_rect.set	(0, GetHeight()-m_CurrentLength,
							GetWidth(), GetHeight());
	}
	
	if(m_CurrentLength>0){
		Fvector2 pos		= m_UIProgressItem.GetWndPos();	
		progress_rect.add	(rect.left + pos.x,rect.top + pos.y);

		UI()->PushScissor	(progress_rect);
		m_UIProgressItem.Draw();
		UI()->PopScissor	();
	}
	m_last_render_frame	= Device.dwFrame;
}