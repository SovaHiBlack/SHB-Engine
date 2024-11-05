// CDemoRecord.h: interface for the CDemoRecord class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "iinputreceiver.h"
#include "effector.h"

class ENGINE_API CDemoRecord :
	public CEffectorCam,
	public IInputReceiver
{
private:
	int			iCount;
	IWriter*	file;
	fVector3		m_HPB;
	fVector3		m_Position;
	fMatrix4x4		m_Camera;
	u32			m_Stage;

	fVector3		m_vT;
	fVector3		m_vR;
	fVector3		m_vVelocity;
	fVector3		m_vAngularVelocity;

	BOOL		m_bMakeCubeMap;
	BOOL		m_bMakeScreenshot;
	BOOL		m_bMakeLevelMap;
	BOOL		m_bOverlapped;

	f32			m_fSpeed0;
	f32			m_fSpeed1;
	f32			m_fSpeed2;
	f32			m_fSpeed3;
	f32			m_fAngSpeed0;
	f32			m_fAngSpeed1;
	f32			m_fAngSpeed2;
	f32			m_fAngSpeed3;

	void		MakeCubeMapFace(fVector3& D, fVector3& N);
	void		MakeLevelMapProcess		();
	void		MakeScreenshotFace		();
	void		RecordKey				();
	void		MakeCubemap				();
	void		MakeScreenshot			();
	void		MakeLevelMapScreenshot	();
public:
				CDemoRecord				(const char *name, f32 life_time=60*60*1000);
	virtual		~CDemoRecord();

	virtual void IR_OnKeyboardPress		(int dik);
	virtual void IR_OnKeyboardHold		(int dik);
	virtual void IR_OnMouseMove			(int dx, int dy);
	virtual void IR_OnMouseHold			(int btn);
	
	virtual BOOL Overlapped				(){return m_bOverlapped;}
	virtual	BOOL Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);
};
