// xrXRC.h: interface for the xrXRC class.
#pragma once

#include "../xrCDB/xrCDB.h"

class ENGINE_API xrXRC
{
	CDB::COLLIDER	CL;

public:
	inline void			ray_options			(u32 f)
	{
		CL.ray_options(f);
	}
	inline void			ray_query			(const CDB::MODEL* m_def, const Fvector3& r_start, const Fvector3& r_dir, float r_range = 10000.0f)
	{

#ifdef DEBUG
		Device.Statistic->clRAY.Begin( );
#endif

		CL.ray_query(m_def, r_start, r_dir, r_range);

#ifdef DEBUG
		Device.Statistic->clRAY.End( );
#endif

	}

	inline void			box_options			(u32 f)
	{
		CL.box_options(f);
	}
	inline void			box_query			(const CDB::MODEL* m_def, const Fvector3& b_center, const Fvector3& b_dim)
	{

#ifdef DEBUG
		Device.Statistic->clBOX.Begin( );
#endif

		CL.box_query(m_def, b_center, b_dim);

#ifdef DEBUG
		Device.Statistic->clBOX.End( );
#endif

	}

	inline void			frustum_options		(u32 f)
	{
		CL.frustum_options(f);
	}
	inline void			frustum_query		(const CDB::MODEL* m_def, const CFrustum& F)
	{

#ifdef DEBUG
		Device.Statistic->clFRUSTUM.Begin( );
#endif

		CL.frustum_query(m_def, F);

#ifdef DEBUG
		Device.Statistic->clFRUSTUM.End( );
#endif

	}

	inline CDB::RESULT* r_begin				( )
	{
		return CL.r_begin( );
	};
	inline CDB::RESULT* r_end				( )
	{
		return CL.r_end( );
	};
	inline void			r_free				( )
	{
		CL.r_free( );
	}
	inline int			r_count				( )
	{
		return CL.r_count( );
	};
	inline void			r_clear				( )
	{
		CL.r_clear( );
	};
	inline void			r_clear_compact		( )
	{
		CL.r_clear_compact( );
	};

					xrXRC				( );
					~xrXRC				( );
};

ENGINE_API extern xrXRC XRC;
