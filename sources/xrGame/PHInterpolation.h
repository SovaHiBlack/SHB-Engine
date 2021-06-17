#pragma once

#include "CycleConstStorage.h"
#include "ode_include.h"

class CPHInterpolation
{
public:
						CPHInterpolation			( );
	void				SetBody						(dBodyID body);
	static const U16	PH_INTERPOLATION_POINTS		= 2;
	void				InterpolatePosition			(Fvector3& pos);
	void				InterpolateRotation			(Fmatrix& rot);
	void				UpdatePositions				( );
	void				UpdateRotations				( );
	void				ResetPositions				( );
	void				ResetRotations				( );
	void				GetRotation					(Fquaternion& q, U16 num);
	void				GetPosition					(Fvector3& p, U16 num);
	void				SetRotation					(const Fquaternion& q, U16 num);
	void				SetPosition					(const Fvector3& p, U16 num);

private:
	dBodyID																m_body;
	CCycleConstStorage<Fvector3, PH_INTERPOLATION_POINTS>				qPositions;
	CCycleConstStorage<Fquaternion, PH_INTERPOLATION_POINTS>			qRotations;
};
