#pragma once

enum EUIItemAlign
{
	alNone = 0x0000,
	alLeft = 0x0001,
	alRight = 0x0002,
	alTop = 0x0004,
	alBottom = 0x0008,
	alCenter = 0x0010
};

enum EUIMirroring
{
	tmNone,
	tmMirrorHorisontal,
	tmMirrorVertical,
	tmMirrorBoth
};

class CUICustomItem
{
protected:
	enum
	{
		flValidRect = 0x0001,
		flValidOriginalRect = 0x0002,
		flValidHeadingPivot = 0x0004
	};

	//прямоугольник(в пикселях) 
	//геом. регион  на который натягикается текстура с текстурными координатами iOriginalRect
	fRect			iVisRect;

	//фрейм текстуры в пикселях отн. 0/0
	fRect			iOriginalRect;

	// точка, относительно которой применяем поворот
	fVector2		iHeadingPivot;

	u32				uFlags;
	u32				uAlign;
	EUIMirroring	eMirrorMode;

public:
	CUICustomItem( );
	virtual			~CUICustomItem( );
	IC void			SetRect(f32 x1, f32 y1, f32 x2, f32 y2)
	{
		iVisRect.set(x1, y1, x2, y2);
		uFlags |= flValidRect;
	}
	IC void			SetRect(const fRect& r)
	{
		iVisRect.set(r);
		uFlags |= flValidRect;
	}
	void			SetOriginalRect(f32 x, f32 y, f32 width, f32 height);

	IC fRect		GetRect( )
	{
		return iVisRect;
	}
	fRect		GetOriginalRect( ) const;
	fRect		GetOriginalRectScaled( );

	void			SetHeadingPivot(const fVector2& p)
	{
		iHeadingPivot = p;
		uFlags |= flValidHeadingPivot;
	}
	fVector2		GetHeadingPivot( )
	{
		return iHeadingPivot;
	}

	void			Render(FVF::TL*& Pointer, const fVector2& pos, u32 color, f32 x1, f32 y1, f32 x2, f32 y2);
	void			Render(FVF::TL*& Pointer, const fVector2& pos, u32 color);
	void			Render(FVF::TL*& Pointer, const fVector2& pos, u32 color, f32 angle);

	IC void			SetAlign(u32 align)
	{
		uAlign = align;
	}
	IC u32			GetAlign( )
	{
		return uAlign;
	}

	IC void			SetMirrorMode(EUIMirroring m)
	{
		eMirrorMode = m;
	}
	IC EUIMirroring GetMirrorMode( )
	{
		return eMirrorMode;
	}
};
