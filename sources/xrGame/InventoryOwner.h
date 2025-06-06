/////////////////////////////////////////////////////
// ��� ����������, ������� ���������
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortion_defs.h"
#include "PDA_space.h"
#include "AttachmentOwner.h"
#include "script_space_forward.h"
#include "CharacterInfo.h"
#include "Inventory_space.h"

class CSE_Abstract;
class CInventory;
class CInventoryItem;
class CTrade;
class CPda;
class CGameObject;
class CEntityAlive;
class CCustomZone;
class CInfoPortionWrapper;
class CNetPacket;
class CCharacterInfo;
class CSpecificCharacter;
class CTradeParameters;
class CPurchaseList;
class CWeapon;
class CCustomOutfit;

class CInventoryOwner : public CAttachmentOwner
{
public:
	CInventoryOwner( );
	virtual			~CInventoryOwner( );

public:
	virtual CInventoryOwner* cast_inventory_owner( )
	{
		return this;
	}

public:
	virtual DLL_Pure* _construct( );
	virtual BOOL		net_Spawn(CSE_Abstract* DC);
	virtual void		net_Destroy( );
	void		Init( );
	virtual void		Load(pcstr section);
	virtual void		reinit( );
	virtual void		reload(pcstr section);
	virtual void		OnEvent(CNetPacket& P, u16 type);

	//serialization
	virtual void	save(CNetPacket& output_packet);
	virtual void	load(IReader& input_packet);

	//����������
	virtual void	UpdateInventoryOwner(u32 deltaT);
	virtual bool	CanPutInSlot(PIItem item, u32 slot)
	{
		return true;
	}

	CPda* GetPDA( ) const;

	// ���������
	CInventory* m_inventory;

	////////////////////////////////////
	//�������� � ������� � ����������

	virtual bool	AllowItemToTrade(CInventoryItem const* item, EItemPlace place) const;
	virtual void	OnFollowerCmd(s32 cmd)
	{ }//redefine for CAI_Stalkker
	//������������� ������� ��������
	CTrade* GetTrade( );

	//��� ��������� ���������
	virtual bool OfferTalk(CInventoryOwner* talk_partner);
	virtual void StartTalk(CInventoryOwner* talk_partner, bool start_trade = true);
	virtual void StopTalk( );
	virtual bool IsTalking( );

	virtual void EnableTalk( )
	{
		m_bAllowTalk = true;
	}
	virtual void DisableTalk( )
	{
		m_bAllowTalk = false;
	}
	virtual bool IsTalkEnabled( )
	{
		return m_bAllowTalk;
	}

	void EnableTrade( )
	{
		m_bAllowTrade = true;
	}
	void DisableTrade( )
	{
		m_bAllowTrade = false;
	}
	bool IsTradeEnabled( )
	{
		return m_bAllowTrade;
	}

	CInventoryOwner* GetTalkPartner( )
	{
		return m_pTalkPartner;
	}
	virtual void	 NewPdaContact(CInventoryOwner*);
	virtual void	 LostPdaContact(CInventoryOwner*);

	//������� ��� 
	virtual pcstr	Name( ) const;
	u32					get_money( ) const
	{
		return m_money;
	}
	void				set_money(u32 amount, bool bSendEvent);

protected:
	u32					m_money;
	// ��������
	CTrade* m_pTrade;
	bool				m_bTalking;
	CInventoryOwner* m_pTalkPartner;

	bool				m_bAllowTalk;
	bool				m_bAllowTrade;

	u32					m_tmp_active_slot_num;

	//////////////////////////////////////////////////////////////////////////
	// �������� ����������
public:
	//�������� ������� ����� ������ ����������
	virtual bool OnReceiveInfo(shared_str info_id) const;
	//������ ����������
	virtual void OnDisableInfo(shared_str info_id) const;
	//��������/������� ���������� ����� ������
	virtual void TransferInfo(shared_str info_id, bool add_info) const;
	//���� �� ���������� � ���������
	virtual bool				HasInfo(shared_str info_id) const;
	virtual bool				GetInfo(shared_str info_id, INFO_DATA&) const;

#ifdef DEBUG
	void CInventoryOwner::DumpInfo( ) const;
#endif

	CInfoPortionWrapper* m_known_info_registry;

	//////////////////////////////////////////////////////////////////////////
	// ��������� 
public:
	const CInventory& inventory( ) const
	{
		VERIFY(m_inventory); return(*m_inventory);
	}
	CInventory& inventory( )
	{
		VERIFY(m_inventory); return(*m_inventory);
	}

	//���������� ������� ������� �������� (� ��������) � ������ ��������
	virtual f32 GetWeaponAccuracy( ) const;
	//������������ ���������� ���
	virtual f32 MaxCarryWeight( ) const;

	virtual CCustomOutfit* GetOutfit( )	const
	{
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//������� �������������� ���������
public:
	CCharacterInfo& CharacterInfo( ) const
	{
		VERIFY(m_pCharacterInfo);
		return *m_pCharacterInfo;
	}
	IC const CSpecificCharacter& SpecificCharacter( ) const
	{
		return CharacterInfo( ).m_SpecificCharacter;
	}
	bool								InfinitiveMoney( )
	{
		return CharacterInfo( ).m_SpecificCharacter.MoneyDef( ).inf_money;
	}

	//��������� ����������� �� ���������� � ��������� ������
	virtual void			SetCommunity(CHARACTER_COMMUNITY_INDEX);
	virtual void			SetRank(CHARACTER_RANK_VALUE);
	virtual void			ChangeRank(CHARACTER_RANK_VALUE);
	virtual void			SetReputation(CHARACTER_REPUTATION_VALUE);
	virtual void			ChangeReputation(CHARACTER_REPUTATION_VALUE);

	//��� ������ � relation system
	u16								object_id( ) const;
	CHARACTER_COMMUNITY_INDEX		Community( ) const
	{
		return CharacterInfo( ).Community( ).index( );
	}
	CHARACTER_RANK_VALUE			Rank( ) const
	{
		return CharacterInfo( ).Rank( ).value( );
	}
	CHARACTER_REPUTATION_VALUE		Reputation( ) const
	{
		return CharacterInfo( ).Reputation( ).value( );
	}

protected:
	CCharacterInfo* m_pCharacterInfo;
	xr_string				m_game_name;

public:
	virtual void			renderable_Render( );
	virtual void			OnItemTake(CInventoryItem* inventory_item);

	virtual void			OnItemBelt(CInventoryItem* inventory_item, EItemPlace previous_place);
	virtual void			OnItemRuck(CInventoryItem* inventory_item, EItemPlace previous_place);
	virtual void			OnItemSlot(CInventoryItem* inventory_item, EItemPlace previous_place);

	virtual void			OnItemDrop(CInventoryItem* inventory_item);

	virtual void			OnItemDropUpdate( );
	virtual bool			use_bolts( ) const
	{
		return(true);
	}
	virtual	void			spawn_supplies( );

	CInventoryItem* CInventoryOwner::GetCurrentOutfit( ) const;

	//////////////////////////////////////////////////////////////////////////
	// ����� �� ���������
	//////////////////////////////////////////////////////////////////////////
protected:
	shared_str					m_item_to_spawn;
	u32							m_ammo_in_box_to_spawn;

public:
	IC const shared_str& item_to_spawn( ) const
	{
		return m_item_to_spawn;
	}
	IC const u32& ammo_in_box_to_spawn( ) const
	{
		return m_ammo_in_box_to_spawn;
	}

public:
	virtual void				on_weapon_shot_start(CWeapon* weapon);
	virtual void				on_weapon_shot_stop(CWeapon* weapon);
	virtual void				on_weapon_hide(CWeapon* weapon);

public:
	virtual bool				use_simplified_visual( ) const
	{
		return false;
	}

private:
	CTradeParameters* m_trade_parameters;
	CPurchaseList* m_purchase_list;
	BOOL						m_need_osoznanie_mode;

public:
	IC		CTradeParameters& trade_parameters( ) const;
	virtual	pcstr				trade_section( ) const;
	f32				deficit_factor(const shared_str& section) const;
	void				buy_supplies(CIniFile& ini_file, pcstr section);
	void				sell_useless_items( );
	virtual	void				on_before_sell(CInventoryItem* item)
	{ }
	virtual	void				on_before_buy(CInventoryItem* item)
	{ }
	virtual bool				can_use_dynamic_lights( )
	{
		return true;
	}
	virtual	bool				use_default_throw_force( );
	virtual	f32				missile_throw_force( );
	virtual	bool				use_throw_randomness( );
	virtual bool				NeedOsoznanieMode( )
	{
		return m_need_osoznanie_mode != FALSE;
	}
};

#include "InventoryOwner_inline.h"
