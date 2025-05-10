#pragma once

class ENGINE_API IParticleCustom : public IRenderVisual
{
public:
	// geometry-format
	ref_geom		geom;

public:
	virtual 		~IParticleCustom( )
	{ }

	virtual void 	OnDeviceCreate( ) = 0;
	virtual void 	OnDeviceDestroy( ) = 0;

	virtual void	UpdateParent(const fMatrix4x4& m, const fVector3& velocity, BOOL bXFORM) = 0;
	virtual void	OnFrame(u32 dt) = 0;

	virtual void	Play( ) = 0;
	virtual void	Stop(BOOL bDefferedStop = TRUE) = 0;
	virtual BOOL	IsPlaying( ) = 0;

	virtual u32		ParticlesCount( ) = 0;

	virtual f32		GetTimeLimit( ) = 0;
	virtual BOOL	IsLooped( )
	{
		return GetTimeLimit( ) < 0.0f;
	}

	virtual const shared_str	Name( ) = 0;

	virtual IParticleCustom* dcast_ParticleCustom( )
	{
		return this;
	}
};
