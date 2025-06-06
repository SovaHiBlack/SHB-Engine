#include	"stdafx.h"

#include	"ik_anim_state.h"

#include	"..\XR_3DA\skeletonanimated.h"


IC bool is_in( const motion_marks::interval &i, f32 v )
{
	if( i.first < i.second )
		return i.first < v && i.second > v;
	else
		return i.first < v || i.second > v;
}

bool blend_in( const CBlend &b, const motion_marks& marks )
{
	VERIFY					(!fis_zero(b.timeTotal));

	f32 blend_time		= ( b.timeCurrent/b.timeTotal ) ;
	blend_time				-= floor( blend_time );
	return marks.pick_mark	( blend_time * b.timeTotal );

//.	return	is_in( interval , blend_time );
}

void	ik_anim_state::update		( CKinematicsAnimated *K, const	CBlend *b, u16 i )
{
 //Andy	is_step = m && b && blend_in( *b, m->get_interval( i ) );
	VERIFY( K );
	is_step = false;
	if( !b || b->blendAmount <  b->blendPower - EPSILON_3)
		return;
	CMotionDef	&MD = *K->LL_GetMotionDef( b->motionID );

	if( MD.marks.size() <= i )
		return;
	is_step =  blend_in( *b, MD.marks[i] );//MD.marks[i].pick_mark( b->timeCurrent );
}
