#include "stdafx.h"
#include "pseudo_gigant_step_effector.h" 

CPseudogigantStepEffector::CPseudogigantStepEffector(f32 time, f32 amp, f32 periods, f32 power)
					: CEffectorCam(eCEPseudoGigantStep, time)
{
	total			= time;

	max_amp			= amp * power;
	period_number	= periods;
	this->power		= power;
}

BOOL CPseudogigantStepEffector::Process(Fvector &p, Fvector &d, Fvector &n, f32& fFov, f32& fFar, f32& fAspect)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;

	// процент оставшегося времени
	f32 time_left_perc = fLifeTime / total;

	// Инициализация
	Fmatrix	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(d);
	Mdef.i.crossproduct	(n,d);
	Mdef.c.set			(p);

	f32 period_all	= period_number * PI_MUL_2;		// макс. значение цикла
	f32 k				= 1 - time_left_perc + EPS_L + (1 - power);
	f32 cur_amp		= max_amp * (PI / 180) / (10 * k * k);

	Fvector dangle; 
	dangle.x = cur_amp/2	* _sin(period_all	* (1.0f - time_left_perc));
	dangle.y = cur_amp		* _cos(period_all/2 * (1.0f - time_left_perc));
	dangle.z = cur_amp/4	* _sin(period_all/4	* (1.0f - time_left_perc));

	// Установить углы смещения
	Fmatrix		R;
	R.setHPB	(dangle.x,dangle.y,dangle.z);

	Fmatrix		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);

	return TRUE;
}
