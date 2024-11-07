#pragma once
#include "UIbutton.h"

class CUIListItem : public CUIButton
{
private:
	typedef CUIButton inherited;

public:
	CUIListItem( );
	virtual ~CUIListItem( );

	virtual void Init(f32 x, f32 y, f32 width, f32 height);
	virtual void Init(pcstr str, f32 x, f32 y, f32 width, f32 height);
	virtual void InitTexture(pcstr tex_name);

	pvoid GetData( )
	{
		return m_pData;
	}
	void SetData(pvoid pData)
	{
		m_pData = pData;
	}

	s32 GetIndex( )
	{
		return m_iIndex;
	}
	void SetIndex(s32 index)
	{
		m_iIndex = index;
		m_iGroupID = index;
	}

	s32 GetValue( )
	{
		return m_iValue;
	}
	void SetValue(s32 value)
	{
		m_iValue = value;
	}

	s32	GetGroupID( )
	{
		return m_iGroupID;
	}
	void SetGroupID(s32 ID)
	{
		m_iGroupID = ID;
	}

	virtual void	MarkSelected(bool b)
	{ }
	// переопределяем критерий подсвечивания текста
	virtual bool IsHighlightText( );
	virtual void SetHighlightText(bool Highlight)
	{
		m_bHighlightText = Highlight;
	}

protected:
	//указатель на произвольные данные, которые могут
	//присоедениены к элементу
	pvoid m_pData;

	//произвольное число, приписанное объекту
	s32 m_iValue;

	//индекс в списке
	s32 m_iIndex;

	// идентификатор группы
	s32 m_iGroupID;

	// подсвечивается кнопка или нет?
	bool m_bHighlightText;
};
