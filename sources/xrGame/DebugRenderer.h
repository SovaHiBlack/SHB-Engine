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
		line_vertex_limit				= 32767
	};

private:
	xr_vector<FVF::L>					m_line_vertices;
	xr_vector<u16>						m_line_indices;

private:
			void	add_lines			(const Fvector* vertices, const u16* pairs, const int& pair_count, const u32& color);

public:
					CDebugRenderer		();
	IC		void	render				();

public:
	IC		void	draw_line			(const Fmatrix& matrix, const Fvector& vertex0, const Fvector& vertex1, const u32& color);
	IC		void	draw_aabb			(const Fvector& center, const f32& half_radius_x, const f32& half_radius_y, const f32& half_radius_z, const u32& color);
			void	draw_obb			(const Fmatrix& matrix, const Fvector& half_size, const u32& color);
			void	draw_ellipse		(const Fmatrix& matrix, const u32& color);
};

#include "DebugRenderer_inline.h"

#endif // def DEBUG

#endif // ndef GAME_DEBUGRENDERER_H_INCLUDED