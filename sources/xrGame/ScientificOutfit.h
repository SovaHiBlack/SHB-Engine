// ScientificOutfit.h
// ScientificOutfit - защитный костюм ученого

#pragma once

#include "CustomOutfit.h"

class CScientificOutfit : public CCustomOutfit
{
private:
	using inherited = CCustomOutfit;

public:
	CScientificOutfit( );
	virtual ~CScientificOutfit( );
};
