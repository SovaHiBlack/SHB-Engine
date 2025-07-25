#pragma once

#include "../xr_level_controller.h"
#include "UIMessages.h"
#include "../script_export_space.h"
#include "uiabstract.h"

class CUIWindow;

struct _12b
{
	DWORD _[3];
};

extern poolSS< _12b, 128>	ui_allocator;

template <class T>
class uialloc
{
public:
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T			value_type;

public:
	template<class _Other>
	struct rebind
	{
		typedef uialloc<_Other> other;
	};

public:
	pointer					address(reference _Val) const
	{
		return (&_Val);
	}
	const_pointer			address(const_reference _Val) const
	{
		return (&_Val);
	}
	uialloc( )
	{ }
	uialloc(const uialloc<T>&)
	{ }
	template<class _Other>							uialloc(const uialloc<_Other>&)
	{ }
	template<class _Other>	uialloc<T>& operator=		(const uialloc<_Other>&)
	{
		return (*this);
	}
	pointer					allocate(size_type n, pcvoid p = 0) const
	{
		VERIFY(1 == n);
		return (pointer) ui_allocator.create( );
	}
	pstr			__charalloc(size_type n)
	{
		VERIFY(1 == n);
		return	(pstr) ui_allocator.create( );
	}
	void					deallocate(pointer p, size_type n) const
	{
		VERIFY(1 == n);
		_12b* p_ = (_12b*) p;
		ui_allocator.destroy(p_);
	}
	void					deallocate(pvoid p, size_type n) const
	{
		VERIFY(1 == n);
		_12b* p_ = (_12b*) p;
		ui_allocator.destroy(p_);
	}
	void					construct(pointer p, const T& _Val)
	{
		std::_Construct(p, _Val);
	}
	void					destroy(pointer p)
	{
		std::_Destroy(p);
	}
	size_type				max_size( ) const
	{
		size_type _Count = (size_type) (-1) / sizeof(T);
		return (0 < _Count ? _Count : 1);
	}
};

template<class _Ty, class _Other>
IC bool operator == (const uialloc<_Ty>&, const uialloc<_Other>&)
{
	return true;
}

template<class _Ty, class _Other>
IC bool operator != (const uialloc<_Ty>&, const uialloc<_Other>&)
{
	return false;
}

template<typename T>
class ui_list : public std::list<T, uialloc<T>>
{
public:
	u32 size( ) const
	{
		return (u32)__super::size( );
	}
};

#define DEF_UILIST(N,T)		typedef ui_list< T > N;			typedef N::iterator N##_it;

//////////////////////////////////////////////////////////////////////////

class CUIWindow : public CUISimpleWindow
{
public:
	using CUISimpleWindow::Init;

	CUIWindow( );
	virtual		~CUIWindow( );

	////////////////////////////////////
	//�������������
	virtual void			Init(fRect* pRect);

	////////////////////////////////////
	//������ � ��������� � ������������� ������
	virtual void			AttachChild(CUIWindow* pChild);
	virtual void			DetachChild(CUIWindow* pChild);
	virtual bool			IsChild(CUIWindow* pChild) const;
	virtual void			DetachAll( );
	s32						GetChildNum( )
	{
		return m_ChildWndList.size( );
	}

	void					SetParent(CUIWindow* pNewParent);
	CUIWindow* GetParent( )	const
	{
		return m_pParentWnd;
	}

	//�������� ���� ������ �������� ������
	CUIWindow* GetTop( )
	{
		if (m_pParentWnd == NULL)
		{
			return this;
		}
		else
		{
			return m_pParentWnd->GetTop( );
		}
	}

	CUIWindow* GetCurrentMouseHandler( );
	CUIWindow* GetChildMouseHandler( );

	//������� �� ������� ������ ��������� �������� ����
	bool					BringToTop(CUIWindow* pChild);

	//������� �� ������� ������ ���� ��������� ���� � ��� ������
	void					BringAllToTop( );

	virtual bool 			OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual void 			OnMouseMove( );
	virtual void 			OnMouseScroll(f32 iDirection);
	virtual bool 			OnDbClick( );
	virtual bool 			OnMouseDown(s32 mouse_btn);
	virtual void 			OnMouseUp(s32 mouse_btn);
	virtual void 			OnFocusReceive( );
	virtual void 			OnFocusLost( );
	bool 			HasChildMouseHandler( );

	//���������/���������� ���� �����
	//��������� ���������� �������� ����� �������������
	void					SetCapture(CUIWindow* pChildWindow, bool capture_status);
	CUIWindow* GetMouseCapturer( )
	{
		return m_pMouseCapturer;
	}

	//������, �������� ������������ ���������,
	//���� NULL, �� ���� �� GetParent()
	void					SetMessageTarget(CUIWindow* pWindow)
	{
		m_pMessageTarget = pWindow;
	}
	CUIWindow* GetMessageTarget( );

	//������� �� ����������
	virtual bool			OnKeyboard(s32 dik, EUIMessages keyboard_action);
	virtual bool			OnKeyboardHold(s32 dik);
	virtual void			SetKeyboardCapture(CUIWindow* pChildWindow, bool capture_status);

	//��������� ��������� �� �������������� ������������ �������������
	//�-��� ������ ����������������
	//pWnd - ��������� �� ����, ������� ������� ���������
	//pData - ��������� �� �������������� ������, ������� ����� ������������
	virtual void			SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = NULL);

	//����������/���������� �� ���� � ����������
	virtual void			Enable(bool status)
	{
		m_bIsEnabled = status;
	}
	virtual bool			IsEnabled( )
	{
		return m_bIsEnabled;
	}

	//������/�������� ���� � ��� �������� ����
	virtual void			Show(bool status)
	{
		SetVisible(status); Enable(status);
	}
	IC		bool			IsShown( )
	{
		return this->GetVisible( );
	}
	void			ShowChildren(bool show);

	//���������� ����������
	IC void					GetAbsoluteRect(fRect& r);
	IC void					GetAbsolutePos(fVector2& p)
	{
		fRect abs;
		GetAbsoluteRect(abs);
		p.set(abs.x1, abs.y1);
	}


	void			SetWndRect_script(f32 x, f32 y, f32 width, f32 height)
	{
		CUISimpleWindow::SetWndRect(x, y, width, height);
	}
	void			SetWndRect_script(fRect rect)
	{
		CUISimpleWindow::SetWndRect(rect);
	}

	//���������� ����
	virtual void			Draw( );
	virtual void			Draw(f32 x, f32 y);
	//���������� ���� ����������������
	virtual void			Update( );

	void			SetPPMode( );
	void			ResetPPMode( );
	IC bool			GetPPMode( )
	{
		return m_bPP;
	}

	//��� �������� ���� � �������� � �������� ���������
	virtual void			Reset( );
	void			ResetAll( );

	virtual void			SetFont(CGameFont* pFont)
	{
		m_pFont = pFont;
	}
	CGameFont* GetFont( )
	{
		if (m_pFont)
		{
			return m_pFont;
		}

		if (m_pParentWnd == NULL)
		{
			return m_pFont;
		}
		else
		{
			return m_pParentWnd->GetFont( );
		}
	}

	DEF_UILIST(WINDOW_LIST, CUIWindow*);
	WINDOW_LIST& GetChildWndList( )
	{
		return m_ChildWndList;
	}

	IC bool					IsAutoDelete( )
	{
		return m_bAutoDelete;
	}
	IC void					SetAutoDelete(bool auto_delete)
	{
		m_bAutoDelete = auto_delete;
	}

	// Name of the window
	const shared_str		WindowName( ) const
	{
		return m_windowName;
	}
	void					SetWindowName(pcstr wn)
	{
		m_windowName = wn;
	}
	pcstr					WindowName_script( )
	{
		return *m_windowName;
	}
	CUIWindow* FindChild(const shared_str name);

	IC bool					CursorOverWindow( ) const
	{
		return m_bCursorOverWindow;
	}

protected:
	IC void					SafeRemoveChild(CUIWindow* child)
	{
		WINDOW_LIST_it it = std::find(m_ChildWndList.begin( ), m_ChildWndList.end( ), child);
		if (it != m_ChildWndList.end( ))
		{
			m_ChildWndList.erase(it);
		}
	}

	shared_str				m_windowName;
	//������ �������� ����
	WINDOW_LIST				m_ChildWndList;

	//��������� �� ������������ ����
	CUIWindow* m_pParentWnd;

	//�������� ���� �������, ��������� ���� ����
	CUIWindow* m_pMouseCapturer;

	//��� ���������� ����������
	//������ ������, ������ �� ������
	//����� ���� ����� � ����������
	CUIWindow* m_pOrignMouseCapturer;

	//�������� ���� �������, ��������� ���� ����������
	CUIWindow* m_pKeyboardCapturer;

	//���� ���� ���������
	CUIWindow* m_pMessageTarget;

	CGameFont* m_pFont;

	// ��������� ������� �����
	fVector2 cursor_pos;

	//����� �������� ����� �����
	//��� ����������� DoubleClick
	u32						m_dwLastClickTime;
	u32						m_dwFocusReceiveTime;

	//���� ��������������� �������� �� ����� ������ �����������
	bool					m_bAutoDelete;

	// ���� �����������/����������� ��������� ���������
	bool					m_bPP;
	//�������� �� ���� ������������
	bool					m_bIsEnabled;

	// ���� ������ ��� �����
	bool					m_bCursorOverWindow;
	bool					m_bClickable;

#ifdef DEBUG
	s32 m_dbg_id;
	flags32					m_dbg_flag;
#endif

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIWindow)
#undef script_type_list
#define script_type_list save_type_list(CUIWindow)
