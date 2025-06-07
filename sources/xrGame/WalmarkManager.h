#pragma once

DEFINE_VECTOR(ref_shader, SHADER_VECTOR, SHADER_VECTOR_IT);

class CWalmarkManager
{
private:
	SHADER_VECTOR					m_wallmarks;
	fVector3						m_pos;

public:
	CObject* m_owner;
	CWalmarkManager( );
	~CWalmarkManager( );
	void	Load(pcstr section);
	void	Clear( );
	void	AddWallmark(const fVector3& dir, const fVector3& start_pos, f32 range, f32 wallmark_size, SHADER_VECTOR& wallmarks_vector, s32 t);
	void	PlaceWallmarks(const fVector3& start_pos);

	void	__stdcall StartWorkflow( );
};
