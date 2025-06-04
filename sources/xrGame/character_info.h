//////////////////////////////////////////////////////////////////////////
// character_info.h			������, ��� ������������� ������������ ��������
// 
//////////////////////////////////////////////////////////////////////////

#pragma	once

#include "character_info_defs.h"
#include "shared_data.h"
#include "xml_str_id_loader.h"

class CNetPacket;

#include "specific_character.h"

#ifdef XRGAME_EXPORTS
	#include "PhraseDialog_defs.h"
	#include "character_community.h"
	#include "character_rank.h"
	#include "character_reputation.h"
	class CSE_ALifeTraderAbstract;
#endif


//////////////////////////////////////////////////////////////////////////
// SCharacterProfile: ������ ������� ���������
//////////////////////////////////////////////////////////////////////////
struct SCharacterProfile : CSharedResource
{
	SCharacterProfile ();
	virtual ~SCharacterProfile ();

    //���� ������, �� ���������� ������ ����� �������,
	//����� ������ ��������,��������������� �������
	shared_str		m_CharacterId;	

	//��������� ��������� ���������
	CHARACTER_CLASS					m_Class;
	CHARACTER_RANK_VALUE			m_Rank;
	CHARACTER_REPUTATION_VALUE		m_Reputation;
};


class CInventoryOwner;
class CSE_ALifeTraderAbstract;

class CCharacterInfo: public CSharedClass<SCharacterProfile, shared_str, false>,
					  public CXML_IdToIndex<CCharacterInfo>
{
private:
	typedef CSharedClass	<SCharacterProfile, shared_str, false>	inherited_shared;
	typedef CXML_IdToIndex	<CCharacterInfo>						id_to_index;

	friend id_to_index;
	friend CInventoryOwner;
	friend CSE_ALifeTraderAbstract;
public:



								CCharacterInfo		();
								~CCharacterInfo		();

	virtual void Load	(shared_str id);

#ifdef XRGAME_EXPORTS
	void 						load				(IReader&);
	void 						save				(CNetPacket&);

	//������������� ������� �������������
	//�������� ���������������� CSpecificCharacter, �� 
	//���������� �������
	void	Init				(CSE_ALifeTraderAbstract* trader);
	void InitSpecificCharacter	(shared_str new_id);
#endif

protected:
	const SCharacterProfile*	data				() const	{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SCharacterProfile*			data				()				{ VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void					InitXmlIdToIndex	();


	//�������� �� XML �����
	virtual void				load_shared			(pcstr);

	//������ ������������ �������
	shared_str					m_ProfileId;
	
	//������ ������ � ���������� ���������, �������
	//������������ � ������ ���������� ������
	shared_str		m_SpecificCharacterId;

#ifdef XRGAME_EXPORTS
	shared_str					m_StartDialog;

	//����������� ���������� � ���������� ���������
	CSpecificCharacter			m_SpecificCharacter;
#endif

public:

#ifdef XRGAME_EXPORTS
	shared_str					Profile()			const;
	pcstr						Name()				const;
	shared_str					Bio()				const;

	const CCharacterCommunity&	Community()			const	{return m_CurrentCommunity;};
	const CCharacterRank&		Rank()				const	{ return m_CurrentRank;};
	const CCharacterReputation&	Reputation()		const	{ return m_CurrentReputation;};

	//�������� ������ � InventoryOwner
protected:
	void						SetRank				(CHARACTER_RANK_VALUE			rank);
	void						SetReputation		(CHARACTER_REPUTATION_VALUE		reputation);
	void						SetCommunity		(const CCharacterCommunity&		community)		{m_CurrentCommunity = community;};

public:
	const shared_str&			IconName			()	const;

	shared_str					StartDialog			()	const;
	const DIALOG_ID_VECTOR&		ActorDialogs		()	const;
#endif

protected:


#ifdef XRGAME_EXPORTS
	CCharacterRank					m_CurrentRank;
	CCharacterReputation			m_CurrentReputation;
	CCharacterCommunity				m_CurrentCommunity;
#endif
};