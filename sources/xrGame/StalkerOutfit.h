///////////////////////////////////////////////////////////////
// StalkerOutfit.cpp
// StalkerOutfit - �������� ������ ��������
///////////////////////////////////////////////////////////////

#pragma once

#include "CustomOutfit.h"
#include "script_export_space.h"

class CStalkerOutfit : public CCustomOutfit
{
private:
	typedef	CCustomOutfit inherited;

public:
	CStalkerOutfit( );
	virtual ~CStalkerOutfit( );

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CStalkerOutfit)
#undef script_type_list
#define script_type_list save_type_list(CStalkerOutfit)
