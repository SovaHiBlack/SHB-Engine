////////////////////////////////////////////////////////////////////////////
//	Module 		: object_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Object manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T>
class CObjectManager {
public:
	typedef xr_vector<T*>	OBJECTS;
protected:
	OBJECTS					m_objects;
	T						*m_selected;

public:
							CObjectManager	();
	virtual					~CObjectManager	();
	virtual void			Load			(pcstr section);
	virtual void			reinit			();
	virtual void			reload			(pcstr section);
	virtual void			update			();
			bool			add				(T *object);
	virtual bool			is_useful		(T *object) const;
	virtual	f32			do_evaluate		(T *object) const;
	virtual	void			reset			();

public:
	IC		T				*selected		() const;
	IC		const OBJECTS	&objects		() const;
};

#include "object_manager_inline.h"