#pragma once

#include "MbHelpers.h"

class ENGINE_API CGameFont : public pureRender
{
public:
	enum EAligment
	{
		alLeft				= 0,
		alRight,
		alCenter
	};

private:
	struct String
	{
		string512			string;
		float				x;
		float				y;
		float				height;
		u32					c;
		EAligment			align;
	};

protected:
	Fvector2				vHalfPixel;
	Ivector2				vTS;

	EAligment				eCurrentAlignment;
	u32						dwCurrentColor;
	float					fCurrentHeight;
	float					fCurrentX;
	float					fCurrentY;
	Fvector2				vInterval;

	Fvector3*				TCMap;
	float					fHeight;
	float					fXStep;
	float					fYStep;
	float					fTCHeight;
	xr_vector<String>		strings;

	ref_shader				pShader;
	ref_geom				pGeom;

	u32						nNumChars;

	u32						uFlags;

public:
	enum
	{
		fsGradient			= (1 << 0),
		fsDeviceIndependent	= (1 << 1),
		fsValid				= (1 << 2),
		fsMultibyte			= (1 << 3),

		fsForceDWORD		= u32(-1)
	};

protected:
	IC const Fvector3&		GetCharTC			(u16 c)
	{
		return TCMap[c];
	}

public:
							CGameFont			(const char* section, u32 flags = 0);
							CGameFont			(const char* shader, const char* texture, u32 flags = 0);
							~CGameFont			( );

	void					Initialize			(const char* shader, const char* texture);

	IC void					SetColor			(u32 C)
	{
		dwCurrentColor = C;
	};

	IC void					SetHeightI			(float S);
	IC void					SetHeight			(float S);

	IC float				GetHeight			( )
	{
		return fCurrentHeight;
	};
	IC void					SetInterval			(float x, float y)
	{
		vInterval.set(x, y);
	};
	IC void					SetInterval			(const Fvector2& v)
	{
		vInterval.set(v);
	};
	IC void					SetAligment			(EAligment aligment)
	{
		eCurrentAlignment = aligment;
	}

	float					SizeOf_				(const char* s);
	float					SizeOf_				(const wide_char* wsStr);
	float					SizeOf_				(const char cChar);

	float					CurrentHeight_		( );

	void					OutSetI				(float x, float y);
	void					OutSet				(float x, float y);

	void					MasterOut			(BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip, float _x, float _y, float _skip, const char* fmt, va_list p);

	u32						smart_strlen		(const char* S);
	BOOL					IsMultibyte			( )
	{
		return (uFlags & fsMultibyte);
	};
	u16						SplitByWidth		(u16* puBuffer, u16 uBufferSize, float fTargetWidth, const char* pszText);
	u16						GetCutLengthPos		(float fTargetWidth, const char* pszText);

	void					OutI				(float _x, float _y, const char* fmt, ...);
	void					Out					(float _x, float _y, const char* fmt, ...);
	void					OutNext				(const char* fmt, ...);
	void					OutPrev				(const char* fmt, ...);

	void					OutSkip				(float val = 1.0f);

	virtual void			OnRender			( );

	IC	void				Clear				( )
	{
		strings.clear( );
	};

#ifdef DEBUG
	shared_str				m_font_name;
#endif // def DEBUG

};
