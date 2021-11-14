#pragma once

class ENGINE_API IRenderDetailModel
{
public:
	struct fvfVertexIn
	{
		Fvector3								P;
		float									u;
		float									v;
	};

	struct fvfVertexOut
	{
		Fvector3								P;
		u32										C;
		float									u;
		float									v;
	};

	Fsphere										bv_sphere;
	Fbox3										bv_bb;
	Flags32										m_Flags;
	float										m_fMinScale;
	float										m_fMaxScale;

	ref_shader									shader;
	fvfVertexIn*								vertices;
	u32											number_vertices;
	unsigned short*										indices;
	u32											number_indices;

	virtual void		transfer				(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, unsigned short* iDest, u32 iOffset) = 0;
	virtual void		transfer				(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, unsigned short* iDest, u32 iOffset, float du, float dv) = 0;
	virtual				~IRenderDetailModel		( )
	{ }
};
