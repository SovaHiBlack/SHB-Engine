// File:        UIInteractiveBackground.h
// Description: класс шаблонов, предназначенный для элементов управления пользовательским интерфейсом для представления их состояния;
//              существует 4 состояния: включено, отключено, выделено и затронуто.
//              Как правило вы можете использовать один из 3 типов фона :
//              Нормальные Текстуры, Строки, Текстуры, Рамки Текстуры(CUIStatic, CUIFrameLineWnd, CUIFrameWindow)

#pragma once

#include "UIFrameWindow.h"//
#include "UIFrameLineWnd.h"//

enum UIState
{
	S_Enabled,
	S_Disabled,
	S_Highlighted,
	S_Touched
};

template <class T>
class CUIInteractiveBackground : public CUIWindow
{
public:
	CUIInteractiveBackground( );
	virtual ~CUIInteractiveBackground( );

	virtual void Init(float x, float y, float width, float height);
	virtual void Init(const char* texture_e, float x, float y, float width, float height);
	T* CreateE( );
	T* CreateD( );
	T* CreateT( );
	T* CreateH( );
	T* GetE( );
	T* GetD( );
	T* GetT( );
	T* GetH( );
	virtual void InitEnabledState(const char* texture_e);
	virtual void InitDisabledState(const char* texture_d);
	virtual void InitHighlightedState(const char* texture_h);
	virtual void InitTouchedState(const char* texture_t);
	virtual void SetState(UIState state);
	virtual void Draw( );

	virtual void SetWidth(float width);
	virtual void SetHeight(float heigth);

protected:
	T* m_stateCurrent;
	T* m_stateEnabled;
	T* m_stateDisabled;
	T* m_stateHighlighted;
	T* m_stateTouched;
};

template <class T>
CUIInteractiveBackground<T>::CUIInteractiveBackground( )
{
	m_stateCurrent = nullptr;
	m_stateEnabled = nullptr;
	m_stateDisabled = nullptr;
	m_stateHighlighted = nullptr;
	m_stateTouched = nullptr;
}

template <class T>
CUIInteractiveBackground<T>::~CUIInteractiveBackground( )
{ }

template <class T>
void CUIInteractiveBackground<T>::Init(float x, float y, float width, float height)
{
	CUIWindow::Init(x, y, width, height);
}

template <class T>
void CUIInteractiveBackground<T>::Init(const char* texture_e, float x, float y, float width, float height)
{
	CUIWindow::Init(x, y, width, height);

	InitEnabledState(texture_e);

	m_stateEnabled->Init(texture_e, 0.0f, 0.0f, width, height);
	m_stateCurrent = this->m_stateEnabled;
}

template <class T>
T* CUIInteractiveBackground<T>::CreateE( )
{
	Frect r = GetWndRect( );
	if (!m_stateEnabled)
	{
		m_stateEnabled = xr_new<T>( );
		m_stateEnabled->SetAutoDelete(true);
		AttachChild(m_stateEnabled);
		m_stateEnabled->Init(0.0f, 0.0f, r.right - r.left, r.bottom - r.top);
	}

	SetState(S_Enabled);
	return m_stateEnabled;
}

template <class T>
T* CUIInteractiveBackground<T>::CreateD( )
{
	Frect r = GetWndRect( );
	if (!m_stateDisabled)
	{
		m_stateDisabled = xr_new<T>( );
		m_stateDisabled->SetAutoDelete(true);
		AttachChild(m_stateDisabled);
		m_stateDisabled->Init(0.0f, 0.0f, r.right - r.left, r.bottom - r.top);
	}

	return m_stateDisabled;
}

template <class T>
T* CUIInteractiveBackground<T>::CreateT( )
{
	Frect r = GetWndRect( );
	if (!m_stateTouched)
	{
		m_stateTouched = xr_new<T>( );
		m_stateTouched->SetAutoDelete(true);
		AttachChild(m_stateTouched);
		m_stateTouched->Init(0.0f, 0.0f, r.right - r.left, r.bottom - r.top);
	}

	return m_stateTouched;
}

template <class T>
T* CUIInteractiveBackground<T>::CreateH( )
{
	Frect r = GetWndRect( );
	if (!m_stateHighlighted)
	{
		m_stateHighlighted = xr_new<T>( );
		m_stateHighlighted->SetAutoDelete(true);
		AttachChild(m_stateHighlighted);
		m_stateHighlighted->Init(0.0f, 0.0f, r.right - r.left, r.bottom - r.top);
	}

	return m_stateHighlighted;
}

template <class T>
T* CUIInteractiveBackground<T>::GetE( )
{
	return m_stateEnabled;
}

template <class T>
T* CUIInteractiveBackground<T>::GetD( )
{
	return m_stateDisabled;
}

template <class T>
T* CUIInteractiveBackground<T>::GetT( )
{
	return m_stateTouched;
}

template <class T>
T* CUIInteractiveBackground<T>::GetH( )
{
	return m_stateHighlighted;
}

template <class T>
void CUIInteractiveBackground<T>::InitEnabledState(const char* texture_e)
{
	Frect r = GetWndRect( );
	if (!m_stateEnabled)
	{
		m_stateEnabled = xr_new<T>( );
		m_stateEnabled->SetAutoDelete(true);
		AttachChild(m_stateEnabled);
	}

	this->m_stateEnabled->Init(texture_e, 0.0f, 0.0f, r.right - r.left, r.bottom - r.top);

	SetState(S_Enabled);
}

template <class T>
void CUIInteractiveBackground<T>::InitDisabledState(const char* texture_d)
{
	Frect r = GetWndRect( );
	if (!m_stateDisabled)
	{
		m_stateDisabled = xr_new<T>( );
		m_stateDisabled->SetAutoDelete(true);
		AttachChild(m_stateDisabled);
	}

	this->m_stateDisabled->Init(texture_d, 0.0f, 0.0f, r.right - r.left, r.bottom - r.top);
}

template <class T>
void CUIInteractiveBackground<T>::InitHighlightedState(const char* texture_h)
{
	Frect r = GetWndRect( );
	if (!m_stateHighlighted)
	{
		m_stateHighlighted = xr_new<T>( );
		m_stateHighlighted->SetAutoDelete(true);
		AttachChild(m_stateHighlighted);
	}

	this->m_stateHighlighted->Init(texture_h, 0.0f, 0.0f, r.right - r.left, r.bottom - r.top);
}

template <class T>
void CUIInteractiveBackground<T>::InitTouchedState(const char* texture_d)
{
	Frect r = GetWndRect( );
	if (!m_stateTouched)
	{
		m_stateTouched = xr_new<T>( );
		m_stateTouched->SetAutoDelete(true);
		AttachChild(m_stateTouched);
	}

	this->m_stateTouched->Init(texture_d, 0.0f, 0.0f, r.right - r.left, r.bottom - r.top);
}

template <class T>
void CUIInteractiveBackground<T>::SetState(UIState state)
{
	switch (state)
	{
		case S_Enabled:
		{
			this->m_stateCurrent = this->m_stateEnabled;
		}
		break;
		case S_Disabled:
		{
			this->m_stateCurrent = this->m_stateDisabled ? this->m_stateDisabled : this->m_stateEnabled;
		}
		break;
		case S_Highlighted:
		{
			this->m_stateCurrent = this->m_stateHighlighted ? this->m_stateHighlighted : this->m_stateEnabled;
		}
		break;
		case S_Touched:
		{
			this->m_stateCurrent = this->m_stateTouched ? this->m_stateTouched : this->m_stateEnabled;
		}
	}
}

template <class T>
void CUIInteractiveBackground<T>::Draw( )
{
	if (m_stateCurrent)
	{
		m_stateCurrent->Draw( );
	}
}

template <class T>
void CUIInteractiveBackground<T>::SetWidth(float width)
{
	if (m_stateEnabled)
	{
		m_stateEnabled->SetWidth(width);
	}

	if (m_stateDisabled)
	{
		m_stateDisabled->SetWidth(width);
	}

	if (m_stateHighlighted)
	{
		m_stateHighlighted->SetWidth(width);
	}

	if (m_stateTouched)
	{
		m_stateTouched->SetWidth(width);
	}
}

template <class T>
void CUIInteractiveBackground<T>::SetHeight(float heigth)
{
	if (m_stateEnabled)
	{
		m_stateEnabled->SetHeight(heigth);
	}

	if (m_stateDisabled)
	{
		m_stateDisabled->SetHeight(heigth);
	}

	if (m_stateHighlighted)
	{
		m_stateHighlighted->SetHeight(heigth);
	}

	if (m_stateTouched)
	{
		m_stateTouched->SetHeight(heigth);
	}
}

using CUI_IB_FrameWindow = CUIInteractiveBackground<CUIFrameWindow>;
using CUI_IB_FrameLineWnd = CUIInteractiveBackground<CUIFrameLineWnd>;
