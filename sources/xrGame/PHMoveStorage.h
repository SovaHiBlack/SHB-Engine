#pragma once

#include "PHGeometryOwner.h"

//DEFINE_VECTOR(dReal *&,POSITIONS_STORAGE,POSITIONS_I);

class CPHPositionsPairs
{
	GEOM_I geom;
public:
	CPHPositionsPairs(GEOM_I i)
	{
		geom=i;
	}
	void Positions(const Fvector3*& p0,const Fvector3*& p1);
	inline CPHPositionsPairs& operator ++	()
	{
		++geom;
		return *this;
	}
	inline dGeomID dGeom()
	{
		return (*geom)->geometry_transform();
	}
	inline CPHPositionsPairs& operator ++	(int)
	{
		geom++;
		return *this;
	}
	inline CPHPositionsPairs& operator =	(const CPHPositionsPairs& right)
	{
		geom=right.geom;
	}
	inline bool operator ==	(const CPHPositionsPairs& right ) const
	{
		return geom==right.geom;
	}
	inline bool operator !=	(const CPHPositionsPairs& right ) const
	{
		return geom!=right.geom;
	}
};

class CPHMoveStorage
{
	GEOM_STORAGE m_trace_geometries;
public:
	typedef CPHPositionsPairs iterator;
	inline	iterator	begin	()					{return	CPHPositionsPairs(m_trace_geometries.begin());}
	inline	iterator	end		()					{return	CPHPositionsPairs(m_trace_geometries.end());}
	inline	bool		empty	()const				{return m_trace_geometries.empty();}
		void		add		(CODEGeom* g)		{m_trace_geometries.push_back(g);}
		void		clear	()					{m_trace_geometries.clear();}
};
