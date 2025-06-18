#pragma once

#include "ui_base.h"
#include "UIStaticItem.h"

class CUIStatic;

class CUICursor : public pureRender
{
	bool			bVisible;
	fVector2		vPos;
	fVector2		vPrevPos;

	CUIStatic* m_static;
	void			InitInternal( );

public:
	CUICursor( );
	virtual			~CUICursor( );
	virtual void	OnRender( );

	fVector2		GetCursorPositionDelta( );

	fVector2		GetCursorPosition( );
	void			SetUICursorPosition(fVector2 pos);
	void			UpdateCursorPosition( );

	bool			IsVisible( )
	{
		return bVisible;
	}
	void			Show( )
	{
		bVisible = true;
	}
	void			Hide( )
	{
		bVisible = false;
	}
};
