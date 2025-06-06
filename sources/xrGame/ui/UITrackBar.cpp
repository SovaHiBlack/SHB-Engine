#include "stdafx.h"

#include "UITrackBar.h"
#include "UIFrameLineWnd.h"
#include "UI3tButton.h"
#include "UITextureMaster.h"
#include "../..\XR_3DA\Input.h"

#define DEF_CONTROL_HEIGHT		21
#define FRAME_LINE_TEXTURE		"ui_slider_e"
#define FRAME_LINE_TEXTURE_D	"ui_slider_d"
#define SLIDER_TEXTURE			"ui_slider_button"

CUITrackBar::CUITrackBar( ) : m_f_min(0.0f), m_f_max(1.0f), m_f_val(0.0f), m_f_back_up(0.0f), m_f_step(0.01f), m_b_is_float(true), m_b_invert(false)
{
	m_pFrameLine = xr_new<CUIFrameLineWnd>( );
	AttachChild(m_pFrameLine);
	m_pFrameLine->SetAutoDelete(true);
	m_pFrameLine_d = xr_new<CUIFrameLineWnd>( );
	m_pFrameLine_d->SetVisible(false);
	AttachChild(m_pFrameLine_d);
	m_pFrameLine_d->SetAutoDelete(true);
	m_pSlider = xr_new<CUI3tButton>( );
	AttachChild(m_pSlider);
	m_pSlider->SetAutoDelete(true);
}

bool CUITrackBar::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	CUIWindow::OnMouse(x, y, mouse_action);

	if (m_bCursorOverWindow)
	{
		if (pInput->iGetAsyncBtnState(0))
		{
			UpdatePosRelativeToMouse( );
		}
	}

	return true;
}

void CUITrackBar::Init(f32 x, f32 y, f32 width, f32 height)
{
	string128 buf;
	f32 item_height;
	f32 item_width;
	CUIWindow::Init(x, y, width, DEF_CONTROL_HEIGHT);

	item_height = CUITextureMaster::GetTextureHeight(strconcat(sizeof(buf), buf, FRAME_LINE_TEXTURE, "_b"));
	m_pFrameLine->Init(0, (height - item_height) / 2, width, item_height);
	m_pFrameLine->InitTexture(FRAME_LINE_TEXTURE);
	m_pFrameLine_d->Init(0, (height - item_height) / 2, width, item_height);
	m_pFrameLine_d->InitTexture(FRAME_LINE_TEXTURE_D);

	strconcat(sizeof(buf), buf, SLIDER_TEXTURE, "_e");
	item_width = CUITextureMaster::GetTextureWidth(buf);
	item_height = CUITextureMaster::GetTextureHeight(buf);
	m_pSlider->Init(0, (height - item_height) / 2, item_width, item_height);
	m_pSlider->InitTexture(SLIDER_TEXTURE);
}

void CUITrackBar::SetCurrentValue( )
{
	if (m_b_is_float)
	{
		GetOptFloatValue(m_f_val, m_f_min, m_f_max);
	}
	else
	{
		GetOptIntegerValue(m_i_val, m_i_min, m_i_max);
	}

	UpdatePos( );
}

void CUITrackBar::Draw( )
{
	CUIWindow::Draw( );
}

void CUITrackBar::SaveValue( )
{
	CUIOptionsItem::SaveValue( );
	if (m_b_is_float)
	{
		SaveOptFloatValue(m_f_val);
	}
	else
	{
		SaveOptIntegerValue(m_i_val);
	}
}

bool CUITrackBar::IsChanged( )
{
	if (m_b_is_float)
	{
		return !fsimilar(m_f_back_up, m_f_val);
	}
	else
	{
		return (m_i_back_up != m_i_val);
	}
}

void CUITrackBar::SetStep(f32 step)
{
	if (m_b_is_float)
	{
		m_f_step = step;
	}
	else
	{
		m_i_step = iFloor(step);
	}
}

void CUITrackBar::SeveBackUpValue( )
{
	if (m_b_is_float)
	{
		m_f_back_up = m_f_val;
	}
	else
	{
		m_i_back_up = m_i_val;
	}
}

void CUITrackBar::Undo( )
{
	if (m_b_is_float)
	{
		m_f_val = m_f_back_up;
	}
	else
	{
		m_i_val = m_i_back_up;
	}

	SaveValue( );
	SetCurrentValue( );
}

void CUITrackBar::Enable(bool status)
{
	m_bIsEnabled = status;
	m_pFrameLine->SetVisible(status);
	m_pSlider->Enable(status);
	m_pFrameLine_d->SetVisible(!status);
}

void CUITrackBar::UpdatePosRelativeToMouse( )
{
	f32 _bkf = 0.0f;
	s32 _bki = 0;
	if (m_b_is_float)
	{
		_bkf = m_f_val;
	}
	else
	{
		_bki = m_i_val;
	}

	f32 btn_width = m_pSlider->GetWidth( );
	f32 window_width = GetWidth( );
	f32 fpos = cursor_pos.x;

	if (GetInvert( ))
	{
		fpos = window_width - fpos;
	}

	if (fpos < btn_width / 2)
	{
		fpos = btn_width / 2;
	}
	else if (fpos > window_width - btn_width / 2)
	{
		fpos = window_width - btn_width / 2;
	}

	f32 __fval;
	f32 __fmax = (m_b_is_float) ? m_f_max : (f32) m_i_max;
	f32 __fmin = (m_b_is_float) ? m_f_min : (f32) m_i_min;
	f32 __fstep = (m_b_is_float) ? m_f_step : (f32) m_i_step;

	__fval = (__fmax - __fmin) * (fpos - btn_width / 2) / (window_width - btn_width) + __fmin;

	f32 _d = (__fval - __fmin);

	f32 _v = _d / __fstep;
	s32 _vi = iFloor(_v);
	f32 _vf = __fstep * _vi;

	if (_d - _vf > __fstep / 2.0f)
	{
		_vf += __fstep;
	}

	__fval = __fmin + _vf;

	clamp(__fval, __fmin, __fmax);

	if (m_b_is_float)
	{
		m_f_val = __fval;
	}
	else
	{
		m_i_val = iFloor(__fval);
	}

	bool b_ch = false;
	if (m_b_is_float)
	{
		b_ch = !fsimilar(_bkf, m_f_val);
	}
	else
	{
		b_ch = (_bki != m_i_val);
	}

	if (b_ch)
	{
		GetMessageTarget( )->SendMessage(this, BUTTON_CLICKED, NULL);
	}

	UpdatePos( );
}

void CUITrackBar::UpdatePos( )
{

#ifdef DEBUG
	if (m_b_is_float)
	{
		R_ASSERT2(m_f_val >= m_f_min && m_f_val <= m_f_max, "CUITrackBar::UpdatePos() - m_val >= m_min && m_val <= m_max");
	}
	else
	{
		R_ASSERT2(m_i_val >= m_i_min && m_i_val <= m_i_max, "CUITrackBar::UpdatePos() - m_val >= m_min && m_val <= m_max");
	}
#endif

	f32 btn_width = m_pSlider->GetWidth( );
	f32 window_width = GetWidth( );
	f32 free_space = window_width - btn_width;
	fVector2 pos = m_pSlider->GetWndPos( );

	f32 __fval = (m_b_is_float) ? m_f_val : (f32) m_i_val;
	f32 __fmax = (m_b_is_float) ? m_f_max : (f32) m_i_max;
	f32 __fmin = (m_b_is_float) ? m_f_min : (f32) m_i_min;

	pos.x = (__fval - __fmin) * free_space / (__fmax - __fmin);
	if (GetInvert( ))
	{
		pos.x = free_space - pos.x;
	}

	m_pSlider->SetWndPos(pos);
	SaveValue( );
}

void CUITrackBar::OnMessage(pcstr message)
{
	if (0 == xr_strcmp(message, "set_default_value"))
	{
		if (m_b_is_float)
		{
			m_f_val = m_f_min + (m_f_max - m_f_min) / 2.0f;
		}
		else
		{
			m_i_val = m_i_min + iFloor((m_i_max - m_i_min) / 2.0f);
		}

		UpdatePos( );
	}
}

bool CUITrackBar::GetCheck( )
{
	VERIFY(!m_b_is_float);
	return !!m_i_val;
}

void CUITrackBar::SetCheck(bool b)
{
	VERIFY(!m_b_is_float);
	m_i_val = (b) ? m_i_max : m_i_min;
}
