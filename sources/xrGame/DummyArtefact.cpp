///////////////////////////////////////////////////////////////
// DummyArtefact.cpp
// DummyArtefact - артефакт пустышка
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DummyArtefact.h"
#include "PhysicsShell.h"

CDummyArtefact::CDummyArtefact( )
{ }

CDummyArtefact::~CDummyArtefact( )
{ }

void CDummyArtefact::Load(pcstr section)
{
	inherited::Load(section);
}
