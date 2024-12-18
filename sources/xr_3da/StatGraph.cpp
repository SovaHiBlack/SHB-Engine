#include "stdafx.h"

#include "StatGraph.h"

CStatGraph::CStatGraph( )
{
	Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);
	OnDeviceCreate( );
	mn = 0.0f;
	mx = 1.0f;
	max_item_count = 1;
	lt.set(0, 0);
	rb.set(200, 200);
	grid.set(1, 1);
	grid_color = 0xFF000000;
	rect_color = 0xFF000000;
	grid_step.set(1.0f, 1.0f);

	AppendSubGraph(stCurve);
}

CStatGraph::~CStatGraph( )
{
	Device.seqRender.Remove(this);
	OnDeviceDestroy( );
	m_Markers.clear( );
}

void CStatGraph::OnDeviceCreate( )
{
	hGeomLine.create(FVF::F_TL0uv, RCache.Vertex.Buffer( ), RCache.Index.Buffer( ));
	hGeomTri.create(FVF::F_TL0uv, RCache.Vertex.Buffer( ), RCache.QuadIB);
}

void CStatGraph::OnDeviceDestroy( )
{
	hGeomLine.destroy( );
	hGeomTri.destroy( );
}

void CStatGraph::RenderBack( )
{
	// draw back
	u32 dwOffset;
	u32 dwCount;
	FVF::TL0uv* pv_start = (FVF::TL0uv*)RCache.Vertex.Lock(4, hGeomTri->vb_stride, dwOffset);
	FVF::TL0uv* pv = pv_start;

	// base rect
	pv->set(lt.x, rb.y, back_color);
	pv++;	// 0
	pv->set(lt.x, lt.y, back_color);
	pv++; 	// 1
	pv->set(rb.x, rb.y, back_color);
	pv++;	// 2
	pv->set(rb.x, lt.y, back_color);
	pv++;	// 3

	// render
	dwCount = u32(pv - pv_start);
	RCache.Vertex.Unlock(dwCount, hGeomTri->vb_stride);
	RCache.set_Geometry(hGeomTri);
	RCache.Render(D3DPT_TRIANGLELIST, dwOffset, 0, dwCount, 0, dwCount / 2);

	//draw rect
	pv_start = (FVF::TL0uv*)RCache.Vertex.Lock(5, hGeomLine->vb_stride, dwOffset);
	pv = pv_start;

	// base rect
	pv->set(lt.x, lt.y, rect_color);
	pv++;	// 0
	pv->set(rb.x - 1, lt.y, rect_color);
	pv++; 	// 1
	pv->set(rb.x - 1, rb.y, rect_color);
	pv++;	// 2
	pv->set(lt.x, rb.y, rect_color);
	pv++;	// 3
	pv->set(lt.x, lt.y, rect_color);
	pv++;	// 0

	// render
	dwCount = u32(pv - pv_start);
	RCache.Vertex.Unlock(dwCount, hGeomLine->vb_stride);
	RCache.set_Geometry(hGeomLine);
	RCache.Render(D3DPT_LINESTRIP, dwOffset, 4);

	// draw grid
	f32 elem_factor = f32(rb.y - lt.y) / f32(mx - mn);
	f32 base_y = f32(rb.y) + (mn * elem_factor);

	s32 PNum_H_LinesUp = s32((base_y - f32(lt.y)) / (grid_step.y * elem_factor));
	s32 PNum_H_LinesDwn = u32((f32(rb.y) - base_y) / (grid_step.y * elem_factor));
	s32 Num_H_LinesUp = (grid.y < PNum_H_LinesUp) ? grid.y : PNum_H_LinesUp;
	s32 Num_H_LinesDwn = (grid.y < PNum_H_LinesUp) ? grid.y : PNum_H_LinesDwn;

	pv_start = (FVF::TL0uv*)RCache.Vertex.Lock(2 + 2 * grid.x + Num_H_LinesUp * 2 + Num_H_LinesDwn * 2, hGeomLine->vb_stride, dwOffset);
	pv = pv_start;

	// base Coordinate Line
	pv->set(lt.x, s32(base_y), base_color);
	pv++; // 0
	pv->set(rb.x, s32(base_y), base_color);
	pv++; // 0

	// grid
//	f32 grid_offs_x = f32(rb.x-lt.x)/f32(grid.x+1);
//	f32 grid_offs_y = f32(rb.y-lt.y)/f32(grid.y+1);
	for (s32 g_x = 1; g_x <= grid.x; g_x++)
	{
		pv->set(s32(lt.x + g_x * grid_step.x * elem_factor), lt.y, grid_color);
		pv++;
		pv->set(s32(lt.x + g_x * grid_step.x * elem_factor), rb.y, grid_color);
		pv++;
	}

	for (s32 g_y = 1; g_y <= Num_H_LinesDwn; g_y++)
	{
		pv->set(lt.x, s32(base_y + g_y * grid_step.y * elem_factor), grid_color);
		pv++;
		pv->set(rb.x, s32(base_y + g_y * grid_step.y * elem_factor), grid_color);
		pv++;
	}

	for (g_y = 1; g_y <= Num_H_LinesUp; g_y++)
	{
		pv->set(lt.x, s32(base_y - g_y * grid_step.y * elem_factor), grid_color);
		pv++;
		pv->set(rb.x, s32(base_y - g_y * grid_step.y * elem_factor), grid_color);
		pv++;
	}

	//	for (s32 g_y = 1; g_y <= grid.y; g_y++)
	//	{
	//		pv->set(lt.x, iFloor(g_y * grid_offs_y + lt.y), grid_color);
	//		pv++;
	//		pv->set(rb.x, iFloor(g_y * grid_offs_y + lt.y), grid_color);
	//		pv++;
	//	}

	dwCount = u32(pv - pv_start);
	RCache.Vertex.Unlock(dwCount, hGeomLine->vb_stride);
	RCache.set_Geometry(hGeomLine);
	RCache.Render(D3DPT_LINELIST, dwOffset, dwCount / 2);
}

void CStatGraph::RenderBars(FVF::TL0uv** ppv, ElementsDeq* pelements)
{
	f32 elem_offs = f32(rb.x - lt.x) / max_item_count;
	f32 elem_factor = f32(rb.y - lt.y) / f32(mx - mn);
	f32 base_y = f32(rb.y) + (mn * elem_factor);

	f32 column_width = elem_offs;
	if (column_width > 1.0f)
	{
		column_width--;
	}

	for (ElementsDeqIt it = pelements->begin( ); it != pelements->end( ); it++)
	{
		f32 X = f32(it - pelements->begin( )) * elem_offs + lt.x;
		f32 Y0 = base_y;
		f32 Y1 = base_y - it->data * elem_factor;

		if (Y1 > Y0)
		{
			(*ppv)->set(X, Y1, it->color);
			(*ppv)++;
			(*ppv)->set(X, Y0, it->color);
			(*ppv)++;
			(*ppv)->set(X + column_width, Y1, it->color);
			(*ppv)++;
			(*ppv)->set(X + column_width, Y0, it->color);
			(*ppv)++;
		}
		else
		{
			(*ppv)->set(X, Y0, it->color);
			(*ppv)++;
			(*ppv)->set(X, Y1, it->color);
			(*ppv)++;
			(*ppv)->set(X + column_width, Y0, it->color);
			(*ppv)++;
			(*ppv)->set(X + column_width, Y1, it->color);
			(*ppv)++;
		}
	}
}

void CStatGraph::RenderLines(FVF::TL0uv** ppv, ElementsDeq* pelements)
{
	f32 elem_offs = f32(rb.x - lt.x) / max_item_count;
	f32 elem_factor = f32(rb.y - lt.y) / f32(mx - mn);
	f32 base_y = f32(rb.y) + (mn * elem_factor);

	for (ElementsDeqIt it = pelements->begin( ) + 1; it != pelements->end( ) && it != pelements->end( ) + 1; it++)
	{
		ElementsDeqIt it_prev = it - 1;
		f32 X0 = f32(it_prev - pelements->begin( )) * elem_offs + lt.x;
		f32 Y0 = base_y - it_prev->data * elem_factor;
		(*ppv)->set(X0, Y0, it->color);
		(*ppv)++;
		f32 X1 = f32(it - pelements->begin( )) * elem_offs + lt.x;
		f32 Y1 = base_y - it->data * elem_factor;
		(*ppv)->set(X1, Y1, it->color);
		(*ppv)++;
	}
}

void CStatGraph::RenderBarLines(FVF::TL0uv** ppv, ElementsDeq* pelements)
{
	f32 elem_offs = f32(rb.x - lt.x) / max_item_count;
	f32 elem_factor = f32(rb.y - lt.y) / f32(mx - mn);
	f32 base_y = f32(rb.y) + (mn * elem_factor);

	for (ElementsDeqIt it = pelements->begin( ) + 1; it != pelements->end( ) && it != pelements->end( ) + 1; it++)
	{
		ElementsDeqIt it_prev = it - 1;
		f32 X0 = f32(it_prev - pelements->begin( )) * elem_offs + lt.x + elem_offs;
		f32 Y0 = base_y - it_prev->data * elem_factor;
		(*ppv)->set(X0, Y0, it->color);
		(*ppv)++;
		f32 X1 = f32(it - pelements->begin( )) * elem_offs + lt.x;
		f32 Y1 = base_y - it->data * elem_factor;
		(*ppv)->set(X1, Y1, it->color);
		(*ppv)++;
		(*ppv)->set(X1, Y1, it->color);
		(*ppv)++;
		X1 += elem_offs;
		(*ppv)->set(X1, Y1, it->color);
		(*ppv)++;
	}
}

void CStatGraph::RenderMarkers(FVF::TL0uv** ppv, MarkersDeq* pmarkers)
{
	f32 elem_offs = f32(rb.x - lt.x) / max_item_count;
	f32 elem_factor = f32(rb.y - lt.y) / f32(mx - mn);
	f32 base_y = f32(rb.y) + (mn * elem_factor);

	for (MarkersDeqIt it = pmarkers->begin( ); it != pmarkers->end( ) && it != pmarkers->end( ) + 1; it++)
	{
		SMarker& CurMarker = *it;
		f32 X0 = 0;
		f32 Y0 = 0;
		f32 X1 = 0;
		f32 Y1 = 0;
		switch (CurMarker.m_eStyle)
		{
			case stVert:
			{
				X0 = CurMarker.m_fPos * elem_offs + lt.x;
				clamp(X0, f32(lt.x), f32(rb.x));
				X1 = X0;
				Y0 = f32(lt.y);
				Y1 = f32(rb.y);
			}
			break;
			case stHor:
			{
				X0 = f32(lt.x);
				X1 = f32(rb.x);
				Y0 = base_y - CurMarker.m_fPos * elem_factor;
				clamp(Y0, f32(lt.y), f32(rb.y));
				Y1 = Y0;
			}
			break;
		}

		(*ppv)->set(X0, Y0, CurMarker.m_dwColor);
		(*ppv)++;
		(*ppv)->set(X1, Y1, CurMarker.m_dwColor);
		(*ppv)++;
	}
}

void CStatGraph::OnRender( )
{
	RCache.OnFrameEnd( );

	RenderBack( );

	u32 TriElem = 0;
	u32 LineElem = 0;
	for (SubGraphVecIt it = subgraphs.begin( ); it != subgraphs.end( ); it++)
	{
		switch (it->style)
		{
			case stBar:
			{
				TriElem += it->elements.size( ) * 4;
			}
			break;
			case stCurve:
			{
				LineElem += it->elements.size( ) * 2;
			}
			break;
			case stBarLine:
			{
				LineElem += it->elements.size( ) * 4;
			}
			break;
			case stPoint:
			{
				//				TriElem += it->elements.size()*4;
			}
			break;
		}
	}

	u32 dwOffsetTri = 0;
	u32 dwOffsetLine = 0;
	u32 dwCount;
	FVF::TL0uv* pv_Tri_start = NULL;
	FVF::TL0uv* pv_Tri;
	FVF::TL0uv* pv_Line_start = NULL;
	FVF::TL0uv* pv_Line;

	if (TriElem)
	{
		pv_Tri_start = (FVF::TL0uv*)RCache.Vertex.Lock(TriElem, hGeomTri->vb_stride, dwOffsetTri);
		pv_Tri = pv_Tri_start;

		pv_Tri = pv_Tri_start;
		for (SubGraphVecIt it = subgraphs.begin( ); it != subgraphs.end( ); it++)
		{
			switch (it->style)
			{
				case stBar:
				{
					RenderBars(&pv_Tri, &(it->elements));
				}
				break;
			}
		}

		dwCount = u32(pv_Tri - pv_Tri_start);
		RCache.Vertex.Unlock(dwCount, hGeomTri->vb_stride);
		RCache.set_Geometry(hGeomTri);
		RCache.Render(D3DPT_TRIANGLELIST, dwOffsetTri, 0, dwCount, 0, dwCount / 2);
	}

	if (LineElem)
	{
		pv_Line_start = (FVF::TL0uv*)RCache.Vertex.Lock(LineElem, hGeomLine->vb_stride, dwOffsetLine);
		pv_Line = pv_Line_start;

		for (SubGraphVecIt it = subgraphs.begin( ); it != subgraphs.end( ); it++)
		{
			switch (it->style)
			{
				case stCurve:
				{
					RenderLines(&pv_Line, &(it->elements));
				}
				break;
				case stBarLine:
				{
					RenderBarLines(&pv_Line, &(it->elements));
				}
				break;
			}
		}

		dwCount = u32(pv_Line - pv_Line_start);
		RCache.Vertex.Unlock(dwCount, hGeomLine->vb_stride);
		RCache.set_Geometry(hGeomLine);
		RCache.Render(D3DPT_LINELIST, dwOffsetLine, dwCount / 2);
	}

	if (!m_Markers.empty( ))
	{
		dwOffsetLine = 0;
		LineElem = m_Markers.size( ) * 2;

		pv_Line_start = (FVF::TL0uv*)RCache.Vertex.Lock(LineElem, hGeomLine->vb_stride, dwOffsetLine);
		pv_Line = pv_Line_start;

		RenderMarkers(&pv_Line, &(m_Markers));

		dwCount = u32(pv_Line - pv_Line_start);
		RCache.Vertex.Unlock(dwCount, hGeomLine->vb_stride);
		RCache.set_Geometry(hGeomLine);
		RCache.Render(D3DPT_LINELIST, dwOffsetLine, dwCount / 2);
	}
}
