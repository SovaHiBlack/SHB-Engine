#pragma once

#include "ISheduled.h"

class ENGINE_API CSheduler
{
private:
	struct SItem
	{
		unsigned int										dwTimeForExecute;
		unsigned int										dwTimeOfLastExecute;
		CSharedString								scheduled_name;
		ISheduled*								Object;
		unsigned int										dwPadding;				// for align-issues

		inline bool		operator <				(SItem& I)
		{
			return dwTimeForExecute > I.dwTimeForExecute;
		}
	};

	struct ItemReg
	{
		BOOL									OP;
		BOOL									RT;
		ISheduled*								Object;
	};

	xr_vector<SItem>								ItemsRT;
	xr_vector<SItem>								Items;
	xr_vector<SItem>								ItemsProcessed;
	xr_vector<ItemReg>							Registration;
	bool										m_processing_now;

	inline void			Push					(SItem& I);
	inline void			Pop						( );
	inline SItem&		Top						( )
	{
		return Items.front( );
	}
	void				internal_Register		(ISheduled* A, BOOL RT = FALSE);
	bool				internal_Unregister		(ISheduled* A, BOOL RT, bool warn_on_not_found = true);
	void				internal_Registration	( );

public:
	unsigned __int64											cycles_start;
	unsigned __int64											cycles_limit;

	void				ProcessStep				( );
	void				Update					( );

#ifdef DEBUG
	bool				Registered				(ISheduled* object) const;
#endif // def DEBUG

	void				Register				(ISheduled* A, BOOL RT = FALSE);
	void				Unregister				(ISheduled* A);
	void				EnsureOrder				(ISheduled* Before, ISheduled* After);

	void				Initialize				( );
	void				Destroy					( );
};
