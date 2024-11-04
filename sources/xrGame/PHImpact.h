#pragma once

struct SPHImpact 
{
	fVector3 force;
	fVector3 point;
	u16		geom;
	SPHImpact(const fVector3& aforce,const fVector3& apoint,u16 root_geom){force.set(aforce);point.set(apoint);geom=root_geom;}
};

DEFINE_VECTOR(SPHImpact,PH_IMPACT_STORAGE,PH_IMPACT_I)
