#pragma once

struct SBoneProtections
{
	struct BoneProtection
	{
		float		koeff;
		float		armour;
		BOOL		BonePassBullet;
	};

	float				m_fHitFrac;
	using storage_type = xr_map<S16, BoneProtection>;
	using storage_it = storage_type::iterator;

	SBoneProtections( )
	{
		m_default.koeff = 1.0f;
		m_default.armour = 0;
	}

	BoneProtection		m_default;
	storage_type		m_bones_koeff;
	void				reload(const CSharedString& outfit_section, CKinematics* kinematics);
	float				getBoneProtection(S16 bone_id);
	float				getBoneArmour(S16 bone_id);
	BOOL				getBonePassBullet(S16 bone_id);
};
