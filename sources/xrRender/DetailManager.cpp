// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DetailManager.h"
#include "..\XR_3DA\cl_intersect.h"

#include "..\XR_3DA\igame_persistent.h"
#include "..\XR_3DA\environment.h"

const f32 dbgOffset			= 0.f;
const int	dbgItems			= 128;

//--------------------------------------------------- Decompression
static int magic4x4[4][4] =
{
 	{ 0, 14,  3, 13},
	{11,  5,  8,  6},
	{12,  2, 15,  1},
	{ 7,  9,  4, 10}
};

void bwdithermap	(int levels, int magic[16][16])
{
	/* Get size of each step */
	f32 N = 255.0f / (levels - 1);

	/*
	* Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
	* and doesn't give us full intensity range (only 17 sublevels).
	*
	* magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
	* N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
	* pixels incremented to the next level (this is reserved for the
	* pixel value with mod N == 0 at the next level).
	*/

	f32	magicfact = (N - 1) / 16;
    for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			for ( int k = 0; k < 4; k++ )
				for ( int l = 0; l < 4; l++ )
					magic[4*k+i][4*l+j] =
					(int)(0.5 + magic4x4[i][j] * magicfact +
					(magic4x4[k][l] / 16.) * magicfact);
}
//--------------------------------------------------- Decompression

void CDetailManager::SSwingValue::lerp(const SSwingValue& A, const SSwingValue& B, f32 f)
{
	f32 fi	= 1.f-f;
	amp1		= fi*A.amp1  + f*B.amp1;
	amp2		= fi*A.amp2  + f*B.amp2;
	rot1		= fi*A.rot1  + f*B.rot1;
	rot2		= fi*A.rot2  + f*B.rot2;
	speed		= fi*A.speed + f*B.speed;
}
//---------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager	()
{
	dtFS 		= 0;
	dtSlots		= 0;
	soft_Geom	= 0;
	hw_Geom		= 0;
	hw_BatchSize= 0;
	hw_VB		= 0;
	hw_IB		= 0;
}

CDetailManager::~CDetailManager	()
{ }

/*
void dump	(CDetailManager::vis_list& lst)
{
	for (int i=0; i<lst.size(); i++)
	{
		Msg("%8x / %8x / %8x",	lst[i]._M_start, lst[i]._M_finish, lst[i]._M_end_of_storage._M_data);
	}
}
*/
void CDetailManager::Load		()
{
	// Open file stream
	if (!FS.exist("$level$","level.details"))
	{
		dtFS	= NULL;
		return;
	}

	string_path			fn;
	FS.update_path		(fn,"$level$","level.details");
	dtFS				= FS.r_open(fn);

	// Header
	dtFS->r_chunk_safe	(0,&dtH,sizeof(dtH));
	R_ASSERT			(dtH.version == DETAIL_VERSION);
	u32 m_count			= dtH.object_count;

	// Models
	IReader* m_fs		= dtFS->open_chunk(1);
	for (u32 m_id = 0; m_id < m_count; m_id++)
	{
		CDetail*		dt	= xr_new<CDetail> ();
		IReader* S			= m_fs->open_chunk(m_id);
		dt->Load			(S);
		objects.push_back	(dt);
		S->close			();
	}
	m_fs->close		();

	// Get pointer to database (slots)
	IReader* m_slots	= dtFS->open_chunk(2);
	dtSlots				= (DetailSlot*)m_slots->pointer();
	m_slots->close		();

	// Initialize 'vis' and 'cache'
	for (u32 i=0; i<3; ++i)	m_visibles[i].resize(objects.size());
	cache_Initialize	();

	// Make dither matrix
	bwdithermap		(2,dither);

	// Hardware specific optimizations
	if (UseVS())	hw_Load		();
	else			soft_Load	();

	// swing desc
	// normal
	swing_desc[0].amp1	= pSettings->r_float("details","swing_normal_amp1");
	swing_desc[0].amp2	= pSettings->r_float("details","swing_normal_amp2");
	swing_desc[0].rot1	= pSettings->r_float("details","swing_normal_rot1");
	swing_desc[0].rot2	= pSettings->r_float("details","swing_normal_rot2");
	swing_desc[0].speed	= pSettings->r_float("details","swing_normal_speed");
	// fast
	swing_desc[1].amp1	= pSettings->r_float("details","swing_fast_amp1");
	swing_desc[1].amp2	= pSettings->r_float("details","swing_fast_amp2");
	swing_desc[1].rot1	= pSettings->r_float("details","swing_fast_rot1");
	swing_desc[1].rot2	= pSettings->r_float("details","swing_fast_rot2");
	swing_desc[1].speed	= pSettings->r_float("details","swing_fast_speed");
}

void CDetailManager::Unload		()
{
	if (UseVS())	hw_Unload	();
	else			soft_Unload	();

	for (DetailIt it=objects.begin(); it!=objects.end(); it++){
		(*it)->Unload();
		xr_delete		(*it);
    }
	objects.clear		();
	m_visibles[0].clear	();
	m_visibles[1].clear	();
	m_visibles[2].clear	();
	FS.r_close			(dtFS);
}

extern f32 r_ssaDISCARD;

void CDetailManager::UpdateVisibleM()
{
	fVector3		EYE				= Device.vCameraPosition;
	
	CFrustum	View;
	View.CreateFromMatrix		(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

	f32 fade_limit			= dm_fade;	fade_limit=fade_limit*fade_limit;
	f32 fade_start			= 1.f;		fade_start=fade_start*fade_start;
	f32 fade_range			= fade_limit-fade_start;
	f32		r_ssaCHEAP		= 16*r_ssaDISCARD;

	// Initialize 'vis' and 'cache'
	// Collect objects for rendering
	Device.Statistic->RenderDUMP_DT_VIS.Begin	();
	for (int _mz=0; _mz<dm_cache1_line; _mz++){
		for (int _mx=0; _mx<dm_cache1_line; _mx++){
			CacheSlot1& MS		= cache_level1[_mz][_mx];
			if (MS.empty)		continue;
			u32 mask			= 0xff;
			u32 res				= View.testSAABB		(MS.vis.sphere.P,MS.vis.sphere.R,MS.vis.box.data(),mask);
			if (fcvNone==res)						 	continue;	// invisible-view frustum
			// test slots
			for (int _i=0; _i<dm_cache1_count*dm_cache1_count; _i++){
				Slot*	PS		= *MS.slots[_i];
				Slot& 	S 		= *PS;

				// if slot empty - continue
				if (S.empty)	continue;

				// if upper test = fcvPartial - test inner slots
				if (fcvPartial==res){
					u32 _mask	= mask;
					u32 _res	= View.testSAABB			(S.vis.sphere.P,S.vis.sphere.R,S.vis.box.data(),_mask);
					if (fcvNone==_res)						continue;	// invisible-view frustum
				}

				if (!RImplementation.HOM.visible(S.vis))	continue;	// invisible-occlusion

				// Add to visibility structures
				if (Device.dwFrame>S.frame){
					// Calc fade factor	(per slot)
					f32	dist_sq		= EYE.distance_to_sqr	(S.vis.sphere.P);
					if		(dist_sq>fade_limit)				continue;
					f32	alpha		= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
					f32	alpha_i		= 1.f - alpha;
					f32	dist_sq_rcp	= 1.f / dist_sq;

					S.frame			= Device.dwFrame+Random.randI(15,30);
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++){
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==DetailSlot::ID_Empty)	continue;

						sp.r_items[0].clear_not_free();
						sp.r_items[1].clear_not_free();
						sp.r_items[2].clear_not_free();

						f32				R		= objects	[sp.id]->bv_sphere.R;
						f32				Rq_drcp	= R*R*dist_sq_rcp;	// reordered expression for 'ssa' calc

						SlotItem			**siIT=&(*sp.items.begin()), **siEND=&(*sp.items.end());
						for (; siIT!=siEND; siIT++){
							SlotItem& Item			= *(*siIT);
							f32 scale			= Item.scale_calculated	= Item.scale*alpha_i;
							f32	ssa				= scale*scale*Rq_drcp;
							if (ssa < r_ssaDISCARD) continue;
							u32		vis_id			= 0;
							if (ssa > r_ssaCHEAP)	vis_id = Item.vis_ID;
							
							sp.r_items[vis_id].push_back	(*siIT);

//2							visible[vis_id][sp.id].push_back(&Item);
						}
					}
				}
				for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++){
					SlotPart&			sp	= S.G		[sp_id];
					if (sp.id==DetailSlot::ID_Empty)	continue;
					if (!sp.r_items[0].empty()) m_visibles[0][sp.id].push_back(&sp.r_items[0]);
					if (!sp.r_items[1].empty()) m_visibles[1][sp.id].push_back(&sp.r_items[1]);
					if (!sp.r_items[2].empty()) m_visibles[2][sp.id].push_back(&sp.r_items[2]);
				}
			}
		}
	}
	Device.Statistic->RenderDUMP_DT_VIS.End	();
}

void CDetailManager::Render	()
{
	if (0==dtFS)						return;
	if (!psDeviceFlags.is(rsDetails))	return;

	// MT
	MT_SYNC					();

	Device.Statistic->RenderDUMP_DT_Render.Begin	();

	f32 factor			= g_pGamePersistent->Environment().wind_strength_factor;
	swing_current.lerp		(swing_desc[0],swing_desc[1],factor);

	RCache.set_CullMode		(CULL_NONE);
	RCache.set_xform_world	(Fidentity);
	if (UseVS())			hw_Render	();
	else					soft_Render	();
	RCache.set_CullMode		(CULL_CCW);
	Device.Statistic->RenderDUMP_DT_Render.End	();
	m_frame_rendered		= Device.dwFrame;
}

void __stdcall	CDetailManager::MT_CALC		()
{
	if (0==RImplementation.Details)		return;	// possibly deleted
	if (0==dtFS)						return;
	if (!psDeviceFlags.is(rsDetails))	return;

	MT.Enter					();
	if (m_frame_calc!=Device.dwFrame)	
		if ((m_frame_rendered+1)==Device.dwFrame) //already rendered
		{
			fVector3		EYE				= Device.vCameraPosition;
			int s_x	= iFloor			(EYE.x/dm_slot_size+.5f);
			int s_z	= iFloor			(EYE.z/dm_slot_size+.5f);

			Device.Statistic->RenderDUMP_DT_Cache.Begin	();
			cache_Update				(s_x,s_z,EYE,dm_max_decompress);
			Device.Statistic->RenderDUMP_DT_Cache.End	();

			UpdateVisibleM				();
			m_frame_calc				= Device.dwFrame;
		}

	MT.Leave					        ();
}
