////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "setup_manager.h"
#include "sight_control_action.h"

namespace LevelGraph {
	class CVertex;
};

class CStalker;

class CSightManager : public CSetupManager<CSightControlAction, CStalker,u32> {
public:
	typedef CSetupManager<CSightControlAction, CStalker,u32> inherited;

private:
	bool			m_enabled;
	bool			m_turning_in_place;
	f32			m_max_left_angle;
	f32			m_max_right_angle;

public:
	CSightManager(CStalker* object);
	virtual			~CSightManager						();
	virtual	void	Load								(pcstr section);
	virtual	void	reinit								();
	virtual	void	reload								(pcstr section);
			void	remove_links						(CObject *object);
			void	Exec_Look							(f32 dt);
			bool	bfIf_I_SeePosition					(fVector3 tPosition) const;
			void	SetPointLookAngles					(const fVector3& tPosition, f32& yaw, f32& pitch, const CGameObject *object = 0);
			void	SetFirePointLookAngles				(const fVector3& tPosition, f32& yaw, f32& pitch, const CGameObject *object = 0);
			void	SetDirectionLook					();
			void	SetLessCoverLook					(const LevelGraph::CVertex *tpNode, bool bDifferenceLook);
			void	SetLessCoverLook					(const LevelGraph::CVertex *tpNode, f32 fMaxHeadTurnAngle, bool bDifferenceLook);
			void	vfValidateAngleDependency			(f32 x1, f32& x2, f32 x3);
			bool	need_correction						(f32 x1, f32 x2, f32 x3);
	IC		bool	GetDirectionAnglesByPrevPositions	(f32& yaw, f32& pitch);
			bool	GetDirectionAngles					(f32& yaw, f32& pitch);
	IC		bool	use_torso_look						() const;
	template <typename T1, typename T2, typename T3>
	IC		void	setup								(T1 _1, T2 _2, T3 _3);
	template <typename T1, typename T2>
	IC		void	setup								(T1 _1, T2 _2);
	template <typename T1>
	IC		void	setup								(T1 _1);
			void	setup								(const CSightAction &sight_action);
	virtual void	update								();
	IC		bool	turning_in_place					() const;
	IC		bool	enabled								() const;
	IC		void	enable								(bool value);
};

#include "sight_manager_inline.h"
