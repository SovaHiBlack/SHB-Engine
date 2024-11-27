// file:		UISpinNum.h
// description:	Spin Button with numerical data (unlike text data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "UICustomSpin.h"

class CUISpinNum : public CUICustomSpin
{
public:
	CUISpinNum( );

	virtual void	Init(f32 x, f32 y, f32 width, f32 height);

	// CUIOptionsItem
	virtual void	SetCurrentValue( );
	virtual void	SaveValue( );
	virtual bool	IsChanged( );

	virtual void	OnBtnUpClick( );
	virtual void	OnBtnDownClick( );

	void	SetMax(s32 max)
	{
		m_iMax = max;
	}
	void	SetMin(s32 min)
	{
		m_iMin = min;
	}
	s32		Value( ) const
	{
		return m_iVal;
	}

protected:
	void	SetValue( );
	virtual bool	CanPressUp( );
	virtual bool	CanPressDown( );
	virtual void		IncVal( );
	virtual void		DecVal( );

	s32		m_iMax;
	s32		m_iMin;
	s32		m_iStep;
	s32		m_iVal;
};

class CUISpinFlt : public CUICustomSpin
{
public:
	CUISpinFlt( );

	virtual void	Init(f32 x, f32 y, f32 width, f32 height);

	// CUIOptionsItem
	virtual void	SetCurrentValue( );
	virtual void	SaveValue( );
	virtual bool	IsChanged( );

	virtual void	OnBtnUpClick( );
	virtual void	OnBtnDownClick( );

	void	SetMax(f32 max);
	void	SetMin(f32 min);

protected:
	void	SetValue( );
	virtual bool	CanPressUp( );
	virtual bool	CanPressDown( );
	virtual void	IncVal( );
	virtual void	DecVal( );

	f32		m_fMax;
	f32		m_fMin;
	f32		m_fStep;
	f32		m_fVal;
};
