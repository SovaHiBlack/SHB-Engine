#pragma once
#include <boost/noncopyable.hpp>

class CBlend;

class animation_movement_controller : private boost::noncopyable
{
	fMatrix4x4& m_pObjXForm;
	fMatrix4x4			m_startObjXForm;
	fMatrix4x4			m_startRootXform;
	CKinematics* m_pKinematicsC;
	CBlend* m_control_blend;
	static void		RootBoneCallback(CBoneInstance* B);
	void			deinitialize( );

public:
	animation_movement_controller(fMatrix4x4* _pObjXForm, CKinematics* _pKinematicsC, CBlend* b);
	~animation_movement_controller( );
	void	ObjStartXform(fMatrix4x4& m) const
	{
		m.set(m_startObjXForm);
	}
	CBlend* ControlBlend( ) const
	{
		return m_control_blend;
	}
	bool	isActive( ) const;
	void	OnFrame( );
};