#include "stdafx.h"

#include "UIMapWndActions.h"//
#include "UIMapWndActionsSpace.h"//
#include "UIMap.h"//
#include "UIMapWnd.h"//

typedef CActionBase<CUIMapWnd>				WORLD_OPERATOR;
static const f32	map_resize_speed = 350.0f;	// y.e./sec
static const f32	map_zoom_time = 0.5f;		// sec
static const f32	min_move_time = 0.25f;	// sec
//actions
class CSomeMapAction : public WORLD_OPERATOR
{
private:
	typedef WORLD_OPERATOR	inherited;

public:
	CSomeMapAction(pcstr action_name) : inherited((CUIMapWnd*)NULL, action_name)
	{ }
	virtual void	initialize( )
	{
		inherited::initialize( );
	}
	virtual void	execute( )
	{
		inherited::execute( );
	}
	virtual void	finalize( )
	{
		inherited::finalize( );
	}
};

class CMapActionZoomControl : public CSomeMapAction
{
private:
	typedef CSomeMapAction	inherited;

protected:
	f32			m_endMovingTime;
	f32			m_targetZoom;
	fRect			m_desiredMapRect;
	void			init_internal( );
	void			update_target_state( );

public:
	CMapActionZoomControl(pcstr action_name) : inherited(action_name)
	{ }
	virtual void	execute( );
	virtual void	initialize( );
	virtual void	finalize( );
};

class CMapActionResize : public CMapActionZoomControl
{
private:
	typedef CMapActionZoomControl	inherited;

public:
	CMapActionResize(pcstr action_name) : inherited(action_name)
	{ }
	virtual void	initialize( );
	virtual void	finalize( );
};

class CMapActionMinimize : public CMapActionZoomControl
{
private:
	typedef CMapActionZoomControl	inherited;

public:
	CMapActionMinimize(pcstr action_name) : inherited(action_name)
	{ }
	virtual void	initialize( );
	virtual void	finalize( );
};

class CMapActionIdle : public CSomeMapAction
{
private:
	typedef CSomeMapAction	inherited;

public:
	CMapActionIdle(pcstr action_name) : inherited(action_name)
	{ }
	virtual void	initialize( );
	virtual void	execute( );
	virtual void	finalize( );
};

//evaluators
class CSomeMapEvaluator : public CPropertyEvaluator<CUIMapWnd>
{
private:
	typedef CPropertyEvaluator<CUIMapWnd>	inherited;

public:
					CSomeMapEvaluator(pcstr evaluator_name) : inherited((CUIMapWnd*)NULL, evaluator_name)
	{ }
	virtual			~CSomeMapEvaluator( )
	{ }
};

class CEvaluatorTargetMapShown : public CSomeMapEvaluator
{
private:
	typedef CSomeMapEvaluator	inherited;

public:
	CEvaluatorTargetMapShown(pcstr evaluator_name = 0) : inherited(evaluator_name)
	{ }
	virtual bool	evaluate( );
};

class CEvaluatorMapMinimized : public CSomeMapEvaluator
{
private:
	typedef CSomeMapEvaluator	inherited;

public:
	CEvaluatorMapMinimized(pcstr evaluator_name = 0) : inherited(evaluator_name)
	{ }
	virtual bool	evaluate( );
};


class CEvaluatorMapResized : public CSomeMapEvaluator
{
private:
	typedef CSomeMapEvaluator	inherited;

public:
	CEvaluatorMapResized(pcstr evaluator_name = 0) : inherited(evaluator_name)
	{ }
	virtual bool	evaluate( );
};

class CEvaluatorMapConst : public CSomeMapEvaluator
{
private:
	typedef CSomeMapEvaluator	inherited;
	bool	ret_value;

public:
	CEvaluatorMapConst(bool val = false, pcstr evaluator_name = 0) : inherited(evaluator_name)
	{
		ret_value = val;
	}
	virtual bool	evaluate( )
	{
		return ret_value;
	}
};

using namespace UIMapWndActionsSpace;

CMapActionPlanner::CMapActionPlanner( )
{ }
CMapActionPlanner::~CMapActionPlanner( )
{ }
pcstr CMapActionPlanner::object_name( ) const
{
	return						("");
}
void CMapActionPlanner::setup(CUIMapWnd* object)
{

#ifdef LOG_ACTION
	set_use_log(false);
#endif

	inherited::setup(object);
	clear( );
	m_storage.set_property(1, false);
	m_storage.set_property(2, false);
	m_storage.set_property(3, false);

	add_evaluator(ePropTargetMapShown, xr_new<CEvaluatorTargetMapShown>("ePropTargetMapShown"));
	add_evaluator(ePropMapMinimized, xr_new<CEvaluatorMapMinimized>("ePropMapMinimized"));
	add_evaluator(ePropMapResized, xr_new<CEvaluatorMapResized>("ePropMapResized"));
	add_evaluator(ePropMapIdle, xr_new<CEvaluatorMapConst>(false, "ePropMapIdle"));

	// final world state
	_world_operator* action = xr_new<CMapActionIdle>("eOperatorMapIdle");
	add_condition(action, ePropMapResized, true);
	add_condition(action, ePropTargetMapShown, true);
	add_condition(action, ePropMapIdle, false);
	add_effect(action, ePropMapIdle, true);
	add_operator(eOperatorMapIdle, action);

	action = xr_new<CMapActionResize>("eOperatorMapResize");
	add_condition(action, ePropTargetMapShown, true);
	add_condition(action, ePropMapResized, false);
	add_effect(action, ePropMapResized, true);
	add_operator(eOperatorMapResize, action);

	action = xr_new<CMapActionMinimize>("eOperatorMapMinimize");
	add_condition(action, ePropTargetMapShown, false);
	add_effect(action, ePropTargetMapShown, true);
	add_operator(eOperatorMapMinimize, action);

	CWorldState target_state;
	target_state.add_condition(CWorldProperty(ePropMapIdle, true));
	set_target_state(target_state);
}

void CMapActionZoomControl::initialize( )
{
	inherited::initialize( );
	init_internal( );
}

void CMapActionZoomControl::init_internal( )
{
	f32 dist = m_object->GlobalMap( )->CalcOpenRect(m_object->m_tgtCenter, m_desiredMapRect, m_targetZoom);
	bool bMove = !fis_zero(dist, EPSILON_3);
	bool bZoom = !fsimilar(m_targetZoom, m_object->GlobalMap( )->GetCurrentZoom( ), EPSILON_3);
	m_endMovingTime = Device.fTimeGlobal;
	if (bZoom && bMove)
	{
		m_endMovingTime += _max(map_zoom_time, dist / map_resize_speed);
	}
	else if (bZoom)
	{
		m_endMovingTime += map_zoom_time;
	}
	else if (bMove)
	{
		m_endMovingTime += _max(dist / map_resize_speed, min_move_time);
	}
}

void CMapActionZoomControl::update_target_state( )
{
	f32 cur_map_zoom = m_object->GetZoom( );
	if (!fsimilar(cur_map_zoom, m_targetZoom))
	{//re-init
		m_targetZoom = cur_map_zoom;
		init_internal( );
	}
}

void CMapActionZoomControl::finalize( )
{
	inherited::finalize( );
	m_object->UpdateScroll( );
}

void CMapActionZoomControl::execute( )
{
	update_target_state( );
	inherited::execute( );
	CUIGlobalMap* gm = m_object->GlobalMap( );
	f32 gt = Device.fTimeGlobal;
	f32 time_to = m_endMovingTime - gt;
	f32 dt = _min(Device.fTimeDelta, time_to);

	if (m_endMovingTime > Device.fTimeGlobal)
	{
		fRect current_rect = gm->GetWndRect( );
		current_rect.x1 += ((m_desiredMapRect.x1 - current_rect.x1) / time_to) * dt;
		current_rect.y1 += ((m_desiredMapRect.y1 - current_rect.y1) / time_to) * dt;
		current_rect.x2 += ((m_desiredMapRect.x2 - current_rect.x2) / time_to) * dt;
		current_rect.y2 += ((m_desiredMapRect.y2 - current_rect.y2) / time_to) * dt;
		gm->SetWndRect(current_rect);
	}
	else
	{
		gm->SetWndRect(m_desiredMapRect);
		m_storage->set_property(3, true);
	}

	gm->Update( );
	m_object->UpdateScroll( );
}

void CMapActionResize::initialize( )
{
	m_targetZoom = m_object->GetZoom( );
	inherited::initialize( );
}

void CMapActionResize::finalize( )
{
	inherited::finalize( );
}

void CMapActionMinimize::initialize( )
{
	m_targetZoom = m_object->GlobalMap( )->GetMinZoom( );
	inherited::initialize( );
	m_endMovingTime = Device.fTimeGlobal + map_zoom_time;
}

void CMapActionMinimize::finalize( )
{
	inherited::finalize( );
}

void CMapActionIdle::initialize( )
{
	inherited::initialize( );
	m_object->UpdateScroll( );
}

void CMapActionIdle::finalize( )
{
	inherited::finalize( );
}

void CMapActionIdle::execute( )
{
	inherited::execute( );
	m_storage->set_property(1, true);
	m_storage->set_property(2, false);
	m_storage->set_property(3, false);
}

bool CEvaluatorTargetMapShown::evaluate( )
{
	if (m_storage->property(1))
	{
		return true;
	}

	if (m_storage->property(2))
	{
		return true;
	}

	fVector2 pt = m_object->m_tgtCenter;
	pt.mul(m_object->GlobalMap( )->GetCurrentZoom( ));
	fVector2 _p;
	m_object->GlobalMap( )->GetAbsolutePos(_p);
	pt.add(_p);
	fRect	rect = m_object->ActiveMapRect( );
	rect.grow(rect.width( ), rect.height( ));
	if (rect.in(pt))
	{
		m_storage->set_property(2, true);
		return true;
	}

	return false;
}

bool CEvaluatorMapMinimized::evaluate( )
{
	if (m_storage->property(1))
	{
		return true;
	}

	bool res = !!fsimilar(m_object->GlobalMap( )->GetCurrentZoom( ), m_object->GlobalMap( )->GetMinZoom( ), EPSILON_3);
	return res;
}

bool CEvaluatorMapResized::evaluate( )
{
	if (m_storage->property(1))
	{
		return true;
	}

	return m_storage->property(3);
}
