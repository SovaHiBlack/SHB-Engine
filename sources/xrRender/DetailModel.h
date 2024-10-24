#ifndef DetailModelH
#define DetailModelH
#pragma once

class CDetail		: public IRender_DetailModel
{
public:
	void			Load		(IReader* S);
	void			Optimize	();
	virtual void	Unload		();

	virtual void	transfer	(fMatrix4x4& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset);
	virtual void	transfer	(fMatrix4x4& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, f32 du, f32 dv);
	virtual			~CDetail	();
};
#endif
