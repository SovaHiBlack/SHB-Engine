// UISleepWnd.h:  окошко для выбора того, сколько спать

#pragma once

#include "UIStatic.h"//

class CUIButton;

class CUISleepWnd: public CUIStatic
{
private:
	typedef			CUIStatic inherited;
public:
					CUISleepWnd			();
	virtual			~CUISleepWnd		();


	virtual void	Init				();
	virtual void	SetText				(const char* str);

	virtual void	SendMessage			(CUIWindow *pWnd, s16 msg, void *pData);

	// Изменяем текущее установленное время отдыха на дельта-значения
	void			ModifyRestTime		(S8 dHours, S8 dMinutes);
	// Сбрасываем время в 0
	void			ResetTime			()								{ SetRestTime(0, 0); }
protected:
	// Устанавливаем на отображение время для сна
	void			SetRestTime			(u8 hours, u8 minutes);
	// Текущее запоменное время отдыха
	S8				m_Hours;
	S8				m_Minutes;

	// Контролы
	CUIStatic*		UIStaticRestAmount;
	CUIButton*		UIPlusBtn;
	CUIButton*		UIMinusBtn;
	CUIButton*		UIRestBtn;
};
