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
		unsigned int										C;
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
	unsigned int											number_vertices;
	unsigned short*										indices;
	unsigned int											number_indices;

	virtual void		transfer				(Fmatrix& mXform, fvfVertexOut* vDest, unsigned int C, unsigned short* iDest, unsigned int iOffset) = 0;
	virtual void		transfer				(Fmatrix& mXform, fvfVertexOut* vDest, unsigned int C, unsigned short* iDest, unsigned int iOffset, float du, float dv) = 0;
	virtual				~IRenderDetailModel		( )
	{ }
};
