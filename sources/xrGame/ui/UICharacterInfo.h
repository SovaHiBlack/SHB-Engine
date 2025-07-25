// UICharacterInfo.h:  ������, � ����������� � ���������
// 
//////////////////////////////////////////////////////////////////////
#pragma once

#include "UIWindow.h"
#include "../alife_space.h"
#include "../CharacterInfo_defs.h"

class CUIStatic;
class CCharacterInfo;
class CUIXml;
class CUIScrollView;

class CUICharacterInfo : public CUIWindow
{
private:
	using inherited = CUIWindow;

protected:
	void				SetRelation			(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill);
	void				ResetAllStrings		( );
	void				UpdateRelation		( );
	bool				hasOwner			( )
	{
		return (m_ownerID != u16(-1));
	}
	// Biography
	CUIScrollView*		pUIBio;
	bool				m_bForceUpdate;
	u16					m_ownerID;

	enum
	{
		eUIIcon = 0,
		eUIName,
		eUIRank,
		eUIRankCaption,
		eUICommunity,
		eUICommunityCaption,
		eUIReputation,
		eUIReputationCaption,
		eUIRelation,
		eUIRelationCaption,
		eMaxCaption
	};

	CUIStatic*			m_icons[eMaxCaption];
	shared_str			m_texture_name;

public:
						CUICharacterInfo	( );
	virtual				~CUICharacterInfo	( );

	void				Init				(f32 x, f32 y, f32 width, f32 height, CUIXml* xml_doc);
	void				Init				(f32 x, f32 y, f32 width, f32 height, pcstr xml_name);
	void				InitCharacter		(u16 id);
	void				ClearInfo			( );

	virtual void		Update				( );

	u16					OwnerID				( ) const
	{
		return m_ownerID;
	}
	CUIStatic&			UIIcon				( )
	{
		VERIFY(m_icons[eUIIcon]);
		return *m_icons[eUIIcon];
	}
	CUIStatic&			UIName				( )
	{
		VERIFY(m_icons[eUIName]);
		return *m_icons[eUIName];
	}
	const shared_str&	IconName			( )
	{
		return m_texture_name;
	}
};
