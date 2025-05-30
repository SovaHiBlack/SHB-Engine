////////////////////////////////////////////////////////////////////////////
//	Module 		: damage_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Damage manager
////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "damage_manager.h"
#include "..\XR_3DA\Object.h"
#include "..\XR_3DA\skeletoncustom.h"

CDamageManager::CDamageManager( )
{ }

CDamageManager::~CDamageManager( )
{ }

DLL_Pure* CDamageManager::_construct( )
{
	m_object = smart_cast<CObject*>(this);
	VERIFY(m_object);
	return					(m_object);
}

void CDamageManager::reload(pcstr section, CIniFile* ini)
{
	m_default_hit_factor = 1.f;
	m_default_wound_factor = 1.f;

	bool section_exist = ini && ini->section_exist(section);

	// ��������� ��������� ���������
	if (section_exist)
	{
		string32 buffer;
		if (ini->line_exist(section, "default"))
		{
			pcstr value = ini->r_string(section, "default");
			m_default_hit_factor = (f32)atof(_GetItem(value, 0, buffer));
			m_default_wound_factor = (f32)atof(_GetItem(value, 2, buffer));
		}
	}

	//���������������� default �����������
	init_bones(section, ini);

	// �������� ������� ����������� ���������
	if (section_exist)
	{
		load_section(section, ini);
	}
}

void CDamageManager::reload(pcstr section, pcstr line, CIniFile* ini)
{
	if (ini && ini->section_exist(section) && ini->line_exist(section, line))
		reload(ini->r_string(section, line), ini);
	else
		reload(section, 0);
}

void CDamageManager::init_bones(pcstr section, CIniFile* ini)
{
	CKinematics* kinematics = smart_cast<CKinematics*>(m_object->Visual( ));
	VERIFY(kinematics);
	for (u16 i = 0; i < kinematics->LL_BoneCount( ); i++)
	{
		CBoneInstance& bone_instance = kinematics->LL_GetBoneInstance(i);
		bone_instance.set_param(0, m_default_hit_factor);
		bone_instance.set_param(1, 1.f);
		bone_instance.set_param(2, m_default_wound_factor);
	}
}
void CDamageManager::load_section(pcstr section, CIniFile* ini)
{
	string32				buffer;
	CKinematics* kinematics = smart_cast<CKinematics*>(m_object->Visual( ));
	CIniFile::Sect& damages = ini->r_section(section);
	for (CIniFile::SectCIt i = damages.Data.begin( ); damages.Data.end( ) != i; ++i)
	{
		if (xr_strcmp(*(*i).first, "default"))
		{ // read all except default line
			VERIFY(m_object);
			s32						bone = kinematics->LL_BoneID(i->first);
			R_ASSERT2(BI_NONE != bone, *(*i).first);
			CBoneInstance& bone_instance = kinematics->LL_GetBoneInstance(u16(bone));
			bone_instance.set_param(0, (f32)atof(_GetItem(*(*i).second, 0, buffer)));
			bone_instance.set_param(1, (f32)atoi(_GetItem(*(*i).second, 1, buffer)));
			bone_instance.set_param(2, (f32)atof(_GetItem(*(*i).second, 2, buffer)));
			if (_GetItemCount(*(*i).second) < 4)
			{
				bone_instance.set_param(3, (f32)atof(_GetItem(*(*i).second, 0, buffer)));
			}
			else
			{
				bone_instance.set_param(3, (f32)atof(_GetItem(*(*i).second, 3, buffer)));
			}
			if (0 == bone && (fis_zero(bone_instance.get_param(0)) || fis_zero(bone_instance.get_param(2))))
			{
				string256 error_str;
				sprintf_s(error_str, "hit_scale and wound_scale for root bone cannot be zero. see section [%s]", section);
				R_ASSERT2(0, error_str);
			}
		}
	}
}


void  CDamageManager::HitScale(const s32 element, f32& hit_scale, f32& wound_scale, bool aim_bullet)
{
	if (BI_NONE == u16(element))
	{
		//������� ��� ��������� ��� BI_NONE ������ ��� 1.f 
		hit_scale = 1.f * m_default_hit_factor;
		wound_scale = 1.f * m_default_wound_factor;
		return;
	}

	CKinematics* V = smart_cast<CKinematics*>(m_object->Visual( ));			VERIFY(V);
	// get hit scale
	f32 scale;
	if (aim_bullet)
	{
		scale = V->LL_GetBoneInstance(u16(element)).get_param(3);
	}
	else
	{
		scale = V->LL_GetBoneInstance(u16(element)).get_param(0);
	}
	hit_scale = scale;

	// get wound scale
	scale = V->LL_GetBoneInstance(u16(element)).get_param(2);
	wound_scale = scale;
}
