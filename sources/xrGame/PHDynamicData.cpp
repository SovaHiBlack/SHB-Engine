// PHDynamicData.cpp: implementation of the PHDynamicData class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PHDynamicData.h"

#if 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PHDynamicData::PHDynamicData()
{
	numOfChilds=0;
	//Childs=NULL;
	p_parent_body_interpolation=NULL;
}

PHDynamicData::~PHDynamicData()
{
	if(numOfChilds){
		//for(unsigned int i=0;i<numOfChilds;++i)
		//	delete
		for(unsigned int i=0;i<numOfChilds;++i){
			Childs[i].Destroy();
		}
		Childs.clear();
		//Childs=NULL;
		numOfChilds=0;
	}
}

PHDynamicData::PHDynamicData(unsigned int numOfchilds,dBodyID Body)
{
	numOfChilds=numOfchilds;
	body=Body;
	geom=NULL;
	transform=NULL;
	Childs.resize(numOfChilds);
	ZeroTransform.identity();
}

bool PHDynamicData::SetChild(unsigned int childNum,unsigned int numOfchilds,dBodyID body)
{

	if(childNum<numOfChilds){
		Childs[childNum].body=body;
		Childs[childNum].geom=NULL;
		Childs[childNum].transform=NULL;
		Childs[childNum].numOfChilds=numOfchilds;
		Childs[childNum].ZeroTransform.identity();
		Childs[childNum].p_parent_body_interpolation=&body_interpolation;
		Childs[childNum].body_interpolation.SetBody(body);

		if(numOfchilds>0)
			//Childs[childNum].Childs=new PHDynamicData[numOfchilds];
			Childs[childNum].Childs.resize(numOfchilds);
		else   
			//Childs[childNum].Childs=NULL;
			Childs[childNum].numOfChilds=0;

		Childs[childNum].geom=NULL;
		Childs[childNum].transform=NULL;
		return true;
	}
	else return false;
}

void PHDynamicData::CalculateR_N_PosOfChilds(dBodyID parent)
{
	fMatrix4x4 parent_transform;//,mYM;
	//mYM.rotateY			(deg2rad(-90.f));
	DMXPStoFMX(dBodyGetRotation(parent),dBodyGetPosition(parent),parent_transform);
	DMXPStoFMX(dBodyGetRotation(body),dBodyGetPosition(body),BoneTransform);
	parent_transform.mulB_43	(ZeroTransform);
	//parent_transform.mulA(mYM);
	parent_transform.invert		();

	//BoneTransform.mulA(mYM);
	BoneTransform.mulA_43		(parent_transform);

	for(unsigned int i=0;i<numOfChilds;++i){

		Childs[i].CalculateR_N_PosOfChilds(body);
	}

}
void PHDynamicData::UpdateInterpolationRecursive(){
	UpdateInterpolation();

	for(unsigned int i=0;i<numOfChilds;++i){
		Childs[i].UpdateInterpolationRecursive();
	}
}

void PHDynamicData::InterpolateTransform(fMatrix4x4& transform){
	//DMXPStoFMX(dBodyGetRotation(body),
	//			dBodyGetPosition(body),BoneTransform);
	body_interpolation.InterpolateRotation(transform);
	body_interpolation.InterpolatePosition(transform.c);
	fMatrix4x4				zero;
	zero.set			(ZeroTransform);
	zero.invert			();
	//BoneTransform.mulB(zero);
	transform.mulB_43	(zero);
}
void PHDynamicData::InterpolateTransformVsParent(fMatrix4x4& transform){
	fMatrix4x4 parent_transform;
	//DMXPStoFMX(dBodyGetRotation(parent),dBodyGetPosition(parent),parent_transform);
	//DMXPStoFMX(dBodyGetRotation(body),dBodyGetPosition(body),BoneTransform);
	p_parent_body_interpolation->InterpolateRotation(parent_transform);
	p_parent_body_interpolation->InterpolatePosition(parent_transform.c);

	body_interpolation.InterpolateRotation(transform);

	body_interpolation.InterpolatePosition(transform.c);
	parent_transform.mulB_43	(ZeroTransform);

	parent_transform.invert();


	//BoneTransform.mulA(parent_transform);
	transform.mulA_43	(parent_transform);
}

PHDynamicData * PHDynamicData::GetChild(unsigned int ChildNum)
{
	if(ChildNum<numOfChilds)
		return &Childs[ChildNum];
	else return NULL;
}

void PHDynamicData::CalculateData()
{
	DMXPStoFMX(dBodyGetRotation(body),
		dBodyGetPosition(body),BoneTransform);
	fMatrix4x4 zero;
	zero.set(ZeroTransform);
	zero.invert();
	BoneTransform.mulB_43(zero);
	for(unsigned int i=0;i<numOfChilds;++i){

		Childs[i].CalculateR_N_PosOfChilds(body);
	}
}

void PHDynamicData::Create(unsigned int numOfchilds, dBodyID Body)
{
	ZeroTransform.identity();
	numOfChilds=numOfchilds;
	body=Body;
	geom=NULL;
	Childs.resize(numOfChilds);
	body_interpolation.SetBody(Body);
}

void PHDynamicData::Destroy()
{
	if(numOfChilds){
		for(unsigned int i=0;i<numOfChilds;++i)
			Childs[i].Destroy();

		Childs.clear();
		//Childs=NULL;
		numOfChilds=0;
	}
}

bool PHDynamicData::SetGeom(dGeomID ageom)
{
	if(! geom){
		geom=ageom;
		return true;
	}
	else
		return false;
}
bool PHDynamicData::SetTransform(dGeomID ageom)
{
	if(!transform){
		transform=ageom;
		return true;
	}
	else
		return false;
}
void PHDynamicData::SetAsZero(){
	ZeroTransform.set(BoneTransform);
}

void PHDynamicData::SetAsZeroRecursive(){
	ZeroTransform.set(BoneTransform);
	for(unsigned int i=0;  i<numOfChilds;++i)
	{
		Childs[i].SetAsZeroRecursive();
	}
}

void PHDynamicData::SetZeroTransform(fMatrix4x4& aTransform){
	ZeroTransform.set(aTransform);
}
#endif // #if 0
