#pragma once

class CPhysicsShellHolder;
struct dContact;
struct SGameMtl;

class CPHCollisionDamageReceiver
{
	typedef std::pair<u16, f32> SControledBone;
	DEFINE_VECTOR(SControledBone, DAMAGE_CONTROLED_BONES_V, DAMAGE_BONES_I);
	struct SFind
	{
		u16 id;
		SFind(u16 _id)
		{
			id = _id;
		};
		bool operator () (const SControledBone& cb)
		{
			return (cb.first == id);
		}
	};
	DAMAGE_CONTROLED_BONES_V m_controled_bones;

protected:
	virtual CPhysicsShellHolder* PPhysicsShellHolder( ) = 0;
	void						Init( );
	void						Hit(u16 source_id, u16 bone_id, f32 power, const fVector3& dir, fVector3& pos);
	void						Clear( );

private:
	void						BoneInsert(u16 id, f32 k);

	IC		DAMAGE_BONES_I				FindBone(u16 id)
	{
		return std::find_if(m_controled_bones.begin( ), m_controled_bones.end( ), SFind(id));
	}
	static	void 						CollisionCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2);
};
