#pragma once

#include "UIWindow.h"

class CUIStatic;

class CUIProgressShape : public CUIWindow {
friend class CUIXmlInit;
public:
						CUIProgressShape		();
	virtual				~CUIProgressShape		();
	void				SetPos					(int pos, int max);
	void				SetPos					(f32 pos);
	void				SetTextVisible			(bool b);

	virtual void		Draw					();

protected:
	bool				m_bClockwise;
	u32					m_sectorCount;
	f32				m_stage;
	CUIStatic*			m_pTexture;
	CUIStatic*			m_pBackground;
	bool				m_bText;
};