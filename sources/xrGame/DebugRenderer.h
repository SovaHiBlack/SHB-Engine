// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: DebugRenderer.h
// Author		: Anahoret
// Description	: debug renderer
// ===================================== SovaHiBlack© - 2022 ======================================

#pragma once
#ifndef GAME_DEBUGRENDERER_H_INCLUDED
#define GAME_DEBUGRENDERER_H_INCLUDED

#ifdef DEBUG
class CDebugRenderer
{
private:
	enum
	{
		line_vertex_limit = 32767
	};

private:
	xr_vector<FVF::L>					m_line_vertices;
	xr_vector<u16>						m_line_indices;

private:
	void	add_lines(const fVector3* vertices, const u16* pairs, const s32& pair_count, const u32& color);

public:
	CDebugRenderer( );
	IC		void	render( );

public:
	IC		void	draw_line(const fMatrix4x4& matrix, const fVector3& vertex0, const fVector3& vertex1, const u32& color);
	IC		void	draw_aabb(const fVector3& center, const f32& half_radius_x, const f32& half_radius_y, const f32& half_radius_z, const u32& color);
	void	draw_obb(const fMatrix4x4& matrix, const fVector3& half_size, const u32& color);
	void	draw_ellipse(const fMatrix4x4& matrix, const u32& color);
};

#include "DebugRenderer_inline.h"
#endif // def DEBUG

#endif // ndef GAME_DEBUGRENDERER_H_INCLUDED