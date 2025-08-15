#pragma once

// refs
struct xr_token;

CORE_API s32			_GetItemCount(pcstr, char separator = ',');
CORE_API pstr			_GetItem(pcstr, s32, pstr, char separator = ',', pcstr = "", bool trim = true);
CORE_API pstr			_GetItems(pcstr, s32, s32, pstr, char separator = ',');
CORE_API pcstr			_SetPos(pcstr src, u32 pos, char separator = ',');
CORE_API pcstr			_CopyVal(pcstr src, pstr dst, char separator = ',');
CORE_API pstr			_Trim(pstr str);
CORE_API pstr			_TrimLeft(pstr str);
CORE_API pstr			_TrimRight(pstr str);
CORE_API pstr			_ChangeSymbol(pstr name, char src, char dest);
CORE_API u32			_ParseItem(pcstr src, xr_token* token_list);
CORE_API u32			_ParseItem(pcstr src, s32 ind, xr_token* token_list);
CORE_API pstr			_ReplaceItem(pcstr src, s32 index, pcstr new_item, pstr dst, char separator);
CORE_API pstr			_ReplaceItems(pcstr src, s32 idx_start, s32 idx_end, pcstr new_items, pstr dst, char separator);
CORE_API void			_SequenceToList(LPSTRVec& lst, pcstr in, char separator = ',');
CORE_API void			_SequenceToList(RStringVec& lst, pcstr in, char separator = ',');
CORE_API void			_SequenceToList(SStringVec& lst, pcstr in, char separator = ',');

CORE_API xr_string&		_Trim(xr_string& src);
CORE_API xr_string&		_TrimLeft(xr_string& src);
CORE_API xr_string&		_TrimRight(xr_string& src);
CORE_API xr_string&		_ChangeSymbol(xr_string& name, char src, char dest);
CORE_API pcstr			_CopyVal(pcstr src, xr_string& dst, char separator = ',');
CORE_API pcstr			_GetItem(pcstr src, s32, xr_string& p, char separator = ',', pcstr = "", bool trim = true);
CORE_API xr_string		_ListToSequence(const SStringVec& lst);
CORE_API shared_str		_ListToSequence(const RStringVec& lst);
