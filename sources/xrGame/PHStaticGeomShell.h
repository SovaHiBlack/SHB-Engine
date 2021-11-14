#pragma once

#include "PHGeometryOwner.h"
#include "PHObject.h"

class CPHStaticGeomShell : public CPHGeometryOwner, public CPHObject, public CPHUpdateObject
{
			void			get_spatial_params	();
virtual		void			EnableObject		(CPHObject* obj){CPHUpdateObject::Activate();}
virtual		dGeomID			dSpacedGeom			(){return dSpacedGeometry();}
virtual		void			PhDataUpdate		(dReal step);
virtual		void			PhTune				(dReal step){}
virtual		void			InitContact			(dContact* c,bool& do_collide, unsigned short /*material_idx_1*/, unsigned short /*material_idx_2*/){}
virtual		unsigned short				get_elements_number				()								{return 0;};
virtual		CPHSynchronize	*get_element_sync				(unsigned short element)					{return nullptr;};

public:
			void			Activate			(const Fmatrix& form);
			void			Deactivate			();
							CPHStaticGeomShell	();
};

CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj,ObjectContactCallbackFun* object_contact_callback);
CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj,ObjectContactCallbackFun* object_contact_callback,Fobb &b);
void				P_BuildStaticGeomShell(CPHStaticGeomShell* shell,CGameObject* obj,ObjectContactCallbackFun* object_contact_callback,Fobb &b);
