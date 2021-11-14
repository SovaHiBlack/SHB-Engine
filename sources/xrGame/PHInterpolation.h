#pragma once

#include "CycleConstStorage.h"
#include "ode_include.h"

class CPHInterpolation
{
public:
						CPHInterpolation			( );
	void				SetBody						(dBodyID body);
	static const unsigned short	PH_INTERPOLATION_POINTS		= 2;
	void				InterpolatePosition			(Fvector3& pos);
	void				InterpolateRotation			(Fmatrix& rot);
	void				UpdatePositions				( );
	void				UpdateRotations				( );
	void				ResetPositions				( );
	void				ResetRotations				( );
	void				GetRotation					(Fquaternion& q, unsigned short num);
	void				GetPosition					(Fvector3& p, unsigned short num);
	void				SetRotation					(const Fquaternion& q, unsigned short num);
	void				SetPosition					(const Fvector3& p, unsigned short num);

private:
	dBodyID																m_body;
	CCycleConstStorage<Fvector3, PH_INTERPOLATION_POINTS>				qPositions;
	CCycleConstStorage<Fquaternion, PH_INTERPOLATION_POINTS>			qRotations;
};
