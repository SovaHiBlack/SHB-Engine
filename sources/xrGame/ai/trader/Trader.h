//	Module 		: Trader.h
//	Description : Trader class

#pragma once

#include "../../CustomMonster.h"
#include "../../inventoryowner.h"
#include "..\..\ScriptEntity.h"
#include "../../sound_player.h"
#include "..\..\AI_PhraseDialogManager.h"
#include "../../script_export_space.h"

class CInventoryItem;
class CArtefact;
class CMotionDef;
class CBlend;
class CSoundPlayer;
class CTraderAnimation;

class CTrader : public CEntityAlive, public CInventoryOwner, public CScriptEntity, public CAI_PhraseDialogManager
{
	using inherited												= CEntityAlive;

private:
	bool														m_busy_now;

public:
									CTrader						( );
	virtual							~CTrader					( );

	virtual CAttachmentOwner*		cast_attachment_owner		( )
	{
		return this;
	}
	virtual CInventoryOwner*		cast_inventory_owner		( )
	{
		return this;
	}
	virtual CEntityAlive*			cast_entity_alive			( )
	{
		return this;
	}
	virtual CEntity*				cast_entity					( )
	{
		return this;
	}
	virtual CGameObject*			cast_game_object			( )
	{
		return this;
	}
	virtual CPHShellHolder*			cast_physics_shell_holder	( )
	{
		return this;
	}
	virtual CParticlesPlayer*		cast_particles_player		( )
	{
		return this;
	}
	virtual CScriptEntity*			cast_script_entity			( )
	{
		return this;
	}

	virtual DLL_Pure*				_construct					( );
	virtual void					Load						(const char* section);
	virtual BOOL					net_Spawn					(CSE_Abstract* DC);
	virtual void					net_Export					(CNetPacket& P);
	virtual void					net_Import					(CNetPacket& P);
	virtual void					net_Destroy					( );

	virtual void					save						(CNetPacket& output_packet);
	virtual void					load						(IReader& input_packet);
	virtual BOOL					net_SaveRelevant			( )
	{
		return inherited::net_SaveRelevant( );
	}

	virtual void					Die							(CObject* who);
	virtual void					Think						( );
	virtual void					HitSignal					(float /**P/**/, Fvector3&/**local_dir/**/, CObject* /**who/**/, signed short /**element/**/)
	{ }
	virtual void					HitImpulse					(float /**P/**/, Fvector3&/**vWorldDir/**/, Fvector3& /**vLocalDir/**/)
	{ }
	virtual void					Hit							(SHit* pHDS)
	{
		inherited::Hit(pHDS);
	}
	virtual void					UpdateCL					( );

	virtual void					g_fireParams				(const CHudItem* pHudItem, Fvector3& P, Fvector3& D);
	virtual void					g_WeaponBones				(int& L, int& R1, int& R2);
	virtual float					ffGetFov					( ) const
	{
		return 150.0f;
	}
	virtual float					ffGetRange					( ) const
	{
		return 30.0f;
	}
	virtual void					OnEvent						(CNetPacket& P, unsigned short type);
	virtual void					feel_touch_new				(CObject* O);
	virtual void					DropItemSendMessage			(CObject* O);
	virtual void					shedule_Update				(u32 dt);

	virtual BOOL					UsedAI_Locations			( );

	///////////////////////////////////////////////////////////////////////
	virtual unsigned short						PHGetSyncItemsNumber		( )
	{
		return inherited::PHGetSyncItemsNumber( );
	}
	virtual CPHSynchronize*			PHGetSyncItem				(unsigned short item)
	{
		return inherited::PHGetSyncItem(item);
	}
	virtual void					PHUnFreeze					( )
	{
		return inherited::PHUnFreeze( );
	}
	virtual void					PHFreeze					( )
	{
		return inherited::PHFreeze( );
	}
	///////////////////////////////////////////////////////////////////////

	virtual void					reinit						( );
	virtual void					reload						(const char* section);

	static void						BoneCallback				(CBoneInstance* B);
	void							LookAtActor					(CBoneInstance* B);

	void							OnStartTrade				( );
	void							OnStopTrade					( );

	// игровое имя
	virtual const char*				Name						( ) const
	{
		return CInventoryOwner::Name( );
	}

	virtual bool					can_attach					(const CInventoryItem* inventory_item) const;
	virtual bool					use_bolts					( ) const;
	virtual void					spawn_supplies				( );

	virtual bool					bfAssignSound				(CScriptEntityAction* tpEntityAction);

	virtual ALife::ERelationType	tfGetRelationType			(const CEntityAlive* tpEntityAlive) const;

	//////////////////////////////////////////////////////////////////////////
	// генерируемые задания
public:
	// проверяет список артефактов в заказах
	virtual u32						ArtefactPrice				(CArtefact* pArtefact);
	// продажа артефакта, с последуещим изменением списка заказов (true - если артефакт был в списке)
	virtual bool					BuyArtefact					(CArtefact* pArtefact);

	inline bool						busy_now					( ) const
	{
		return m_busy_now;
	}

private:
	CSoundPlayer*												m_sound_player;

public:
	inline CSoundPlayer&			sound						( ) const
	{
		VERIFY(m_sound_player);
		return *m_sound_player;
	}
	virtual bool					natural_weapon				( ) const
	{
		return false;
	}
	virtual bool					natural_detector			( ) const
	{
		return false;
	}
	virtual bool					AllowItemToTrade			(CInventoryItem const* item, EItemPlace place) const;

	void							dialog_sound_start			(const char* phrase);
	void							dialog_sound_stop			( );

private:
	CTraderAnimation*											AnimMan;

public:
	CTraderAnimation&				animation					( )
	{
		return *AnimMan;
	}

	static void						script_register				(lua_State*);
};

add_to_type_list(CTrader)
#undef script_type_list
#define script_type_list save_type_list(CTrader)
