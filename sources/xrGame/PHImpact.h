#pragma once

struct SPHImpact 
{
	Fvector3 force;
	Fvector3 point;
	u16		geom;
	SPHImpact(const Fvector3& aforce,const Fvector3& apoint,u16 root_geom){force.set(aforce);point.set(apoint);geom=root_geom;}
};

//DEFINE_VECTOR(SPHImpact,PH_IMPACT_STORAGE,PH_IMPACT_I)
using PH_IMPACT_STORAGE = xr_vector<SPHImpact>;
using PH_IMPACT_I = PH_IMPACT_STORAGE::iterator;
