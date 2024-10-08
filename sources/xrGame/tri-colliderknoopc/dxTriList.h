#pragma once

#include "../ode_include.h"

struct dcVector3
{
	f32 x;
	f32 y;
	f32 z;

	dcVector3(){}

	dcVector3(dReal x, dReal y, dReal z){

		this->x = (f32)x;

		this->y = (f32)y;

		this->z = (f32)z;
	}

	dcVector3(const dReal* v){

		x = (f32)v[0];

		y = (f32)v[1];

		z = (f32)v[2];
	}

	~dcVector3(){}

	operator f32* (){  //&slipch

	return reinterpret_cast<f32*>(this);
	}
	/* Add */

	dcVector3 operator+(const dcVector3& v) const{

		dcVector3 Out;

		Out.x = x + v.x;

		Out.y = y + v.y;

		Out.z = z + v.z;

		return Out;

	}



	dcVector3& operator+=(const dcVector3& v){

		x += v.x;

		y += v.y;

		z += v.z;

		return *this;

	}



	/* Sub */

	dcVector3 operator-(const dcVector3& v) const{

		dcVector3 Out;

		Out.x = x - v.x;

		Out.y = y - v.y;

		Out.z = z - v.z;

		return Out;

	}



	dcVector3& operator-=(const dcVector3& v){

		x -= v.x;

		y -= v.y;

		z -= v.z;

		return *this;

	}



	/* Mul */

	dcVector3 operator*(const dcVector3& v) const{

		dcVector3 Out;

		Out.x = x * v.x;

		Out.y = y * v.y;

		Out.z = z * v.z;

		return Out;

	}



	dcVector3 operator*(f32 Scalar) const{

		dcVector3 Out;

		Out.x = x * Scalar;

		Out.y = y * Scalar;

		Out.z = z * Scalar;

		return Out;

	}



	dcVector3& operator*=(const dcVector3& v){

		x *= v.x;

		y *= v.y;

		z *= v.z;

		return *this;

	}



	dcVector3& operator*=(f32 Scalar){

		x *= Scalar;

		y *= Scalar;

		z *= Scalar;

		return *this;

	}



	/* Div */

	dcVector3 operator/(const dcVector3& v) const{

		dcVector3 Out;

		Out.x = x / v.x;

		Out.y = y / v.y;

		Out.z = z / v.z;

		return Out;

	}



	dcVector3 operator/(f32 Scalar) const{

		dcVector3 Out;

		Out.x = x / Scalar;

		Out.y = y / Scalar;

		Out.z = z / Scalar;

		return Out;

	}



	dcVector3& operator/=(const dcVector3& v){

		x /= v.x;

		y /= v.y;

		z /= v.z;

		return *this;

	}



	dcVector3& operator/=(f32 Scalar){

		x /= Scalar;

		y /= Scalar;

		z /= Scalar;

		return *this;

	}



	/* Negative */

	dcVector3& operator-(){

		x = -x;

		y = -y;

		z = -z;

		return *this;

	}



	/* Comparison */

	bool operator==(const dcVector3& v) const{

		return x == v.x && y == v.y && z == v.z;

	}



	bool operator!=(const dcVector3& v) const{

		return v.x != x || v.y != y || v.z != z;

	}



	f32 DotProduct(const dcVector3& v) const{

		return x * v.x + y * v.y + z * v.z;

	}



	dcVector3 CrossProduct(const dcVector3& v) const{

		dcVector3 Out;

		Out.x = y * v.z - z * v.y;

		Out.y = z * v.x - x * v.z;

		Out.z = x * v.y - y * v.x;

		return Out;

	}



	f32 MagnitudeSq() const{

		return DotProduct(*this);

	}



	f32 Magnitude() const{

		return _sqrt(MagnitudeSq());

	}



	void Normalize(){

		operator/=(Magnitude());

	}



	/* Member access */

	f32& operator[](int Index){

		return *(&x + Index);

	}



	f32 operator[](int Index) const{

		return *(&x + Index);

	}

};





/* Class ID */

extern int dTriListClass;



/* Per triangle callback */

typedef int dTriCallback(dGeomID TriList, dGeomID RefObject, int TriangleIndex);

void dGeomTriListSetCallback(dGeomID g, dTriCallback* Callback);

dTriCallback* dGeomTriListGetCallback(dGeomID g);



/* Per object callback */

typedef void dTriArrayCallback(dGeomID TriList, dGeomID RefObject, const int* TriIndices, int TriCount);

void dGeomTriListSetArrayCallback(dGeomID g, dTriArrayCallback* ArrayCallback);

dTriArrayCallback* dGeomTriListGetArrayCallback(dGeomID g);



/* Construction */

dxGeom* dCreateTriList(dSpaceID space, dTriCallback* Callback, dTriArrayCallback* ArrayCallback);



/* Setting data */

void dGeomTriListBuild(dGeomID g, const dcVector3* Vertices, int VertexCount, const int* Indices, int IndexCount);



/* Getting data */

void dGeomTriListGetTriangle(dGeomID g, int Index, dVector3* v0, dVector3* v1, dVector3* v2);



/* Internal types */

class dcTriListCollider;



struct dxTriList{

	dReal p[4];						// dxPlane

	dTriCallback* Callback;

	dTriArrayCallback* ArrayCallback;

	dcTriListCollider* Collider;

};



struct dcPlane{

	dcVector3 Normal;

	f32 Distance;



	dcPlane(){}

	dcPlane(const dcVector3& v0, const dcVector3& v1, const dcVector3& v2){

		dcVector3 u = v1 - v0;

		dcVector3 v = v2 - v0;



		Normal = u.CrossProduct(v);

		Distance = v0.DotProduct(Normal);

		Normalize();

	}



	void Normalize(){

		f32 Factor = 1.0f / Normal.Magnitude();

		Normal *= Factor;

		Distance *= Factor;

	}



	bool Contains(const dcVector3& RefObject, f32 Epsilon = 0.0f) const{

		return Normal.DotProduct(RefObject) - Distance >= - Epsilon; //@slipch ">=" instead ">"

	}

};



template<class T> const T& dcMAX(const T& x, const T& y){

	return x > y ? x : y;

}



template<class T> const T& dcMIN(const T& x, const T& y){

	return x < y ? x : y;

}
