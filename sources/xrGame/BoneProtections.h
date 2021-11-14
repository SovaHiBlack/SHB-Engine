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
	using storage_type = xr_map<signed short, BoneProtection>;
	using storage_it = storage_type::iterator;

	SBoneProtections( )
	{
		m_default.koeff = 1.0f;
		m_default.armour = 0;
	}

	BoneProtection		m_default;
	storage_type		m_bones_koeff;
	void				reload(const CSharedString& outfit_section, CKinematics* kinematics);
	float				getBoneProtection(signed short bone_id);
	float				getBoneArmour(signed short bone_id);
	BOOL				getBonePassBullet(signed short bone_id);
};
