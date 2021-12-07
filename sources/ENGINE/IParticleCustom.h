#pragma once

class ENGINE_API IParticleCustom : public IRender_Visual
{
public:
	// geometry-format
	ref_geom												geom;

	virtual						~IParticleCustom			( )
	{ }

	virtual void				OnDeviceCreate				( ) = 0;
	virtual void				OnDeviceDestroy				( ) = 0;

	virtual void				UpdateParent				(const Fmatrix& m, const Fvector3& velocity, BOOL bXFORM) = 0;
	virtual void				OnFrame						(unsigned int dt) = 0;

	virtual void				Play						( ) = 0;
	virtual void				Stop						(BOOL bDefferedStop = TRUE) = 0;
	virtual BOOL				IsPlaying					( ) = 0;

	virtual unsigned int					ParticlesCount				( ) = 0;

	virtual float				GetTimeLimit				( ) = 0;
	virtual bool				IsLooped					( )
	{
		return GetTimeLimit( ) < 0.0f;
	}

	virtual const CSharedString	Name						( ) = 0;

	virtual IParticleCustom*	dcast_ParticleCustom		( )
	{
		return this;
	}
};
