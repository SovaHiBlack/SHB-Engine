#pragma once

#include "IInputReceiver.h"
#include "EffectorCam.h"

class ENGINE_API CDemoRecord : public CEffectorCam, public IInputReceiver
{
private:
	int										iCount;
	IWriter*								file;
	Fvector3								m_HPB;
	Fvector3								m_Position;
	Fmatrix									m_Camera;
	unsigned int										m_Stage;

	Fvector3								m_vT;
	Fvector3								m_vR;
	Fvector3								m_vVelocity;
	Fvector3								m_vAngularVelocity;

	bool									m_bMakeCubeMap;
	bool									m_bMakeScreenshot;
	bool									m_bMakeLevelMap;
	bool									m_bOverlapped;

	float									m_fSpeed0;
	float									m_fSpeed1;
	float									m_fSpeed2;
	float									m_fSpeed3;
	float									m_fAngSpeed0;
	float									m_fAngSpeed1;
	float									m_fAngSpeed2;
	float									m_fAngSpeed3;

	void			MakeCubeMapFace			(Fvector3& D, Fvector3& N);
	void			MakeLevelMapProcess		( );
	void			MakeScreenshotFace		( );
	void			RecordKey				( );
	void			MakeCubemap				( );
	void			MakeScreenshot			( );
	void			MakeLevelMapScreenshot	( );

public:
					CDemoRecord				(const char* name, float life_time = 60 * 60 * 1000);
	virtual			~CDemoRecord			( );

	virtual void	IR_OnKeyboardPress		(int dik);
	virtual void	IR_OnKeyboardHold		(int dik);
	virtual void	IR_OnMouseMove			(int dx, int dy);
	virtual void	IR_OnMouseHold			(int btn);

	virtual BOOL	Overlapped				( )
	{
		return m_bOverlapped;
	}
	virtual BOOL	Process					(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);
};
