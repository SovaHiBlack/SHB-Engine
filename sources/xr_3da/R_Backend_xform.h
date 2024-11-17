#pragma once

class ENGINE_API R_xforms
{
public:
	fMatrix4x4			m_w;		// Basic	- world
	fMatrix4x4			m_v;		// Basic	- view
	fMatrix4x4			m_p;		// Basic	- projection
	fMatrix4x4			m_wv;		// Derived	- world2view
	fMatrix4x4			m_vp;		// Derived	- view2projection
	fMatrix4x4			m_wvp;		// Derived	- world2view2projection

	R_constant* c_w;
	R_constant* c_v;
	R_constant* c_p;
	R_constant* c_wv;
	R_constant* c_vp;
	R_constant* c_wvp;

public:
	R_xforms( );
	void			unmap( );
	void			set_W(const fMatrix4x4& m);
	void			set_V(const fMatrix4x4& m);
	void			set_P(const fMatrix4x4& m);
	IC const fMatrix4x4& get_W( )
	{
		return m_w;
	}
	IC const fMatrix4x4& get_V( )
	{
		return m_v;
	}
	IC const fMatrix4x4& get_P( )
	{
		return m_p;
	}
	IC void			set_c_w(R_constant* C);
	IC void			set_c_v(R_constant* C);
	IC void			set_c_p(R_constant* C);
	IC void			set_c_wv(R_constant* C);
	IC void			set_c_vp(R_constant* C);
	IC void			set_c_wvp(R_constant* C);
};
