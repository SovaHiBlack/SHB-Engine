///////////////////////////////////////////////////////////////
// GraviArtifact.cpp
// GraviArtefact - �������������� ��������, ������� �� �����
// � ����������� ����� ��� ������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraviArtifact.h"
#include "PhysicsShell.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "phworld.h"

extern CPHWorld* ph_world;
#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

CGraviArtefact::CGraviArtefact( )
{
	shedule.t_min = 20;
	shedule.t_max = 50;

	m_fJumpHeight = 0;
	m_fEnergy = 1.f;
}

CGraviArtefact::~CGraviArtefact( )
{ }

void CGraviArtefact::Load(pcstr section)
{
	inherited::Load(section);

	if (pSettings->line_exist(section, "jump_height")) m_fJumpHeight = pSettings->r_float(section, "jump_height");
//	m_fEnergy = pSettings->r_float(section,"energy");
}

void CGraviArtefact::UpdateCLChild( )
{
	VERIFY(!ph_world->Processing( ));
	if (getVisible( ) && m_pPhysicsShell)
	{
		if (m_fJumpHeight)
		{
			fVector3 dir;
			dir.set(0.0f, -1.0f, 0.0f);
			collide::rq_result RQ;

			//��������� ������ ���������
			if (Level( ).ObjectSpace.RayPick(Position( ), dir, m_fJumpHeight, collide::rqtBoth, RQ, this))
			{
				dir.y = 1.0f;
				m_pPhysicsShell->applyImpulse(dir, 30.f * Device.fTimeDelta * m_pPhysicsShell->getMass( ));
			}
		}
	}
	else if (H_Parent( ))
	{
		XFORM( ).set(H_Parent( )->XFORM( ));
	}
}
