#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CFlesh : public CBaseMonster, public CControlledEntity<CFlesh>
{
	using inherited = CBaseMonster;
	using CControlled = CControlledEntity<CFlesh>;

public:
	CFlesh();
	virtual					~CFlesh();
	
	virtual	void	Load					(const char* section);
	virtual	BOOL	net_Spawn				(CSE_Abstract* DC);

	virtual	void	CheckSpecParams			(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}

private:
	bool	ConeSphereIntersection	(Fvector3 ConeVertex, float ConeAngle, Fvector3 ConeDir, Fvector3 SphereCenter, float SphereRadius);
	
public:
	static void script_register(lua_State*);
};

add_to_type_list(CFlesh)
#undef script_type_list
#define script_type_list save_type_list(CFlesh)
