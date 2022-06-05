#pragma once

#include "MbHelpers.h"

class ENGINE_API CGameFont
	: public pureRender
{
public:
	enum EAligment{
		alLeft				= 0,
		alRight,
		alCenter
	};
private:
	struct String
	{
		string512	string;
		F32			x;
		F32			y;
		F32			height;
		u32			c;
		EAligment	align;
	};
protected:
	Fvector2				vHalfPixel;
	Ivector2				vTS;

	EAligment				eCurrentAlignment;
	u32						dwCurrentColor;
	F32						fCurrentHeight;
	F32						fCurrentX;
	F32						fCurrentY;
	Fvector2				vInterval;

	Fvector 				*TCMap;
	F32						fHeight;
	F32						fXStep;
	F32						fYStep;
	F32						fTCHeight;
	xr_vector<String>		strings;

	ref_shader				pShader;
	ref_geom				pGeom;

	u32						nNumChars;

	u32						uFlags;

public:
	enum
	{
		fsGradient			= (1<<0),
		fsDeviceIndependent	= (1<<1),
		fsValid 			= (1<<2),

		fsMultibyte			= (1<<3),

		fsForceDWORD		= u32(-1)
	};

protected:
	IC const Fvector&		GetCharTC		(u16 c)		{return TCMap[c];}

public:
							CGameFont		(pcstr section, u32 flags=0);
							CGameFont		(pcstr shader, pcstr texture, u32 flags=0);
							~CGameFont		();

	void					Initialize		(pcstr shader, pcstr texture);

	IC void					SetColor		(u32 C)		{dwCurrentColor=C;};

	IC void					SetHeightI		(F32 S);
	IC void					SetHeight		(F32 S);

	IC F32				GetHeight		(){return fCurrentHeight;};
	IC void					SetInterval		(F32 x, F32 y) {vInterval.set(x,y);};
	IC void					SetInterval		(const Fvector2& v) {vInterval.set(v);};
	IC void					SetAligment		(EAligment aligment){ eCurrentAlignment=aligment; }

	F32					SizeOf_			(pcstr s );
	F32					SizeOf_			( const wide_char *wsStr );

	F32					SizeOf_			( const char cChar );

	F32					CurrentHeight_	();

	void					OutSetI			(F32 x, F32 y);
	void					OutSet			(F32 x, F32 y);

	void 					MasterOut( 	BOOL bCheckDevice , BOOL bUseCoords , BOOL bScaleCoords , BOOL bUseSkip ,
									  F32 _x , F32 _y , F32 _skip , pcstr fmt , va_list p );

	u32						smart_strlen(pcstr S );
	BOOL					IsMultibyte() { return ( uFlags & fsMultibyte ); };
	u16						SplitByWidth( u16 * puBuffer , u16 uBufferSize , F32 fTargetWidth , const char * pszText );
	u16						GetCutLengthPos(F32 fTargetWidth , const char * pszText );

	void  					OutI			(F32 _x , F32 _y , pcstr fmt , ... );
	void  					Out				(F32 _x , F32 _y , pcstr fmt , ... );
	void             		OutNext			(pcstr fmt , ... );
	void             		OutPrev			(pcstr fmt , ... );

	void					OutSkip			(F32 val=1.0f);

	virtual void			OnRender		();

	IC	void				Clear			()  { strings.clear(); };

#ifdef DEBUG
	shared_str				m_font_name;
#endif // DEBUG

};
