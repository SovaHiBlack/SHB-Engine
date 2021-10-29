#pragma once

#include "TelekineticObject.h"
#include "../../PHObject.h"

class CTelekinesis : public CPHUpdateObject
{
protected:
	using TelekineticObjectsVec									= xr_vector<CTelekineticObject*>;
	using TelekineticObjectsVec_it								= TelekineticObjectsVec::iterator;
	TelekineticObjectsVec										objects;
	xr_vector<CObject*>											m_nearest;
	bool														active;

public:
									CTelekinesis				( );
	virtual							~CTelekinesis				( );

	// allocates relevant TelekineticObject
	// активировать объект
	virtual CTelekineticObject*		activate					(CPHShellHolder* obj, float strength, float height, unsigned int max_time_keep, bool rot = true);

	// деактивировать все объекты
	void							deactivate					( );

	// clear objects (does not call release, but call switch to TS_None)
	void							clear_deactivate			( );
	// clear
	virtual void					clear						( );
	virtual void					clear_notrelevant			( );
	// деактивировать объект
	void							deactivate					(CPHShellHolder* obj);
	void							remove_object				(TelekineticObjectsVec_it it);
	void							remove_object				(CPHShellHolder* obj);
	// бросить все объекты в позицию 'target'
	void							fire_all					(const Fvector3& target);

	// бросить объект 'obj' в позицию 'target' с учетом коэф силы
	void							fire						(CPHShellHolder* obj, const Fvector3& target, float power);

	// бросить объект 'obj' в позицию 'target' с учетом коэф силы
	void							fire_t						(CPHShellHolder* obj, const Fvector3& target, float time);

	// вернуть активность телекинеза
	bool							is_active					( )
	{
		return active;
	}

	// вернуть активность объекта
	bool							is_active_object			(CPHShellHolder* obj);

	// вернуть количество контролируемых объектов (в состо€нии TS_Raise & TS_Keep)
	unsigned int					get_objects_count			( );

	// вернуть количество контролируемых объектов (всех)
	unsigned int					get_objects_total_count		( )
	{
		return objects.size( );
	}

	// вернуть объект по индексу в массиве
	// a	copy of the object!
	CTelekineticObject				get_object_by_index			(unsigned int index)
	{
		VERIFY(objects.size( ) > index);
		return *objects[index];
	}

	// обновить состон€ие на shedule_Update
	void							schedule_update				( );

	// объект был удален - удалить все св€зи на объект
	void							remove_links				(CObject* O);

protected:
	virtual CTelekineticObject*		alloc_tele_object			( )
	{
		return xr_new<CTelekineticObject>( );
	}

private:
	// обновление на шагах физики
	virtual void					PhDataUpdate				(dReal step);
	virtual void					PhTune						(dReal step);
};
