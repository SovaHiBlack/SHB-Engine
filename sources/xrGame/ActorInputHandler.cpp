#include "stdafx.h"
#include "ActorInputHandler.h"
#include "Actor.h"
#include "level.h"

void CActorInputHandler::reinit( )
{
	m_actor = 0;
}

void CActorInputHandler::install( )
{
	m_actor = smart_cast<CActor*> (Level( ).CurrentEntity( ));
	VERIFY(m_actor);

	m_actor->set_input_external_handler(this);
}

void CActorInputHandler::install(CActor* actor)
{
	m_actor = actor;
	VERIFY(m_actor);
	actor->set_input_external_handler(this);
}

void CActorInputHandler::release( )
{
	VERIFY(m_actor);

	m_actor->set_input_external_handler(0);
	m_actor = 0;
}
