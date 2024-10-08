///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Sphere-AABB overlap test, based on Jim Arvo's code.
 *	\param		center		[in] box center
 *	\param		extents		[in] box extents
 *	\return		TRUE on overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline_ BOOL SphereCollider::SphereAABBOverlap(const Point& center, const Point& extents)
{ 
	// Stats
	mNbVolumeBVTests++;

	F32 d = 0.0f;

	//find the square of the distance
	//from the sphere to the box

//	F32 tmp = mCenter.x - center.x;
//	F32 s = tmp + extents.x;

	F32 tmp,s;

	tmp = mCenter.x - center.x;
	s = tmp + extents.x;

	if(s<0.0f)
	{
		d += s*s;
		if(d>mRadius2)	return FALSE;
	}
	else
	{
		s = tmp - extents.x;
		if(s>0.0f)
		{
			d += s*s;
			if(d>mRadius2)	return FALSE;
		}
	}

	tmp = mCenter.y - center.y;
	s = tmp + extents.y;

	if(s<0.0f)
	{
		d += s*s;
		if(d>mRadius2)	return FALSE;
	}
	else
	{
		s = tmp - extents.y;
		if(s>0.0f)
		{
			d += s*s;
			if(d>mRadius2)	return FALSE;
		}
	}

	tmp = mCenter.z - center.z;
	s = tmp + extents.z;

	if(s<0.0f)
	{
		d += s*s;
		if(d>mRadius2)	return FALSE;
	}
	else
	{
		s = tmp - extents.z;
		if(s>0.0f)
		{
			d += s*s;
			if(d>mRadius2)	return FALSE;
		}
	}

	return d <= mRadius2;
}
