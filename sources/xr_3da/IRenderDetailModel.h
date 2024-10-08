#pragma once

//////////////////////////////////////////////////////////////////////////
// definition (Detail Model)
class	ENGINE_API	IRender_DetailModel
{
public:
	struct fvfVertexIn	{
		Fvector	P;
		F32		u;
		F32		v;
	};
	struct fvfVertexOut	{
		Fvector P;
		u32		C;
		F32		u;
		F32		v;
	};
public:
	Fsphere		bv_sphere;
	Fbox		bv_bb;
	flags32		m_Flags;
	F32			m_fMinScale;
	F32			m_fMaxScale;

	ref_shader	shader;
	fvfVertexIn	*vertices;
	u32			number_vertices;     
	u16			*indices;
	u32			number_indices;
public:
	virtual void					transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset)	= 0;
	virtual void					transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, F32 du, F32 dv)	= 0;
	virtual ~IRender_DetailModel()	{};
};
