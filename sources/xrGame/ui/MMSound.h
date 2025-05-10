#pragma once

class CUIXml;

class CMMSound
{
public:
					CMMSound				( );
					~CMMSound				( );
	void			Init					(CUIXml& xml_doc, pcstr path);
	void			whell_Play				( );
	void			whell_Stop				( );
	void			whell_Click				( );
	void			whell_UpdateMoving		(f32 frequency);

	void			music_Play				( );
	void			music_Stop				( );
	void			music_Update			( );

	void			all_Stop				( );

protected:
	IC bool			check_file				(pcstr fname);
	ref_sound								m_music_l;
	ref_sound								m_music_r;
	ref_sound								m_whell;
	ref_sound								m_whell_click;
	bool									m_bRandom;
	xr_vector<xr_string>					m_play_list;
};
