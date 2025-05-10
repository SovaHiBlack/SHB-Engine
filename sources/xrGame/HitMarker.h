#pragma once

class CUIStaticItem;
class CLightAnimItem;

struct SHitMark
{
	CUIStaticItem*				m_UIStaticItem;
	f32							m_StartTime;
	f32							m_HitDirection;
	CLightAnimItem*				m_lanim;

					SHitMark	(const ref_shader& sh, const fVector3& dir);
					~SHitMark	( );
	bool			IsActive	( );
	void			UpdateAnim	( );
	void			Draw		(f32 dir);
};

class CHitMarker
{
public:
	typedef xr_deque<SHitMark*>	HITMARKS;
	ref_shader					hShader2;
	HITMARKS					m_HitMarks;

public:
					CHitMarker	( );
					~CHitMarker	( );

	void			Render		( );
	void			Hit			(s32 id, const fVector3& dir);
	void			InitShader	(pcstr tex_name);
};
