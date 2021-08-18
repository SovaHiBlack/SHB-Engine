#include "stdafx.h"

CORE_API extern CStringContainer* g_pStringContainer = nullptr;

#define HEADER		12			// ref + len + crc

SStringValue* CStringContainer::dock(const char* value)
{
	if (0 == value)
	{
		return nullptr;
	}

	cs.Enter( );

	SStringValue* result = nullptr;

	// calc len
	unsigned int s_len = xr_strlen(value);
	unsigned int s_len_with_zero = (unsigned int) s_len + 1;
	VERIFY(HEADER + s_len_with_zero < 4096);

	// setup find structure
	char header[16];
	SStringValue* sv = (SStringValue*) header;
	sv->dwReference = 0;
	sv->dwLength = s_len;
	sv->dwCRC = crc32(value, s_len);

	// search
	cdb::iterator I = container.find(sv);	// only integer compares :)
	if (I != container.end( ))
	{
		// something found - verify, it is exactly our string
		cdb::iterator save = I;
		for (; I != container.end( ) && (*I)->dwCRC == sv->dwCRC; ++I)
		{
			SStringValue* V = (*I);
			if (V->dwLength != sv->dwLength)
			{
				continue;
			}

			if (0 != memcmp(V->value, value, s_len))
			{
				continue;
			}

			result = V;				// found
			break;
		}
	}

	// it may be the case, string is not fount or has "non-exact" match
	if (0 == result)
	{
		// Insert string
		result = (SStringValue*) Memory.mem_alloc(HEADER + s_len_with_zero);
		result->dwReference = 0;
		result->dwLength = sv->dwLength;
		result->dwCRC = sv->dwCRC;
		CopyMemory(result->value, value, s_len_with_zero);
		container.insert(result);
	}

	cs.Leave( );

	return result;
}

void CStringContainer::clean( )
{
	cs.Enter( );
	cdb::iterator it = container.begin( );
	cdb::iterator end = container.end( );
	for (; it != end; )
	{
		SStringValue* sv = *it;
		if (0 == sv->dwReference)
		{
			cdb::iterator i_current = it;
			cdb::iterator i_next = ++it;
			xr_free(sv);
			container.erase(i_current);
			it = i_next;
		}
		else
		{
			it++;
		}
	}

	if (container.empty( ))
	{
		container.clear( );
	}

	cs.Leave( );
}

void CStringContainer::verify( )
{
	cs.Enter( );
	cdb::iterator it = container.begin( );
	cdb::iterator end = container.end( );
	for (; it != end; ++it)
	{
		SStringValue* sv = *it;
		unsigned int crc = crc32(sv->value, sv->dwLength);
		char crc_str[32];
		R_ASSERT3(crc == sv->dwCRC, "CorePanic: read-only memory corruption (shared_strings)", itoa(sv->dwCRC, crc_str, 16));
		R_ASSERT3(sv->dwLength == xr_strlen(sv->value), "CorePanic: read-only memory corruption (shared_strings, internal structures)", sv->value);
	}

	cs.Leave( );
}

void CStringContainer::dump( )
{
	cs.Enter( );
	cdb::iterator it = container.begin( );
	cdb::iterator end = container.end( );
	FILE* F = fopen("x:\\$str_dump$.txt", "w");
	for (; it != end; it++)
	{
		fprintf(F, "ref[%4u]-len[%3u]-crc[%8X] : %s\n", (*it)->dwReference, (*it)->dwLength, (*it)->dwCRC, (*it)->value);
	}

	fclose(F);
	cs.Leave( );
}

unsigned int CStringContainer::stat_economy( )
{
	cs.Enter( );
	cdb::iterator it = container.begin( );
	cdb::iterator end = container.end( );
	int counter = 0;
	counter -= sizeof(*this);
	counter -= sizeof(cdb::allocator_type);
	const int node_size = 20;
	for (; it != end; it++)
	{
		counter -= HEADER;
		counter -= node_size;
		counter += int((int((*it)->dwReference) - 1) * int((*it)->dwLength + 1));
	}

	cs.Leave( );
	return (unsigned int) counter;
}

CStringContainer::~CStringContainer( )
{
	clean( );
//	R_ASSERT(container.empty( ));
}
