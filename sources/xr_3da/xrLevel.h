#pragma once

struct xrGUID
{
	u64	g[2];

	ICF	bool operator==	(const xrGUID &o) const
	{
		return	((g[0] == o.g[0]) && (g[1] == o.g[1]));
	}

	ICF	bool operator!=	(const xrGUID &o) const
	{
		return	!(*this == o);
	}
};

enum fsL_Chunks
{
	fsL_HEADER			=1,		//*
	fsL_SHADERS			=2,		//*
	fsL_VISUALS			=3,		//*
	fsL_PORTALS			=4,		//*		- Portal polygons
	fsL_LIGHT_DYNAMIC	=6,		//*
	fsL_GLOWS			=7,		//*		- All glows inside level
	fsL_SECTORS			=8,		//*		- All sectors on level
	fsL_VB				=9,		//*		- Static geometry
	fsL_IB				=10,	//*
	fsL_SWIS			=11,	//*		- collapse info, usually for trees
	fsL_forcedword		= 0xFFFFFFFF
};

enum fsESectorChunks
{
	fsP_Portals = 1,	// - portal polygons
	fsP_Root,			// - geometry root
	fsP_forcedword = u32(-1)
};

enum fsSLS_Chunks
{
	fsSLS_Description	= 1,	// Name of level
	fsSLS_ServerState,
	fsSLS_forcedword = u32(-1)
};

enum EBuildQuality
{
	ebqDraft			= 0,
	ebqHigh,
	ebqCustom,
	ebq_force_u16		= u16(-1)
};

#pragma pack(push,8)
struct hdrLEVEL
{
	u16		XRLC_version;
	u16		XRLC_quality;
};

struct hdrCFORM
{
	u32		version;
	u32		vertcount;
	u32		facecount;
	fBox3	aabb;
};

struct	hdrNODES
{
	u32		version;
	u32		count;
	f32	size;
	f32	size_y;
	fBox3	aabb;
	xrGUID	guid;
};
#pragma pack(pop)

#pragma pack(push,1)
#pragma pack(1)
class NodePosition {
	u8	data[5];
	
	ICF	void xz	(u32 value)	{ CopyMemory	(data,&value,3);		}
	ICF	void y	(u16 value)	{ CopyMemory	(data + 3,&value,2);	}
public:
	ICF	u32	xz	() const	{
		return			((*((u32*)data)) & 0x00ffffff);
	}
	ICF	u32	x	(u32 row) const		{
		return			(xz() / row);
	}
	ICF	u32	z	(u32 row) const		{
		return			(xz() % row);
	}
	ICF	u32	y	() const			{
		return			(*((u16*)(data + 3)));
	}

	friend class	CLevelGraph;
	friend struct	CNodePositionCompressor;
	friend struct	CNodePositionConverter;
};

struct NodeCompressed
{
public:
	u8				data[12];
private:
	
	ICF	void link(u8 link_index, u32 value)
	{
		value			&= 0x007fffff;
		switch (link_index) {
			case 0 : {
				value	|= (*(u32*)data) & 0xff800000;
				CopyMemory(data, &value, sizeof(u32));
				break;
			}
			case 1 : {
				value	<<= 7;
				value	|= (*(u32*)(data + 2)) & 0xc000007f;
				CopyMemory(data + 2, &value, sizeof(u32));
				break;
			}
			case 2 : {
				value	<<= 6;
				value	|= (*(u32*)(data + 5)) & 0xe000003f;
				CopyMemory(data + 5, &value, sizeof(u32));
				break;
			}
			case 3 : {
				value	<<= 5;
				value	|= (*(u32*)(data + 8)) & 0xf000001f;
				CopyMemory(data + 8, &value, sizeof(u32));
				break;
			}
		}
	}
	
	ICF	void light(u8 value)
	{
		data[11]		&= 0x0f;
		data[11]		|= value << 4;
	}

public:
	u16				cover0 : 4;
	u16				cover1 : 4;
	u16				cover2 : 4;
	u16				cover3 : 4; 
	u16				plane;
	NodePosition	p;
	// 4 + 4 + 4 + 4 + 16 + 40 + 96 = 168 bits = 21 byte

	ICF	u32	link(u8 index) const
	{
		switch (index) {
			case 0 :	return	((*(u32*)data) & 0x007fffff);
			case 1 :	return	(((*(u32*)(data + 2)) >> 7) & 0x007fffff);
			case 2 :	return	(((*(u32*)(data + 5)) >> 6) & 0x007fffff);
			case 3 :	return	(((*(u32*)(data + 8)) >> 5) & 0x007fffff);
			default :	NODEFAULT;
		}
#ifdef DEBUG
		return			(0);
#endif
	}
	
	ICF	u8	light() const
	{
		return			(data[11] >> 4);
	}
	
	ICF	u16	cover(u8 index) const
	{
		switch (index) {
			case 0 : return(cover0);
			case 1 : return(cover1);
			case 2 : return(cover2);
			case 3 : return(cover3);
			default : NODEFAULT;
		}
#ifdef DEBUG
		return				(u8(-1));
#endif
	}

	friend class	CLevelGraph;
	friend struct	CNodeCompressed;
	friend class	CNodeRenumberer;
	friend class	CRenumbererConverter;
};									// 2+2+5+12 = 21b

struct SNodePositionOld
{
	s16				x;
	u16				y;
	s16				z;
};
#pragma pack	(pop)

const u32 XRCL_PRODUCTION_VERSION	= 14;	// output 
const u32 CFORM_CURRENT_VERSION		= 4;
const u32 MAX_NODE_BIT_COUNT		= 23;

const u32 XRAI_CURRENT_VERSION		= 8;
