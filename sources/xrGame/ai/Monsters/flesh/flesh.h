#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CAI_Flesh : public CBaseMonster,
				  public CControlledEntity<CAI_Flesh> {

	typedef		CBaseMonster					inherited;
	typedef		CControlledEntity<CAI_Flesh>	CControlled;

public:
							CAI_Flesh		();
	virtual					~CAI_Flesh		();	
	
	virtual	void	Load					(pcstr section);
	virtual	BOOL	net_Spawn				(CSE_Abstract* DC);

	virtual	void	CheckSpecParams			(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}

private:
	bool	ConeSphereIntersection(fVector3 ConeVertex, f32 ConeAngle, fVector3 ConeDir, fVector3 SphereCenter, f32 SphereRadius);
	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_Flesh)
#undef script_type_list
#define script_type_list save_type_list(CAI_Flesh)
