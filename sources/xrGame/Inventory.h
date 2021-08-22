#pragma once

#include "InventoryItem.h"

class CInventory;
class CInventoryItem;
class CHudItem;
class CInventoryOwner;

class CInventorySlot
{
public:
								CInventorySlot					( );
	virtual						~CInventorySlot					( );

	bool						CanBeActivated					( ) const;
	bool						IsBlocked						( ) const;

	PIItem														m_pIItem;
	bool														m_bPersistent;
	bool														m_bVisible;
	int															m_blockCounter;
};

enum EActivationReason
{
	eGeneral,
	eKeyAction,
	eImportUpdate
};

using InventorySlotVec											= xr_vector<CInventorySlot>;

class CInventory
{
public:
								CInventory						( );
	virtual						~CInventory						( );

	float						TotalWeight						( ) const;
	float						CalcTotalWeight					( );

	void						Take							(CGameObject* pObj, bool bNotActivate, bool strict_placement);
	bool						DropItem						(CGameObject* pObj);
	void						Clear							( );

	bool						Slot							(PIItem pIItem, bool bNotActivate = false);
	bool						Belt							(PIItem pIItem);
	bool						Ruck							(PIItem pIItem);

	bool						InSlot							(PIItem pIItem) const;
	bool						InBelt							(PIItem pIItem) const;
	bool						InRuck							(PIItem pIItem) const;

	bool						CanPutInSlot					(PIItem pIItem) const;
	bool						CanPutInBelt					(PIItem pIItem);
	bool						CanPutInRuck					(PIItem pIItem) const;

	bool						CanTakeItem						(CInventoryItem* inventory_item) const;

	bool						Activate						(unsigned int slot, EActivationReason reason = eGeneral, bool bForce = false);
	void						Activate_deffered				(unsigned int slot, unsigned int _frame);
	PIItem						ActiveItem						( ) const
	{
		return m_iActiveSlot == NO_ACTIVE_SLOT ? NULL : m_slots[m_iActiveSlot].m_pIItem;
	}
	PIItem						ItemFromSlot					(unsigned int slot) const;
	void						ActivateNextItemInActiveSlot	( );
	bool						Action							(int cmd, unsigned int flags);
	void						Update							( );
	//ищет на поясе аналогичный IItem
	PIItem						Same							(const PIItem pIItem, bool bSearchRuck) const;
	//ищет на поясе IItem для указанного слота
	PIItem						SameSlot						(const unsigned int slot, PIItem pIItem, bool bSearchRuck) const;
	//ищет на поясе или в рюкзаке IItem с указанным именем (cName())
	PIItem						Get								(const char* name, bool bSearchRuck) const;
	//ищет на поясе или в рюкзаке IItem с указанным именем (id)
	PIItem						Get								(const unsigned short id, bool bSearchRuck) const;
	//ищет на поясе или в рюкзаке IItem с указанным CLS_ID
	PIItem						Get								(CLASS_ID cls_id, bool bSearchRuck) const;
	PIItem						GetAny							(const char* name) const;	//search both (ruck and belt)
	PIItem						item							(CLASS_ID cls_id) const;

	//получите все элементы с одинаковым section name
	virtual unsigned int		dwfGetSameItemCount				(const char* caSection, bool SearchAll = false);
	virtual unsigned int		dwfGetGrenadeCount				(const char* caSection, bool SearchAll);
	//получите все элементы с одинаковым object id
	virtual bool				bfCheckForObject				(ALife::_OBJECT_ID tObjectID);
	PIItem						get_object_by_id				(ALife::_OBJECT_ID tObjectID);

	unsigned int				dwfGetObjectCount				( );
	PIItem						tpfGetObjectByIndex				(int iIndex);
	PIItem						GetItemFromInventory			(const char* caItemName);

	bool						Eat								(PIItem pIItem);

	unsigned int				GetActiveSlot					( ) const
	{
		return m_iActiveSlot;
	}

	void						SetPrevActiveSlot				(unsigned int ActiveSlot)
	{
		m_iPrevActiveSlot = ActiveSlot;
	}
	unsigned int				GetPrevActiveSlot				( ) const
	{
		return m_iPrevActiveSlot;
	}
	unsigned int				GetNextActiveSlot				( ) const
	{
		return m_iNextActiveSlot;
	}

	void						SetActiveSlot					(unsigned int ActiveSlot)
	{
		m_iActiveSlot = m_iNextActiveSlot = ActiveSlot;
	}

	bool						IsSlotsUseful					( ) const
	{
		return m_bSlotsUseful;
	}
	void						SetSlotsUseful					(bool slots_useful)
	{
		m_bSlotsUseful = slots_useful;
	}
	bool						IsBeltUseful					( ) const
	{
		return m_bBeltUseful;
	}
	void						SetBeltUseful					(bool belt_useful)
	{
		m_bBeltUseful = belt_useful;
	}

	void						SetSlotsBlocked					(unsigned short mask, bool bBlock);
	TIItemContainer												m_all;
	TIItemContainer												m_ruck;
	TIItemContainer												m_belt;
	InventorySlotVec											m_slots;

	//возвращает все кроме PDA в слоте и болта
	void						AddAvailableItems				(TIItemContainer& items_container, bool for_trade) const;

	float						GetTakeDist						( ) const
	{
		return m_fTakeDist;
	}

	float						GetMaxWeight					( ) const
	{
		return m_fMaxWeight;
	}
	void						SetMaxWeight					(float weight)
	{
		m_fMaxWeight = weight;
	}

	unsigned int				BeltWidth						( ) const;

	inline CInventoryOwner*		GetOwner						( ) const
	{
		return m_pOwner;
	}

	//объект на который наведен прицел
	PIItem														m_pTarget;

	friend class CInventoryOwner;

	unsigned int				ModifyFrame						( ) const
	{
		return m_dwModifyFrame;
	}
	void						InvalidateState					( )
	{
		m_dwModifyFrame = Device.dwFrame;
	}
	void						Items_SetCurrentEntityHud		(bool current_entity);
	bool						isBeautifulForActiveSlot		(CInventoryItem* pIItem);

protected:
	void						UpdateDropTasks					( );
	void						UpdateDropItem					(PIItem pIItem);

	//Активный слот и слот который станет активным после смены
	//значения совпадают в обычном состоянии (нет смены слотов)
	unsigned int												m_iActiveSlot;
	unsigned int												m_iNextActiveSlot;
	unsigned int												m_iPrevActiveSlot;
	unsigned int												m_iLoadActiveSlot;
	unsigned int												m_iLoadActiveSlotFrame;
	EActivationReason											m_ActivationSlotReason;

	CInventoryOwner*											m_pOwner;

	//флаг, показывающий наличие пояса в инвенторе
	bool														m_bBeltUseful;
	//флаг, допускающий использование слотов
	bool														m_bSlotsUseful;

	//максимальный вес инвентаря
	float														m_fMaxWeight;
	//текущий вес в инвентаре
	float														m_fTotalWeight;

	//максимальное кол-во объектов на поясе
	unsigned int												m_iMaxBelt;
	//максимальное расстояние на котором можно подобрать объект
	float														m_fTakeDist;

	//кадр на котором произошло последнее изменение в инвенторе
	unsigned int												m_dwModifyFrame;

	bool														m_drop_last_frame;

	void						SendActionEvent					(int cmd, unsigned int flags);
};
