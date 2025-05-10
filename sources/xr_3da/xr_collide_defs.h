#pragma once

class ENGINE_API CObject;
namespace collide
{
	struct tri
	{
		fVector3	e10;
		f32 e10s;
		fVector3	e21;
		f32 e21s;
		fVector3	e02;
		f32 e02s;
		fVector3 p[3];
		fVector3 N;
		f32	d;
	};
	struct elipsoid
	{
		fMatrix4x4	mL2W;		// convertion from sphere(000,1) to real space
		fMatrix4x4	mW2L;		// convertion from real space to sphere(000,1)
	};
	struct ray_cache
	{
		// previous state
		fVector3				start;
		fVector3				dir;
		f32				range;
		BOOL				result;

		// cached vertices
		fVector3				verts[3];
		ray_cache( )
		{
			start.set(0, 0, 0);
			dir.set(0, 0, 0);
			range = 0;
			result = FALSE;
			verts[0].set(0, 0, 0);
			verts[1].set(0, 0, 0);
			verts[2].set(0, 0, 0);
		}
		void				set(const fVector3& _start, const fVector3& _dir, const f32 _range, const BOOL _result)
		{
			start = _start;
			dir = _dir;
			range = _range;
			result = _result;
		}
		BOOL				similar(const fVector3& _start, const fVector3& _dir, const f32 _range)
		{
			if (!_start.similar(start))					return FALSE;
			if (!fsimilar(1.f, dir.dotproduct(_dir)))	return FALSE;
			if (!fsimilar(_range, range))				return FALSE;
			return			TRUE;
		}
	};
	enum rq_target
	{
		rqtNone = (0),
		rqtObject = (1 << 0),
		rqtStatic = (1 << 1),
		rqtShape = (1 << 2),
		rqtObstacle = (1 << 3),
		rqtBoth = (rqtObject | rqtStatic),
		rqtDyn = (rqtObject | rqtShape | rqtObstacle)
	};
	struct			ray_defs
	{
		fVector3		start;
		fVector3		dir;
		f32		range;
		u32			flags;
		rq_target	tgt;
		ray_defs(const fVector3& _start, const fVector3& _dir, f32 _range, u32 _flags, rq_target _tgt)
		{
			start = _start;
			dir = _dir;
			range = _range;
			flags = _flags;
			tgt = _tgt;
		}
	};
	struct			rq_result
	{
		CObject* O;				// if NULL - static
		f32		range;			// range to intersection
		s32			element;		// номер кости/номер треугольника
		IC rq_result& set(CObject* _O, f32 _range, s32 _element)
		{
			O = _O;
			range = _range;
			element = _element;
			return	*this;
		}
		IC BOOL		set_if_less(CDB::RESULT* I)
		{
			if (I->range < range)
			{
				set(0, I->range, I->id);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		IC BOOL		set_if_less(rq_result* R)
		{
			if (R->range < range)
			{
				set(R->O, R->range, R->element);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		IC BOOL		set_if_less(CObject* _who, f32 _range, s32 _element)
		{
			if (_range < range)
			{
				set(_who, _range, _element);
				{
					return TRUE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		IC BOOL		valid( )
		{
			return (element >= 0);
		}
	};
	DEFINE_VECTOR(rq_result, rqVec, rqIt);

	class			rq_results
	{
	protected:
		rqVec		results;
		static bool	r_sort_pred(const rq_result& a, const rq_result& b)
		{
			return a.range < b.range;
		}
	public:
		IC BOOL		append_result(CObject* _who, f32 _range, s32 _element, BOOL bNearest)
		{
			if (bNearest && !results.empty( ))
			{
				rq_result& R = results.back( );
				if (_range < R.range)
				{
					R.O = _who;
					R.range = _range;
					R.element = _element;
					return TRUE;
				}

				return FALSE;
			}
			results.push_back(rq_result( ));
			rq_result& rq = results.back( );
			rq.range = _range;
			rq.element = _element;
			rq.O = _who;
			return TRUE;
		}
		IC void		append_result(rq_result& res)
		{
			if (0 == results.capacity( ))
			{
				results.reserve(8);
			}

			results.push_back(res);
		}
		IC s32			r_count( )
		{
			return results.size( );
		}
		IC rq_result* r_begin( )
		{
			return &*results.begin( );
		}
		IC rq_result* r_end( )
		{
			return &*results.end( );
		}
		IC void			r_clear( )
		{
			results.clear_not_free( );
		}
		IC void			r_sort( )
		{
			std::sort(results.begin( ), results.end( ), r_sort_pred);
		}
	};
	typedef  BOOL		rq_callback(rq_result& result, LPVOID user_data);
	typedef  BOOL		test_callback(const ray_defs& rd, CObject* object, LPVOID user_data);
};
