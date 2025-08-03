#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"
#include "../PhraseDialog_defs.h"

class CActor;
class CInventoryOwner;
class CPhraseDialogManager;
class CUITalkDialogWnd;
class CUITradeWnd;

class CUITalkWnd : public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
	ref_sound			m_sound;
	void				PlaySnd(pcstr text);
	void				StopSnd( );

public:
	CUITalkWnd( );
	virtual				~CUITalkWnd( );

	virtual void		Init( );

	virtual bool		StopAnyMove( )
	{
		return true;
	}
	virtual void		SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = NULL);

	virtual void		Draw( );
	virtual void		Update( );

	virtual void		Show( );
	virtual void		Hide( );

	void				Stop( );					//deffered

	void				UpdateQuestions( );
	void				NeedUpdateQuestions( );
	//������������� ���������� ������� �����������
	void				InitOthersStartDialog( );
	virtual bool		IR_OnKeyboardPress(s32 dik);
	virtual bool		OnKeyboard(s32 dik, EUIMessages keyboard_action);
	void				SwitchToTrade( );
	void				AddIconedMessage(pcstr text, pcstr texture_name, fRect texture_rect, pcstr templ_name);

protected:
	//������
	void				InitTalkDialog( );
	void				AskQuestion( );

	void				SayPhrase(const shared_str& phrase_id);

	// ������� ���������� ����� � ����� �������� � �������
public:
	void				AddQuestion(const shared_str& text, const shared_str& id);
	void				AddAnswer(const shared_str& text, pcstr SpeakerName);

protected:
	//��� ������ ��������
	CUITradeWnd* UITradeWnd;
	CUITalkDialogWnd* UITalkDialogWnd;


	//��������� �� ��������� ��������� ���������� �������
	//� ��� �����������
	CActor* m_pActor;
	CInventoryOwner* m_pOurInvOwner;
	CInventoryOwner* m_pOthersInvOwner;

	CPhraseDialogManager* m_pOurDialogManager;
	CPhraseDialogManager* m_pOthersDialogManager;

	//����. ����������, ����� ��� ���� ����� RemoveAll
	//����� ���� ��������� ������� �� SendMessage
	//��� ��� ����� ��������� ��������, ��� �����, �������
	//������ ��� SendMessage ���������� � ���������� ������� pListItem
	bool				m_bNeedToUpdateQuestions;

	//������� ������, ���� NULL, �� ��������� � ����� ������ ����
	DIALOG_SHARED_PTR	m_pCurrentDialog;
	bool				TopicMode( );
	void				ToTopicMode( );
};
