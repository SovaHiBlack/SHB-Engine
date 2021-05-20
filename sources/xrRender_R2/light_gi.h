#pragma once

struct	light_indirect
{
	Fvector3			P;
	Fvector3			D;
	float			E;
	IRender_Sector*	S;
};
