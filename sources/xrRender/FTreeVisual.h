#pragma once

// refs
struct	FSlideWindowItem;

class	FTreeVisual : public IRenderVisual, public IRenderMesh
{
private:
	struct	_5color
	{
		fVector3					rgb;		// - all static lighting
		f32					hemi;		// - hemisphere
		f32					sun;		// - sun
	};

protected:
	_5color						c_scale;
	_5color						c_bias;
	fMatrix4x4						xform;

public:
	virtual void Render			(f32 LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void Copy(IRenderVisual* pFrom);
	virtual void Release		();

	FTreeVisual(void);
	virtual ~FTreeVisual(void);
};

class FTreeVisual_ST :	public FTreeVisual
{
	typedef FTreeVisual inherited;
public:
					FTreeVisual_ST	(void);
	virtual			~FTreeVisual_ST	(void);

	virtual void	Render			(f32 LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void	Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void	Copy(IRenderVisual* pFrom);
	virtual void	Release			();
private:
	FTreeVisual_ST				(const FTreeVisual_ST& other);
	void	operator=			( const FTreeVisual_ST& other);
};

class FTreeVisual_PM :	public FTreeVisual
{
	typedef FTreeVisual inherited;
private:
	FSlideWindowItem*	pSWI;
	u32					last_lod;
public:
					FTreeVisual_PM	(void);
	virtual			~FTreeVisual_PM	(void);

	virtual void	Render			(f32 LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void	Load			(pcstr N, IReader *data, u32 dwFlags);
	virtual void	Copy(IRenderVisual* pFrom);
	virtual void	Release			();
private:
	FTreeVisual_PM				(const FTreeVisual_PM& other);
	void	operator=			( const FTreeVisual_PM& other);
};

const int		FTreeVisual_tile	= 16;
const int		FTreeVisual_quant	= 32768/FTreeVisual_tile;
