#pragma once

class ENGINE_API CStatGraph : public pureRender
{
public:
	enum EStyle
	{
		stBar,
		stCurve,
		stBarLine,
		stPoint,
		stVert,
		stHor
	};

protected:
	struct SElement
	{
		u32 			color;
		f32				data;
		SElement(f32 d, u32 clr)
		{
			color = clr;
			data = d;
		}
	};
	DEFINE_DEQUE(SElement, ElementsDeq, ElementsDeqIt);

	struct SSubGraph
	{
		EStyle			style;
		ElementsDeq		elements;
		SSubGraph(EStyle s)
		{
			style = s;
		}
		void SetStyle(EStyle s)
		{
			style = s;
		}
	};
	DEFINE_VECTOR(SSubGraph, SubGraphVec, SubGraphVecIt);
	SubGraphVec		subgraphs;

	f32				mn;
	f32				mx;
	u32				max_item_count;
	iVector2		lt;
	iVector2		rb;
	iVector2		grid;
	fVector2		grid_step;
	u32				grid_color;
	u32				base_color;
	u32				rect_color;
	u32				back_color;
	ref_geom		hGeomTri;
	ref_geom		hGeomLine;

	struct SMarker
	{
		EStyle			m_eStyle;
		f32				m_fPos;
		u32				m_dwColor;
	};
	DEFINE_DEQUE(SMarker, MarkersDeq, MarkersDeqIt);
	MarkersDeq		m_Markers;

protected:
	virtual void		RenderBack( );

	virtual void		RenderBars(FVF::TL0uv** ppv, ElementsDeq* pelements);
	virtual void		RenderLines(FVF::TL0uv** ppv, ElementsDeq* pelements);
	virtual void		RenderBarLines(FVF::TL0uv** ppv, ElementsDeq* pelements);
	virtual void		RenderMarkers(FVF::TL0uv** ppv, MarkersDeq* pmarkers);

public:
						CStatGraph( );
						~CStatGraph( );
	virtual void		OnRender( );
	void				OnDeviceCreate( );
	void				OnDeviceDestroy( );

	IC void				SetStyle(EStyle s, u32 SubGraphID = 0)
	{
		if (SubGraphID >= subgraphs.size( ))
		{
			return;
		}

		SubGraphVecIt it = subgraphs.begin( ) + SubGraphID;
		it->SetStyle(s);
	}

	IC void				SetRect(s32 l, s32 t, s32 w, s32 h, u32 rect_clr, u32 back_clr)
	{
		lt.set(l, t);
		rb.set(l + w, t + h);
		rect_color = rect_clr;
		back_color = back_clr;
	}
	IC void				SetGrid(s32 w_div, f32 w_step, s32 h_div, f32 h_step, u32 grid_clr, u32 base_clr)
	{
		grid.set(w_div, h_div);
		grid_step.set(w_step, h_step);
		grid_color = grid_clr;
		base_color = base_clr;
	}
	IC void				SetMinMax(f32 _mn, f32 _mx, u32 item_count)
	{
		mn = _mn;
		mx = _mx;
		max_item_count = item_count;
		for (SubGraphVecIt it = subgraphs.begin( ); it != subgraphs.end( ); it++)
		{
			while (it->elements.size( ) > max_item_count)
			{
				it->elements.pop_front( );
			}
		}
	}
	IC void				AppendItem(f32 d, u32 clr, u32 SubGraphID = 0)
	{
		if (SubGraphID >= subgraphs.size( ))
		{
			return;
		}

		clamp(d, mn, mx);

		SubGraphVecIt it = subgraphs.begin( ) + SubGraphID;
		it->elements.push_back(SElement(d, clr));
		while (it->elements.size( ) > max_item_count)
		{
			it->elements.pop_front( );
		}
	}
	IC u32				AppendSubGraph(EStyle S)
	{
		subgraphs.push_back(SSubGraph(S));
		return (subgraphs.size( ) - 1);
	}

	IC void				AddMarker(EStyle Style, f32 pos, u32 Color)
	{
		SMarker NewMarker;
		NewMarker.m_dwColor = Color;
		NewMarker.m_eStyle = Style;
		NewMarker.m_fPos = pos;

		m_Markers.push_back(NewMarker);
	}

	IC const SMarker&	Marker(u32 ID)
	{
		VERIFY(ID < m_Markers.size( ));
		return m_Markers[ID];
	}

	IC void				UpdateMarkerPos(u32 ID, f32 NewPos)
	{
		if (ID >= m_Markers.size( ))
		{
			return;
		}

		SMarker& pMarker = m_Markers[ID];
		pMarker.m_fPos = NewPos;
	}
	IC void				ClearMarkers( )
	{
		m_Markers.clear( );
	}

	IC void				RemoveMarker(u32 ID)
	{
		if (ID >= m_Markers.size( ))
		{
			return;
		}

		m_Markers.erase(m_Markers.begin( ) + ID);
	}
};
