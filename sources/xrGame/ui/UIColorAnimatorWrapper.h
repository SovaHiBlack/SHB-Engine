//=============================================================================
//  Filename:   UIColorAnimatorWrapper.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ��������� ��� LightAnimLibrary, � ������� ����������� ����� �������
//	������ � ���������, � ��������� ������ ������� ����������� ��� UI: ���
//	��� UI ���������� �� �� ����� �����, �� ������� ������� ������������
//	Device.fTimeDelta ������ - �������� ����������� ����������. ���������� 
//	��������� ������� ����� ����� ���������.
//=============================================================================
#pragma once

class CLightAnimItem;

class CUIColorAnimatorWrapper
{
public:
				CUIColorAnimatorWrapper		( );
				~CUIColorAnimatorWrapper	( )
	{ }
	// animationName - ��� �������� ��������
	// colorToModify - ��������� �� ���� ������� ������
	explicit	CUIColorAnimatorWrapper		(const shared_str& animationName);
				CUIColorAnimatorWrapper		(u32* colorToModify);
	// ���������� ����� �������� ��������
	void		SetColorAnimation			(const shared_str& animationName);
	void		SetColorToModify			(u32* colorToModify);
	// ������� ������� ���������� ����������� ��������� � Update �������, ��� ���������� ��������
	void		Update						( );
	// �������� ����� �������� ����� �������� �������.
	// ����� ������ �� ������ ����������� Reset()
	// ��� ������������ ������ 1 ����� �������� Cyclic(false);
	void		Cyclic						(bool cyclic)
	{
		isCyclic = cyclic;
	}
	void		Reset						( );

	u32			GetColor					( ) const
	{
		return currColor;
	}
	s32			LastFrame					( ) const
	{
		return currFrame;
	}
	s32			TotalFrames					( ) const;
	bool		Done						( ) const
	{
		return isDone;
	}
	void		SetDone						(bool value)
	{
		isDone = value;
	}
	void		Reverese					(bool value);
	void		GoToEnd						( );

private:
	// ���������� ��������
	CLightAnimItem*								colorAnimation;
	// ���������� ���������� �������� ����������� ������� ����
	f32										prevGlobalTime;
	// ����� ��������� � ������ ��������
	f32										animationTime;
	// ����������� ����������� ������������
	bool									isCyclic;
	// ���� ������� �� ����� �������� ���������� ��� �����
	u32*									color;
	// ������� ���� ��������
	u32										currColor;
	// ������� ���� ��������
	s32										currFrame;
	// �������� �����������?
	bool									isDone;
	// �������� �������������� � ����������� �������? �� ��������� ��� ����������� ��������
	bool									reverse;
	// ����������� ��� ��������� ������� ��� ����������� ������������
	f32										kRev;

public:
	CLightAnimItem*	GetAnimation				( ) const
	{
		return colorAnimation;
	}
};
