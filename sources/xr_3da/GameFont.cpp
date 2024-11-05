#include "stdafx.h"

#include "GameFont.h"
#include "Render.h"

extern ENGINE_API BOOL g_bRendering;
ENGINE_API fVector2		g_current_font_scale = { 1.0f,1.0f };

CGameFont::CGameFont(pcstr section, u32 flags)
{
	fCurrentHeight = 0.0f;
	fXStep = 0.0f;
	fYStep = 0.0f;
	uFlags = flags;
	nNumChars = 0x100;
	TCMap = NULL;
	Initialize(pSettings->r_string(section, "shader"), pSettings->r_string(section, "texture"));
	if (pSettings->line_exist(section, "size"))
	{
		f32 sz = pSettings->r_float(section, "size");
		if (uFlags & fsDeviceIndependent)
		{
			SetHeightI(sz);
		}
		else
		{
			SetHeight(sz);
		}
	}

	if (pSettings->line_exist(section, "interval"))
	{
		SetInterval(pSettings->r_fvector2(section, "interval"));
	}
}

CGameFont::CGameFont(pcstr shader, pcstr texture, u32 flags)
{
	fCurrentHeight = 0.0f;
	fXStep = 0.0f;
	fYStep = 0.0f;
	uFlags = flags;
	nNumChars = 0x100;
	TCMap = NULL;
	Initialize(shader, texture);
}

void CGameFont::Initialize(pcstr cShader, pcstr cTextureName)
{
	string_path cTexture;

	pcstr _lang = pSettings->r_string("string_table", "font_prefix");
	bool is_di = strstr(cTextureName, "ui_font_hud_01") || strstr(cTextureName, "ui_font_hud_02") || strstr(cTextureName, "ui_font_console_02");
	if (_lang && !is_di)
	{
		strconcat(sizeof(cTexture), cTexture, cTextureName, _lang);
	}
	else
	{
		strcpy_s(cTexture, sizeof(cTexture), cTextureName);
	}

	uFlags &= ~fsValid;
	vTS.set(1.f, 1.f); // обязательно !!!

	eCurrentAlignment = alLeft;
	vInterval.set(1.0f, 1.0f);

	strings.reserve(128);

	// check ini exist
	string_path fn, buf;
	strcpy_s(buf, cTexture); if (strext(buf)) *strext(buf) = 0;
	R_ASSERT2(FS.exist(fn, "$game_textures$", buf, ".ini"), fn);
	CInifile* ini = CInifile::Create(fn);

	nNumChars = 0x100;
	TCMap = (fVector3*)xr_realloc((pvoid)TCMap, nNumChars * sizeof(fVector3));

	if (ini->section_exist("mb_symbol_coords"))
	{
		nNumChars = 0x10000;
		TCMap = (fVector3*)xr_realloc((pvoid)TCMap, nNumChars * sizeof(fVector3));
		uFlags |= fsMultibyte;
		fHeight = ini->r_float("mb_symbol_coords", "height");

		fXStep = ceil(fHeight / 2.0f);

		for (u32 i = 0; i < nNumChars; i++)
		{
			sprintf_s(buf, sizeof(buf), "%05d", i);
			if (ini->line_exist("mb_symbol_coords", buf))
			{
				fVector3 v = ini->r_fvector3("mb_symbol_coords", buf);
				TCMap[i].set(v.x, v.y, 1 + v[2] - v[0]);
			}
			else
			{
				TCMap[i].set(0.0f, 0.0f, 0.0f);
			}
		}
	}
	else
		if (ini->section_exist("symbol_coords"))
		{
			fHeight = ini->r_float("symbol_coords", "height");
			for (u32 i = 0; i < nNumChars; i++)
			{
				sprintf_s(buf, sizeof(buf), "%03d", i);
				fVector3 v = ini->r_fvector3("symbol_coords", buf);
				TCMap[i].set(v.x, v.y, v[2] - v[0]);
			}
		}
		else
		{
			if (ini->section_exist("char widths"))
			{
				fHeight = ini->r_float("char widths", "height");
				s32 cpl = 16;
				for (u32 i = 0; i < nNumChars; i++)
				{
					sprintf_s(buf, sizeof(buf), "%d", i);
					f32 w = ini->r_float("char widths", buf);
					TCMap[i].set((i % cpl) * fHeight, (i / cpl) * fHeight, w);
				}
			}
			else
			{
				R_ASSERT(ini->section_exist("font_size"));
				fHeight = ini->r_float("font_size", "height");
				f32 width = ini->r_float("font_size", "width");
				const s32 cpl = ini->r_s32("font_size", "cpl");
				for (u32 i = 0; i < nNumChars; i++)
				{
					TCMap[i].set((i % cpl) * width, (i / cpl) * fHeight, width);
				}
			}
		}

	fCurrentHeight = fHeight;

	CInifile::Destroy(ini);

	// Shading
	pShader.create(cShader, cTexture);
	pGeom.create(FVF::F_TL, RCache.Vertex.Buffer( ), RCache.QuadIB);
}

CGameFont::~CGameFont( )
{
	if (TCMap)
	{
		xr_free(TCMap);
	}

	// Shading
	pShader.destroy( );
	pGeom.destroy( );
}

#define DI2PX(x) f32(iFloor((x+1)*f32(::Render->getTarget()->get_width())*0.5f))
#define DI2PY(y) f32(iFloor((y+1)*f32(::Render->getTarget()->get_height())*0.5f))

void CGameFont::OutSet(f32 x, f32 y)
{
	fCurrentX = x;
	fCurrentY = y;
}

void CGameFont::OutSetI(f32 x, f32 y)
{
	OutSet(DI2PX(x), DI2PY(y));
}

u32 CGameFont::smart_strlen(pcstr S)
{
	return (IsMultibyte( ) ? mbhMulti2Wide(NULL, NULL, 0, S) : xr_strlen(S));
}

void CGameFont::OnRender( )
{
	VERIFY(g_bRendering);
	if (pShader)
	{
		RCache.set_Shader(pShader);
	}

	if (!(uFlags & fsValid))
	{
		CTexture* T = RCache.get_ActiveTexture(0);
		vTS.set((s32)T->get_Width( ), (s32)T->get_Height( ));
		fTCHeight = fHeight / f32(vTS.y);
		uFlags |= fsValid;
	}

	for (u32 i = 0; i < strings.size( ); )
	{
		// calculate first-fit
		s32		count = 1;

		s32 length = smart_strlen(strings[i].string);

		while ((i + count) < strings.size( ))
		{
			s32 L = smart_strlen(strings[i + count].string);

			if ((L + length) < MAX_MB_CHARS)
			{
				count++;
				length += L;
			}
			else
			{
				break;
			}
		}

		// lock AGP memory
		u32	vOffset;
		FVF::TL* v = (FVF::TL*)RCache.Vertex.Lock(length * 4, pGeom.stride( ), vOffset);
		FVF::TL* start = v;

		// fill vertices
		u32 last = i + count;
		for (; i < last; i++)
		{
			String& PS = strings[i];
			wide_char wsStr[MAX_MB_CHARS];

			s32 len = IsMultibyte( ) ? mbhMulti2Wide(wsStr, NULL, MAX_MB_CHARS, PS.string) : xr_strlen(PS.string);

			if (len)
			{
				f32	X = f32(iFloor(PS.x));
				f32	Y = f32(iFloor(PS.y));
				f32	S = PS.height * g_current_font_scale.y;
				f32	Y2 = Y + S;
				f32 fSize = 0.0f;

				if (PS.align)
				{
					fSize = IsMultibyte( ) ? SizeOf_(wsStr) : SizeOf_(PS.string);
				}

				switch (PS.align)
				{
					case alCenter:
					{
						X -= (iFloor(fSize * 0.5f)) * g_current_font_scale.x;
					}
					break;
					case alRight:
					{
						X -= iFloor(fSize);
					}
					break;
				}

				u32	clr, clr2;
				clr2 = clr = PS.c;
				if (uFlags & fsGradient)
				{
					u32	_R = color_get_R(clr) / 2;
					u32	_G = color_get_G(clr) / 2;
					u32	_B = color_get_B(clr) / 2;
					u32	_A = color_get_A(clr);
					clr2 = color_rgba(_R, _G, _B, _A);
				}

				f32	tu;
				f32	tv;
				for (s32 j = 0; j < len; j++)
				{
					fVector3 l;
					l = IsMultibyte( ) ? GetCharTC(wsStr[1 + j]) : GetCharTC((u16)(u8)PS.string[j]);

					f32 scw = l.z * g_current_font_scale.x;

					f32 fTCWidth = l.z / vTS.x;

					if (!fis_zero(l.z))
					{
						tu = (l.x / vTS.x) + (0.5f / vTS.x);
						tv = (l.y / vTS.y) + (0.5f / vTS.y);

						v->set(X, Y2, clr2, tu, tv + fTCHeight);
						v++;
						v->set(X, Y, clr, tu, tv);
						v++;
						v->set(X + scw, Y2, clr2, tu + fTCWidth, tv + fTCHeight);
						v++;
						v->set(X + scw, Y, clr, tu + fTCWidth, tv);
						v++;
					}

					X += scw * vInterval.x;
					if (IsMultibyte( ))
					{
						X -= 2;
						if (IsNeedSpaceCharacter(wsStr[1 + j]))
						{
							X += fXStep;
						}
					}
				}
			}
		}

		// Unlock and draw
		u32 vCount = (u32)(v - start);
		RCache.Vertex.Unlock(vCount, pGeom.stride( ));
		if (vCount)
		{
			RCache.set_Geometry(pGeom);
			RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, vCount, 0, vCount / 2);
		}
	}

	strings.clear_not_free( );
}

u16 CGameFont::GetCutLengthPos(f32 fTargetWidth, pcstr pszText)
{
	VERIFY(pszText);

	wide_char wsStr[MAX_MB_CHARS], wsPos[MAX_MB_CHARS];
	f32 fCurWidth = 0.0f;
	f32 fDelta = 0.0f;

	u16	len = mbhMulti2Wide(wsStr, wsPos, MAX_MB_CHARS, pszText);

	for (u16 i = 1; i <= len; i++)
	{
		fDelta = GetCharTC(wsStr[i]).z - 2;

		if (IsNeedSpaceCharacter(wsStr[i]))
		{
			fDelta += fXStep;
		}

		if ((fCurWidth + fDelta) > fTargetWidth)
		{
			break;
		}
		else
		{
			fCurWidth += fDelta;
		}
	}

	return wsPos[i - 1];
}

u16 CGameFont::SplitByWidth(u16* puBuffer, u16 uBufferSize, f32 fTargetWidth, pcstr pszText)
{
	VERIFY(puBuffer && uBufferSize && pszText);

	wide_char wsStr[MAX_MB_CHARS], wsPos[MAX_MB_CHARS];
	f32 fCurWidth = 0.0f;
	f32 fDelta = 0.0f;
	u16 nLines = 0;

	u16	len = mbhMulti2Wide(wsStr, wsPos, MAX_MB_CHARS, pszText);

	for (u16 i = 1; i <= len; i++)
	{
		fDelta = GetCharTC(wsStr[i]).z - 2;

		if (IsNeedSpaceCharacter(wsStr[i]))
		{
			fDelta += fXStep;
		}

		if (
			((fCurWidth + fDelta) > fTargetWidth) && // overlength
			(!IsBadStartCharacter(wsStr[i])) && // can start with this character
			(i < len) && // is not the last character
			((i > 1) && (!IsBadEndCharacter(wsStr[i - 1]))) // && // do not stop the string on a "bad" character
			//				( ( i > 1 ) && ( ! ( ( IsAlphaCharacter( wsStr[ i - 1 ] ) ) && (  IsAlphaCharacter( wsStr[ i ] ) ) ) ) ) // do not split numbers or words
			)
		{
			fCurWidth = fDelta;
			VERIFY(nLines < uBufferSize);
			puBuffer[nLines++] = wsPos[i - 1];
		}
		else
		{
			fCurWidth += fDelta;
		}
	}

	return nLines;
}

void CGameFont::MasterOut(
	BOOL bCheckDevice, BOOL bUseCoords, BOOL bScaleCoords, BOOL bUseSkip,
	f32 _x, f32 _y, f32 _skip, pcstr fmt, va_list p)
{
	if (bCheckDevice && (!Device.b_is_Active))
	{
		return;
	}

	String rs;

	rs.x = (bUseCoords ? (bScaleCoords ? (DI2PX(_x)) : _x) : fCurrentX);
	rs.y = (bUseCoords ? (bScaleCoords ? (DI2PY(_y)) : _y) : fCurrentY);
	rs.c = dwCurrentColor;
	rs.height = fCurrentHeight;
	rs.align = eCurrentAlignment;

	int vs_sz = _vsnprintf(rs.string, sizeof(rs.string) - 1, fmt, p);

	VERIFY((vs_sz != -1) && (rs.string[vs_sz] == '\0'));

	if (vs_sz == -1)
	{
		return;
	}

	if (vs_sz)
	{
		strings.push_back(rs);
	}

	if (bUseSkip)
	{
		OutSkip(_skip);
	}
}

#define MASTER_OUT(CHECK_DEVICE,USE_COORDS,SCALE_COORDS,USE_SKIP,X,Y,SKIP,FMT) \
	{ va_list p; va_start ( p , fmt ); \
	  MasterOut( CHECK_DEVICE , USE_COORDS , SCALE_COORDS , USE_SKIP , X , Y , SKIP , FMT, p ); \
	  va_end( p ); }

void __cdecl CGameFont::OutI(f32 _x, f32 _y, pcstr fmt, ...)
{
	MASTER_OUT(FALSE, TRUE, TRUE, FALSE, _x, _y, 0.0f, fmt);
}

void __cdecl CGameFont::Out(f32 _x, f32 _y, pcstr fmt, ...)
{
	MASTER_OUT(TRUE, TRUE, FALSE, FALSE, _x, _y, 0.0f, fmt);
}

void __cdecl CGameFont::OutNext(pcstr fmt, ...)
{
	MASTER_OUT(TRUE, FALSE, FALSE, TRUE, 0.0f, 0.0f, 1.0f, fmt);
}

void __cdecl CGameFont::OutPrev(pcstr fmt, ...)
{
	MASTER_OUT(TRUE, FALSE, FALSE, TRUE, 0.0f, 0.0f, -1.0f, fmt);
}

void CGameFont::OutSkip(f32 val)
{
	fCurrentY += val * CurrentHeight_( );
}

f32 CGameFont::SizeOf_(const char cChar)
{
	VERIFY(!IsMultibyte( ));

	return ((GetCharTC((u16)(u8)cChar).z * vInterval.x));
}

f32 CGameFont::SizeOf_(pcstr s)
{
	if (!(s && s[0]))
	{
		return 0.0f;
	}

	if (IsMultibyte( ))
	{
		wide_char wsStr[MAX_MB_CHARS];
		mbhMulti2Wide(wsStr, NULL, MAX_MB_CHARS, s);
		return SizeOf_(wsStr);
	}

	s32 len = xr_strlen(s);
	f32 X = 0.0f;
	if (len)
	{
		for (s32 j = 0; j < len; j++)
		{
			X += GetCharTC((u16)(u8)s[j]).z;
		}
	}

	return (X * vInterval.x/**vTS.x*/);
}

f32 CGameFont::SizeOf_(const wide_char* wsStr)
{
	if (!(wsStr && wsStr[0]))
	{
		return 0.0f;
	}

	u32 len = wsStr[0];
	f32 X = 0.0f;
	f32 fDelta = 0.0f;

	if (len)
	{
		for (u32 j = 1; j <= len; j++)
		{
			fDelta = GetCharTC(wsStr[j]).z - 2;
			if (IsNeedSpaceCharacter(wsStr[j]))
			{
				fDelta += fXStep;
			}

			X += fDelta;
		}
	}

	return (X * vInterval.x);
}

f32 CGameFont::CurrentHeight_( )
{
	return (fCurrentHeight * vInterval.y);
}

void CGameFont::SetHeightI(f32 S)
{
	VERIFY(uFlags & fsDeviceIndependent);
	fCurrentHeight = S * Device.dwHeight;
}

void CGameFont::SetHeight(f32 S)
{
	VERIFY(uFlags & fsDeviceIndependent);
	fCurrentHeight = S;
}
