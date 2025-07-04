#include "stdafx.h"

#include "MMSound.h"//
#include "UIXml.h"

CMMSound::CMMSound( )
{ }

CMMSound::~CMMSound( )
{
	all_Stop( );
}

void CMMSound::Init(CUIXml& xml_doc, pcstr path)
{
	string256 _path;
	m_bRandom = xml_doc.ReadAttribInt(path, 0, "random") ? true : false;

	s32 nodes_num = xml_doc.GetNodesNum(path, 0, "menu_music");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path, 0);
	xml_doc.SetLocalRoot(tab_node);
	for (s32 i = 0; i < nodes_num; ++i)
	{
		m_play_list.push_back(xml_doc.Read("menu_music", i, ""));
	}

	xml_doc.SetLocalRoot(xml_doc.GetRoot( ));

	strconcat(sizeof(_path), _path, path, ":whell_sound");
	if (check_file(xml_doc.Read(_path, 0, "")))
	{
		m_whell.create(xml_doc.Read(_path, 0, ""), st_Effect, sg_SourceType);
	}

	strconcat(sizeof(_path), _path, path, ":whell_click");
	if (check_file(xml_doc.Read(_path, 0, "")))
	{
		m_whell_click.create(xml_doc.Read(_path, 0, ""), st_Effect, sg_SourceType);
	}
}

bool CMMSound::check_file(pcstr fname)
{
	string_path _path;
	strconcat(sizeof(_path), _path, fname, ".ogg");
	return FS.exist("$game_sounds$", _path) ? true : false;
}

void CMMSound::whell_Play( )
{
	if (m_whell._handle( ) && !m_whell._feedback( ))
	{
		m_whell.play(NULL, sm_Looped | sm_2D);
	}
}

void CMMSound::whell_Stop( )
{
	if (m_whell._feedback( ))
	{
		m_whell.stop( );
	}
}

void CMMSound::whell_Click( )
{
	if (m_whell_click._handle( ))
	{
		m_whell_click.play(NULL, sm_2D);
	}
}

void CMMSound::whell_UpdateMoving(f32 frequency)
{
	m_whell.set_frequency(frequency);
}

void CMMSound::music_Play( )
{
	if (m_play_list.empty( ))
	{
		return;
	}

	s32 i = Random.randI(m_play_list.size( ));

	string_path _path;
	string_path _path2;
	strconcat(sizeof(_path), _path, m_play_list[i].c_str( ), "_l.ogg");
	strconcat(sizeof(_path2), _path2, m_play_list[i].c_str( ), "_r.ogg");
	VERIFY(FS.exist("$game_sounds$", _path));
	VERIFY(FS.exist("$game_sounds$", _path2));

	m_music_l.create(_path, st_Music, sg_SourceType);
	m_music_r.create(_path2, st_Music, sg_SourceType);

	m_music_l.play_at_pos(NULL, fVector3( ).set(-0.5f, 0.0f, 0.3f), sm_2D);
	m_music_r.play_at_pos(NULL, fVector3( ).set(+0.5f, 0.0f, 0.3f), sm_2D);
}

void CMMSound::music_Update( )
{
	if (Device.Paused( ))
	{
		return;
	}
	if (0 == m_music_l._feedback( ) || 0 == m_music_r._feedback( ))
	{
		music_Play( );
	}
}

void CMMSound::music_Stop( )
{
	m_music_l.stop( );
	m_music_r.stop( );
}

void CMMSound::all_Stop( )
{
	music_Stop( );
	m_whell.stop( );
	m_whell_click.stop( );
}
