#include "stdafx.h"

#include "PHDestroyableNotificate.h"
//#include "ALife_space.h"
//#include "Hit.h"
#include "PHDestroyable.h"
#include "Level.h"
//#include "xrServer_Object_Base.h"
//#include "..\ENGINE\Object.h"
#include "PHShellHolder.h"
#include "xrServer_Objects.h"

void CPHDestroyableNotificate::spawn_notificate(CSE_Abstract* so)
{
	CPHDestroyableNotificator* D = nullptr;
	CSE_PHSkeleton* po = smart_cast<CSE_PHSkeleton*>(so);
	unsigned short id = unsigned short(-1);
	if (po)
	{
		id = po->get_source_id( );
	}

	if (id != unsigned short(-1))
	{
		D = smart_cast<CPHDestroyableNotificator*>(Level( ).Objects.net_Find(id));
	}

	if (D)
	{
		D->NotificateDestroy(this);
	}

	po->source_id = BI_NONE;
}
