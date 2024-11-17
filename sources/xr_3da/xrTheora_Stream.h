#pragma once

#include <theora/theora.h>
#include "../xrCore/StreamReader.h"

class ENGINE_API CTheoraStream
{
	friend	class		CTheoraSurface;

	ogg_sync_state		o_sync_state;
	ogg_page			o_page;
	ogg_stream_state	o_stream_state;
	theora_info			t_info;
	theora_comment		t_comment;
	theora_state		t_state;

	CStreamReader* source;

	yuv_buffer			t_yuv_buffer;

	ogg_int64_t			d_frame;
	u32					tm_total;
	u32					key_rate;			// theora have const key rate
	f32				fpms;

protected:
	s32					ReadData( );
	BOOL				ParseHeaders( );

public:
	CTheoraStream( );
	virtual				~CTheoraStream( );

	BOOL				Load(pcstr fname);

	void				Reset( );

	BOOL				Decode(u32 tm_play);

	yuv_buffer* CurrentFrame( )
	{
		return &t_yuv_buffer;
	}
};
