// DummyArtefact.cpp
// DummyArtefact - артефакт пустышка

#include "stdafx.h"

#include "DummyArtefact.h"

CDummyArtefact::CDummyArtefact( )
{ }

CDummyArtefact::~CDummyArtefact( )
{ }

void CDummyArtefact::Load(const char* section)
{
	inherited::Load(section);
}
