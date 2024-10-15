// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"

class CHUDCrosshair
{
private:
	f32			cross_length_perc;
	f32			min_radius_perc;
	f32			max_radius_perc;

	// текущий радиус прицела
	f32			radius;
	f32			target_radius;
	f32			radius_speed_perc;

	ref_geom 		hGeomLine;
	ref_shader		hShader;

public:
	u32				cross_color;

	CHUDCrosshair( );
	~CHUDCrosshair( );

	void	OnRender( );
	void	SetDispersion(f32 disp);

	void	Load( );
};
