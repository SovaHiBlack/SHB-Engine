#pragma once

#if XRGAME_EXPORTS
#	define	_memcpy CopyMemory
#	define	_memset Memory.mem_fill
#	define	_strlen xr_strlen
#else
#	define	_memcpy memcpy
#	define	_memset memset
#	define	_strlen strlen
#endif

class CMailSlotMsg
{
	string2048	m_buff;
	DWORD	m_len;
	s32     m_pos;
	IC void Read(pvoid dst, s32 sz)
	{
		_memcpy(dst, (pvoid)(&m_buff[0] + m_pos), sz);
		m_pos += sz;
	}
	IC void Write(pcvoid src, s32 sz)
	{
		_memcpy((pvoid)(&m_buff[0] + m_pos), src, sz);
		m_pos += sz; m_len = m_pos;
	}

public:
	CMailSlotMsg( )
	{
		Reset( );
	}
	IC void  Reset( )
	{
		m_len = 0;
		m_pos = 0;
		_memset(m_buff, 0, 2048);
	}
	IC void  SetBuffer(pcstr b, s32 sz)
	{
		Reset( );
		_memcpy(m_buff, b, sz);
		m_len = sz;
		m_pos = 0;
	}
	IC pvoid GetBuffer( )
	{
		return m_buff;
	}
	IC void	 SetLen(DWORD l)
	{
		m_len = l;
	}
	IC DWORD GetLen( ) const
	{
		return m_len;
	}

	IC BOOL	r_string(pstr dst)
	{
		s32 sz;
		r_int(sz);
		Read(dst, sz + 1);
		return TRUE;
	}

	IC BOOL	w_string(pcstr dst)
	{
		size_t sz = _strlen(dst);
		w_int((s32)sz);
		Write(dst, (s32)(sz + 1)); return TRUE;
	}

	IC BOOL	r_float(f32& dst)
	{
		Read(&dst, sizeof(f32));
		return TRUE;
	}

	IC BOOL	w_float(const f32 src)
	{
		Write(&src, sizeof(f32));
		return TRUE;
	}

	IC BOOL	r_int(s32& dst)
	{
		Read(&dst, sizeof(s32));
		return TRUE;
	}

	IC BOOL	w_int(const s32 src)
	{
		Write(&src, sizeof(s32));
		return TRUE;
	}

	IC BOOL	r_buff(pvoid dst, s32 sz)
	{
		Read(dst, sz);
		return TRUE;
	}

	IC BOOL	w_buff(pvoid src, s32 sz)
	{
		Write(src, sz);
		return TRUE;
	}
};

IC HANDLE CreateMailSlotByName(pstr slotName)
{
	HANDLE  hSlot = CreateMailslot(slotName,
								   0,                             // no maximum message size 
								   MAILSLOT_WAIT_FOREVER,         // no time-out for operations 
								   (LPSECURITY_ATTRIBUTES)NULL); // no security attributes 

	return hSlot;
}
IC BOOL CheckExisting(pstr slotName)
{
	HANDLE hFile;
	BOOL res;
	hFile = CreateFile(slotName,
					   GENERIC_WRITE,
					   FILE_SHARE_READ,  // required to write to a mailslot 
					   (LPSECURITY_ATTRIBUTES)NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   (HANDLE)NULL);

	res = (hFile != INVALID_HANDLE_VALUE);

	if (res)
		CloseHandle(hFile);

	return res;
}
IC BOOL SendMailslotMessage(pstr slotName, CMailSlotMsg& msg)
{
	BOOL fResult;
	HANDLE hFile;
	DWORD cbWritten;

	hFile = CreateFile(slotName,
					   GENERIC_WRITE,
					   FILE_SHARE_READ,  // required to write to a mailslot 
					   (LPSECURITY_ATTRIBUTES)NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   (HANDLE)NULL);

	R_ASSERT(hFile != INVALID_HANDLE_VALUE);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;


	fResult = WriteFile(hFile,
						msg.GetBuffer( ),
						msg.GetLen( ),
						&cbWritten,
						(LPOVERLAPPED)NULL);

	R_ASSERT(fResult);
	fResult = CloseHandle(hFile);
	R_ASSERT(fResult);
	return fResult;
}

IC BOOL CheckMailslotMessage(HANDLE hSlot, CMailSlotMsg& msg)
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	HANDLE hEvent;
	OVERLAPPED ov;

	cbMessage = cMessage = cbRead = 0;

	hEvent = CreateEvent(NULL, FALSE, FALSE, "__Slot");
	if (NULL == hEvent)
		return FALSE;
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;


	fResult = GetMailslotInfo(hSlot, // mailslot handle 
							  (LPDWORD)NULL,               // no maximum message size 
							  &cbMessage,                   // size of next message 
							  &cMessage,                    // number of messages 
							  (LPDWORD)NULL);              // no read time-out 

	R_ASSERT(fResult);

	if (!fResult || cbMessage == MAILSLOT_NO_MESSAGE)
	{
		CloseHandle(hEvent);
		return false;
	}

	msg.Reset( );
	fResult = ReadFile(hSlot,
					   msg.GetBuffer( ),
					   cbMessage,
					   &cbRead,
					   &ov);
	msg.SetLen(cbRead);
	R_ASSERT(fResult);

	CloseHandle(hEvent);
	return fResult;
}
