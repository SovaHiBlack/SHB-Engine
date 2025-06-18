#include "stdafx.h"
#include "uistatic.h"
#include "UIXmlInit.h"
#include "UITextureMaster.h"
#include "uiframewindow.h"
#include "../HUDManager.h"
#include "../..\XR_3DA\LightAnimLibrary.h"
#include "uilines.h"
#include "../StringTable.h"
#include "ui_base.h"

pcstr clDefault = "default";

#define CREATE_LINES if (!m_pLines) {m_pLines = xr_new<CUILines>(); m_pLines->SetTextAlignment(CGameFont::alLeft);}
#define LA_CYCLIC			(1<<0)
#define LA_ONLYALPHA		(1<<1)
#define LA_TEXTCOLOR		(1<<2)
#define LA_TEXTURECOLOR		(1<<3)
//(1<<4) registered !!!
void lanim_cont::set_defaults( )
{
	m_lanim = NULL;
	m_lanim_start_time = -1.0f;
	m_lanim_delay_time = 0.0f;
	m_lanimFlags.zero( );
}

CUIStatic::CUIStatic( )
{
	m_bAvailableTexture = false;
	m_bTextureEnable = true;
	m_bClipper = false;
	m_bStretchTexture = false;

	m_TextureOffset.set(0.0f, 0.0f);
	m_TextOffset.set(0.0f, 0.0f);

	m_pMask = NULL;
	m_ElipsisPos = eepNone;
	m_iElipsisIndent = 0;

	m_ClipRect.set(-1.0f, -1.0f, -1.0f, -1.0f);

	m_bCursorOverWindow = false;
	m_bHeading = false;
	m_fHeading = 0.0f;
	m_lanim_clr.set_defaults( );
	m_lanim_xform.set_defaults( );

	m_pLines = NULL;
	m_bEnableTextHighlighting = false;
}

CUIStatic::~CUIStatic( )
{
	xr_delete(m_pLines);
}

void CUIStatic::SetXformLightAnim(pcstr lanim, bool bCyclic)
{
	if (lanim && lanim[0] != 0)
	{
		m_lanim_xform.m_lanim = LALib.FindItem(lanim);
	}
	else
	{
		m_lanim_xform.m_lanim = NULL;
	}

	m_lanim_xform.m_lanimFlags.zero( );
	m_lanim_xform.m_lanimFlags.set(LA_CYCLIC, bCyclic);
}

void CUIStatic::SetClrLightAnim(pcstr lanim, bool bCyclic, bool bOnlyAlpha, bool bTextColor, bool bTextureColor)
{
	if (lanim && lanim[0] != 0)
	{
		m_lanim_clr.m_lanim = LALib.FindItem(lanim);
	}
	else
	{
		m_lanim_clr.m_lanim = NULL;
	}

	m_lanim_clr.m_lanimFlags.zero( );
	m_lanim_clr.m_lanimFlags.set(LA_CYCLIC, bCyclic);
	m_lanim_clr.m_lanimFlags.set(LA_ONLYALPHA, bOnlyAlpha);
	m_lanim_clr.m_lanimFlags.set(LA_TEXTCOLOR, bTextColor);
	m_lanim_clr.m_lanimFlags.set(LA_TEXTURECOLOR, bTextureColor);
}

void CUIStatic::Init(pcstr tex_name, f32 x, f32 y, f32 width, f32 height)
{
	Init(x, y, width, height);
	InitTexture(tex_name);
}

void CUIStatic::InitEx(pcstr tex_name, pcstr sh_name, f32 x, f32 y, f32 width, f32 height)
{
	Init(x, y, width, height);
	InitTextureEx(tex_name, sh_name);
}

void CUIStatic::Init(f32 x, f32 y, f32 width, f32 height)
{
	CUIWindow::Init(x, y, width, height);
	m_xxxRect.set(x, y, x + width, y + height);
}

void CUIStatic::InitTexture(pcstr texture)
{
	InitTextureEx(texture);
}

void CUIStatic::CreateShader(pcstr tex, pcstr sh)
{
	m_UIStaticItem.CreateShader(tex, sh);
}

ref_shader& CUIStatic::GetShader( )
{
	return m_UIStaticItem.GetShader( );
}

void CUIStatic::SetTextureColor(u32 color)
{
	m_UIStaticItem.SetColor(color);
}

u32 CUIStatic::GetTextureColor( ) const
{
	return m_UIStaticItem.GetColor( );
}

void CUIStatic::InitTextureEx(pcstr tex_name, pcstr sh_name)
{
	string_path buff;
	u32 v_dev = CAP_VERSION(HW.Caps.raster_major, HW.Caps.raster_minor);
	u32 v_need = CAP_VERSION(2, 0);
	if ((v_dev >= v_need) && FS.exist(buff, "$game_textures$", tex_name, ".ogm"))
	{
		CUITextureMaster::InitTexture(tex_name, "hud\\movie", &m_UIStaticItem);
	}
	else
	{
		CUITextureMaster::InitTexture(tex_name, sh_name, &m_UIStaticItem);
	}

	fVector2 p = GetWndPos( );
	m_UIStaticItem.SetPos(p.x, p.y);
	m_bAvailableTexture = true;
}

void CUIStatic::Draw( )
{
	if (m_bClipper)
	{
		fRect clip_rect;
		if (-1 == m_ClipRect.left && -1 == m_ClipRect.right && -1 == m_ClipRect.top && -1 == m_ClipRect.left)
		{
			fRect our_rect;
			GetAbsoluteRect(our_rect);
			clip_rect = our_rect;
			fRect _r;
			GetParent( )->GetAbsoluteRect(_r);
			if (GetParent( ))
			{
				clip_rect.intersection(our_rect, _r);
			}
		}
		else
		{
			clip_rect = m_ClipRect;
		}

		UI( )->PushScissor(clip_rect);
	}

	DrawTexture( );
	inherited::Draw( );
	DrawText( );

	if (m_bClipper)
	{
		UI( )->PopScissor( );
	}
}

void CUIStatic::DrawText( )
{
	if (m_pLines)
	{
		m_pLines->SetWndSize(m_wndSize);

		if (IsHighlightText( ) && xr_strlen(m_pLines->GetText( )) > 0 && m_bEnableTextHighlighting)
		{
			DrawHighlightedText( );
		}
		else
		{
			fVector2 p;
			GetAbsolutePos(p);
			m_pLines->Draw(p.x + m_TextOffset.x, p.y + m_TextOffset.y);
		}
	}
}

void CUIStatic::DrawTexture( )
{
	if (m_bAvailableTexture && m_bTextureEnable)
	{
		fRect rect;
		GetAbsoluteRect(rect);
		m_UIStaticItem.SetPos(rect.left + m_TextureOffset.x, rect.top + m_TextureOffset.y);

		if (m_bStretchTexture)
		{
			m_UIStaticItem.SetRect(0, 0, rect.width( ), rect.height( ));
		}
		else
		{
			fRect r = { 0.0f,0.0f, m_UIStaticItem.GetOriginalRectScaled( ).width( ), m_UIStaticItem.GetOriginalRectScaled( ).height( ) };
			if (r.width( ) && r.height( ))
			{
				m_UIStaticItem.SetRect(r);
			}
		}

		if (Heading( ))
		{
			m_UIStaticItem.Render(GetHeading( ));
		}
		else
		{
			m_UIStaticItem.Render( );
		}
	}
}

void CUIStatic::Update( )
{
	inherited::Update( );
	//update light animation if defined
	if (m_lanim_clr.m_lanim)
	{
		if (m_lanim_clr.m_lanim_start_time < 0.0f)
		{
			ResetClrAnimation( );
		}

		f32 t = Device.dwTimeContinual / 1000.0f;

		if (t < m_lanim_clr.m_lanim_start_time)	// consider animation delay
		{
			return;
		}

		if (m_lanim_clr.m_lanimFlags.test(LA_CYCLIC) || t - m_lanim_clr.m_lanim_start_time < m_lanim_clr.m_lanim->Length_sec( ))
		{
			s32 frame;
			u32 clr = m_lanim_clr.m_lanim->CalculateRGB(t - m_lanim_clr.m_lanim_start_time, frame);

			if (m_lanim_clr.m_lanimFlags.test(LA_TEXTURECOLOR))
			{
				if (m_lanim_clr.m_lanimFlags.test(LA_ONLYALPHA))
				{
					SetColor(subst_alpha(GetColor( ), color_get_A(clr)));
				}
				else
				{
					SetColor(clr);
				}
			}

			if (m_lanim_clr.m_lanimFlags.test(LA_TEXTCOLOR))
			{
				if (m_lanim_clr.m_lanimFlags.test(LA_ONLYALPHA))
				{
					SetTextColor(subst_alpha(GetTextColor( ), color_get_A(clr)));
				}
				else
				{
					SetTextColor(clr);
				}
			}
		}
	}

	if (m_lanim_xform.m_lanim)
	{
		if (m_lanim_xform.m_lanim_start_time < 0.0f)
		{
			ResetXformAnimation( );
		}

		f32 t = Device.dwTimeContinual / 1000.0f;

		if (m_lanim_xform.m_lanimFlags.test(LA_CYCLIC) || t - m_lanim_xform.m_lanim_start_time < m_lanim_xform.m_lanim->Length_sec( ))
		{
			s32 frame;
			u32 clr = m_lanim_xform.m_lanim->CalculateRGB(t - m_lanim_xform.m_lanim_start_time, frame);

			EnableHeading_int(true);
			f32 heading = (PI_MUL_2 / 255.0f) * color_get_A(clr);
			SetHeading(heading);

			f32 _value = (f32) color_get_R(clr);

			f32 f_scale = _value / 64.0f;
			fVector2 _sz;
			_sz.set(m_xxxRect.width( ) * f_scale, m_xxxRect.height( ) * f_scale);
			SetWndSize(_sz);
		}
		else
		{
			EnableHeading_int(!!m_lanim_xform.m_lanimFlags.test(1 << 4));
			SetWndSize(fVector2( ).set(m_xxxRect.width( ), m_xxxRect.height( )));
		}
	}
}

void CUIStatic::ResetXformAnimation( )
{
	m_lanim_xform.m_lanim_start_time = Device.dwTimeContinual / 1000.0f;
}

void CUIStatic::ResetClrAnimation( )
{
	m_lanim_clr.m_lanim_start_time = Device.dwTimeContinual / 1000.0f + m_lanim_clr.m_lanim_delay_time / 1000.0f;
}

void CUIStatic::SetClrAnimDelay(f32 delay)
{
	m_lanim_clr.m_lanim_delay_time = delay;
}

bool CUIStatic::IsClrAnimStoped( )
{
	if (m_lanim_clr.m_lanimFlags.test(LA_CYCLIC) || m_lanim_clr.m_lanim_start_time < 0.0f)
	{
		return false;
	}

	f32 t = Device.dwTimeContinual / 1000.0f;
	if (t - m_lanim_clr.m_lanim_start_time < m_lanim_clr.m_lanim->Length_sec( ))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CUIStatic::SetFont(CGameFont* pFont)
{
	CUIWindow::SetFont(pFont);
	CREATE_LINES;
	m_pLines->SetFont(pFont);
}

void CUIStatic::SetTextComplexMode(bool md)
{
	CREATE_LINES;
	m_pLines->SetTextComplexMode(md);
}

CGameFont* CUIStatic::GetFont( )
{
	CREATE_LINES;
	return m_pLines->GetFont( );
}

void CUIStatic::TextureClipper(f32 offset_x, f32 offset_y, fRect* pClipRect)
{
	TextureClipper(offset_x, offset_y, pClipRect, m_UIStaticItem);
}

void CUIStatic::TextureClipper(f32 offset_x, f32 offset_y, fRect* pClipRect, CUIStaticItem& UIStaticItem)
{
	fRect parent_rect;

	if (pClipRect == NULL)
	{
		if (GetParent( ))
		{
			GetParent( )->GetAbsoluteRect(parent_rect);
		}
		else
		{
			GetAbsoluteRect(parent_rect);
		}
	}
	else
	{
		parent_rect = *pClipRect;
	}

	fRect rect;
	GetAbsoluteRect(rect);
	fRect out_rect;

	//проверить попадает ли изображение в окно
	if (rect.left > parent_rect.right || rect.right<parent_rect.left || rect.top>parent_rect.bottom || rect.bottom < parent_rect.top)
	{
		fRect r;
		r.set(0.0f, 0.0f, 0.0f, 0.0f);
		UIStaticItem.SetRect(r);
		return;
	}

	f32 out_x, out_y;
	out_x = rect.left;
	out_y = rect.top;

	// out_rect - прямоугольная область в которую будет выводиться
	// изображение, вычисляется с учетом положения относительно родительского
	// окна, а также размеров прямоугольника на текстуре с изображением.

	out_rect.intersection(parent_rect, rect);
	out_rect.left -= out_x;
	out_rect.top -= out_y;
	out_rect.right -= out_x;
	out_rect.bottom -= out_y;

	if (m_bStretchTexture)
	{
		UIStaticItem.SetRect(out_rect);
	}
	else
	{
		fRect r;
		r.x1 = out_rect.left;
		r.x2 = out_rect.right < UIStaticItem.GetOriginalRectScaled( ).width( ) ? out_rect.right : UIStaticItem.GetOriginalRectScaled( ).width( );

		r.y1 = out_rect.top;
		r.y2 = out_rect.bottom < UIStaticItem.GetOriginalRectScaled( ).height( ) ? out_rect.bottom : UIStaticItem.GetOriginalRectScaled( ).height( );

		UIStaticItem.SetRect(r);
	}
}

void CUIStatic::ClipperOn( )
{
	m_bClipper = true;

	TextureClipper(0.0f, 0.0f);
}

void CUIStatic::ClipperOff(CUIStaticItem& UIStaticItem)
{
	m_bClipper = false;

	fRect out_rect;

	out_rect.top = 0;
	out_rect.bottom = GetHeight( );
	out_rect.left = 0;
	out_rect.right = GetWidth( );

	fRect r;
	r.x1 = out_rect.left;
	r.x2 = out_rect.right < UIStaticItem.GetOriginalRectScaled( ).width( ) ? out_rect.right : UIStaticItem.GetOriginalRectScaled( ).width( );

	r.y1 = out_rect.top;
	r.y2 = out_rect.bottom < UIStaticItem.GetOriginalRectScaled( ).height( ) ? out_rect.bottom : UIStaticItem.GetOriginalRectScaled( ).height( );
	UIStaticItem.SetRect(r);
}

void CUIStatic::ClipperOff( )
{
	ClipperOff(m_UIStaticItem);
}

void CUIStatic::SetShader(const ref_shader& sh)
{
	m_UIStaticItem.SetShader(sh);
	m_bAvailableTexture = true;
}

pcstr CUIStatic::GetText( )
{
	static const char empty = 0;
	if (m_pLines)
	{
		return m_pLines->GetText( );
	}
	else
	{
		return &empty;
	}
}

void CUIStatic::SetTextColor(u32 color)
{
	CREATE_LINES;
	m_pLines->SetTextColor(color);
}

u32 CUIStatic::GetTextColor( )
{
	CREATE_LINES;
	return m_pLines->GetTextColor( );
}

u32& CUIStatic::GetTextColorRef( )
{
	return m_pLines->GetTextColorRef( );
}

void CUIStatic::SetText(pcstr str)
{
	if (!str)
	{
		return;
	}

	CREATE_LINES;
	m_pLines->SetText(str);
}

void CUIStatic::SetTextColor(u32 color, E4States state)
{
	m_dwTextColor[state] = color;
	m_bUseTextColor[state] = true;
}

fRect CUIStatic::GetClipperRect( )
{
	if (m_bClipper)
	{
		return m_ClipRect;
	}
	else
	{
		return GetSelfClipRect( );
	}
}

fRect CUIStatic::GetSelfClipRect( )
{
	fRect	r;
	if (m_bClipper)
	{
		r.set(GetUIStaticItem( ).GetRect( ));
		r.add(GetUIStaticItem( ).GetPosX( ), GetUIStaticItem( ).GetPosY( ));
	}
	else
	{
		r.set(0.0f, 0.0f, UI_BASE_WIDTH, UI_BASE_HEIGHT);
	}

	return r;
}

void CUIStatic::SetMask(CUIFrameWindow* pMask)
{
	DetachChild(m_pMask);
	m_pMask = pMask;
	if (m_pMask)
	{
		AttachChild(m_pMask);
		fRect r = GetWndRect( );
		m_pMask->SetWidth(r.right - r.left);
		m_pMask->SetHeight(r.bottom - r.top);
	}
}

ETextAlignment CUIStatic::GetTextAlignment( )
{
	return m_pLines->GetTextAlignment( );
}

void CUIStatic::SetTextAlignment(ETextAlignment align)
{
	CREATE_LINES;
	m_pLines->SetTextAlignment(align);
	m_pLines->GetFont( )->SetAligment((ETextAlignment) align);
}

void CUIStatic::SetVTextAlignment(EVTextAlignment al)
{
	CREATE_LINES;
	m_pLines->SetVTextAlignment(al);
}

void CUIStatic::SetTextAlign_script(u32 align)
{
	m_pLines->SetTextAlignment((ETextAlignment) align);
	m_pLines->GetFont( )->SetAligment((ETextAlignment) align);
}

u32 CUIStatic::GetTextAlign_script( )
{
	return static_cast<u32>(m_pLines->GetTextAlignment( ));
}

void CUIStatic::Elipsis(const fRect& rect, EElipsisPosition elipsisPos)
{
#pragma todo("Satan->Satan : need adaptation")
	//if (eepNone == elipsisPos) return;

	//CUIStatic::Elipsis(m_sEdit, rect, elipsisPos, GetFont());

	//// Now paste elipsis
	//m_str = &m_sEdit.front();
	//str_len = m_sEdit.size();
	//buf_str.resize(str_len + 1);
}

void CUIStatic::SetElipsis(EElipsisPosition pos, s32 indent)
{
#pragma todo("Satan->Satan : need adaptation")
	m_ElipsisPos = pos;
	m_iElipsisIndent = indent;
}

void CUIStatic::SetClipRect(fRect r)
{
	m_ClipRect = r;
}

void CUIStatic::OnFocusReceive( )
{
	inherited::OnFocusReceive( );
	if (GetMessageTarget( ))
	{
		GetMessageTarget( )->SendMessage(this, STATIC_FOCUS_RECEIVED, NULL);
	}
}

void CUIStatic::OnFocusLost( )
{
	inherited::OnFocusLost( );
	if (GetMessageTarget( ))
	{
		GetMessageTarget( )->SendMessage(this, STATIC_FOCUS_LOST, NULL);
	}
}

void CUIStatic::AdjustHeightToText( )
{
	m_pLines->SetWidth(GetWidth( ));
	m_pLines->ParseText( );
	SetHeight(m_pLines->GetVisibleHeight( ));
}

void CUIStatic::AdjustWidthToText( )
{
	f32 _len = m_pLines->GetFont( )->SizeOf_(m_pLines->GetText( ));
	UI( )->ClientToScreenScaledWidth(_len);
	SetWidth(_len);
}

void CUIStatic::RescaleRelative2Rect(const fRect& r)
{
	SetStretchTexture(true);
	fRect my_r = m_xxxRect;
	f32 h_rel = my_r.width( ) / r.width( );
	f32 v_rel = my_r.height( ) / r.height( );

	if (ui_core::is_16_9_mode( ))
	{
		h_rel *= (3.0f / 4.0f);
	}

	f32 w;
	f32 h;
	if (h_rel < v_rel)
	{
		w = r.width( ) * h_rel;
		h = r.height( ) * h_rel;
	}
	else
	{
		w = r.width( ) * v_rel;
		h = r.height( ) * v_rel;
	}

	my_r.x1 += (m_xxxRect.width( ) - w) / 2;
	my_r.y1 += (m_xxxRect.height( ) - h) / 2;
	my_r.x2 = my_r.x1 + w;
	my_r.y2 = my_r.y1 + h;
	SetWndRect(my_r);
}

void CUIStatic::SetTextST(pcstr str_id)
{
	SetText(*CStringTable( ).translate(str_id));
}

void CUIStatic::DrawHighlightedText( )
{
	fRect rect;
	GetAbsoluteRect(rect);
	u32 def_col = m_pLines->GetTextColor( );
	m_pLines->SetTextColor(m_HighlightColor);
	m_pLines->Draw(rect.left + 0 + m_TextOffset.x, rect.top - 0 + m_TextOffset.y);
	m_pLines->SetTextColor(def_col);
}

bool CUIStatic::IsHighlightText( )
{
	return m_bCursorOverWindow;
}
