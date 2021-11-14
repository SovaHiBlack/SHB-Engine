#pragma once

class CPHShellHolder;
struct dContact;
struct SGameMtl;

class CPHCollisionDamageReceiver
{
	using SControledBone = std::pair<unsigned short, float>;

//	DEFINE_VECTOR(SControledBone, DAMAGE_CONTROLED_BONES_V, DAMAGE_BONES_I);
	using DAMAGE_CONTROLED_BONES_V = xr_vector<SControledBone>;
	using DAMAGE_BONES_I = DAMAGE_CONTROLED_BONES_V::iterator;

	struct SFind
	{
		unsigned short id;
		SFind(unsigned short _id)
		{
			id = _id;
		}
		bool operator () (const SControledBone& cb)
		{
			return cb.first == id;
		}
	};
	DAMAGE_CONTROLED_BONES_V m_controled_bones;

protected:
	virtual CPHShellHolder* PPhysicsShellHolder( ) = 0;
	void						Init( );
	void						Hit(unsigned short source_id, unsigned short bone_id, float power, const Fvector3& dir, Fvector3& pos);
	void						Clear( );

private:
	void						BoneInsert(unsigned short id, float k);

	inline		DAMAGE_BONES_I				FindBone(unsigned short id)
	{
		return std::find_if(m_controled_bones.begin( ), m_controled_bones.end( ), SFind(id));
	}
	static void 						CollisionCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2);
};
