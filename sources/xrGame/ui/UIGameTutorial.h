#pragma once

class CUIWindow;
class CUIStatic;
class CUISequenceItem;
class CUIXml;

class CUISequencer : public pureFrame, public pureRender, public IInputReceiver
{
protected:
	CUIWindow* m_UIWindow;
	xr_deque<CUISequenceItem*>m_items;
	bool					m_bActive;
	bool					m_bPlayEachItem;
	bool					GrabInput( );

public:
	IInputReceiver* m_pStoredInputReceiver;
	CUISequencer( );
	void					Start(pcstr tutor_name);
	void					Stop( );
	void					Next( );

	void					Destroy( );				//be careful

	virtual void			OnFrame( );
	virtual void			OnRender( );
	CUIWindow* MainWnd( )
	{
		return m_UIWindow;
	}
	bool					IsActive( )
	{
		return m_bActive;
	}

	//IInputReceiver
	virtual void			IR_OnMousePress(s32 btn);
	virtual void			IR_OnMouseRelease(s32 btn);
	virtual void			IR_OnMouseHold(s32 btn);
	virtual void			IR_OnMouseMove(s32 x, s32 y);
	virtual void			IR_OnMouseStop(s32 x, s32 y);

	virtual void			IR_OnKeyboardPress(s32 dik);
	virtual void			IR_OnKeyboardRelease(s32 dik);
	virtual void			IR_OnKeyboardHold(s32 dik);

	virtual void			IR_OnMouseWheel(s32 direction);
	virtual void			IR_OnActivate(void);
};

class CUISequenceItem
{
	xr_vector<s32>			m_disabled_actions;

protected:
	enum
	{
		etiNeedPauseOn = (1 << 0),
		etiNeedPauseOff = (1 << 1),
		etiStoredPauseState = (1 << 2),
		etiCanBeStopped = (1 << 3),
		etiGrabInput = (1 << 4),
		etiNeedPauseSound = (1 << 5),
		eti_last = 6
	};
	xr_vector<luabind::functor<void> >	m_start_lua_functions;
	xr_vector<luabind::functor<void> >	m_stop_lua_functions;

	Flags32					m_flags;
	CUISequencer* m_owner;

public:
	CUISequenceItem(CUISequencer* owner) :m_owner(owner)
	{
		m_flags.zero( );
	}
	virtual					~CUISequenceItem( )
	{ }
	virtual void			Load(CUIXml* xml, s32 idx) = 0;

	virtual void			Start( ) = 0;
	virtual bool			Stop(bool bForce = false) = 0;

	virtual void			Update( ) = 0;
	virtual void			OnRender( ) = 0;
	virtual void			OnKeyboardPress(s32 dik) = 0;

	virtual bool			IsPlaying( ) = 0;

	bool					AllowKey(s32 dik);
	bool					GrabInput( )
	{
		return !!m_flags.test(etiGrabInput);
	}
};

class CUISequenceSimpleItem : public CUISequenceItem
{
	typedef CUISequenceItem	inherited;
	struct SSubItem
	{
		CUIStatic* m_wnd;
		f32				m_start;
		f32				m_length;
		bool				m_visible;

	public:
		virtual void		Start( );
		virtual void		Stop( );
	};
	DEFINE_VECTOR(SSubItem, SubItemVec, SubItemVecIt);
	SubItemVec				m_subitems;

public:
	CUIWindow* m_UIWindow;
	ref_sound				m_sound;
	f32					m_time_start;
	f32					m_time_length;
	string64				m_pda_section;
	fVector2				m_desired_cursor_pos;
	s32						m_continue_dik_guard;

public:
	CUISequenceSimpleItem(CUISequencer* owner) :CUISequenceItem(owner)
	{ }
	virtual					~CUISequenceSimpleItem( );
	virtual void			Load(CUIXml* xml, s32 idx);

	virtual void			Start( );
	virtual bool			Stop(bool bForce = false);

	virtual void			Update( );
	virtual void			OnRender( )
	{ }
	virtual void			OnKeyboardPress(s32 dik);

	virtual bool			IsPlaying( );
};

class CUISequenceVideoItem : public CUISequenceItem
{
	typedef CUISequenceItem	inherited;
	ref_sound				m_sound[2];
	CTexture* m_texture;
	enum
	{
		etiPlaying = (1 << (eti_last + 0)),
		etiNeedStart = (1 << (eti_last + 1)),
		etiDelayed = (1 << (eti_last + 2)),
		etiBackVisible = (1 << (eti_last + 3))
	};
	f32					m_delay;
	CUIStatic* m_wnd;
	u32						m_time_start;
	u32						m_sync_time;

public:
	CUISequenceVideoItem(CUISequencer* owner);
	virtual					~CUISequenceVideoItem( );
	virtual void			Load(CUIXml* xml, s32 idx);

	virtual void			Start( );
	virtual bool			Stop(bool bForce = false);

	virtual void			Update( );
	virtual void			OnRender( );
	virtual void			OnKeyboardPress(s32 dik)
	{ }

	virtual bool			IsPlaying( );
};
