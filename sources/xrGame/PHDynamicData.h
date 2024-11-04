// PHDynamicData.h: interface for the PHDynamicData class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ode_include.h"
#include "PHInterpolation.h"
#include "MathUtils.h"

class PHDynamicData;

class PHDynamicData  
{
#if 0
public:
dVector3 pos;
dMatrix3 R;
fMatrix4x4 BoneTransform;
private:
dBodyID body;
CPHInterpolation* p_parent_body_interpolation;
CPHInterpolation body_interpolation;
dGeomID geom;
dGeomID transform;
//PHDynamicData* Childs;
//xr_vector<PHDynamicData>  Childs;
unsigned int numOfChilds;
fMatrix4x4 ZeroTransform;
public:
	inline void UpdateInterpolation(){
		body_interpolation.UpdatePositions();
		body_interpolation.UpdateRotations();
	}
	void UpdateInterpolationRecursive()	;
	void InterpolateTransform(fMatrix4x4& transform);
	void InterpolateTransformVsParent(fMatrix4x4& transform);
	PHDynamicData& operator [] (unsigned int i) {return Childs[i];};
	void Destroy();
	void Create(unsigned int numOfchilds,dBodyID Body);
	void CalculateData(void);
	PHDynamicData * GetChild(unsigned int ChildNum);
	bool SetChild(unsigned int ChildNum,unsigned int numOfchilds,dBodyID body);
	void SetAsZero();
	void SetAsZeroRecursive();
	void SetZeroTransform(fMatrix4x4& aTransform);
	PHDynamicData(unsigned int numOfchilds,dBodyID body);
	PHDynamicData();
	virtual ~PHDynamicData();
	void GetWorldMX(fMatrix4x4& aTransform){
			dMatrix3 R;
			dQtoR(dBodyGetQuaternion(body),R);
			DMXPStoFMX(R,dBodyGetPosition(body),aTransform);
			}
	void GetTGeomWorldMX(fMatrix4x4& aTransform){
			if(!transform) return;
			fMatrix4x4 NormTransform;
			fMatrix4x4 Transform;
			dVector3 P0={0,0,0,-1};
			fVector3 Translate;
			fVector3 Translate1;
			//compute_final_tx(geom);
			//dQtoR(dBodyGetQuaternion(body),R);
			DMXPStoFMX(dBodyGetRotation(body),P0,NormTransform);
			DMXPStoFMX(dGeomGetRotation(dGeomTransformGetGeom(transform)),P0,Transform);
	

			dVectorSet((dReal*)&Translate,dGeomGetPosition(dGeomTransformGetGeom(transform)));
			dVectorSet((dReal*)&Translate1,dBodyGetPosition(body));

			aTransform.identity			();
			aTransform.translate_over	(Translate);
			aTransform.mulA_43			(NormTransform);
			aTransform.translate_over	(Translate1);
			aTransform.mulA_43			(Transform);
			
		//	Translate.add(Translate1);	
			//transform.translate_over(Translate1);

			//transform.translate_add
			//normalTransform=oMatrix4x4(dGeomGetRotation(dGeomTransformGetGeom(geom)))*normalTransform;
			//oMatrix4x4 meshTransform(normalTransform);
			
			//meshTransform.PreTranslate(oVector3(dGeomGetPosition(dGeomTransformGetGeom(geom))));
			//meshTransform.PostTranslate(oVector3(dBodyGetPosition(body)));
			}
#endif // #if 0
public:
	static IC void DMXPStoFMX(const dReal* R,const dReal* pos, fMatrix4x4& aTransform){

			CopyMemory(&aTransform,R,sizeof(dMatrix3));
			aTransform.transpose();
			CopyMemory(&aTransform.c,pos,sizeof(fVector3));
			aTransform._14=0.f;
			aTransform._24=0.f;
			aTransform._34=0.f;
			aTransform._44=1.f;
		};
	static IC void DMXtoFMX(const dReal* R, fMatrix4x4& aTransform){
			aTransform._11=R[0];
			aTransform._12=R[4];
			aTransform._13=R[8];
			aTransform._14=0.f;

			aTransform._21=R[1];
			aTransform._22=R[5];
			aTransform._23=R[9];
			aTransform._24=0.f;
			
			aTransform._31=R[2];
			aTransform._32=R[6];
			aTransform._33=R[10];
			aTransform._34=0.f;
			aTransform._44=1.f;
	};
	static IC void FMX33toDMX(const fMatrix3x3& aTransform,dReal* R){
		R[0]=aTransform._11;
		R[4]=aTransform._12;
		R[8]=aTransform._13;

		R[1]=aTransform._21;
		R[5]=aTransform._22;
		R[9]=aTransform._23;

		R[2]=aTransform._31;
		R[6]=aTransform._32;
		R[10]=aTransform._33;
		};
	static IC void FMXtoDMX(const fMatrix4x4& aTransform,dReal* R){
		R[0]=aTransform._11;
		R[4]=aTransform._12;
		R[8]=aTransform._13;

		R[1]=aTransform._21;
		R[5]=aTransform._22;
		R[9]=aTransform._23;

		R[2]=aTransform._31;
		R[6]=aTransform._32;
		R[10]=aTransform._33;
	};
#if 0
private:
	void CalculateR_N_PosOfChilds(dBodyID parent);
public:
	bool SetGeom(dGeomID ageom);
	bool SetTransform(dGeomID ageom);
#endif // #if 0
};
