#pragma once

#include "UIDialogWnd.h"//
#include "UIPdaAux.h"//
#include "..\EncyclopediaArticle_defs.h"

class CInventoryOwner;
class CUIFrameLineWnd;
class CUIButton;
class CUITabControl;
class CUIStatic;
class CUIMapWnd;
class CUIEncyclopediaWnd;
class CUIDiaryWnd;
class CUIActorInfoWnd;
class CUIStalkersRankingWnd;
class CUIEventsWnd;
class CUIPdaContactsWnd;

class CUIPdaWnd : public CUIDialogWnd
{
private:
	using inherited									= CUIDialogWnd;

protected:
	//элементы декоративного интерфейса
	CUIFrameLineWnd*								UIMainButtonsBackground;
	CUIFrameLineWnd*								UITimerBackground;

	// кнопки PDA
	CUITabControl*									UITabControl;

	// Установить игровое время
	void					UpdateDateTime			( );
	void					DrawUpdatedSections		( );

	// Бэкграунд
	CUIStatic*										UIMainPdaFrame;
	CUIStatic*										m_updatedSectionImage;
	CUIStatic*										m_oldSectionImage;

	// Текущий активный диалог
	CUIWindow*										m_pActiveDialog;
	EPdaTabs										m_pActiveSection;
	xr_vector<Fvector2>								m_sign_places_main;

public:
	// Поддиалоги PDA
	CUIMapWnd*										UIMapWnd;
	CUIPdaContactsWnd*								UIPdaContactsWnd;
	CUIEncyclopediaWnd*								UIEncyclopediaWnd;
	CUIDiaryWnd*									UIDiaryWnd;
	CUIActorInfoWnd*								UIActorInfo;
	CUIStalkersRankingWnd*							UIStalkersRanking;
	CUIEventsWnd*									UIEventsWnd;
	virtual void			Reset					( );

							CUIPdaWnd				( );
	virtual					~CUIPdaWnd				( );

	virtual void 			Init					( );

	virtual void 			SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = nullptr);

	virtual void 			Draw					( );
	virtual void 			Update					( );
	virtual void 			Show					( );
	virtual void 			Hide					( );
	virtual bool			OnMouse					(float x, float y, EUIMessages mouse_action)
	{
		CUIDialogWnd::OnMouse(x, y, mouse_action);
		return true;
	} //always true because StopAnyMove() == false

	void					SetActiveSubdialog		(EPdaTabs section);
	virtual bool			StopAnyMove				( )
	{
		return false;
	}

	void					PdaContentsChanged		(pda_section::part type);
};
