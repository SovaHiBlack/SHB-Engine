#include "stdafx.h"

#include "ResourceManager.h"
#include "R_DStreams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int		rsDVB_Size			= 512+1024;
int		rsDIB_Size			= 512;

void _VertexStream::Create	()
{
	Device.Resources->Evict		();

	mSize					= rsDVB_Size*1024;
	R_CHK					(HW.pDevice->CreateVertexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &pVB, NULL));
	R_ASSERT				(pVB);

	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DVB created: %dK", mSize/1024);
}

void _VertexStream::Destroy	()
{
	_RELEASE				(pVB);
	_clear					();
}

void* _VertexStream::Lock	(unsigned int vl_Count, unsigned int Stride, unsigned int& vOffset )
{

#ifdef DEBUG
	PGO					(Msg("PGO:VB_LOCK:%d",vl_Count));
	VERIFY				(0==dbg_lock);
	dbg_lock			++;
#endif // DEBUG

	// Ensure there is enough space in the VB for this data
	unsigned int	bytes_need		= vl_Count*Stride;
	R_ASSERT			((bytes_need<=mSize) && vl_Count);

	// Vertex-local info
	unsigned int vl_mSize		= mSize/Stride;
	unsigned int vl_mPosition	= mPosition/Stride + 1;

	// Check if there is need to flush and perform lock
	BYTE* pData			= 0;
	if ((vl_Count+vl_mPosition) >= vl_mSize)
	{
		// FLUSH-LOCK
		mPosition			= 0;
		vOffset				= 0;
		mDiscardID			++;

		pVB->Lock			( mPosition, bytes_need, (void**)&pData, LOCKFLAGS_FLUSH);
	} else {
		// APPEND-LOCK
		mPosition			= vl_mPosition*Stride;
		vOffset				= vl_mPosition;

		pVB->Lock			( mPosition, bytes_need, (void**)&pData, LOCKFLAGS_APPEND);
	}
	VERIFY				( pData );

	return LPVOID		( pData );
}

void	_VertexStream::Unlock		(unsigned int Count, unsigned int Stride)
{

#ifdef DEBUG
	PGO					(Msg("PGO:VB_UNLOCK:%d",Count));
	VERIFY				(1==dbg_lock);
	dbg_lock			--;
#endif // DEBUG

	mPosition			+=	Count*Stride;

	VERIFY				(pVB);
	pVB->Unlock			();
}

void	_VertexStream::reset_begin	()
{
	old_pVB				= pVB;
	Destroy				();
}
void	_VertexStream::reset_end	()
{
	Create				();
	//old_pVB				= NULL;
}

//////////////////////////////////////////////////////////////////////////
void	_IndexStream::Create	()
{
	Device.Resources->Evict		();

	mSize					= rsDIB_Size*1024;
	R_CHK					(HW.pDevice->CreateIndexBuffer( mSize, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB, NULL));
	R_ASSERT				(pIB);

	mPosition				= 0;
	mDiscardID				= 0;

	Msg("* DIB created: %dK", mSize/1024);
}

void	_IndexStream::Destroy()
{
	_RELEASE				(pIB);
	_clear					();
}

unsigned short*	_IndexStream::Lock	(unsigned int Count, unsigned int& vOffset )
{
	PGO						(Msg("PGO:IB_LOCK:%d",Count));
	vOffset					= 0;
	BYTE* pLockedData		= 0;

	// Ensure there is enough space in the VB for this data
	R_ASSERT				((2*Count<=mSize) && Count);

	// If either user forced us to flush,
	// or there is not enough space for the index data,
	// then flush the buffer contents
	unsigned int dwFlags = LOCKFLAGS_APPEND;
	if ( 2*( Count + mPosition ) >= mSize )
	{
		mPosition	= 0;						// clear position
		dwFlags		= LOCKFLAGS_FLUSH;			// discard it's contens
		mDiscardID	++;
	}
	pIB->Lock				( mPosition * 2, Count * 2, (void**) &pLockedData, dwFlags);
	VERIFY					(pLockedData);

	vOffset					=	mPosition;

	return					LPWORD(pLockedData);
}

void	_IndexStream::Unlock(unsigned int RealCount)
{
	PGO						(Msg("PGO:IB_UNLOCK:%d",RealCount));
	mPosition				+=	RealCount;
	VERIFY					(pIB);
	pIB->Unlock				();
}

void	_IndexStream::reset_begin	()
{
	old_pIB				= pIB;
	Destroy				();
}
void	_IndexStream::reset_end	()
{
	Create				();
	//old_pIB				= NULL;
}
