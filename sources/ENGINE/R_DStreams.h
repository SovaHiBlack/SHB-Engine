#pragma once

enum
{
	LOCKFLAGS_FLUSH = D3DLOCK_DISCARD,
	LOCKFLAGS_APPEND = D3DLOCK_NOOVERWRITE
};

class ENGINE_API _VertexStream
{
private:
	IDirect3DVertexBuffer9* pVB;
	unsigned int							mSize;			// size in bytes
	unsigned int							mPosition;		// position in bytes
	unsigned int							mDiscardID;		// ID of discard - usually for caching

public:
	IDirect3DVertexBuffer9* old_pVB;

#ifdef DEBUG
	unsigned int							dbg_lock;
#endif // DEBUG

private:
	void						_clear( )
	{
		pVB = NULL;
		mSize = 0;
		mPosition = 0;
		mDiscardID = 0;

#ifdef DEBUG
		dbg_lock = 0;
#endif // DEBUG

	}
public:
	void						Create( );
	void						Destroy( );
	void						reset_begin( );
	void						reset_end( );

	inline IDirect3DVertexBuffer9* Buffer( )
	{
		return pVB;
	}
	inline unsigned int						DiscardID( )
	{
		return mDiscardID;
	}
	inline void						Flush( )
	{
		mPosition = mSize;
	}

	void* Lock(unsigned int vl_Count, unsigned int Stride, unsigned int& vOffset);
	void						Unlock(unsigned int Count, unsigned int Stride);

	_VertexStream( )
	{
		_clear( );
	};
	~_VertexStream( )
	{
		Destroy( );
	};
};

class ENGINE_API _IndexStream
{
private:
	IDirect3DIndexBuffer9* pIB;
	unsigned int							mSize;		// real size (usually mCount, aligned on 512b boundary)
	unsigned int							mPosition;
	unsigned int							mDiscardID;

public:
	IDirect3DIndexBuffer9* old_pIB;

private:
	void						_clear( )
	{
		pIB = nullptr;
		mSize = 0;
		mPosition = 0;
		mDiscardID = 0;
	}

public:
	void						Create( );
	void						Destroy( );
	void						reset_begin( );
	void						reset_end( );

	inline IDirect3DIndexBuffer9* Buffer( )
	{
		return pIB;
	}
	inline unsigned int						DiscardID( )
	{
		return mDiscardID;
	}
	void						Flush( )
	{
		mPosition = mSize;
	}

	unsigned short* Lock(unsigned int Count, unsigned int& vOffset);
	void						Unlock(unsigned int RealCount);

	_IndexStream( )
	{
		_clear( );
	}
	~_IndexStream( )
	{
		Destroy( );
	}
};
