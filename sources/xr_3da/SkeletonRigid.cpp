#include "stdafx.h"

#include "SkeletonCustom.h"

extern s32	psSkeletonUpdate;

#ifdef DEBUG
void check_kinematics(CKinematics* _k, pcstr s);
#endif

void CKinematics::CalculateBones(BOOL bForceExact)
{
	// early out.
	// check if the info is still relevant
	// skip all the computations - assume nothing changes in a small period of time :)
	if (Device.dwTimeGlobal == UCalc_Time)	// early out for "fast" update
	{
		return;
	}

	UCalc_mtlock	lock;
	OnCalculateBones( );
	if (!bForceExact && (Device.dwTimeGlobal < (UCalc_Time + UCalc_Interval)))	// early out for "slow" update
	{
		return;
	}

	if (Update_Visibility)
	{
		Visibility_Update( );
	}

	_DBG_SINGLE_USE_MARKER;
	// here we have either:
	//	1:	timeout elapsed
	//	2:	exact computation required
	UCalc_Time = Device.dwTimeGlobal;

	// exact computation
	// Calculate bones
#ifdef DEBUG
	Device.Statistic->Animation.Begin( );
#endif

	Bone_Calculate(bones->at(iRoot), &Fidentity);

#ifdef DEBUG
	check_kinematics(this, dbg_name.c_str( ));
	Device.Statistic->Animation.End( );
#endif

	// Calculate BOXes/Spheres if needed
	UCalc_Visibox++;
	if (UCalc_Visibox >= psSkeletonUpdate)
	{
		// mark
		UCalc_Visibox = -(::Random.randI(psSkeletonUpdate - 1));

		// the update itself
		fBox3	Box;
		Box.invalidate( );
		for (u32 b = 0; b < bones->size( ); b++)
		{
			if (!LL_GetBoneVisible(u16(b)))
			{
				continue;
			}

			fObb& obb = (*bones)[b]->obb;
			fMatrix4x4& Mbone = bone_instances[b].mTransform;
			fMatrix4x4 Mbox;
			obb.xform_get(Mbox);
			fMatrix4x4 X;
			X.mul_43(Mbone, Mbox);
			fVector3& S = obb.m_halfsize;

			fVector3 P;
			fVector3 A;
			A.set(-S.x, -S.y, -S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
			A.set(-S.x, -S.y, S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
			A.set(S.x, -S.y, S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
			A.set(S.x, -S.y, -S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
			A.set(-S.x, S.y, -S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
			A.set(-S.x, S.y, S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
			A.set(S.x, S.y, S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
			A.set(S.x, S.y, -S.z);
			X.transform_tiny(P, A);
			Box.modify(P);
		}

		if (bones->size( ))
		{
			// previous frame we have updated box - update sphere
			vis.box.min = (Box.min);
			vis.box.max = (Box.max);
			vis.box.getsphere(vis.sphere.P, vis.sphere.R);
		}

#ifdef DEBUG
		// Validate
		VERIFY3(_valid(vis.box.min) && _valid(vis.box.max), "Invalid bones-xform in model", dbg_name.c_str( ));
		if (vis.sphere.R > 1000.f)
		{
			for (u16 ii = 0; ii < LL_BoneCount( ); ++ii)
			{
				fMatrix4x4 tr;
				tr = LL_GetTransform(ii);
				Log("bone ", LL_BoneName_dbg(ii));
				Log("bone_matrix", tr);
			}

			Log("end-------");
		}

		VERIFY3(vis.sphere.R < 1000.f, "Invalid bones-xform in model", dbg_name.c_str( ));
#endif

	}

	//
	if (Update_Callback)
	{
		Update_Callback(this);
	}
}

#ifdef DEBUG
void check_kinematics(CKinematics* _k, pcstr s)
{
	CKinematics* K = _k;
	fMatrix4x4& MrootBone = K->LL_GetBoneInstance(K->LL_GetBoneRoot( )).mTransform;
	if (MrootBone.c.y > 10000)
	{
		Msg("all bones transform:--------[%s]", s);

		for (u16 ii = 0; ii < K->LL_BoneCount( ); ++ii)
		{
			fMatrix4x4 tr;
			tr = K->LL_GetTransform(ii);
			Log("bone ", K->LL_BoneName_dbg(ii));
			Log("bone_matrix", tr);
		}

		Log("end-------");
		VERIFY3(0, "check_kinematics failed for ", s);
	}
}
#endif

void CKinematics::Bone_Calculate(CBoneData* bd, fMatrix4x4* parent)
{
	u16 SelfID = bd->GetSelfID( );
	if (LL_GetBoneVisible(SelfID))
	{
		CBoneInstance& INST = LL_GetBoneInstance(SelfID);
		if (INST.Callback_overwrite)
		{
			if (INST.Callback)
			{
				INST.Callback(&INST);
			}
		}
		else
		{
			// Build matrix
			INST.mTransform.mul_43(*parent, bd->bind_transform);
			if (INST.Callback)
			{
				INST.Callback(&INST);
			}
		}

		INST.mRenderTransform.mul_43(INST.mTransform, bd->m2b_transform);

		// Calculate children
		for (xr_vector<CBoneData*>::iterator C = bd->children.begin( ); C != bd->children.end( ); C++)
		{
			Bone_Calculate(*C, &INST.mTransform);
		}
	}
}
