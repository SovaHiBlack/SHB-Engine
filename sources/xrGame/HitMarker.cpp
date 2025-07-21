#include "stdafx.h"

#include "HitMarker.h"
#include "..\XR_3DA\Render.h"
#include "..\XR_3DA\LightAnimLibrary.h"
#include "ui\UIStaticItem.h"

static fVector2			as_PC[5];
static fVector2			as_TC[5];
const static u32		as_id[4 * 3] = {0,1,4,  1,2,4,  2,3,4,  3,0,4};

CHitMarker::CHitMarker( )
{
	InitShader(pSettings->r_string("hud_hitmark", "hit_mark_texture"));
}

void CHitMarker::InitShader(pcstr tex_name)
{
	hShader2.create("hud\\default", tex_name);
}

CHitMarker::~CHitMarker( )
{
	while (m_HitMarks.size( ))
	{
		xr_delete(m_HitMarks.front( ));
		m_HitMarks.pop_front( );
	}
}

const static f32 fShowTime = 0.5f;
void CHitMarker::Render( )
{
	f32 h1;
	f32 p1;
	Device.vCameraDirection.getHP(h1, p1);

	while (m_HitMarks.size( ) && !m_HitMarks.front( )->IsActive( ))
	{
		xr_delete(m_HitMarks.front( ));
		m_HitMarks.pop_front( );
	}

	HITMARKS::iterator it = m_HitMarks.begin( );
	HITMARKS::iterator it_e = m_HitMarks.end( );
	for (; it != it_e; ++it)
	{
		(*it)->Draw(-h1);
	}
}

void CHitMarker::Hit(s32 id, const fVector3& dir)
{
	fVector3 hit_dir = dir;
	hit_dir.mul(-1.0f);
	m_HitMarks.push_back(xr_new<SHitMark>(hShader2, hit_dir));
}

SHitMark::SHitMark(const ref_shader& sh, const fVector3& dir)
{
	m_StartTime = Device.fTimeGlobal;
	m_lanim = LALib.FindItem("hud_hit_mark");
	m_HitDirection = dir.getH( );
	m_UIStaticItem = xr_new<CUIStaticItem>( );
	m_UIStaticItem->SetShader(sh);
	m_UIStaticItem->SetPos(256.0f, 128.0f);
	m_UIStaticItem->SetRect(0.0f, 0.0f, 512.0f, 512.0f);
}

void SHitMark::UpdateAnim( )
{
	s32 frame;
	u32 clr = m_lanim->CalculateRGB(Device.fTimeGlobal - m_StartTime, frame);
	m_UIStaticItem->SetColor(subst_alpha(m_UIStaticItem->GetColor( ), color_get_A(clr)));
}

SHitMark::~SHitMark( )
{
	xr_delete(m_UIStaticItem);
}

bool SHitMark::IsActive( )
{
	return ((Device.fTimeGlobal - m_StartTime) < m_lanim->Length_sec( ));
}

void SHitMark::Draw(f32 cam_dir)
{
	UpdateAnim( );

	f32 res_h = cam_dir + m_HitDirection;
	m_UIStaticItem->Render(res_h);
}
