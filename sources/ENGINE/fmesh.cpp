#include "stdafx.h"

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)
#include "fmesh.h"

BOOL ValidateIndices(unsigned int vCount, unsigned int iCount, unsigned short* pIndices)
{
	if (vCount>65535)	return FALSE;
	if (iCount%3)		return FALSE;

	for (unsigned int I=0; I<iCount; I++)
	{
		if (unsigned int(pIndices[I])>=vCount)	return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------------------------------------------
static unsigned int dwPositionPart[8] =
{
	0,	// no position
	3,	// x,y,z
	4,	// sx,sy,sz,rhw
	4,	// x,y,z,b1
	5,	// x,y,z,b1,b2
	6,	// x,y,z,b1,b2,b3
	7,	// x,y,z,b1,b2,b3,b4
	8	// x,y,z,b1,b2,b3,b4,b5
};

#define FAKES 0xffffffff
#define FAKEZ 0xfffffffe

void ConvertVertices(unsigned int dwTypeDest, void *pDest, unsigned int dwTypeSrc, void *pSource, unsigned int dwCount)
// assuming that pDest is large enought to maintain all the data
{
	unsigned int	TransferMask	[64];
	unsigned int	tmPos			= 0;
	unsigned int	tmPosSrc		= 0;
	unsigned int	dwSizeSrc		= D3DXGetFVFVertexSize(dwTypeSrc)/4;
	unsigned int	dwSizeDest		= D3DXGetFVFVertexSize(dwTypeDest)/4;
	unsigned int*	dest		= (unsigned int*)pDest;
	unsigned int*	src			= (unsigned int*)pSource;

	// avoid redundant processing
	if (dwTypeDest==dwTypeSrc) {
		CopyMemory	(pDest,pSource,dwSizeDest*dwCount*4);
		return;
	}

	// how many bytes to 'simple copy'
	unsigned int dwPosDest	= (dwTypeDest&D3DFVF_POSITION_MASK)>>1;
	unsigned int dwPosSrc	= (dwTypeSrc&D3DFVF_POSITION_MASK)>>1;
	if (dwPosDest==dwPosSrc) {
		unsigned int cnt = dwPositionPart[dwPosSrc];
		for (unsigned int i=0; i<cnt; i++) TransferMask[tmPos++]=i;
		tmPosSrc = tmPos;
	} else {
		FATAL		("Can't convert between different vertex positions");
	}

	// ---------------------- "Reserved" property
	if ((dwTypeDest&D3DFVF_PSIZE) && (dwTypeSrc&D3DFVF_PSIZE))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_PSIZE) && !(dwTypeSrc&D3DFVF_PSIZE))
	{	// DEST & !SRC
		TransferMask[tmPos++]=FAKEZ;// fake data
	}
	if (!(dwTypeDest&D3DFVF_PSIZE) && (dwTypeSrc&D3DFVF_PSIZE))
	{	// !DEST & SRC
		tmPosSrc++;					// skip it
	}

	// ---------------------- "Normal" property
	if ((dwTypeDest&D3DFVF_NORMAL) && (dwTypeSrc&D3DFVF_NORMAL))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
		TransferMask[tmPos++]=tmPosSrc++;
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_NORMAL) && !(dwTypeSrc&D3DFVF_NORMAL))
	{	// DEST & !SRC
		FATAL	("Source format doesn't have NORMAL but destination HAS");
	}
	if (!(dwTypeDest&D3DFVF_NORMAL) && (dwTypeSrc&D3DFVF_NORMAL))
	{	// !DEST & SRC
		tmPosSrc++;					// skip it
		tmPosSrc++;					// skip it
		tmPosSrc++;					// skip it
	}

	// ---------------------- "Diffuse" property
	if ((dwTypeDest&D3DFVF_DIFFUSE) && (dwTypeSrc&D3DFVF_DIFFUSE))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_DIFFUSE) && !(dwTypeSrc&D3DFVF_DIFFUSE))
	{	// DEST & !SRC
		TransferMask[tmPos++]=FAKES;	// fake data - white
	}
	if (!(dwTypeDest&D3DFVF_DIFFUSE) && (dwTypeSrc&D3DFVF_DIFFUSE))
	{	// !DEST & SRC
		tmPosSrc++;						// skip it
	}

	// ---------------------- "Specular" property
	if ((dwTypeDest&D3DFVF_SPECULAR) && (dwTypeSrc&D3DFVF_SPECULAR))
	{	// DEST & SRC
		TransferMask[tmPos++]=tmPosSrc++;
	}
	if ((dwTypeDest&D3DFVF_SPECULAR) && !(dwTypeSrc&D3DFVF_SPECULAR))
	{	// DEST & !SRC
		TransferMask[tmPos++]=FAKES;	// fake data - white
	}
	if (!(dwTypeDest&D3DFVF_SPECULAR) && (dwTypeSrc&D3DFVF_SPECULAR))
	{	// !DEST & SRC
		tmPosSrc++;						// skip it
	}

	// ---------------------- "Texture coords" property
	unsigned int dwTDest = ((dwTypeDest&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	unsigned int dwTSrc  = ((dwTypeSrc &D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	if (dwTDest<=dwTSrc) {
		for (unsigned int i=0; i<dwTDest; i++) {
			TransferMask[tmPos++]=tmPosSrc++;
			TransferMask[tmPos++]=tmPosSrc++;
		}
	} else {
		if (dwTSrc==0) {
			FATAL	("Source vertex format doesn't has texture coords at all");
		}
		// Copy real TC
		unsigned int dwStage0TC = tmPosSrc;
		for (unsigned int i=0; i<dwTSrc; i++) {
			TransferMask[tmPos++]=tmPosSrc++;
			TransferMask[tmPos++]=tmPosSrc++;
		}
		// Duplicate stage0 TC
		for (i=dwTSrc; i<dwTDest; i++) {
			TransferMask[tmPos++]=dwStage0TC;
			TransferMask[tmPos++]=dwStage0TC+1;
		}
	}

	// ---------------------- REAL CONVERTION USING BUILDED MASK
	for (unsigned int i=0; i<dwCount; i++) {
		// one vertex
		for (unsigned int j=0; j<dwSizeDest; j++) {
			unsigned int m = TransferMask[j];
			if (m == FAKES) dest[j]=0xffffffff;
			else if (m == FAKEZ) dest[j]=0;
			else dest[j]=src[m];
		}
		dest	+= dwSizeDest;
		src		+= dwSizeSrc;
	}
	return;
}

void ogf_desc::Load(IReader& F)
{
	F.r_stringZ	(source_file);
	F.r_stringZ	(build_name);
	F.r			(&build_time,sizeof(build_time));
	F.r_stringZ	(create_name);
	F.r			(&create_time,sizeof(create_time));
	F.r_stringZ	(modif_name);
	F.r			(&modif_time,sizeof(modif_time));
}
void ogf_desc::Save(IWriter& F)
{
	F.w_stringZ	(source_file);
	F.w_stringZ	(build_name);
	F.w			(&build_time,sizeof(build_time));
	F.w_stringZ	(create_name);
	F.w			(&create_time,sizeof(create_time));
	F.w_stringZ	(modif_name);
	F.w			(&modif_time,sizeof(modif_time));
}

