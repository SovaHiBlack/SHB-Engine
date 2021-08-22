#pragma once

//DEFINE_VECTOR(ref_shader, SHADER_VECTOR, SHADER_VECTOR_IT);
using SHADER_VECTOR = xr_vector<ref_shader>;
using SHADER_VECTOR_IT = SHADER_VECTOR::iterator;

class CWalmarkManager
{
private:
	SHADER_VECTOR					m_wallmarks;
	Fvector3						m_pos;

public:
	CObject* m_owner;
	CWalmarkManager( );
	~CWalmarkManager( );
	void	Load(const char* section);
	void	Clear( );
	void	AddWallmark(const Fvector3& dir, const Fvector3& start_pos, float range, float wallmark_size, SHADER_VECTOR& wallmarks_vector, int t);

	void	PlaceWallmarks(const Fvector3& start_pos);

	void	__stdcall StartWorkflow( );
};
