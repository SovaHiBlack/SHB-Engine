// Для персонажей, имеющих инвентарь
// InventoryOwner.h

#pragma once

#include "InfoPortion_defs.h"
#include "pda_space.h"
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

	virtual CInventoryOwner* cast_inventory_owner( )
	{
		return this;
	}

	virtual DLL_Pure* _construct( );
	virtual BOOL		net_Spawn(CSE_Abstract* DC);
	virtual void		net_Destroy( );
	void		Init( );
	virtual void		Load(const char* section);
	virtual void		reinit( );
	virtual void		reload(const char* section);
	virtual void		OnEvent(CNetPacket& P, unsigned short type);

	// serialization
	virtual void	save(CNetPacket& output_packet);
	virtual void	load(IReader& input_packet);

	// обновление
	virtual void	UpdateInventoryOwner(u32 deltaT);
	virtual bool	CanPutInSlot(PIItem item, u32 slot)
	{
		return true;
	}

	CPda* GetPDA( ) const;

	// инвентарь
	CInventory* m_inventory;

	////////////////////////////////////
	// торговля и общение с персонажем

	virtual bool	AllowItemToTrade(CInventoryItem const* item, EItemPlace place) const;

	// инициализация объекта торговли
	CTrade* GetTrade( );

	// для включения разговора
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

	// игровое имя
	virtual const char* Name( ) const;
	u32					get_money( ) const
	{
		return m_money;
	}
	void				set_money(u32 amount, bool bSendEvent);

protected:
	u32					m_money;
	// торговля
	CTrade* m_pTrade;
	bool				m_bTalking;
	CInventoryOwner* m_pTalkPartner;

	bool				m_bAllowTalk;
	bool				m_bAllowTrade;

	u32					m_tmp_active_slot_num;

	//////////////////////////////////////////////////////////////////////////
	// сюжетная информация
public:
	// персонаж получил новую порцию информации
	virtual bool OnReceiveInfo(CSharedString info_id) const;
	// убрать информацию
	virtual void OnDisableInfo(CSharedString info_id) const;
	// передать/удалить информацию через сервер
	virtual void TransferInfo(CSharedString info_id, bool add_info) const;
	// есть ли информация у персонажа
	virtual bool				HasInfo(CSharedString info_id) const;
	virtual bool				GetInfo(CSharedString info_id, INFO_DATA&) const;

#ifdef DEBUG
	void CInventoryOwner::DumpInfo( ) const;
#endif

	CInfoPortionWrapper* m_known_info_registry;

	//////////////////////////////////////////////////////////////////////////
	// инвентарь
	const CInventory& inventory( ) const
	{
		VERIFY(m_inventory);
		return(*m_inventory);
	}
	CInventory& inventory( )
	{
		VERIFY(m_inventory);
		return(*m_inventory);
	}

	// возвращает текуший разброс стрельбы (в радианах) с учетом движения
	virtual float GetWeaponAccuracy( ) const;
	// максимальный переносимы вес
	virtual float MaxCarryWeight( ) const;

	virtual CCustomOutfit* GetOutfit( ) const
	{
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	// игровые характеристики персонажа
	CCharacterInfo& CharacterInfo( ) const
	{
		VERIFY(m_pCharacterInfo);
		return *m_pCharacterInfo;
	}
	inline const CSpecificCharacter& SpecificCharacter( ) const
	{
		return CharacterInfo( ).m_SpecificCharacter;
	}
	bool								InfinitiveMoney( )
	{
		return CharacterInfo( ).m_SpecificCharacter.MoneyDef( ).inf_money;
	}

	// установка группировки на клиентском и серверном объкте
	virtual void			SetCommunity(CharacterCommunityIndex);
	virtual void			SetRank(CharacterRankValue);
	virtual void			ChangeRank(CharacterRankValue);
	virtual void			SetReputation(CharacterReputationValue);
	virtual void			ChangeReputation(CharacterReputationValue);

	// для работы с relation system
	unsigned short								object_id( ) const;
	CharacterCommunityIndex		Community( ) const
	{
		return CharacterInfo( ).Community( ).index( );
	}
	CharacterRankValue			Rank( ) const
	{
		return CharacterInfo( ).Rank( ).value( );
	}
	CharacterReputationValue		Reputation( ) const
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
	virtual void			spawn_supplies( );

	CInventoryItem* CInventoryOwner::GetCurrentOutfit( ) const;

	//////////////////////////////////////////////////////////////////////////
	// связь со скриптами
	//////////////////////////////////////////////////////////////////////////
protected:
	CSharedString					m_item_to_spawn;
	u32							m_ammo_in_box_to_spawn;

public:
	inline const CSharedString& item_to_spawn( ) const
	{
		return m_item_to_spawn;
	}
	inline const u32& ammo_in_box_to_spawn( ) const
	{
		return m_ammo_in_box_to_spawn;
	}

	virtual void				on_weapon_shot_start(CWeapon* weapon);
	virtual void				on_weapon_shot_stop(CWeapon* weapon);
	virtual void				on_weapon_hide(CWeapon* weapon);

	virtual bool				use_simplified_visual( ) const
	{
		return false;
	}

private:
	CTradeParameters* m_trade_parameters;
	CPurchaseList* m_purchase_list;
	BOOL						m_need_osoznanie_mode;

public:
	inline CTradeParameters& trade_parameters( ) const;
	virtual const char* trade_section( ) const;
	float				deficit_factor(const CSharedString& section) const;
	void				buy_supplies(CConfigurationFile& ini_file, const char* section);
	void				sell_useless_items( );
	virtual void				on_before_sell(CInventoryItem* item)
	{ }
	virtual void				on_before_buy(CInventoryItem* item)
	{ }
	virtual bool				can_use_dynamic_lights( )
	{
		return true;
	}
	virtual bool				use_default_throw_force( );
	virtual float				missile_throw_force( );
	virtual bool				use_throw_randomness( );
	virtual bool				NeedOsoznanieMode( )
	{
		return m_need_osoznanie_mode != FALSE;
	}
};

#include "InventoryOwner_inline.h"
