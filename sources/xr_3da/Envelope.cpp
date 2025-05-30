#include "stdafx.h"

#include "Envelope.h"

CEnvelope::~CEnvelope( )
{
	Clear( );
}

CEnvelope::CEnvelope(CEnvelope* source)
{
	*this = *source;
	for (u32 i = 0; i < source->keys.size( ); i++)
	{
		keys[i] = xr_new<st_Key>(*source->keys[i]);
	}
}

void CEnvelope::Clear( )
{
	for (KeyIt k_it = keys.begin( ); k_it != keys.end( ); k_it++)
	{
		xr_delete(*k_it);
	}
}

void CEnvelope::FindNearestKey(f32 t, KeyIt& min_k, KeyIt& max_k, f32 eps)
{
	for (KeyIt k_it = keys.begin( ); k_it != keys.end( ); k_it++)
	{
		if (fsimilar((*k_it)->time, t, eps))
		{
			max_k = k_it + 1;
			min_k = (k_it == keys.begin( )) ? k_it : k_it - 1;
			return;
		}

		if ((*k_it)->time > t)
		{
			max_k = k_it;
			min_k = (k_it == keys.begin( )) ? k_it : k_it - 1;
			return;
		}
	}

	min_k = keys.empty( ) ? keys.end( ) : keys.end( ) - 1;
	max_k = keys.end( );
}

KeyIt CEnvelope::FindKey(f32 t, f32 eps)
{
	for (KeyIt k_it = keys.begin( ); k_it != keys.end( ); k_it++)
	{
		if (fsimilar((*k_it)->time, t, eps))
		{
			return k_it;
		}

		if ((*k_it)->time > t)
		{
			return keys.end( );
		}
	}

	return keys.end( );
}

void CEnvelope::InsertKey(f32 t, f32 val)
{
	for (KeyIt k_it = keys.begin( ); k_it != keys.end( ); k_it++)
	{
		if (fsimilar((*k_it)->time, t, EPSILON_3))
		{
			(*k_it)->value = val;
			return;
		}

		// insert before
		if ((*k_it)->time > t)
		{
			break;
		}
	}

	// create _new key
	st_Key* K = xr_new<st_Key>( );
	K->time = t;
	K->value = val;
	K->shape = SHAPE_TCB;
	behavior[0] = BEH_CONSTANT;
	behavior[1] = BEH_CONSTANT;
	keys.insert(k_it, K);
}

void CEnvelope::DeleteKey(f32 t)
{
	for (KeyIt k_it = keys.begin( ); k_it != keys.end( ); k_it++)
	{
		if (fsimilar((*k_it)->time, t, EPSILON_3))
		{
			xr_delete(*k_it);
			keys.erase(k_it);
			return;
		}
	}
}

BOOL CEnvelope::ScaleKeys(f32 from_time, f32 to_time, f32 scale_factor, f32 eps)
{
	KeyIt min_k = FindKey(from_time, eps);
	if (min_k == keys.end( ))
	{
		KeyIt k0;
		FindNearestKey(from_time, k0, min_k, eps);
	}

	KeyIt max_k = FindKey(to_time, eps);
	if (max_k == keys.end( ))
	{
		KeyIt k1;
		FindNearestKey(to_time, max_k, k1, eps);
	}

	if (min_k != keys.end( ) && min_k != max_k)
	{
		if (max_k != keys.end( ))
		{
			max_k++;
		}

		f32 t0 = (*min_k)->time;
		f32 offset = 0.0f;
		for (KeyIt it = min_k + 1; it != max_k; it++)
		{
			f32 new_time = offset + t0 + ((*it)->time - t0) * scale_factor;
			offset += ((new_time - (*(it - 1))->time) - ((*it)->time - t0));
			t0 = (*it)->time;
			(*it)->time = new_time;
		}

		for (; it != keys.end( ); it++)
		{
			f32 new_time = offset + (*it)->time;
			offset += ((new_time - (*(it - 1))->time) - ((*it)->time - t0));
			(*it)->time = new_time;
		}

		return TRUE;
	}

	return FALSE;
}

f32 CEnvelope::GetLength(f32* mn, f32* mx)
{
	if (!keys.empty( ))
	{
		if (mn)
		{
			*mn = keys.front( )->time;
		}

		if (mx)
		{
			*mx = keys.back( )->time;
		}

		return (keys.back( )->time - keys.front( )->time);
	}

	if (mn)
	{
		*mn = 0.0f;
	}

	if (mx)
	{
		*mx = 0.0f;
	}

	return 0.0f;
}

void CEnvelope::RotateKeys(f32 angle)
{
	for (u32 i = 0; i < keys.size( ); i++)
	{
		keys[i]->value += angle;
	}
}

extern f32 evalEnvelope(CEnvelope* env, f32 time);
f32 CEnvelope::Evaluate(f32 time)
{
	return evalEnvelope(this, time);
}

void CEnvelope::Save(IWriter& F)
{
	F.w_u8((u8)behavior[0]);
	F.w_u8((u8)behavior[1]);
	F.w_u16((u16)keys.size( ));
	for (KeyIt k_it = keys.begin( ); k_it != keys.end( ); k_it++)
	{
		(*k_it)->Save(F);
	}
}

void CEnvelope::Load_1(IReader& F)
{
	Clear( );
	F.r(behavior, sizeof(s32) * 2);
	s32 y = F.r_u32( );
	keys.resize(y);
	for (u32 i = 0; i < keys.size( ); i++)
	{
		keys[i] = xr_new<st_Key>( );
		keys[i]->Load_1(F);
	}
}

void CEnvelope::Load_2(IReader& F)
{
	Clear( );
	behavior[0] = F.r_u8( );
	behavior[1] = F.r_u8( );
	keys.resize(F.r_u16( ));
	for (u32 i = 0; i < keys.size( ); i++)
	{
		keys[i] = xr_new<st_Key>( );
		keys[i]->Load_2(F);
	}
}

void CEnvelope::SaveA(IWriter&)
{ }

void CEnvelope::LoadA(IReader& F)
{
	Clear( );
	string512 buf;
	f32 f[9];
	F.r_string(buf, sizeof(buf));
	if (strstr(buf, "{ Envelope"))
	{
		F.r_string(buf, sizeof(buf));
		s32 nkeys = atoi(buf);
		keys.resize(nkeys);
		for (u32 i = 0; i < keys.size( ); i++)
		{
			keys[i] = xr_new<st_Key>( );
			st_Key& K = *keys[i];
			F.r_string(buf, sizeof(buf));
			s32 cnt = sscanf(buf, "Key %f %f %f %f %f %f %f %f %f", f + 0, f + 1, f + 2, f + 3, f + 4, f + 5, f + 6, f + 7, f + 8);
			R_ASSERT(cnt == 9);
			K.value = f[0];
			K.time = f[1];
			K.shape = (u8)f[2];
			if (K.shape == SHAPE_TCB)
			{
				K.tension = f[3];
				K.continuity = f[4];
				K.bias = f[5];
			}

			if (K.shape == SHAPE_BEZ2)
			{
				K.param[0] = f[3];
				K.param[1] = f[4];
				K.param[2] = f[5];
				K.param[3] = f[6];
			}
			else
			{
				K.param[0] = f[6];
				K.param[1] = f[7];
			}
		}

		// behavior <pre> <post>
		F.r_string(buf, sizeof(buf));
		s32 cnt = sscanf(buf, "Behaviors %d %d", behavior[0], behavior[1]);
		R_ASSERT(cnt == 2);
	}
}

void CEnvelope::Optimize( )
{
	KeyIt it = keys.begin( );
	st_Key K = **it;
	it++;
	bool equal = true;
	for (; it != keys.end( ); it++)
	{
		if (!(*it)->equal(K))
		{
			equal = false;
			break;
		}
	}

	if (equal && (keys.size( ) > 2))
	{
		KeyVec new_keys;
		new_keys.push_back(xr_new<st_Key>(*keys.front( )));
		new_keys.push_back(xr_new<st_Key>(*keys.back( )));
		for (KeyIt k_it = keys.begin( ); k_it != keys.end( ); k_it++)
		{
			xr_delete(*k_it);
		}

		keys.clear_and_free( );
		keys = new_keys;
	}
}
