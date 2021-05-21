#pragma once

#include "PHNetState.h"

class CNetPacket;

class CPHSynchronize
{
public:
	virtual void						net_Export(CNetPacket& P)
	{ };					// export to server
	virtual void						net_Import(CNetPacket& P)
	{ };
	virtual void						get_State(SPHNetState& state) = 0;
	virtual void						set_State(const	SPHNetState& state) = 0;
	virtual void						cv2obj_Xfrom(const Fquaternion& q, const Fvector3& pos, Fmatrix& xform) = 0;
	virtual void			cv2bone_Xfrom(const Fquaternion& q, const Fvector3& pos, Fmatrix& xform) = 0;
};
