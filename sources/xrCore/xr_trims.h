#pragma once

// refs
struct xr_token;

XRCORE_API int		    	_GetItemCount			(pcstr, char separator=',');
XRCORE_API LPSTR	    	_GetItem				(pcstr, int, LPSTR, char separator=',', pcstr ="", bool trim=true );
XRCORE_API LPSTR	    	_GetItems				(pcstr, int, int, LPSTR, char separator=',');
XRCORE_API pcstr	    	_SetPos					(pcstr src, u32 pos, char separator=',' );
XRCORE_API pcstr	    	_CopyVal				(pcstr src, LPSTR dst, char separator=',' );
XRCORE_API LPSTR	    	_Trim					( LPSTR str );
XRCORE_API LPSTR	    	_TrimLeft				( LPSTR str );
XRCORE_API LPSTR	    	_TrimRight				( LPSTR str );
XRCORE_API LPSTR	    	_ChangeSymbol			( LPSTR name, char src, char dest );
XRCORE_API u32		    	_ParseItem				(pcstr src, xr_token* token_list);
XRCORE_API u32		    	_ParseItem				(pcstr src, int ind, xr_token* token_list);
XRCORE_API LPSTR 	    	_ReplaceItem 			(pcstr src, int index, pcstr new_item, LPSTR dst, char separator );
XRCORE_API LPSTR 	    	_ReplaceItems 			(pcstr src, int idx_start, int idx_end, pcstr new_items, LPSTR dst, char separator );
XRCORE_API void 	    	_SequenceToList			( LPSTRVec& lst, pcstr in, char separator=',' );
XRCORE_API void 			_SequenceToList			( RStringVec& lst, pcstr in, char separator=',' );
XRCORE_API void 			_SequenceToList			( SStringVec& lst, pcstr in, char separator=',' );

XRCORE_API xr_string& 		_Trim					( xr_string& src );
XRCORE_API xr_string& 		_TrimLeft				( xr_string& src );
XRCORE_API xr_string&		_TrimRight				( xr_string& src );
XRCORE_API xr_string&   	_ChangeSymbol			( xr_string& name, char src, char dest );
XRCORE_API pcstr		 	_CopyVal 				(pcstr src, xr_string& dst, char separator=',' );
XRCORE_API pcstr			_GetItem				(pcstr src, int, xr_string& p, char separator=',', pcstr ="", bool trim=true );
XRCORE_API xr_string		_ListToSequence			( const SStringVec& lst );
XRCORE_API shared_str		_ListToSequence			( const RStringVec& lst );
