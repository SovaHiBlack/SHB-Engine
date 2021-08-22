// ExoOutfit.h
// ExoOutfit - защитный костюм с усилением

#pragma once

#include "CustomOutfit.h"

class CExoOutfit : public CCustomOutfit
{
private:
	using inherited = CCustomOutfit;

public:
	CExoOutfit( );
	virtual ~CExoOutfit( );
};
