// ElectricBallArtefact.cpp
// ElectricBallArtefact - артефакт электрический шар

#include "stdafx.h"

#include "ElectricBallArtefact.h"

CElectricBallArtefact::CElectricBallArtefact( )
{ }

CElectricBallArtefact::~CElectricBallArtefact( )
{ }

void CElectricBallArtefact::Load(const char* section)
{
	inherited::Load(section);
}

void CElectricBallArtefact::UpdateCLChild( )
{
	inherited::UpdateCLChild( );

	if (H_Parent( ))
	{
		XFORM( ).set(H_Parent( )->XFORM( ));
	}
};
