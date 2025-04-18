// XR_IOConsole.cpp: implementation of the CConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

//#include "x_ray.h"
#include "Input.h"
#include "Console.h"
#include "ConsoleCommand.h"
#include "GameFont.h"
//#include "CustomHUD.h"

#define LDIST 0.05f

ENGINE_API CConsole* Console = NULL;
pcstr				ioc_prompt = ">>> ";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CConsole::AddCommand(IConsoleCommand* C)
{
	Commands[C->Name( )] = C;
}

void CConsole::RemoveCommand(IConsoleCommand* C)
{
	vecCMD_IT it = Commands.find(C->Name( ));
	if (Commands.end( ) != it)
	{
		Commands.erase(it);
	}
}

void CConsole::Reset( )
{
	if (pFont)
	{
		xr_delete(pFont);
	}
}

void CConsole::Initialize( )
{
	scroll_delta = cmd_delta = old_cmd_delta = 0;
	editor[0] = 0;
	bShift = false;
	RecordCommands = false;
	cur_time = rep_time = 0.0f;
	fAccel = 1.0f;
	bVisible = false;
	rep_time = 0.0f;
	pFont = 0;
	last_mm_timer = 0;
	// Commands
	extern void CCC_Register( );
	CCC_Register( );
}

void CConsole::Destroy( )
{
	Execute("cfg_save");
	xr_delete(pFont);
	Commands.clear( );
}

void CConsole::OnFrame( )
{
	u32 mm_timer = Device.dwTimeContinual;
	f32 fDelta = (mm_timer - last_mm_timer) / 1000.0f;
	if (fDelta > 0.06666f)
	{
		fDelta = 0.06666f;
	}

	last_mm_timer = mm_timer;

	cur_time += fDelta;
	rep_time += fDelta * fAccel;
	if (cur_time > 0.1f)
	{
		cur_time -= 0.1f;
		bCursor = !bCursor;
	}

	if (rep_time > 0.2f)
	{
		rep_time -= 0.2f;
		bRepeat = true;
		fAccel += 0.2f;
	}
}

void out_font(CGameFont* pFont, pcstr text, f32& pos_y)
{
	f32 str_length = pFont->SizeOf_(text);
	if (str_length > 1024.0f)
	{
		f32 _l = 0.0f;
		s32 _sz = 0;
		s32 _ln = 0;
		string1024 _one_line;

		while (text[_sz])
		{
			_one_line[_ln + _sz] = text[_sz];
			_one_line[_ln + _sz + 1] = 0;
			f32 _t = pFont->SizeOf_(_one_line + _ln);
			if (_t > 1024.0f)
			{
				out_font(pFont, text + _sz, pos_y);
				pos_y -= LDIST;
				pFont->OutI(-1.0f, pos_y, "%s", _one_line + _ln);
				_l = 0.0f;
				_ln = _sz;
			}
			else
			{
				_l = _t;
			}

			++_sz;
		}
	}
	else
	{
		pFont->OutI(-1.0f, pos_y, "%s", text);
	}
}

void CConsole::OnRender( )
{
	f32 fMaxY;
	BOOL bGame;

	if (!bVisible)
	{
		return;
	}

	if (0 == pFont)
	{
		pFont = xr_new<CGameFont>("hud_font_di", CGameFont::fsDeviceIndependent);
	}

	bGame = false;
	if ((g_pGameLevel && g_pGameLevel->bReady) || (g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive( )))
	{
		bGame = true;
	}

	VERIFY(HW.pDevice);

	// Shadow
	D3DRECT R = { 0, 0, Device.dwWidth, Device.dwHeight };
	if (bGame)
	{
		R.y2 /= 2;
	}

	CHK_DX(HW.pDevice->Clear(1, &R, D3DCLEAR_TARGET, D3DCOLOR_XRGB(32, 32, 32), 1.0f, 0));

	f32 dwMaxY = f32(Device.dwHeight);
	if (bGame)
	{
		fMaxY = 0.0f;
		dwMaxY /= 2.0f;
	}
	else
	{
		fMaxY = 1.0f;
	}

	char buf[MAX_LEN + 5];
	strcpy_s(buf, ioc_prompt);
	strcat(buf, editor);
	if (bCursor)
	{
		strcat(buf, "|");
	}

	pFont->SetColor(color_rgba(128, 128, 255, 255));
	pFont->SetHeightI(0.025f);
	pFont->OutI(-1.0f, fMaxY - LDIST, "%s", buf);

	f32 ypos = fMaxY - LDIST - LDIST;
	for (s32 i = LogFile->size( ) - 1 - scroll_delta; i >= 0; i--)
	{
		ypos -= LDIST;
		if (ypos < -1.0f)
		{
			break;
		}

		pcstr ls = *(*LogFile)[i];
		if (0 == ls)
		{
			continue;
		}

		switch (ls[0])
		{
			case '~':
			{
				pFont->SetColor(color_rgba(255, 255, 0, 255));
				out_font(pFont, &ls[2], ypos);
			}
			break;
			case '!':
			{
				pFont->SetColor(color_rgba(255, 0, 0, 255));
				out_font(pFont, &ls[2], ypos);
			}
			break;
			case '*':
			{
				pFont->SetColor(color_rgba(128, 128, 128, 255));
				out_font(pFont, &ls[2], ypos);
			}
			break;
			case '-':
			{
				pFont->SetColor(color_rgba(0, 255, 0, 255));
				out_font(pFont, &ls[2], ypos);
			}
			break;
			case '#':
			{
				pFont->SetColor(color_rgba(0, 222, 205, 145));
				out_font(pFont, &ls[2], ypos);
			}
			break;
			default:
			{
				pFont->SetColor(color_rgba(255, 255, 255, 255));
				out_font(pFont, ls, ypos);
			}
		}
	}

	pFont->OnRender( );
}

void CConsole::OnPressKey(s32 dik, BOOL bHold)
{
	if (!bHold)
	{
		fAccel = 1.0f;
	}

	switch (dik)
	{
		case DIK_GRAVE:
		{
			if (bShift)
			{
				strcat(editor, "~");
			}
		}
		break;
		case DIK_ESCAPE:
		{
			if (!bHold)
			{
				if (g_pGameLevel || (g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive( )))
				{
					Hide( );
				}
			}
		}
		break;
		case DIK_PRIOR:
		{
			scroll_delta++;
			if (scroll_delta > s32(LogFile->size( )) - 1)
			{
				scroll_delta = LogFile->size( ) - 1;
			}
		}
		break;
		case DIK_NEXT:
		{
			scroll_delta--;
			if (scroll_delta < 0)
			{
				scroll_delta = 0;
			}
		}
		break;
		case DIK_TAB:
		{
			pcstr radmin_cmd_name = "ra ";
			bool b_ra = (editor == strstr(editor, radmin_cmd_name));
			s32 offset = (b_ra) ? xr_strlen(radmin_cmd_name) : 0;
			vecCMD_IT I = Commands.lower_bound(editor + offset);
			if (I != Commands.end( ))
			{
				IConsoleCommand& O = *(I->second);
				strcpy_s(editor + offset, sizeof(editor) - offset, O.Name( ));
				strcat(editor + offset, " ");
			}
		}
		break;
		case DIK_UP:
		{
			cmd_delta--;
			SelectCommand( );
		}
		break;
		case DIK_DOWN:
		{
			cmd_delta++;
			SelectCommand( );
		}
		break;
		case DIK_BACK:
		{
			if (xr_strlen(editor) > 0)
			{
				editor[xr_strlen(editor) - 1] = 0;
			}
		}
		break;
		case DIK_LSHIFT:
		case DIK_RSHIFT:
		{
			bShift = true;
		}
		break;
		case DIK_1:
		{
			if (bShift)
			{
				strcat(editor, "!");
			}
			else
			{
				strcat(editor, "1");
			}
		}
		break;
		case DIK_2:
		{
			if (bShift)
			{
				strcat(editor, "@");
			}
			else
			{
				strcat(editor, "2");
			}
		}
		break;
		case DIK_3:
		{
			if (bShift)
			{
				strcat(editor, "#");
			}
			else
			{
				strcat(editor, "3");
			}
		}
		break;
		case DIK_4:
		{
			if (bShift)
			{
				strcat(editor, "$");
			}
			else
			{
				strcat(editor, "4");
			}
		}
		break;
		case DIK_5:
		{
			if (bShift)
			{
				strcat(editor, "%");
			}
			else
			{
				strcat(editor, "5");
			}
		}
		break;
		case DIK_6:
		{
			if (bShift)
			{
				strcat(editor, "^");
			}
			else
			{
				strcat(editor, "6");
			}
		}
		break;
		case DIK_7:
		{
			if (bShift)
			{
				strcat(editor, "&");
			}
			else
			{
				strcat(editor, "7");
			}
		}
		break;
		case DIK_8:
		{
			if (bShift)
			{
				strcat(editor, "*");
			}
			else
			{
				strcat(editor, "8");
			}
		}
		break;
		case DIK_9:
		{
			if (bShift)
			{
				strcat(editor, "(");
			}
			else
			{
				strcat(editor, "9");
			}
		}
		break;
		case DIK_0:
		{
			if (bShift)
			{
				strcat(editor, ")");
			}
			else
			{
				strcat(editor, "0");
			}
		}
		break;
		case DIK_A:
		{
			strcat(editor, "a");
		}
		break;
		case DIK_B:
		{
			strcat(editor, "b");
		}
		break;
		case DIK_C:
		{
			strcat(editor, "c");
		}
		break;
		case DIK_D:
		{
			strcat(editor, "d");
		}
		break;
		case DIK_E:
		{
			strcat(editor, "e");
		}
		break;
		case DIK_F:
		{
			strcat(editor, "f");
		}
		break;
		case DIK_G:
		{
			strcat(editor, "g");
		}
		break;
		case DIK_H:
		{
			strcat(editor, "h");
		}
		break;
		case DIK_I:
		{
			strcat(editor, "i");
		}
		break;
		case DIK_J:
		{
			strcat(editor, "j");
		}
		break;
		case DIK_K:
		{
			strcat(editor, "k");
		}
		break;
		case DIK_L:
		{
			strcat(editor, "l");
		}
		break;
		case DIK_M:
		{
			strcat(editor, "m");
		}
		break;
		case DIK_N:
		{
			strcat(editor, "n");
		}
		break;
		case DIK_O:
		{
			strcat(editor, "o");
		}
		break;
		case DIK_P:
		{
			strcat(editor, "p");
		}
		break;
		case DIK_Q:
		{
			strcat(editor, "q");
		}
		break;
		case DIK_R:
		{
			strcat(editor, "r");
		}
		break;
		case DIK_S:
		{
			strcat(editor, "s");
		}
		break;
		case DIK_T:
		{
			strcat(editor, "t");
		}
		break;
		case DIK_U:
		{
			strcat(editor, "u");
		}
		break;
		case DIK_V:
		{
			strcat(editor, "v");
		}
		break;
		case DIK_W:
		{
			strcat(editor, "w");
		}
		break;
		case DIK_X:
		{
			strcat(editor, "x");
		}
		break;
		case DIK_Y:
		{
			strcat(editor, "y");
		}
		break;
		case DIK_Z:
		{
			strcat(editor, "z");
		}
		break;
		case DIK_SPACE:
		{
			strcat(editor, " ");
		}
		break;
		case DIK_BACKSLASH:
		{
			if (bShift)
			{
				strcat(editor, "|");
			}
			else
			{
				strcat(editor, "\\");
			}
		}
		break;
		case DIK_LBRACKET:
		{
			if (bShift)
			{
				strcat(editor, "{");
			}
			else
			{
				strcat(editor, "[");
			}
		}
		break;
		case DIK_RBRACKET:
		{
			if (bShift)
			{
				strcat(editor, "}");
			}
			else
			{
				strcat(editor, "]");
			}
		}
		break;
		case DIK_APOSTROPHE:
		{
			if (bShift)
			{
				strcat(editor, "\"");
			}
			else
			{
				strcat(editor, "'");
			}
		}
		break;
		case DIK_COMMA:
		{
			if (bShift)
			{
				strcat(editor, "<");
			}
			else
			{
				strcat(editor, ",");
			}
		}
		break;
		case DIK_PERIOD:
		{
			if (bShift)
			{
				strcat(editor, ">");
			}
			else
			{
				strcat(editor, ".");
			}
		}
		break;
		case DIK_EQUALS:
		{
			if (bShift)
			{
				strcat(editor, "+");
			}
			else
			{
				strcat(editor, "=");
			}
		}
		break;
		case DIK_MINUS:
		{
			if (bShift)
			{
				strcat(editor, "_");
			}
			else
			{
				strcat(editor, "-");
			}
		}
		break;
		case DIK_SEMICOLON:
		{
			if (bShift)
			{
				strcat(editor, ":");
			}
			else
			{
				strcat(editor, ";");
			}
		}
		break;
		case DIK_SLASH:
		{
			if (bShift)
			{
				strcat(editor, "?");
			}
			else
			{
				strcat(editor, "/");
			}
		}
		break;
		case DIK_RETURN:
		{
			ExecuteCommand( );
		}
		break;
		case DIK_INSERT:
		if (OpenClipboard(0))
		{
			HGLOBAL hmem = GetClipboardData(CF_TEXT);
			if (hmem)
			{
				pcstr clipdata = (pcstr)GlobalLock(hmem);
				strncpy(editor, clipdata, MAX_LEN - 1);
				editor[MAX_LEN - 1] = 0;
				for (u32 i = 0; i < xr_strlen(editor); i++)
				{
					if (isprint(editor[i]))
					{
						editor[i] = char(tolower(editor[i]));
					}
					else
					{
						editor[i] = ' ';
					}
				}

				GlobalUnlock(hmem);
				CloseClipboard( );
			}
		}
		break;
		default:
		{
		}
		break;
	}

	u32	clip = MAX_LEN - 8;
	if (xr_strlen(editor) >= clip)
	{
		editor[clip - 1] = 0;
	}

	bRepeat = false;
	rep_time = 0.0f;
}

void CConsole::IR_OnKeyboardPress(s32 dik)
{
	OnPressKey(dik);
}

void CConsole::IR_OnKeyboardRelease(s32 dik)
{
	fAccel = 1.0f;
	rep_time = 0.0f;
	switch (dik)
	{
		case DIK_LSHIFT:
		case DIK_RSHIFT:
		{
			bShift = false;
		}
		break;
	}
}

void CConsole::IR_OnKeyboardHold(s32 dik)
{
	f32 fRep = rep_time;
	if (bRepeat)
	{
		OnPressKey(dik, true);
		bRepeat = false;
	}

	rep_time = fRep;
}

void CConsole::ExecuteCommand( )
{
	char first_word[MAX_LEN];
	char last_word[MAX_LEN];
	char converted[MAX_LEN];
	s32 i;
	s32 j;
	s32 len;

	scroll_delta = 0;
	cmd_delta = 0;
	old_cmd_delta = 0;

	len = xr_strlen(editor);
	for (i = 0; i < len; i++)
	{
		if ((editor[i] == '\n') || (editor[i] == '\t'))
		{
			editor[i] = ' ';
		}
	}

	j = 0;
	for (i = 0; i < len; i++)
	{
		switch (editor[i])
		{
			case ' ':
			{
				if (editor[i + 1] == ' ')
				{
					continue;
				}

				if (i == len - 1)
				{
					goto outloop;
				}
			}
			break;
		}

		converted[j++] = editor[i];
	}

outloop:
	converted[j] = 0;
	if (converted[0] == ' ')
	{
		strcpy_s(editor, &(converted[1]));
	}
	else
	{
		strcpy_s(editor, converted);
	}

	if (editor[0] == 0)
	{
		return;
	}

	if (RecordCommands)
	{
		Log("~", editor);
	}

	// split into cmd/params
	editor[j++] = ' ';
	editor[len = j] = 0;
	for (i = 0; i < len; i++)
	{
		if (editor[i] != ' ')
		{
			first_word[i] = editor[i];
		}
		else
		{
			// last 'word' - exit
			strcpy_s(last_word, editor + i + 1);
			break;
		}
	}

	first_word[i] = 0;
	if (last_word[xr_strlen(last_word) - 1] == ' ')
	{
		last_word[xr_strlen(last_word) - 1] = 0;
	}

	// search
	vecCMD_IT I = Commands.find(first_word);
	if (I != Commands.end( ))
	{
		IConsoleCommand& C = *(I->second);
		if (C.bEnabled)
		{
			if (C.bLowerCaseArgs) strlwr(last_word);
			if (last_word[0] == 0)
			{
				if (C.bEmptyArgsHandled)
				{
					C.Execute(last_word);
				}
				else
				{
					IConsoleCommand::TStatus S; C.Status(S);
					Msg("- %s %s", C.Name( ), S);
				}
			}
			else
			{
				C.Execute(last_word);
			}
		}
		else
		{
			Log("! Command disabled.");
		}
	}
	else
	{
		Log("! Unknown command: ", first_word);
	}

	editor[0] = 0;
}

void CConsole::Show( )
{
	if (bVisible)
	{
		return;
	}

	bVisible = true;

	editor[0] = 0;
	rep_time = 0.0f;
	fAccel = 1.0f;

	IR_Capture( );
	Device.seqRender.Add(this, 1);
	Device.seqFrame.Add(this);
}

void CConsole::Hide( )
{
	if (!bVisible)
	{
		return;
	}

	bVisible = false;
	Device.seqFrame.Remove(this);
	Device.seqRender.Remove(this);
	IR_Release( );
}

void CConsole::SelectCommand( )
{
	s32 p;
	s32 k;
	bool found = false;
	for (p = LogFile->size( ) - 1, k = 0; p >= 0; p--)
	{
		if (0 == *(*LogFile)[p])
		{
			continue;
		}

		if ((*LogFile)[p][0] == '~')
		{
			k--;
			if (k == cmd_delta)
			{
				strcpy_s(editor, &(*(*LogFile)[p])[2]);
				found = true;
			}
		}
	}

	if (!found)
	{
		if (cmd_delta > old_cmd_delta)
		{
			editor[0] = 0;
		}

		cmd_delta = old_cmd_delta;
	}
	else
	{
		old_cmd_delta = cmd_delta;
	}
}

void CConsole::Execute(pcstr cmd)
{
	strncpy(editor, cmd, MAX_LEN - 1);
	editor[MAX_LEN - 1] = 0;
	RecordCommands = false;
	ExecuteCommand( );
	RecordCommands = true;
}
void CConsole::ExecuteScript(pcstr N)
{
	string128 cmd;
	strconcat(sizeof(cmd), cmd, "cfg_load ", N);
	Execute(cmd);
}

BOOL CConsole::GetBool(pcstr cmd, BOOL& val)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I != Commands.end( ))
	{
		IConsoleCommand* C = I->second;
		CCC_Mask* cf = dynamic_cast<CCC_Mask*>(C);
		if (cf)
		{
			val = cf->GetValue( );
		}
		else
		{
			CCC_Integer* cf = dynamic_cast<CCC_Integer*>(C);
			val = !!cf->GetValue( );
		}
	}

	return val;
}

f32 CConsole::GetFloat(pcstr cmd, f32& val, f32& min, f32& max)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I != Commands.end( ))
	{
		IConsoleCommand* C = I->second;
		CCC_Float* cf = dynamic_cast<CCC_Float*>(C);
		val = cf->GetValue( );
		min = cf->GetMin( );
		max = cf->GetMax( );
		return val;
	}

	return val;
}

s32 CConsole::GetInteger(pcstr cmd, s32& val, s32& min, s32& max)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I != Commands.end( ))
	{
		IConsoleCommand* C = I->second;
		CCC_Integer* cf = dynamic_cast<CCC_Integer*>(C);
		if (cf)
		{
			val = cf->GetValue( );
			min = cf->GetMin( );
			max = cf->GetMax( );
		}
		else
		{
			CCC_Mask* cm = dynamic_cast<CCC_Mask*>(C);
			R_ASSERT(cm);
			val = (0 != cm->GetValue( )) ? 1 : 0;
			min = 0;
			max = 1;
		}

		return val;
	}

	return val;
}

pstr CConsole::GetString(pcstr cmd)
{
	static IConsoleCommand::TStatus stat;
	vecCMD_IT I = Commands.find(cmd);
	if (I != Commands.end( ))
	{
		IConsoleCommand* C = I->second;
		C->Status(stat);
		return stat;
	}

	return NULL;
}

pstr CConsole::GetToken(pcstr cmd)
{
	return GetString(cmd);
}

xr_token* CConsole::GetXRToken(pcstr cmd)
{
	vecCMD_IT I = Commands.find(cmd);
	if (I != Commands.end( ))
	{
		IConsoleCommand* C = I->second;
		CCC_Token* cf = dynamic_cast<CCC_Token*>(C);
		return cf->GetToken( );
	}

	return NULL;
}
