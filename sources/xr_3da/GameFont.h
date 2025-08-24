#pragma once

#include "MbHelpers.h"

class ENGINE_API CGameFont : public pureRender
{
public:
	enum EAligment
	{
		alLeft = 0,
		alRight,
		alCenter
	};

private:
	struct String
	{
		string512	string;
		f32			x;
		f32			y;
		f32			height;
		u32			c;
		EAligment	align;
	};

protected:
	fVector2				vHalfPixel;
	iVector2				vTS;

	EAligment				eCurrentAlignment;
	u32						dwCurrentColor;
	f32						fCurrentHeight;
	f32						fCurrentX;
	f32						fCurrentY;
	fVector2				vInterval;

	fVector3*				TCMap;
	f32						fHeight;
	f32						fXStep;
	f32						fYStep;
	f32						fTCHeight;
	xr_vector<String>		strings;

	ref_shader				pShader;
	ref_geom				pGeom;

	u32						nNumChars;

	u32						uFlags;

public:
	enum
	{
		fsGradient = (1 << 0),
		fsDeviceIndependent = (1 << 1),
		fsValid = (1 << 2),
		fsMultibyte = (1 << 3),
		fsForceDWORD = u32(-1)
	};

protected:
	IC const fVector3&		GetCharTC(u16 c)
	{
		return TCMap[c];
	}

public:
							CGameFont(pcstr section, u32 flags = 0);
							CGameFont(pcstr shader, pcstr texture, u32 flags = 0);
							~CGameFont( );

	void					Initialize(pcstr shader, pcstr texture);

	IC void					SetColor(u32 C)
	{
		dwCurrentColor = C;
	}

	IC void					SetHeightI(f32 S);
	IC void					SetHeight(f32 S);

	IC f32					GetHeight( )
	{
		return fCurrentHeight;
	}
	IC void					SetInterval(f32 x, f32 y)
	{
		vInterval.set(x, y);
	}
	IC void					SetInterval(const fVector2& v)
	{
		vInterval.set(v);
	}
	IC void					SetAligment(EAligment aligment)
	{
		eCurrentAlignment = aligment;
	}

	f32						SizeOf_(pcstr s);
	f32						SizeOf_(const wide_char* wsStr);

	f32						SizeOf_(const char cChar);

	f32						CurrentHeight_( );

	void					OutSetI(f32 x, f32 y);
	void					OutSet(f32 x, f32 y);

	void 					MasterOut(BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip, f32 _x, f32 _y, f32 _skip, pcstr fmt, va_list p);

	u32						smart_strlen(pcstr S);
	BOOL					IsMultibyte( )
	{
		return (uFlags & fsMultibyte);
	}
	u16						SplitByWidth(u16* puBuffer, u16 uBufferSize, f32 fTargetWidth, pcstr pszText);
	u16						GetCutLengthPos(f32 fTargetWidth, pcstr pszText);

	void					OutI(f32 _x, f32 _y, pcstr fmt, ...);
	void					Out(f32 _x, f32 _y, pcstr fmt, ...);
	void					OutNext(pcstr fmt, ...);
	void					OutPrev(pcstr fmt, ...);

	void					OutSkip(f32 val = 1.0f);

	virtual void			OnRender( );

	IC void					Clear( )
	{
		strings.clear( );
	}

#ifdef DEBUG
	shared_str				m_font_name;
#endif // def DEBUG

};
