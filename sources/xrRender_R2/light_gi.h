#pragma once

struct	light_indirect
{
	Fvector			P;
	Fvector			D;
	f32			E;
	IRender_Sector*	S;
};
