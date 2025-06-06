//////////////////////////////////////////////////////////////////////
// InventoryOwner_info.h:	��� ������ � �������� �����������

#include "stdafx.h"

#include "InventoryOwner.h"
#include "GameObject.h"
#include "xrMessages.h"
#include "AISpace.h"
#include "ai_debug.h"
#include "alife_simulator.h"
#include "alife_registry_container.h"
#include "script_game_object.h"
#include "level.h"
#include "InfoPortion.h"
#include "alife_registry_wrappers.h"
#include "script_callback_ex.h"
#include "GameObject_space.h"

void CInventoryOwner::OnEvent(CNetPacket& P, u16 type)
{
	switch (type)
	{
		case GE_INFO_TRANSFER:
		{
			u16				id;
			shared_str		info_id;
			u8				add_info;

			P.r_u16(id);				//�����������
			P.r_stringZ(info_id);		//����� ���������� ����������
			P.r_u8(add_info);			//���������� ��� �������� ����������

			if (add_info)
			{
				OnReceiveInfo(info_id);
			}
			else
			{
				OnDisableInfo(info_id);
			}
		}
		break;
	}
}

class CFindByIDPred
{
public:
	CFindByIDPred(shared_str element_to_find)
	{
		element = element_to_find;
	}
	bool operator () (const INFO_DATA& data) const
	{
		return data.info_id == element;
	}

private:
	shared_str element;
};

bool CInventoryOwner::OnReceiveInfo(shared_str info_id) const
{
	VERIFY(info_id.size( ));
	//�������� ������ � ������
	KNOWN_INFO_VECTOR& known_info = m_known_info_registry->registry( ).objects( );
	KNOWN_INFO_VECTOR_IT it = std::find_if(known_info.begin( ), known_info.end( ), CFindByIDPred(info_id));
	if (known_info.end( ) == it)
		known_info.push_back(INFO_DATA(info_id, Level( ).GetGameTime( )));
	else
		return false;

#ifdef DEBUG
	if (psAI_Flags.test(aiInfoPortion))
		Msg("[%s] Received Info [%s]", Name( ), *info_id);
#endif

	//��������� ���������� callback
	const CGameObject* pThisGameObject = smart_cast<const CGameObject*>(this);
	VERIFY(pThisGameObject);

	//	SCRIPT_CALLBACK_EXECUTE_2(*m_pInfoCallback, pThisGameObject->lua_game_object(), info_index);
	//	pThisGameObject->callback(GameObject::eInventoryInfo)(pThisGameObject->lua_game_object(), *info_id);


	CInfoPortion info_portion;
	info_portion.Load(info_id);

	//��������� ���������� �������
	info_portion.RunScriptActions(pThisGameObject);

	//�������� �� info portions ������� ����� �������������
	for (u32 i = 0; i < info_portion.DisableInfos( ).size( ); i++)
	{
		TransferInfo(info_portion.DisableInfos( )[i], false);
	}

	return true;
}

#ifdef DEBUG
void CInventoryOwner::DumpInfo( ) const
{
	KNOWN_INFO_VECTOR& known_info = m_known_info_registry->registry( ).objects( );

	Msg("------------------------------------------");
	Msg("Start KnownInfo dump for [%s]", Name( ));
	KNOWN_INFO_VECTOR_IT it = known_info.begin( );
	for (s32 i = 0; it != known_info.end( ); ++it, ++i)
	{
		Msg("known info[%d]:%s", i, *(*it).info_id);
	}

	Msg("------------------------------------------");
}
#endif

void CInventoryOwner::OnDisableInfo(shared_str info_id) const
{
	VERIFY(info_id.size( ));
	//������� ������ �� �������

#ifdef DEBUG
	if (psAI_Flags.test(aiInfoPortion))
	{
		Msg("[%s] Disabled Info [%s]", Name( ), *info_id);
	}
#endif

	KNOWN_INFO_VECTOR& known_info = m_known_info_registry->registry( ).objects( );

	KNOWN_INFO_VECTOR_IT it = std::find_if(known_info.begin( ), known_info.end( ), CFindByIDPred(info_id));
	if (known_info.end( ) == it)
	{
		return;
	}

	known_info.erase(it);
}

void CInventoryOwner::TransferInfo(shared_str info_id, bool add_info) const
{
	VERIFY(info_id.size( ));
	const CObject* pThisObject = smart_cast<const CObject*>(this); VERIFY(pThisObject);

	//���������� �� ������ PDA ����� ���������� � �������
	CNetPacket		P;
	CGameObject::u_EventGen(P, GE_INFO_TRANSFER, pThisObject->ID( ));
	P.w_u16(pThisObject->ID( ));					//�����������
	P.w_stringZ(info_id);							//���������
	P.w_u8(add_info ? 1 : 0);							//��������/������� ����������
	CGameObject::u_EventSend(P);

	CInfoPortion info_portion;
	info_portion.Load(info_id);
	{
		if (add_info)
		{
			OnReceiveInfo(info_id);
		}
		else
		{
			OnDisableInfo(info_id);
		}
	}
}

bool CInventoryOwner::HasInfo(shared_str info_id) const
{
	VERIFY(info_id.size( ));
	const KNOWN_INFO_VECTOR* known_info = m_known_info_registry->registry( ).objects_ptr( );
	if (!known_info)
	{
		return false;
	}

	if (std::find_if(known_info->begin( ), known_info->end( ), CFindByIDPred(info_id)) == known_info->end( ))
	{
		return false;
	}

	return true;
}

bool CInventoryOwner::GetInfo(shared_str info_id, INFO_DATA& info_data) const
{
	VERIFY(info_id.size( ));
	const KNOWN_INFO_VECTOR* known_info = m_known_info_registry->registry( ).objects_ptr( );
	if (!known_info)
	{
		return false;
	}

	KNOWN_INFO_VECTOR::const_iterator it = std::find_if(known_info->begin( ), known_info->end( ), CFindByIDPred(info_id));
	if (known_info->end( ) == it)
	{
		return false;
	}

	info_data = *it;
	return true;
}
