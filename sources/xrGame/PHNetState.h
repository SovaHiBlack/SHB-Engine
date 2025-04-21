#pragma once

class CNetPacket;

struct SPHNetState
{
	fVector3		linear_vel;
	fVector3		angular_vel;
	fVector3		force;
	fVector3		torque;
	fVector3		position;
	fVector3		previous_position;
	union{
		fQuaternion quaternion;
		struct{
			fVector3	accel;
			f32	max_velocity;
		};
	};
	fQuaternion	previous_quaternion;
	bool		enabled;
	void								net_Export			(CNetPacket&		P);
	void								net_Import			(CNetPacket&		P);
	void								net_Import			(		IReader&		P);
	void								net_Save			(CNetPacket&		P);
	void								net_Load			(CNetPacket&		P);
	void								net_Load			(		IReader&		P);
	void								net_Save			(CNetPacket&		P,const fVector3& min,const fVector3& max);
	void								net_Load			(CNetPacket&		P,const fVector3& min,const fVector3& max);
	void								net_Load			(		IReader&		P,const fVector3& min,const fVector3& max);
private:
template<typename src>
	void								read				(		src&			P);
template<typename src>
	void								read				(		src&		P,const fVector3& min,const fVector3& max);
};

DEFINE_VECTOR(SPHNetState,PHNETSTATE_VECTOR,PHNETSTATE_I);

struct SPHBonesData 
{
	u64				  bones_mask;
	u16				  root_bone;
	PHNETSTATE_VECTOR bones;
	fVector3			  m_min;
	fVector3			  m_max;

public:
	SPHBonesData		()						  ;
	void								net_Save			(CNetPacket&		P);
	void								net_Load			(CNetPacket&		P);
	void								net_Load			(		IReader&		P);
	void								set_min_max			(const fVector3& _min, const fVector3& _max);
	const fVector3&						get_min				()	const	{return m_min;}
	const fVector3&						get_max				()	const	{return m_max;}
};
