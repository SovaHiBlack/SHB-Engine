//=============================================================================
//  �������� ��������� ������ � ���������� ������ ��������
//=============================================================================
#pragma once

#include "UIStatic.h"

//-----------------------------------------------------------------------------/
//  ����� ���������� �������
//-----------------------------------------------------------------------------/

struct EffectParams
{
	friend class	CUITextBanner;
	f32			fPeriod;
	bool			bCyclic;
	bool			bOn;
	int				iAdditionalParam;
	f32			fAdditionalParam;
	int				iEffectStage;

	// Constructor
	EffectParams()
		:	fPeriod				(0.0f),
			fTimePassed			(0.0f),
			iEffectStage		(0),
			fAdditionalParam	(0.0f),
			iAdditionalParam	(0),
			bCyclic				(true),
			bOn					(true)
	{}
private:
	f32			fTimePassed;
};

//-----------------------------------------------------------------------------/
//  ����� ������������� �������
//-----------------------------------------------------------------------------/

class CUITextBanner
{
public:
	enum TextBannerStyles
	{
		tbsNone		= 0,
		tbsFlicker	= 1,
		tbsFade		= 1 << 1
	};
	// Ctor and Dtor
	CUITextBanner				();
	~CUITextBanner				();

	virtual void	Update		();
	void			Out			(f32 x, f32 y, pcstr fmt, ...);

	// ���������� ��������� ������������ �������. ����� ��. ������������ TextBannerStyles
	EffectParams * SetStyleParams(const TextBannerStyles styleName);
	void		ResetAnimation		(const TextBannerStyles styleName);

	// Font
	void		SetFont				(CGameFont *pFont)	{ m_pFont = pFont; }
	CGameFont	*GetFont			() const { return m_pFont; }
	void		SetFontSize			(f32 sz)	{ fontSize = sz; }
	void		SetFontAlignment	(CGameFont::EAligment al) {aligment = al;}

	// Color
	void		SetTextColor		(u32 cl);
	u32			GetTextColor		();

	// ���/���� ��������
	void		PlayAnimation		()					{ m_bAnimate = true;	}
	void		StopAnimation		()					{ m_bAnimate = false;	}

protected:
	// ���������� ������� ��� ������� �� ������.
	// � ����:	first	- ����������� ������ ������� (���������� �������������)
	//			second	- ��������� ����� � �������� �������
	typedef xr_map<TextBannerStyles, EffectParams>		StyleParams;
	typedef StyleParams::iterator						StyleParams_it;
	StyleParams											m_StyleParams;

	// ��������� ��������� ���������� ������� ��� ��������
	void EffectFade();
	void EffectFlicker();

	// ����, ������� ���������� ��������� ��������
	bool		m_bAnimate;

	// Font
	CGameFont				*m_pFont;
	f32					fontSize;
	CGameFont::EAligment	aligment;

	// Letters color
	u32			m_Cl;
};
