///////////////////////////////////////////////////////////////////////

#pragma once

class CPHShell;
class CPHShellSplitterHolder;

#include "PHJoint.h"
#include "PHElement.h"
#include "PHDefs.h"
#include "PHShellSplitter.h"
#include "phmovestorage.h"

class CPHShell: public CPhysicsShell,public CPHObject {

	friend class CPHShellSplitterHolder;
	enum				
	{
		flActive									=	1<<0,
		flActivating								=	1<<1,
		flRemoveCharacterCollisionAfterDisable		=	1<<2,
	};
	s16						m_active_count;
	flags8					m_flags;
	ELEMENT_STORAGE			elements;
	JOINT_STORAGE			joints;
	CPHShellSplitterHolder	*m_spliter_holder;
	CPHMoveStorage			m_traced_geoms;

protected:
	dSpaceID			    m_space;

public:
	fMatrix4x4					m_object_in_root;
	CPHShell								();							
	virtual ~CPHShell						();
	virtual void			applyImpulseTrace		(const fVector3& pos, const fVector3& dir, f32 val,const u16 id);
	virtual void			applyHit				(const fVector3& pos, const fVector3& dir, f32 val,const u16 id,ALife::EHitType hit_type);

	static void 			BonesCallback				(CBoneInstance* B);
	static void 			StataticRootBonesCallBack	(CBoneInstance* B);
	virtual	BoneCallbackFun* GetBonesCallback		()	{return BonesCallback ;}
	virtual BoneCallbackFun* GetStaticObjectBonesCallback()	{return StataticRootBonesCallBack;}
	virtual	void			add_Element				(CPhysicsElement* E);
	virtual	void			ToAnimBonesPositions	();
	virtual bool			AnimToVelocityState		(f32 dt, f32 l_limit, f32 a_limit );
	virtual void			SetBonesCallbacksOverwrite(bool v);
	void					SetPhObjectInElements	();
	virtual	void			EnableObject			(CPHObject* obj);
	virtual	void			DisableObject			();
	virtual void			SetAirResistance		(dReal linear=default_k_l, dReal angular=default_k_w)
	{
		xr_vector<CPHElement*>::iterator i;
		for(i=elements.begin();elements.end()!=i;++i)
			(*i)->SetAirResistance(linear,angular);
	}
	virtual void			GetAirResistance		(f32& linear, f32& angular)
	{
		(*elements.begin())->GetAirResistance(linear,angular);
	}
	virtual	void			add_Joint				(CPhysicsJoint* J);

	virtual CPHIsland*		PIsland					(){return &Island();};
	virtual void			applyImpulseTrace		(const fVector3& pos, const fVector3& dir, f32 val)	;

	virtual void			Update					()	;											

	virtual void			Activate				(const fMatrix4x4& m0, f32 dt01, const fMatrix4x4& m2,bool disable=false);
	virtual void			Activate				(const fMatrix4x4& transform,const fVector3& lin_vel,const fVector3& ang_vel,bool disable=false);
	virtual void			Activate				(bool disable=false);
	virtual void			Activate				(const fMatrix4x4& start_from, bool disable=false){};

private:
			void			activate				(bool disable);	

public:
	virtual	void			Build					(bool disable=false);
	virtual	void			RunSimulation			(bool place_current_forms=true);
	virtual	void			net_Import				(NET_Packet& P);
	virtual	void			net_Export				(NET_Packet& P);
			void			PresetActive			();
			void			AfterSetActive			();
			void			PureActivate			();
	virtual void			Deactivate				();
	virtual const	CGID&	GetCLGroup				()const;
	virtual			void	RegisterToCLGroup		(CGID g)									;
	virtual			bool	IsGroupObject			()											;
	virtual			void	SetIgnoreStatic			()											;
	virtual			void	SetIgnoreDynamic		()											;
	virtual			void	SetRagDoll				()											;
	virtual			void	SetIgnoreRagDoll		()											;
	virtual			void	SetSmall				()											;
	virtual			void	SetIgnoreSmall			()											;
	virtual void			setMass					(f32 M)									;

	virtual void			setMass1				(f32 M)									;
	virtual	void			setEquelInertiaForEls	(const dMass& M)							;
	virtual	void			addEquelInertiaToEls	(const dMass& M)							;
	virtual f32			getMass					()											;
	virtual void			setDensity				(f32 M)									;
	virtual f32			getDensity				()											;
	virtual f32			getVolume				()											;
	virtual	void			get_Extensions			(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext);
	virtual void			applyForce				(const fVector3& dir, f32 val)				;
	virtual void			applyForce				(f32 x, f32 y, f32 z)					;
	virtual void			applyImpulse			(const fVector3& dir, f32 val)				;
	virtual void			applyGravityAccel		(const fVector3& accel);
	virtual void			setTorque				(const fVector3& torque);
	virtual void			setForce				(const fVector3& force);
	virtual void			set_JointResistance		(f32 force)
	{
		JOINT_I i;
		for(i=joints.begin();joints.end() != i;++i)
		{
			(*i)->SetForce(force);
			(*i)->SetVelocity();
		}
		//(*i)->SetForceAndVelocity(force);
	}
	virtual		void				set_DynamicLimits				(f32 l_limit=default_l_limit, f32 w_limit=default_w_limit);
	virtual		void				set_DynamicScales				(f32 l_scale=default_l_scale, f32 w_scale=default_w_scale);
	virtual		void				set_ContactCallback				(ContactCallbackFun* callback);
	virtual		void				set_ObjectContactCallback		(ObjectContactCallbackFun* callback);
	virtual		void				add_ObjectContactCallback		(ObjectContactCallbackFun* callback);
	virtual		void				remove_ObjectContactCallback		(ObjectContactCallbackFun* callback);
	virtual		void				set_CallbackData				(void * cd);
	virtual		void				*get_CallbackData				();
	virtual		void				set_PhysicsRefObject			(CPhysicsShellHolder* ref_object);
	
	//breakbable interface
	virtual		bool				isBreakable						();
	virtual		bool				isFractured						();
	virtual		CPHShellSplitterHolder*	SplitterHolder				(){return m_spliter_holder;}
	virtual		void				SplitProcess					(PHSHELL_PAIR_VECTOR &out_shels);
	virtual		void				BlockBreaking					(){if(m_spliter_holder)m_spliter_holder->SetUnbreakable();}
	virtual		void				UnblockBreaking					(){if(m_spliter_holder)m_spliter_holder->SetBreakable();}
	virtual		bool				IsBreakingBlocked				(){return m_spliter_holder&&m_spliter_holder->IsUnbreakable();}
	///////	////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void				get_LinearVel					(fVector3& velocity);
	virtual		void				get_AngularVel					(fVector3& velocity);
	virtual		void				set_LinearVel					(const fVector3& velocity);
	virtual		void				set_AngularVel					(const fVector3& velocity);
	virtual		void				TransformPosition				(const fMatrix4x4& form);
	virtual		void				SetGlTransformDynamic			(const fMatrix4x4& form);
	virtual		void				set_ApplyByGravity				(bool flag);
	virtual		bool				get_ApplyByGravity				();
	virtual		void				SetMaterial						(u16 m);
	virtual		void				SetMaterial						(pcstr m);
	virtual		ELEMENT_STORAGE		&Elements						(){return elements;}
	virtual		CPhysicsElement		*get_Element					(u16 bone_id);
	virtual		CPhysicsElement		*get_Element					(const shared_str & bone_name);
	virtual		CPhysicsElement		*get_Element					(pcstr bone_name);
	virtual		CPhysicsElement		*get_ElementByStoreOrder		(u16 num);
	virtual		u16					get_ElementsNumber				(){return (u16)elements.size();}
	virtual		CPHSynchronize		*get_ElementSync				(u16 element);
	virtual		u16					get_elements_number				(){return get_ElementsNumber();}
	virtual		CPHSynchronize		*get_element_sync				(u16 element){return get_ElementSync(element);}
	virtual		CPhysicsElement		*NearestToPoint					(const fVector3& point);
	virtual		CPhysicsJoint		*get_Joint						(u16 bone_id);
	virtual		CPhysicsJoint		*get_Joint						(const shared_str & bone_name);
	virtual		CPhysicsJoint		*get_Joint						(pcstr bone_name);
	virtual		CPhysicsJoint		*get_JointByStoreOrder			(u16 num);
	virtual		u16					get_JointsNumber				();
	virtual		CODEGeom			*get_GeomByID					(u16 bone_id);

	virtual		void				Enable							();
	virtual		void				Disable							();
	virtual		void				DisableCollision				();
	virtual		void				EnableCollision					();
	virtual		void				DisableCharacterCollision		();
	virtual		void				SetRemoveCharacterCollLADisable	(){m_flags.set(flRemoveCharacterCollisionAfterDisable,TRUE);}
	virtual		bool				isEnabled						(){return CPHObject::is_active();}
	virtual		bool				isActive						(){return !!m_flags.test(flActive);}
	virtual		bool				isFullActive					(){return isActive()&&!m_flags.test(flActivating);}	
				void				SetNotActivating				(){m_flags.set(flActivating,FALSE);}
//CPHObject	 
	virtual		void				vis_update_activate				();
	virtual		void				vis_update_deactivate	  		();
	virtual		void				PureStep						(f32 step);
	virtual		void				CollideAll						();
	virtual		void				PhDataUpdate					(dReal step);
	virtual		void				PhTune							(dReal step);
	virtual		void				InitContact						(dContact* c,bool &do_collide,u16 /*material_idx_1*/,u16 /*material_idx_2*/){};
	virtual		void				FreezeContent					();
	virtual		void				UnFreezeContent					();
	virtual		void				Freeze							();
	virtual		void				UnFreeze						();
	virtual		void				NetInterpolationModeON			(){CPHObject::NetInterpolationON();}
	virtual		void				NetInterpolationModeOFF			(){CPHObject::NetInterpolationOFF();}
	virtual		void				StepFrameUpdate					(dReal step){};
	virtual		CPHMoveStorage*		MoveStorage						(){return &m_traced_geoms;}
	virtual		void				build_FromKinematics			(CKinematics* K,BONE_P_MAP* p_geting_map=NULL);
	virtual		void				preBuild_FromKinematics			(CKinematics* K,BONE_P_MAP* p_geting_map);
	virtual		void                ZeroCallbacks					();
	virtual		void				ResetCallbacks					(u16 id, flags64& mask);
				void				PlaceBindToElForms				();
	virtual		void				SetCallbacks					(BoneCallbackFun* callback);
	virtual		void				EnabledCallbacks				(BOOL val);
	virtual		void				set_DisableParams				(const SAllDDOParams& params);
	virtual		void				UpdateRoot						();
	virtual		void				SmoothElementsInertia			(f32 k);
	virtual		void				InterpolateGlobalTransform		(fMatrix4x4* m);
	virtual		void				InterpolateGlobalPosition		(fVector3* v);
	virtual		void				GetGlobalTransformDynamic		(fMatrix4x4* m);
	virtual		void				GetGlobalPositionDynamic		(fVector3* v);
	virtual		fMatrix4x4&			ObjectInRoot					(){return m_object_in_root;}
	virtual		void				ObjectToRootForm				(const fMatrix4x4& form);
	virtual		dSpaceID			dSpace							(){return m_space;}
	virtual		void				SetTransform					(const fMatrix4x4& m0);
	virtual		void				AddTracedGeom					(u16 element=0,u16 geom=0);
	virtual		void				SetAllGeomTraced				();
	virtual		void				SetPrefereExactIntegration		();
	virtual		void				CutVelocity						(f32 l_limit, f32 a_limit);
///////////	//////////////////////////////////////////////////////////////////////////////////////////
				void				CreateSpace						()																				;
				void				PassEndElements					(u16 from,u16 to,CPHShell *dest)												;
				void				PassEndJoints					(u16 from,u16 to,CPHShell *dest)												;
				void				DeleteElement					(u16 element)																	;
				void				DeleteJoint						(u16 joint)																		;
				u16					BoneIdToRootGeom				(u16 id)																		;
/////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	virtual		void				get_spatial_params				()																				;
	virtual		dGeomID				dSpacedGeom						()														{return (dGeomID)m_space;}

	virtual		void				ClearRecentlyDeactivated		()																				;
				void				ClearCashedTries				()																				;
private:
				//breakable
				void				setEndElementSplitter	  		()																				;
				void				setElementSplitter		  		(u16 element_number,u16 splitter_position)										;
				void				setEndJointSplitter	  			()																				;
				void				AddSplitter			  			(CPHShellSplitter::EType type,u16 element,u16 joint)							;
				void				AddSplitter			  			(CPHShellSplitter::EType type,u16 element,u16 joint,u16 position)				;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				void				AddElementRecursive				(CPhysicsElement* root_e, u16 id, fMatrix4x4 global_parent,u16 element_number,bool *vis_check)		;
				void				PlaceBindToElFormsRecursive		(fMatrix4x4 parent,u16 id,u16 element, flags64& mask);
				void				BonesBindCalculate				(u16 id_from=0);
				void				BonesBindCalculateRecursive		(fMatrix4x4 parent,u16 id);
				void				ZeroCallbacksRecursive			(u16 id)																		;
				void				SetCallbacksRecursive			(u16 id,u16 element)															;
				void				ResetCallbacksRecursive			(u16 id,u16 element, flags64& mask)												;
				void				SetJointRootGeom				(CPhysicsElement* root_e,CPhysicsJoint* J)										;
				void				ReanableObject					()																				;
				void				ExplosionHit					(const fVector3& pos, const fVector3& dir, f32 val,const u16 id)				;
				void				ClearBreakInfo					();
};
