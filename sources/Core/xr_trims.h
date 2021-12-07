#pragma once

// refs
struct xr_token;

CORE_API int		    	_GetItemCount(const char*, char separator = ',');
CORE_API char* _GetItem(const char*, int, char*, char separator = ',', const char* = "", bool trim = true);
CORE_API char* _GetItems(const char*, int, int, char*, char separator = ',');
CORE_API const char* _SetPos(const char* src, unsigned int pos, char separator = ',');
CORE_API const char* _CopyVal(const char* src, char* dst, char separator = ',');
CORE_API char* _Trim(char* str);
CORE_API char* _TrimLeft(char* str);
CORE_API char* _TrimRight(char* str);
CORE_API char* _ChangeSymbol(char* name, char src, char dest);
CORE_API unsigned int		    	_ParseItem(const char* src, xr_token* token_list);
CORE_API unsigned int		    	_ParseItem(const char* src, int ind, xr_token* token_list);
CORE_API char* _ReplaceItem(const char* src, int index, const char* new_item, char* dst, char separator);
CORE_API char* _ReplaceItems(const char* src, int idx_start, int idx_end, const char* new_items, char* dst, char separator);
CORE_API void 	    	_SequenceToList(LPSTRVec& lst, const char* in, char separator = ',');
CORE_API void 			_SequenceToList(RStringVec& lst, const char* in, char separator = ',');
CORE_API void 			_SequenceToList(SStringVec& lst, const char* in, char separator = ',');

CORE_API xr_string& _Trim(xr_string& src);
CORE_API xr_string& _TrimLeft(xr_string& src);
CORE_API xr_string& _TrimRight(xr_string& src);
CORE_API xr_string& _ChangeSymbol(xr_string& name, char src, char dest);
CORE_API const char* _CopyVal(const char* src, xr_string& dst, char separator = ',');
CORE_API const char* _GetItem(const char* src, int, xr_string& p, char separator = ',', const char* = "", bool trim = true);
CORE_API xr_string		_ListToSequence(const SStringVec& lst);
CORE_API CSharedString		_ListToSequence(const RStringVec& lst);
