#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CFlesh : public CBaseMonster,
				  public CControlledEntity<CFlesh> {

	typedef		CBaseMonster					inherited;
	typedef		CControlledEntity<CFlesh>	CControlled;

public:
	CFlesh();
	virtual					~CFlesh();
	
	virtual	void	Load					(const char* section);
	virtual	BOOL	net_Spawn				(CSE_Abstract* DC);

	virtual	void	CheckSpecParams			(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}

private:
	bool	ConeSphereIntersection	(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, 
									Fvector SphereCenter, float SphereRadius);
	
public:
	static void script_register(lua_State*);
};

add_to_type_list(CFlesh)
#undef script_type_list
#define script_type_list save_type_list(CFlesh)
