#include "stdafx.h"
#include "pseudo_gigant_step_effector.h" 

CPseudogigantStepEffector::CPseudogigantStepEffector(F32 time, F32 amp, F32 periods, F32 power)
					: CEffectorCam(eCEPseudoGigantStep, time)
{
	total			= time;

	max_amp			= amp * power;
	period_number	= periods;
	this->power		= power;
}

BOOL CPseudogigantStepEffector::Process(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;

	// ������� ����������� �������
	F32 time_left_perc = fLifeTime / total;

	// �������������
	Fmatrix	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(d);
	Mdef.i.crossproduct	(n,d);
	Mdef.c.set			(p);

	F32 period_all	= period_number * PI_MUL_2;		// ����. �������� �����
	F32 k				= 1 - time_left_perc + EPS_L + (1 - power);
	F32 cur_amp		= max_amp * (PI / 180) / (10 * k * k);

	Fvector dangle; 
	dangle.x = cur_amp/2	* _sin(period_all	* (1.0f - time_left_perc));
	dangle.y = cur_amp		* _cos(period_all/2 * (1.0f - time_left_perc));
	dangle.z = cur_amp/4	* _sin(period_all/4	* (1.0f - time_left_perc));

	// ���������� ���� ��������
	Fmatrix		R;
	R.setHPB	(dangle.x,dangle.y,dangle.z);

	Fmatrix		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);

	return TRUE;
}
