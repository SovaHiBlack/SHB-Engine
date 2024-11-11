//=============================================================================
//  Filename:   UIColorAnimatorWrapper.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Оберточка над LightAnimLibrary, в которой реализуется более удобная
//	работа с анимацией, и прикручен апдейт времени специфичный для UI: так
//	как UI апдейтится не на кажом кадре, то кормить функции интерполяции
//	Device.fTimeDelta нельзя - анимация тормознутая получается. Приходится 
//	постоянно считать время между апдейтами.
//=============================================================================
#pragma once

class CLightAnimItem;

class CUIColorAnimatorWrapper
{
public:
				CUIColorAnimatorWrapper		( );
				~CUIColorAnimatorWrapper	( )
	{ }
	// animationName - имя цветовой анимации
	// colorToModify - указатель на цвет который меняем
	explicit	CUIColorAnimatorWrapper		(const shared_str& animationName);
				CUIColorAnimatorWrapper		(u32* colorToModify);
	// Установить новую цветовую анимацию
	void		SetColorAnimation			(const shared_str& animationName);
	void		SetColorToModify			(u32* colorToModify);
	// Функция которую необходимо обязательно поместить в Update диалога, для обновления анимации
	void		Update						( );
	// Анимация сразу играется после создания объекта.
	// Чтобы начать ее заново используйте Reset()
	// Для проигрывания только 1 цикла вызовите Cyclic(false);
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
	// Собственно анимация
	CLightAnimItem*								colorAnimation;
	// Предыдущее запоменное значение глобального времени игры
	f32										prevGlobalTime;
	// Время прошедшее с начала анимации
	f32										animationTime;
	// Циклическое бесконечное проигрывание
	bool									isCyclic;
	// Цвет который мы можем изменять переданный нам извне
	u32*									color;
	// Текущий цвет анимации
	u32										currColor;
	// Текущий кадр анимации
	s32										currFrame;
	// Анимация закончилась?
	bool									isDone;
	// Анимация прокручивается в реверсивном порядке? Не применимо для бесконечной анимации
	bool									reverse;
	// Коеффициент для пересчета времени при реверснутом проигрывании
	f32										kRev;

public:
	CLightAnimItem*	GetAnimation				( ) const
	{
		return colorAnimation;
	}
};
