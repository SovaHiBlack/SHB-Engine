///////////////////////////////////////////////////////////////
// ini_id_loader.h
// ������������ �����, ������� ��������� �� ini ����� 
// ������ � ���������� id, ����� ����������� ������� ����������
// id ���������� index
///////////////////////////////////////////////////////////////

#pragma once

//T_ID, T_INDEX -	��� ������� � id

//ITEM_DATA		-	��������� � ������ id � index ���� T_ID � T_INDEX,
//					����������� ����� ����������� � ����������� (T_INDEX index, T_ID id, LPCSTR r1, ..., LPCSTR rN)
//					N = ITEM_REC_NUM - ����� ���. ���������� � ITEM_DATA 

//T_INIT		-	����� ��� ���������� ����������� InitIdToIndex
//					������� ������������� section_name � line_name

#define TEMPLATE_SPECIALIZATION		template<u32 ITEM_REC_NUM, typename ITEM_DATA, typename T_ID, typename T_INDEX, typename T_INIT>
#define CSINI_IdToIndex CIni_IdToIndex	<ITEM_REC_NUM, ITEM_DATA, T_ID, T_INDEX, T_INIT>

TEMPLATE_SPECIALIZATION
class CIni_IdToIndex
{
public:
	typedef T_INDEX index_type;
	typedef T_ID	id_type;

protected:
	typedef xr_vector<ITEM_DATA>	T_VECTOR;
	static	T_VECTOR* m_pItemDataVector;

	template <u32 NUM>
	static void						LoadItemData(u32, pcstr)
	{
		STATIC_CHECK(false, Specialization_for_LoadItemData_in_CIni_IdToIndex_not_found);
		NODEFAULT;
	}

	template <>
	static  void				LoadItemData<0>(u32 count, pcstr cfgRecord)
	{
		for (u32 k = 0; k < count; k += 1)
		{
			string64 buf;
			pcstr id_str = _GetItem(cfgRecord, k, buf);
			pstr id_str_lwr = xr_strdup(id_str);
			xr_strlwr(id_str_lwr);
			ITEM_DATA item_data(T_INDEX(m_pItemDataVector->size( )), T_ID(id_str));
			m_pItemDataVector->push_back(item_data);
			xr_free(id_str_lwr);
		}
	}

	template <>
	static  void				LoadItemData<1>(u32 count, pcstr cfgRecord)
	{
		for (u32 k = 0; k < count; k += 2)
		{
			string64 buf, buf1;
			pcstr id_str = _GetItem(cfgRecord, k, buf);
			pstr id_str_lwr = xr_strdup(id_str);
			xr_strlwr(id_str_lwr);
			pcstr rec1 = _GetItem(cfgRecord, k + 1, buf1);
			ITEM_DATA item_data(T_INDEX(m_pItemDataVector->size( )), T_ID(id_str), rec1);
			m_pItemDataVector->push_back(item_data);
			xr_free(id_str_lwr);
		}
	}

	//��� ������ � ����� ������ ����� ����������� id
	static pcstr section_name;
	static pcstr line_name;

public:
	CIni_IdToIndex( );
	virtual									~CIni_IdToIndex( );

	static	void							InitInternal( );
	static const ITEM_DATA* GetById(const T_ID& str_id, bool no_assert = false);
	static const ITEM_DATA* GetByIndex(T_INDEX index, bool no_assert = false);

	static const T_INDEX					IdToIndex(const T_ID& str_id, T_INDEX default_index = T_INDEX(-1), bool no_assert = false)
	{
		const ITEM_DATA* item = GetById(str_id, no_assert);
		return item ? item->index : default_index;
	}
	static const T_ID						IndexToId(T_INDEX index, T_ID default_id = NULL, bool no_assert = false)
	{
		const ITEM_DATA* item = GetByIndex(index, no_assert);
		return item ? item->id : default_id;
	}

	static const T_INDEX					GetMaxIndex( )
	{
		return m_pItemDataVector->size( ) - 1;
	}

	//�������� ����������� �������
	static void								DeleteIdToIndexData( );
};

TEMPLATE_SPECIALIZATION
typename CSINI_IdToIndex::T_VECTOR* CSINI_IdToIndex::m_pItemDataVector = NULL;

TEMPLATE_SPECIALIZATION
pcstr CSINI_IdToIndex::section_name = NULL;
TEMPLATE_SPECIALIZATION
pcstr CSINI_IdToIndex::line_name = NULL;

TEMPLATE_SPECIALIZATION
CSINI_IdToIndex::CIni_IdToIndex( )
{ }

TEMPLATE_SPECIALIZATION
CSINI_IdToIndex::~CIni_IdToIndex( )
{ }

TEMPLATE_SPECIALIZATION
const typename ITEM_DATA* CSINI_IdToIndex::GetById(const T_ID& str_id, bool no_assert)
{
	for (T_VECTOR::iterator it = m_pItemDataVector->begin( ); m_pItemDataVector->end( ) != it; it++)
	{
		if (!xr_strcmp((*it).id, str_id))
		{
			break;
		}
	}

	if (m_pItemDataVector->end( ) == it)
	{
		R_ASSERT3(no_assert, "item not found, id", *str_id);
		return NULL;
	}

	return &(*it);
}

TEMPLATE_SPECIALIZATION
const typename ITEM_DATA* CSINI_IdToIndex::GetByIndex(T_INDEX index, bool no_assert)
{
	if ((size_t) index >= m_pItemDataVector->size( ))
	{
		if (!no_assert)
		{
			Debug.fatal(DEBUG_INFO, "item by index not found in section %s, line %s", section_name, line_name);
		}

		return NULL;
	}

	return &(m_pItemDataVector->at(index));
}

TEMPLATE_SPECIALIZATION
void CSINI_IdToIndex::DeleteIdToIndexData( )
{
	xr_delete(m_pItemDataVector);
}

TEMPLATE_SPECIALIZATION
typename void	CSINI_IdToIndex::InitInternal( )
{
	VERIFY(!m_pItemDataVector);
	T_INIT::InitIdToIndex( );
	{
		m_pItemDataVector = xr_new<T_VECTOR>( );

		VERIFY(section_name);
		VERIFY(line_name);

		pcstr	cfgRecord = pSettings->r_string(section_name, line_name); VERIFY(cfgRecord);
		u32		count = _GetItemCount(cfgRecord);
		LoadItemData<ITEM_REC_NUM>(count, cfgRecord);
	}
}

#undef TEMPLATE_SPECIALIZATION