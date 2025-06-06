#include "stdafx.h"
#include "ai_monster_utils.h"
#include "../../Entity.h"
#include "../../ai_object_location.h"
#include "../../AISpace.h"
#include "../../level_graph.h"
#include "../../..\XR_3DA\skeletoncustom.h"

// ���������, ��������� �� ������ entity �� ����
// ���������� ������� �������, ���� �� ��������� �� ����, ��� ����� ��� ����
fVector3 get_valid_position(const CEntity *entity, const fVector3& actual_position)
{
	if (
		ai().level_graph().valid_vertex_id(entity->ai_location().level_vertex_id()) &&
		ai().level_graph().valid_vertex_position(entity->Position()) && 
		ai().level_graph().inside(entity->ai_location().level_vertex_id(), entity->Position())
		)
		return			(actual_position);
	else
		return			(ai().level_graph().vertex_position(entity->ai_location().level_vertex()));
}

// ���������� true, ���� ������ entity ��������� �� ����
bool object_position_valid(const CEntity *entity)
{
	return				(
		ai().level_graph().valid_vertex_id(entity->ai_location().level_vertex_id()) &&
		ai().level_graph().valid_vertex_position(entity->Position()) && 
		ai().level_graph().inside(entity->ai_location().level_vertex_id(), entity->Position())
		);
}

fVector3 get_bone_position	(CObject *object, pcstr bone_name)
{
	u16 bone_id			= smart_cast<CKinematics*>(object->Visual())->LL_BoneID				(bone_name);
	CBoneInstance &bone = smart_cast<CKinematics*>(object->Visual())->LL_GetBoneInstance	(bone_id);

	fMatrix4x4	global_transform;
	global_transform.mul	(object->XFORM(),bone.mTransform);

	return	(global_transform.c);
}
