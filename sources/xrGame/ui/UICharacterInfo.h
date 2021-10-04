// UICharacterInfo.h:  окошко, с информацией о персонаже
#pragma once

#include "UIWindow.h"//
#include "..\ALife_space.h"
#include "../CharacterInfo_defs.h"

class CUIStatic;
class CCharacterInfo;
class CUIXml;
class CUIScrollView;

class CUICharacterInfo : public CUIWindow
{
private:
	using inherited							= CUIWindow;

protected:
	void				SetRelation			(ALife::ERelationType relation, CharacterGoodwill goodwill);
	void				ResetAllStrings		( );
	void				UpdateRelation		( );
	bool				hasOwner			( )
	{
		return (m_ownerID != U16(-1));
	}

	//Biography
	CUIScrollView*							pUIBio;
	bool									m_bForceUpdate;
	U16										m_ownerID;

	enum
	{
		eUIIcon								= 0,
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

	CUIStatic*								m_icons[eMaxCaption];
	CSharedString								m_texture_name;

public:
						CUICharacterInfo	( );
	virtual				~CUICharacterInfo	( );

	void				Init				(float x, float y, float width, float height, CUIXml* xml_doc);
	void				Init				(float x, float y, float width, float height, const char* xml_name);
	void				InitCharacter		(U16 id);
	void				ClearInfo			( );

	virtual void		Update				( );

	U16					OwnerID				( ) const
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
	const CSharedString&	IconName			( )
	{
		return m_texture_name;
	}
};
