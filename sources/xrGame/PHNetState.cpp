#include "stdafx.h"

#include "PHNetState.h"
#include "../../xrNetServer/net_utils.h"

//////////////////////////////////////8/////////////////////////////////////////////////////

static void w_vec_q8(CNetPacket& P, const fVector3& vec, const fVector3& min, const fVector3& max)
{
	P.w_float_q8(vec.x, min.x, max.x);
	P.w_float_q8(vec.y, min.y, max.y);
	P.w_float_q8(vec.z, min.z, max.z);
}

template<typename src>
static void r_vec_q8(src& P, fVector3& vec, const fVector3& min, const fVector3& max)
{
	vec.x = P.r_float_q8(min.x, max.x);
	vec.y = P.r_float_q8(min.y, max.y);
	vec.z = P.r_float_q8(min.z, max.z);

	clamp(vec.x, min.x, max.x);
	clamp(vec.y, min.y, max.y);
	clamp(vec.z, min.z, max.z);
}

static void w_qt_q8(CNetPacket& P, const fQuaternion& q)
{
	P.w_float_q8(q.x, -1.0f, 1.0f);
	P.w_float_q8(q.y, -1.0f, 1.0f);
	P.w_float_q8(q.z, -1.0f, 1.0f);
	P.w_float_q8(q.w, -1.0f, 1.0f);
}

template<typename src>
static void r_qt_q8(src& P, fQuaternion& q)
{
	q.x = P.r_float_q8(-1.0f, 1.0f);
	q.y = P.r_float_q8(-1.0f, 1.0f);
	q.z = P.r_float_q8(-1.0f, 1.0f);
	q.w = P.r_float_q8(-1.0f, 1.0f);

	clamp(q.x, -1.0f, 1.0f);
	clamp(q.y, -1.0f, 1.0f);
	clamp(q.z, -1.0f, 1.0f);
	clamp(q.w, -1.0f, 1.0f);
}

#ifdef XRGAME_EXPORTS
/////////////////////////////////16////////////////////////////////////////////////////////////////
static void w_vec_q16(CNetPacket& P, const fVector3& vec, const fVector3& min, const fVector3& max)
{
	P.w_float_q16(vec.x, min.x, max.x);
	P.w_float_q16(vec.y, min.y, max.y);
	P.w_float_q16(vec.z, min.z, max.z);
}

static void r_vec_q16(CNetPacket& P, fVector3& vec, const fVector3& min, const fVector3& max)
{
	P.r_float_q16(vec.x, min.x, max.x);
	P.r_float_q16(vec.y, min.y, max.y);
	P.r_float_q16(vec.z, min.z, max.z);
}

template<typename src>
static void w_qt_q16(src& P, const fQuaternion& q)
{
	P.w_float_q16(q.x, -1.0f, 1.0f);
	P.w_float_q16(q.y, -1.0f, 1.0f);
	P.w_float_q16(q.z, -1.0f, 1.0f);
	P.w_float_q16(q.w, -1.0f, 1.0f);
}

static void r_qt_q16(CNetPacket& P, fQuaternion& q)
{
	P.r_float_q16(q.x, -1.0f, 1.0f);
	P.r_float_q16(q.y, -1.0f, 1.0f);
	P.r_float_q16(q.z, -1.0f, 1.0f);
	P.r_float_q16(q.w, -1.0f, 1.0f);
}
#endif
///////////////////////////////////////////////////////////////////////////////////

void SPHNetState::net_Export(CNetPacket& P)
{
	P.w_vec3(linear_vel);
	//P.w_vec3(angular_vel);
	//P.w_vec3(force);
	//P.w_vec3(torque);
	P.w_vec3(position);
	P.w_vec4(*((fVector4*)&quaternion));
	//P.w_vec4(*((fVector4*)&previous_quaternion));
	P.w_u8((u8)enabled);
}

template<typename src>
void SPHNetState::read(src& P)
{
	linear_vel = P.r_vec3( );
	angular_vel.set(0.f, 0.f, 0.f);		//P.r_vec3(angular_vel);
	force.set(0.f, 0.f, 0.f);				//P.r_vec3(force);
	torque.set(0.f, 0.f, 0.f);			//P.r_vec3(torque);
	position = P.r_vec3( );
	*((fVector4*)&quaternion) = P.r_vec4( );
	previous_quaternion.set(quaternion);//P.r_vec4(*((fVector4*)&previous_quaternion));
	enabled = !!P.r_u8( );
}

void SPHNetState::net_Import(CNetPacket& P)
{
	read(P);
}

void SPHNetState::net_Import(IReader& P)
{
	read(P);
}

void SPHNetState::net_Save(CNetPacket& P)
{
	net_Export(P);
}

void SPHNetState::net_Load(CNetPacket& P)
{
	net_Import(P);
	previous_position.set(position);
}

void SPHNetState::net_Load(IReader& P)
{
	net_Import(P);
	previous_position.set(position);
}

void SPHNetState::net_Save(CNetPacket& P, const fVector3& min, const fVector3& max)
{
	//P.w_vec3(linear_vel);
	//P.w_vec3(angular_vel);
	//P.w_vec3(force);
	//P.w_vec3(torque);
	//P.w_vec3(position);
	w_vec_q8(P, position, min, max);
	w_qt_q8(P, quaternion);
	//P.w_vec4(*((fVector4*)&quaternion));
	//P.w_vec4(*((fVector4*)&previous_quaternion));
	P.w_u8((u8)enabled);
}

template<typename src>
void SPHNetState::read(src& P, const fVector3& min, const fVector3& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	linear_vel.set(0.0f, 0.0f, 0.0f);
	angular_vel.set(0.0f, 0.0f, 0.0f);
	force.set(0.0f, 0.0f, 0.0f);
	torque.set(0.0f, 0.0f, 0.0f);
	r_vec_q8(P, position, min, max);
	previous_position.set(position);
	r_qt_q8(P, quaternion);
	previous_quaternion.set(quaternion);
	enabled = !!P.r_u8( );
}

void SPHNetState::net_Load(CNetPacket& P, const fVector3& min, const fVector3& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	read(P, min, max);
}

void SPHNetState::net_Load(IReader& P, const fVector3& min, const fVector3& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	read(P, min, max);
}

SPHBonesData::SPHBonesData( )
{
	bones_mask = u64(-1);
	root_bone = 0;

	fVector3 _mn;
	fVector3 _mx;

	_mn.set(-100.0f, -100.0f, -100.0f);
	_mx.set(100.0f, 100.0f, 100.0f);
	set_min_max(_mn, _mx);
}

void SPHBonesData::net_Save(CNetPacket& P)
{
	P.w_u64(bones_mask);
	P.w_u16(root_bone);

	P.w_vec3(get_min( ));
	P.w_vec3(get_max( ));
	P.w_u16((u16)bones.size( ));//bones number;
	PHNETSTATE_I	i = bones.begin( ), e = bones.end( );
	for (; e != i; i++)
	{
		(*i).net_Save(P, get_min( ), get_max( ));
	}
	//	this comment is added by Dima (correct me if this is wrong)
	//  if we call 2 times in a row StateWrite then we get different results
	//	WHY???
	//	bones.clear		();
}

void SPHBonesData::net_Load(CNetPacket& P)
{
	bones.clear( );

	bones_mask = P.r_u64( );
	root_bone = P.r_u16( );
	fVector3 _mn;
	fVector3 _mx;
	P.r_vec3(_mn);
	P.r_vec3(_mx);
	set_min_max(_mn, _mx);

	u16 bones_number = P.r_u16( );//bones number /**/
	for (s32 i = 0; i < bones_number; i++)
	{
		SPHNetState	S;
		S.net_Load(P, get_min( ), get_max( ));
		bones.push_back(S);
	}
}

void SPHBonesData::set_min_max(const fVector3& _min, const fVector3& _max)
{
	VERIFY(!_min.similar(_max));
	m_min = _min;
	m_max = _max;
}
