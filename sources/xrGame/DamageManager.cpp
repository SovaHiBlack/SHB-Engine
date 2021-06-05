#include "stdafx.h"

#include "DamageManager.h"
#include "..\ENGINE\skeletoncustom.h"

C_DamageManager::C_DamageManager( )
{ }

C_DamageManager::~C_DamageManager( )
{ }

DLL_Pure* C_DamageManager::_construct( )
{
	m_object								= smart_cast<CObject*>(this);
	VERIFY									(m_object);
	return									m_object;
}

void C_DamageManager::reload(const char* section, CIniFile* ini)
{
	m_default_hit_factor					= 1.0f;
	m_default_wound_factor					= 1.0f;

	bool section_exist						= ini && ini->section_exist(section);

	// прочитать дефолтные параметры
	if (section_exist)
	{
		string32							buffer;
		if (ini->line_exist(section, "default"))
		{
			const char* value				= ini->r_string(section, "default");
			m_default_hit_factor			= (float) atof(_GetItem(value, 0, buffer));
			m_default_wound_factor			= (float) atof(_GetItem(value, 2, buffer));
		}
	}

	// инициализировать default параметрами
	init_bones								(section, ini);

	// записать поверху прописанные параметры
	if (section_exist)
	{
		load_section						(section, ini);
	}
}

void C_DamageManager::reload(const char* section, const char* line, CIniFile* ini)
{
	if (ini && ini->section_exist(section) && ini->line_exist(section, line))
	{
		reload								(ini->r_string(section, line), ini);
	}
	else
	{
		reload								(section, 0);
	}
}

void C_DamageManager::HitScale(const int element, float& hit_scale, float& wound_scale, bool aim_bullet)
{
	if (BI_NONE == u16(element))
	{	// считаем что параметры для BI_NONE заданы как 1.0f
		hit_scale							= 1.0f * m_default_hit_factor;
		wound_scale							= 1.0f * m_default_wound_factor;
		return;
	}

	CKinematics* V							= smart_cast<CKinematics*>(m_object->Visual( ));
	VERIFY									(V);

	// get hit scale
	float scale;
	if (aim_bullet)
	{
		scale								= V->LL_GetBoneInstance(u16(element)).get_param(3);
	}
	else
	{
		scale								= V->LL_GetBoneInstance(u16(element)).get_param(0);
	}

	hit_scale								= scale;

	// get wound scale
	scale									= V->LL_GetBoneInstance(u16(element)).get_param(2);
	wound_scale								= scale;
}

void C_DamageManager::load_section(const char* section, CIniFile* ini)
{
	string32								buffer;
	CKinematics* kinematics					= smart_cast<CKinematics*>(m_object->Visual( ));
	CIniFile::Sect& damages					= ini->r_section(section);
	for (CIniFile::SectCIt i = damages.Data.begin( ); damages.Data.end( ) != i; ++i)
	{
		if (xr_strcmp(*(*i).first, "default"))
		{
			// read all except default line
			VERIFY							(m_object);
			int bone						= kinematics->LL_BoneID(i->first);
			R_ASSERT2						(BI_NONE != bone, *(*i).first);
			CBoneInstance& bone_instance	= kinematics->LL_GetBoneInstance(u16(bone));
			bone_instance.set_param			(0, (float) atof(_GetItem(*(*i).second, 0, buffer)));
			bone_instance.set_param			(1, (float) atoi(_GetItem(*(*i).second, 1, buffer)));
			bone_instance.set_param			(2, (float) atof(_GetItem(*(*i).second, 2, buffer)));
			if (_GetItemCount(*(*i).second) < 4)
			{
				bone_instance.set_param		(3, (float) atof(_GetItem(*(*i).second, 0, buffer)));
			}
			else
			{
				bone_instance.set_param		(3, (float) atof(_GetItem(*(*i).second, 3, buffer)));
			}

			if (0 == bone && (fis_zero(bone_instance.get_param(0)) || fis_zero(bone_instance.get_param(2))))
			{
				string256					error_str;
				sprintf_s					(error_str, "hit_scale and wound_scale for root bone cannot be zero. see section [%s]", section);
				R_ASSERT2					(0, error_str);
			}
		}
	}
}

void C_DamageManager::init_bones(const char* section, CIniFile* ini)
{
	CKinematics* kinematics					= smart_cast<CKinematics*>(m_object->Visual( ));
	VERIFY									(kinematics);
	for (u16 i = 0; i < kinematics->LL_BoneCount( ); i++)
	{
		CBoneInstance& bone_instance		= kinematics->LL_GetBoneInstance(i);
		bone_instance.set_param				(0, m_default_hit_factor);
		bone_instance.set_param				(1, 1.0f);
		bone_instance.set_param				(2, m_default_wound_factor);
	}
}
