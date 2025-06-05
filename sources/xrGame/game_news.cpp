///////////////////////////////////////////////////////////////
// game_news.cpp
// реестр новостей: новости симул€ции + сюжетные
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_news.h"
#include "object_broker.h"

#include "AISpace.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "game_graph.h"

#include "date_time.h"
#include "xrServer_Objects_ALife_Monsters.h"

#include "SpecificCharacter.h"

SGameNewsData::SGameNewsData( )
{
	m_type = eNews;
	tex_rect.set(0.0f, 0.0f, 0.0f, 0.0f);
	show_time = DEFAULT_NEWS_SHOW_TIME;
}

void SGameNewsData::save(IWriter& stream)
{
	save_data(m_type, stream);
	save_data(news_text, stream);
	save_data(receive_time, stream);
	save_data(texture_name, stream);
	save_data(tex_rect, stream);
}

void SGameNewsData::load(IReader& stream)
{
	load_data(m_type, stream);
	load_data(news_text, stream);
	load_data(receive_time, stream);
	load_data(texture_name, stream);
	load_data(tex_rect, stream);
}

pcstr SGameNewsData::SingleLineText( )
{
	if (xr_strlen(full_news_text.c_str( )))
	{
		return full_news_text.c_str( );
	}

	string128 time = "";

	// Calc current time
	u32 years;
	u32 months;
	u32 days;
	u32 hours;
	u32 minutes;
	u32 seconds;
	u32 milliseconds;
	split_time(receive_time, years, months, days, hours, minutes, seconds, milliseconds);
	sprintf_s(time, "%02i:%02i ", hours, minutes);

	full_news_text = time;
	full_news_text += news_text.c_str( );

	return full_news_text.c_str( );
}
