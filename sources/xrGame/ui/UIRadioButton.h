//////////////////////////////////////////////////////////////////////
// UIRadioButton.h: ����� ������, ������� 2 ���������
// � ���������� � ������ � ������ �� ��������
//////////////////////////////////////////////////////////////////////
#pragma once

#include "UITabButton.h"

class CUIRadioButton : public CUITabButton
{
public:
	virtual void Init(f32 x, f32 y, f32 width, f32 height);
	virtual void InitTexture(pcstr tex_name);
	virtual void SetTextX(f32 x)	{/*do nothing*/}
};
