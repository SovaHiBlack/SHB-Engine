// HierrarhyVisual.cpp: implementation of the CHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HierrarhyVisual.h"
#include "mesh.h"
#include "Render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHierrarhyVisual::CHierrarhyVisual( ) : IRenderVisual( )
{
	bDontDelete = FALSE;
}

CHierrarhyVisual::~CHierrarhyVisual( )
{
	if (!bDontDelete)
	{
		for (u32 i = 0; i < children.size( ); i++)
		{
			::Render->model_Delete(children[i]);
		}
	}

	children.clear( );
}

void CHierrarhyVisual::Release( )
{
	if (!bDontDelete)
	{
		for (u32 i = 0; i < children.size( ); i++)
		{
			children[i]->Release( );
		}
	}
}

void CHierrarhyVisual::Load(pcstr N, IReader* data, u32 dwFlags)
{
	IRenderVisual::Load(N, data, dwFlags);
	if (data->find_chunk(OGF_CHILDREN_L))
	{
		// From Link
		u32 cnt = data->r_u32( );
		children.resize(cnt);
		for (u32 i = 0; i < cnt; i++)
		{
			u32 ID = data->r_u32( );
			children[i] = ::Render->getVisual(ID);
		}

		bDontDelete = TRUE;
	}
	else
	{
		if (data->find_chunk(OGF_CHILDREN))
		{
			// From stream
			IReader* OBJ = data->open_chunk(OGF_CHILDREN);
			if (OBJ)
			{
				IReader* O = OBJ->open_chunk(0);
				for (s32 count = 1; O; count++)
				{
					string_path name_load;
					string_path short_name;
					string_path num;
					strcpy_s(short_name, N);
					if (strext(short_name))
					{
						*strext(short_name) = 0;
					}

					strconcat(sizeof(name_load), name_load, short_name, ":", itoa(count, num, 10));
					children.push_back(::Render->model_CreateChild(name_load, O));
					O->close( );
					O = OBJ->open_chunk(count);
				}

				OBJ->close( );
			}

			bDontDelete = FALSE;
		}
		else
		{
			FATAL("Invalid visual");
		}
	}
}

void CHierrarhyVisual::Copy(IRenderVisual* pSrc)
{
	IRenderVisual::Copy(pSrc);

	CHierrarhyVisual* pFrom = (CHierrarhyVisual*)pSrc;

	children.clear( );
	children.reserve(pFrom->children.size( ));
	for (u32 i = 0; i < pFrom->children.size( ); i++)
	{
		IRenderVisual* p = ::Render->model_Duplicate(pFrom->children[i]);
		children.push_back(p);
	}

	bDontDelete = FALSE;
}
