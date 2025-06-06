//////////////////////////////////////////////////////////////////////
// ExplosiveItem.cpp:	����� ��� ���� ������� ���������� ��� 
//						��������� ��������� ����� (��������,
//						������ � ����� � �.�.)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ExplosiveItem.h"

CExplosiveItem::CExplosiveItem( )
{ }
CExplosiveItem::~CExplosiveItem( )
{ }

void CExplosiveItem::Load(pcstr section)
{
	inherited::Load(section);
	CExplosive::Load(section);
	m_flags.set(FUsingCondition, TRUE);
	CDelayedActionFuse::Initialize(pSettings->r_float(section, "time_to_explode"), pSettings->r_float(section, "condition_to_explode"));
	VERIFY(pSettings->line_exist(section, "set_timer_particles"));
}

void CExplosiveItem::net_Destroy( )
{
	inherited::net_Destroy( );
	CExplosive::net_Destroy( );
}

void	CExplosiveItem::Hit(SHit* pHDS)
{
	if (CDelayedActionFuse::isActive( ))pHDS->power = 0.f;
	inherited::Hit(pHDS);
	if (!CDelayedActionFuse::isActive( ) && CDelayedActionFuse::CheckCondition(GetCondition( )) && CExplosive::Initiator( ) == u16(-1))
	{
		//��������� ����, ��� ������� ����
		SetInitiator(pHDS->who->ID( ));
	}
}
void CExplosiveItem::StartTimerEffects( )
{
	CParticlesPlayer::StartParticles(pSettings->r_string(*cNameSect( ), "set_timer_particles"), fVector3( ).set(0.0f, 1.0f, 0.0f), ID( ));

}
void CExplosiveItem::OnEvent(CNetPacket& P, u16 type)
{
	CExplosive::OnEvent(P, type);
	inherited::OnEvent(P, type);

}
void CExplosiveItem::UpdateCL( )
{
	CExplosive::UpdateCL( );
	inherited::UpdateCL( );
}
void CExplosiveItem::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	if (CDelayedActionFuse::isActive( ) && CDelayedActionFuse::Update(GetCondition( )))
	{
		fVector3 normal;
		FindNormal(normal);
		CExplosive::GenExplodeEvent(Position( ), normal);
		CParticlesPlayer::StopParticles(ID( ), BI_NONE, true);
	}
}

bool CExplosiveItem::shedule_Needed( )
{
//.	return true;

	return (inherited::shedule_Needed( ) || CDelayedActionFuse::isActive( ));
}

void CExplosiveItem::renderable_Render( )
{
	inherited::renderable_Render( );
}
void CExplosiveItem::net_Relcase(CObject* O)
{
	CExplosive::net_Relcase(O);
	inherited::net_Relcase(O);
}
void	CExplosiveItem::ActivateExplosionBox(const fVector3& size, fVector3& in_out_pos)
{
	//PKinematics(Visual())->CalculateBones();
}
void CExplosiveItem::GetRayExplosionSourcePos(fVector3& pos)
{
	random_point_in_object_box(pos, this);
}
