#pragma once
#include "UIStatic.h"
#include "UIWndCallback.h"

class CUIGlobalMapSpot;
class CUIMapWnd;

class CUICustomMap : public CUIStatic, public CUIWndCallback
{
protected:	
	shared_str		m_name;
	fRect			m_BoundRect;// real map size (meters)
	Flags16			m_flags;
	enum EFlags{	eLocked	=(1<<0),};
	f32			m_pointer_dist;

public:
	fRect			m_prevRect;
					CUICustomMap					();
	virtual			~CUICustomMap					();
	virtual void	SetActivePoint					(const Fvector &vNewPoint);

	virtual void	Init							(shared_str name, CInifile& gameLtx, pcstr sh_name);
	virtual fVector2 ConvertRealToLocal				(const fVector2& src);// meters->pixels (relatively own left-top pos)
	fVector2		ConvertLocalToReal				(const fVector2& src);
	fVector2		ConvertRealToLocalNoTransform	(const fVector2& src);// meters->pixels (relatively own left-top pos)

	bool			GetPointerTo					(const fVector2& src, f32 item_radius, fVector2& pos, f32& heading);//position and heading for drawing pointer to src pos

	void			FitToWidth						(f32 width);
	void			FitToHeight						(f32 height);
	f32			GetCurrentZoom					(){return GetWndRect().width()/m_BoundRect.width();}
	const fRect&    BoundRect						()const					{return m_BoundRect;};
	virtual void	OptimalFit						(const fRect& r);

	shared_str		MapName							() {return m_name;}
	virtual CUIGlobalMapSpot*	GlobalMapSpot		() {return NULL;}

	virtual void	Update							();
	virtual void	SendMessage						(CUIWindow* pWnd, s16 msg, void* pData);
			bool	IsRectVisible					(fRect r);
			bool	NeedShowPointer					(fRect r);
			bool	Locked							()				{return !!m_flags.test(eLocked);}
			void	SetLocked						(bool b)		{m_flags.set(eLocked,b);}
			void	SetPointerDistance				(f32 d)		{m_pointer_dist=d;};
			f32	GetPointerDistance				()				{return m_pointer_dist;};

protected:
	virtual void	UpdateSpots						() {};
};


class CUIGlobalMap: public CUICustomMap{
	typedef  CUICustomMap inherited;

private:
	shared_str		m_prev_active_map;
	CUIMapWnd*		m_mapWnd;
	f32			m_minZoom;
	f32			m_max_zoom;

public:
	virtual fVector2 ConvertRealToLocal		(const fVector2& src);// pixels->pixels (relatively own left-top pos)

					CUIGlobalMap			(CUIMapWnd*	pMapWnd);
	virtual			~CUIGlobalMap			();
	
	IC void			SetMinZoom				(f32 zoom){m_minZoom=zoom;}
	IC f32		GetMinZoom				(){return m_minZoom;}
	IC f32		GetMaxZoom				(){return m_max_zoom;}
	IC void			SetMaxZoom				(f32 zoom){m_max_zoom = zoom;}

	virtual void	Init					(shared_str name, CInifile& gameLtx, pcstr sh_name);
	virtual bool	OnMouse					(f32 x, f32 y, EUIMessages mouse_action);

	CUIMapWnd*		MapWnd					() {return m_mapWnd;}
	void			MoveWndDelta			(const fVector2& d);

	f32			CalcOpenRect			(const fVector2& center_point, fRect& map_desired_rect, f32 tgt_zoom);

	void			ClipByVisRect			();
	virtual void	Update					();
};

class CUILevelMap: public CUICustomMap{
	typedef  CUICustomMap inherited;
	CUIMapWnd*					m_mapWnd;
	fRect						m_GlobalRect;			// virtual map size (meters)

private:
								CUILevelMap			(const CUILevelMap &obj) {}
			CUILevelMap			&operator=			(const CUILevelMap &obj) {}

public:
								CUILevelMap			(CUIMapWnd*);
	virtual						~CUILevelMap		();
	virtual void				Init				(shared_str name, CInifile& gameLtx, pcstr sh_name);
	const fRect&				GlobalRect			() const								{return m_GlobalRect;}
	virtual void				Draw				();
	virtual void				Update				();
	virtual bool				OnMouse				(f32 x, f32 y, EUIMessages mouse_action);
	virtual void				SendMessage			(CUIWindow* pWnd, s16 msg, void* pData);
	
	fRect						CalcWndRectOnGlobal	();
	CUIMapWnd*					MapWnd				() {return m_mapWnd;}

	virtual		void			OnFocusLost			();

protected:
	virtual void				UpdateSpots			();
};

class CUIMiniMap: public CUICustomMap{
	typedef  CUICustomMap inherited;

public:
								CUIMiniMap			();
	virtual						~CUIMiniMap			();
	virtual void				Init				(shared_str name, CInifile& gameLtx, pcstr sh_name);

protected:
	virtual void				UpdateSpots			();
};
