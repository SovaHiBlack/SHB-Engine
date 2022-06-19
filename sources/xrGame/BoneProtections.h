#pragma once

struct SBoneProtections{
	struct BoneProtection {
		f32		koeff;
		f32		armour;
		BOOL		BonePassBullet;
	};
	f32				m_fHitFrac;
	typedef xr_map<s16,BoneProtection>		storage_type;
	typedef storage_type::iterator	storage_it;
						SBoneProtections	()								{m_default.koeff = 1.0f; m_default.armour = 0;}
	BoneProtection		m_default;
	storage_type		m_bones_koeff;
	void				reload				(const shared_str& outfit_section, CKinematics* kinematics);
	f32				getBoneProtection	(s16 bone_id);
	f32				getBoneArmour		(s16 bone_id);
	BOOL				getBonePassBullet	(s16 bone_id);
};

