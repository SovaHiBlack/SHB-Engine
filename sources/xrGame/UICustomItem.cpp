#include "stdafx.h"
#include "uicustomitem.h"
#include "hudmanager.h"

CUICustomItem::CUICustomItem( )
{
	uAlign = alNone;
	uFlags = 0;
	iVisRect.set(0.0f, 0.0f, 0.0f, 0.0f);
	iOriginalRect.set(0.0f, 0.0f, 0.0f, 0.0f);
	eMirrorMode = tmNone;
	iHeadingPivot.set(0.0f, 0.0f);
}
//--------------------------------------------------------------------

CUICustomItem::~CUICustomItem( )
{ }

void CUICustomItem::Render(FVF::TL*& Pointer, const fVector2& pos, u32 color, f32 x1, f32 y1, f32 x2, f32 y2)
{
	CTexture* T = RCache.get_ActiveTexture(0);
	fVector2		ts;
	ts.set(f32(T->get_Width( )), f32(T->get_Height( )));

	if (!(uFlags & flValidRect))
	{
		SetRect(0.0f, 0.0f, ts.x, ts.y);
	}

	if (!(uFlags & flValidOriginalRect))
	{
		iOriginalRect.set(0.0f, 0.0f, ts.x, ts.y);
		uFlags |= flValidOriginalRect;
	}

	fVector2 LTp;
	fVector2 RBp;
	fVector2 LTt;
	fVector2 RBt;
	//���������� �� ������ � ��������
	UI( )->ClientToScreenScaled(LTp, x1, y1);
	LTp.add(pos);

	UI( )->ClientToScreenScaled(RBp, x2, y2);
	RBp.add(pos);

	//���������� ����������
	LTt.set(iOriginalRect.x1 / ts.x, iOriginalRect.y1 / ts.y);
	RBt.set(iOriginalRect.x2 / ts.x, iOriginalRect.y2 / ts.y);

	// Check mirror mode
	if (tmMirrorHorisontal == eMirrorMode || tmMirrorBoth == eMirrorMode)
	{
		std::swap(LTt.x, RBt.x);
	}

	if (tmMirrorVertical == eMirrorMode || tmMirrorBoth == eMirrorMode)
	{
		std::swap(LTt.y, RBt.y);
	}

	// clip poly
	sPoly2D			S;
	S.resize(4);
	S[0].set(LTp.x - 0.5f, LTp.y - 0.5f, LTt.x, LTt.y);	// LT
	S[1].set(RBp.x - 0.5f, LTp.y - 0.5f, RBt.x, LTt.y);	// RT
	S[2].set(RBp.x - 0.5f, RBp.y - 0.5f, RBt.x, RBt.y);	// RB
	S[3].set(LTp.x - 0.5f, RBp.y - 0.5f, LTt.x, RBt.y);	// LB

	sPoly2D D;
	sPoly2D* R = UI( )->ScreenFrustum( ).ClipPoly(S, D);
	if (R && R->size( ))
	{
		for (u32 k = 0; k < R->size( ) - 2; k++)
		{
			Pointer->set((*R)[0 + 0].pt.x, (*R)[0 + 0].pt.y, color, (*R)[0 + 0].uv.x, (*R)[0 + 0].uv.y); Pointer++;
			Pointer->set((*R)[k + 1].pt.x, (*R)[k + 1].pt.y, color, (*R)[k + 1].uv.x, (*R)[k + 1].uv.y); Pointer++;
			Pointer->set((*R)[k + 2].pt.x, (*R)[k + 2].pt.y, color, (*R)[k + 2].uv.x, (*R)[k + 2].uv.y); Pointer++;
		}
	}
}
//--------------------------------------------------------------------
void CUICustomItem::Render(FVF::TL*& Pointer, const fVector2& pos, u32 color)
{
	Render(Pointer, pos, color, iVisRect.x1, iVisRect.y1, iVisRect.x2, iVisRect.y2);
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const fVector2& pos_ns, u32 color, f32 angle)
{
	//.	angle = -0.3f;
	CTexture* T = RCache.get_ActiveTexture(0);
	fVector2		ts;
	fVector2		hp;
	ts.set(f32(T->get_Width( )), f32(T->get_Height( )));
	hp.set(0.5f / ts.x, 0.5f / ts.y);

	if (!(uFlags & flValidRect))	SetRect(0, 0, ts.x, ts.y);

	if (!(uFlags & flValidOriginalRect))
	{
		iOriginalRect.set(0, 0, ts.x, ts.y);
		uFlags |= flValidOriginalRect;
	}

	fVector2							pivot;
	fVector2							offset;
	fVector2							SZ;
	SZ.set(iVisRect.rb);

	//	UI()->ClientToScreenScaled			(SZ, iVisRect.x2, iVisRect.y2);

	f32 cosA = _cos(angle);
	f32 sinA = _sin(angle);

	// Rotation
	if (!(uFlags & flValidHeadingPivot))	pivot.set(iVisRect.x2 / 2.f, iVisRect.y2 / 2.f);
	else								pivot.set(iHeadingPivot.x, iHeadingPivot.y);

	//.	UI()->ClientToScreenScaled			(pivot, pivot.x, pivot.y);
	pivot.set(pivot);
	offset.set(pos_ns);

	fVector2							LTt;
	fVector2							RBt;
	LTt.set(iOriginalRect.x1 / ts.x + hp.x, iOriginalRect.y1 / ts.y + hp.y);
	RBt.set(iOriginalRect.x2 / ts.x + hp.x, iOriginalRect.y2 / ts.y + hp.y);
	// Check mirror mode
	if (tmMirrorHorisontal == eMirrorMode || tmMirrorBoth == eMirrorMode)	std::swap(LTt.x, RBt.x);
	if (tmMirrorVertical == eMirrorMode || tmMirrorBoth == eMirrorMode)		std::swap(LTt.y, RBt.y);

	f32 kx = (UI( )->is_16_9_mode( )) ? 0.8333f : 1.0f;
	// clip poly
	sPoly2D			S; S.resize(4);
	// LT
	S[0].set(0.f, 0.f, LTt.x, LTt.y);
	S[0].rotate_pt(pivot, cosA, sinA, kx);
	S[0].pt.add(offset);

	// RT
	S[1].set(SZ.x, 0.f, RBt.x, LTt.y);
	S[1].rotate_pt(pivot, cosA, sinA, kx);
	S[1].pt.add(offset);
	// RB
	S[2].set(SZ.x, SZ.y, RBt.x, RBt.y);
	S[2].rotate_pt(pivot, cosA, sinA, kx);
	S[2].pt.add(offset);
	// LB
	S[3].set(0.f, SZ.y, LTt.x, RBt.y);
	S[3].rotate_pt(pivot, cosA, sinA, kx);
	S[3].pt.add(offset);

	for (int i = 0; i < 4; ++i)
		UI( )->ClientToScreenScaled(S[i].pt);

	sPoly2D D;
	sPoly2D* R = UI( )->ScreenFrustum( ).ClipPoly(S, D);
	if (R && R->size( ))
		for (u32 k = 0; k < R->size( ); k++, Pointer++)
		{
			//.			fVector2 _pt;
			//.			UI()->ClientToScreenScaled			(_pt, (*R)[k].pt.x, (*R)[k].pt.y);
			//.			Pointer->set						(_pt.x, _pt.y,	color, (*R)[k].uv.x, (*R)[k].uv.y); 
			Pointer->set((*R)[k].pt.x, (*R)[k].pt.y, color, (*R)[k].uv.x, (*R)[k].uv.y);
		}
}

fRect CUICustomItem::GetOriginalRectScaled( )
{
	fRect rect = iOriginalRect;

	rect.x2 = rect.x1 + rect.width( );
	rect.y2 = rect.y1 + rect.height( );

	return rect;
}

fRect CUICustomItem::GetOriginalRect( ) const
{
	return iOriginalRect;
}

void CUICustomItem::SetOriginalRect(f32 x, f32 y, f32 width, f32 height)
{
	iOriginalRect.set(x, y, x + width, y + height);
	uFlags |= flValidOriginalRect;
}
