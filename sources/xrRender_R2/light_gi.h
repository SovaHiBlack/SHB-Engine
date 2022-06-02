#pragma once

struct	light_indirect
{
	Fvector			P;
	Fvector			D;
	F32			E;
	IRender_Sector*	S;
};
