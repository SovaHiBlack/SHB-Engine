#pragma once

class CUIStaticItem;
class CLAItem;

struct SHitMark{
	CUIStaticItem*	m_UIStaticItem;
	float			m_StartTime;
	float			m_HitDirection;
	CLAItem*		m_lanim;

					SHitMark		(const ref_shader& sh, const Fvector3& dir);
					~SHitMark		();
	bool			IsActive		();
	void			UpdateAnim		();
	void			Draw			(float dir);
};

class CHitMarker
{
public:
/*
	float					fHitMarks[4];
	ref_shader				hShader;
	ref_geom				hGeom;
*/	
	typedef xr_deque<SHitMark*> HITMARKS;
	ref_shader				hShader2;
	HITMARKS				m_HitMarks;

public:
							CHitMarker	();
							~CHitMarker	();

	void					Render		();
	void					Hit			(int id, const Fvector3& dir);
	void					InitShader	(const char* tex_name);
};
