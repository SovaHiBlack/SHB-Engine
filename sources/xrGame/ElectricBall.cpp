///////////////////////////////////////////////////////////////
// ElectricBall.cpp
// ElectricBall - �������� ������������� ���
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ElectricBall.h"
#include "PhysicsShell.h"

CElectricBall::CElectricBall( )
{ }

CElectricBall::~CElectricBall( )
{ }

void CElectricBall::Load(pcstr section)
{
	inherited::Load(section);
}

void CElectricBall::UpdateCLChild( )
{
	inherited::UpdateCLChild( );

	if (H_Parent( )) XFORM( ).set(H_Parent( )->XFORM( ));
}
