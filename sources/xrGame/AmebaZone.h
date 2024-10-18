#pragma once

class CAmebaZone :
	public CVisualZone,
	public CPHUpdateObject
{
typedef				CVisualZone		inherited;	
f32 m_fVelocityLimit;
public:
									CAmebaZone			()						;
									~CAmebaZone			()						;
	virtual				void		Affect				(SZoneObjectInfo* O)		;
	
protected:
	virtual				void		PhTune				(dReal step)			;
	virtual				void		PhDataUpdate		(dReal step)			{;}
	virtual				bool		BlowoutState		()						;
	virtual				void		SwitchZoneState		(EZoneState new_state)	;
	virtual				void		Load				(pcstr section)		;
	virtual				f32		distance_to_center	(CObject* O)			;
};
