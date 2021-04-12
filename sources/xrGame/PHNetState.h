#pragma once

class NET_Packet;

struct SPHNetState
{
	Fvector3		linear_vel;
	Fvector3		angular_vel;
	Fvector3		force;
	Fvector3		torque;
	Fvector3		position;
	Fvector3		previous_position;
	union
	{
		Fquaternion quaternion;
		struct
		{
			Fvector3	accel;
			float	max_velocity;
		};
	};
	Fquaternion	previous_quaternion;
	bool		enabled;
	void								net_Export(NET_Packet& P);
	void								net_Import(NET_Packet& P);
	void								net_Import(IReader& P);
	void								net_Save(NET_Packet& P);
	void								net_Load(NET_Packet& P);
	void								net_Load(IReader& P);
	void								net_Save(NET_Packet& P, const Fvector3& min, const Fvector3& max);
	void								net_Load(NET_Packet& P, const Fvector3& min, const Fvector3& max);
	void								net_Load(IReader& P, const Fvector3& min, const Fvector3& max);

private:
	template<typename src>
	void								read(src& P);
	template<typename src>
	void								read(src& P, const Fvector3& min, const Fvector3& max);
};

//DEFINE_VECTOR(SPHNetState, PHNETSTATE_VECTOR, PHNETSTATE_I);
using PHNETSTATE_VECTOR = xr_vector<SPHNetState>;
using PHNETSTATE_I = PHNETSTATE_VECTOR::iterator;

struct SPHBonesData
{
	u64				  bones_mask;
	u16				  root_bone;
	PHNETSTATE_VECTOR bones;
	Fvector3			  m_min;
	Fvector3			  m_max;

public:
	SPHBonesData( );
	void								net_Save(NET_Packet& P);
	void								net_Load(NET_Packet& P);
	void								net_Load(IReader& P);
	void								set_min_max(const Fvector3& _min, const Fvector3& _max);
	const Fvector3& get_min( ) const
	{
		return m_min;
	}
	const Fvector3& get_max( ) const
	{
		return m_max;
	}
};
