#pragma once

#include "Geometry.h"
#include "gamemtllib.h"

//DEFINE_VECTOR(CCodeGeom*,GEOM_STORAGE,GEOM_I)
using GEOM_STORAGE = xr_vector<CCodeGeom*>;
using GEOM_I = GEOM_STORAGE::iterator;

struct SBoneShape;
class CKinematics;

class CPHGeometryOwner
{
protected:
		GEOM_STORAGE			m_geoms;					//e			
		//bl
		bool					b_builded;
		dSpaceID				m_group;					//e					//bl
		Fvector3					m_mass_center;				//e ??				//bl
		CPHShellHolder*	m_phys_ref_object;			//->to shell ??		//bl
		float					m_volume;					//e ??				//bl
		unsigned short						ul_material;				//e ??				//bl
		ContactCallbackFun*			contact_callback;		//->to shell ??		//bt
		ObjectContactCallbackFun*	object_contact_callback;//->to shell ??		//st
public:
	///
		void						add_Sphere								(const Fsphere&		V);															//aux
		void						add_Box									(const Fobb&		V);															//aux
		void						add_Cylinder							(const Fcylinder&	V);															//aux
		void						add_Shape								(const SBoneShape& shape);														//aux
		void						add_Shape								(const SBoneShape& shape,const Fmatrix& offset);								//aux
		CCodeGeom*					last_geom								(){if(m_geoms.empty())return NULL; return m_geoms.back();}						//aux
		bool						has_geoms								(){return !m_geoms.empty();}
		void						set_ContactCallback						(ContactCallbackFun* callback);													//aux (may not be)
		void						set_ObjectContactCallback				(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
		void						add_ObjectContactCallback				(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
		void						remove_ObjectContactCallback			(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
		void						set_CallbackData						(void * cd);
		void						*get_CallbackData						();
		ObjectContactCallbackFun	*get_ObjectContactCallback				();
		void						set_PhysicsRefObject					(CPHShellHolder* ref_object);												//aux
		CPHShellHolder*		PhysicsRefObject						(){return m_phys_ref_object;}													//aux
		void						SetPhObjectInGeomData					(CPHObject* O);		

		void						SetMaterial								(unsigned short m)		  ;
		void						SetMaterial								(const char* m){SetMaterial(GMLib.GetMaterialIdx(m));}								//aux
	inline	CCodeGeom*					Geom									(unsigned short num)		{R_ASSERT2 (num<m_geoms.size(),"out of range"); return m_geoms[num]; }
	CCodeGeom*					GeomByBoneID							(unsigned short bone_id);
	unsigned short							numberOfGeoms							();																				//aux
		dGeomID						dSpacedGeometry							();																				//aux
		Fvector3						get_mc_data								();																				//aux
		Fvector3						get_mc_geoms							();																				//aux
		void						get_mc_kinematics						(CKinematics* K, Fvector3& mc,float& mass);
		void						calc_volume_data						();																				//aux
const	Fvector3&					local_mass_Center						()		{return m_mass_center;}													//aux
		float						get_volume								()		{calc_volume_data();return m_volume;};									//aux
		void						get_Extensions							(const Fvector3& axis,float center_prg,float& lo_ext, float& hi_ext);			//aux
		void						get_MaxAreaDir							(Fvector3& dir);
		float						getRadius								();	
		void						setStaticForm							(const Fmatrix& form);
		void						setPosition								(const Fvector3& pos);
		void						clear_cashed_tries						();
		void						get_mc_vs_transform						(Fvector3& mc,const Fmatrix& m);

protected:
		void						build									();
		void						CreateSimulBase							();
		void						destroy									();
		void						build_Geom								(CCodeGeom&	V);																	//aux
		void						build_Geom								(unsigned short i);
		void						set_body								(dBodyID body);

									CPHGeometryOwner						();
virtual								~CPHGeometryOwner						();
private:
};
