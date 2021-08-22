// MilitaryOutfit.h
// MilitaryOutfit - защитный костюм военного

#pragma once

#include "CustomOutfit.h"

class CMilitaryOutfit : public CCustomOutfit
{
private:
	using inherited = CCustomOutfit;

public:
	CMilitaryOutfit( );
	virtual ~CMilitaryOutfit( );
};
