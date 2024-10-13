/*
  This source code is a part of IKAN.
  Copyright (c) 2000 University of Pennsylvania
  Center for Human Modeling and Simulation
  All Rights Reserved.

  IN NO EVENT SHALL THE UNIVERSITY OF PENNSYLVANIA BE LIABLE TO ANY
  PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
  DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
  SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF PENNSYLVANIA
  HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes,
  without fee, and without a written agreement is hereby granted,
  provided that the above copyright notice and the following three
  paragraphs appear in all copies. For for-profit purposes, please
  contact University of Pennsylvania
 (http://hms.upenn.edu/software/ik/ik.html) for the software license
  agreement.


  THE UNIVERSITY OF PENNSYLVANIA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
  BASIS, AND THE UNIVERSITY OF PENNSYLVANIA HAS NO OBLIGATION
  TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
  MODIFICATIONS.

 */
#ifndef _AINTH
#define _AINTH

//#ifdef _WINDOWS
//#define M_PI            3.14159265358979323846
//#endif

#define AINT_EPSILON     (1e-5f)
#define AINT_BIG_EPSILON (.01f)

inline int equal(f32 x, f32 y, const f32 eps = AINT_EPSILON)
{
	return (_abs(x - y) < eps);
}

inline int istwopi(f32 x, const f32 eps = AINT_EPSILON)
{
	return equal(x, 2.0f*M_PI, eps);
}

inline int iszero(f32 x, const f32 eps = AINT_EPSILON)
{
	return _abs(x) < eps;
}

inline int le(f32 x, f32 y, const f32 eps = AINT_EPSILON)
{
	return (x < y) || equal(x,y,eps);
}

inline int ge(f32 x, f32 y, const f32 eps = AINT_EPSILON)
{
	return (x > y) || equal(x,y,eps);
}
			  

//
// Puts an angle in the range 0..2*PI
// 
//inline f32 angle_normalize(f32 psi)
//{
//    if (fabs(psi-2*M_PI) < AINT_EPSILON)
//	psi = 2*M_PI;
//    else
//	while (psi > 2*M_PI)  psi -= 2* M_PI;
//
//    if (fabs(psi) < AINT_EPSILON)
//	psi = 0.0;
//    else
//	while (psi < 0) psi += 2* M_PI;
//
//    return psi;
//}


//
// Returns the closest distance between two angles measured either
// clockwise or counterclockwise. Always return a positive value

inline f32 angle_distance(f32 a1, f32 a2)
{
	f32 t1;
	f32 t2;

	a1 = angle_normalize(a1);
	a2 = angle_normalize(a2);

	if (a1 > a2)
	{
	t1 = 2*M_PI - a1 + a2;
	t2 = a1 - a2;
	}
	else
	{
	t1 = 2*M_PI - a2 + a1;
	t2 = a2 - a1;
	}
	if (t2 < t1)
	t1 = t2;
	if (t1 < AINT_EPSILON)
	t1 = 0.0;

	return t1;
}

//
// Stores a legal range of angles corresponding to a joint limit or something
// analogous. All angles are mapped to the range 0..2PI
//
// Angle ranges are represented in counterclockwise notation so low does
// not have to less than high
// 
// eg: low = 275 deg, high = 30 deg means that any angle in the range 275..360 or 0..30 
//     is legal
// By contrast  
//     low = 30, high = 275 means that any angle in the range 30..275 is legal
//


class AngleInt
{
private:
	friend class AngleIntIterator;
	friend class AngleIntList;
	f32 low;
	f32 high;

	int merge_aux(const AngleInt &a, AngleInt &b, f32 eps) const;
	int merge(const AngleInt &a, AngleInt &b, f32 eps) const;

public:
	// Splits an angle interval of the form low > high into two intervals with high > low
	void split(AngleInt &l, AngleInt &h) const
	{
	l.Set(0, high);
	h.Set(low, 2*M_PI);
	}

	AngleInt() : low(0), high(2*M_PI) {}
	AngleInt(f32 l, f32 h);

	void SetLow(f32 l);
	void SetHigh(f32 l);
	void Set(f32 l, f32 h) { SetLow(l); SetHigh(h); }

	f32 Low() const { return low; }
	f32 High() const { return high; }

	int IsFullRange(f32 eps = AINT_BIG_EPSILON) const
	{
		return _abs(high-2*M_PI) < eps && _abs(low) < eps;
	}

	int IsEmpty(f32 eps = AINT_BIG_EPSILON) const
	{
		if (low <= high)
			return (
			_abs(low-high) < eps);
		else
			return (_abs(low-2*M_PI) + _abs(high) < eps);
		}

		// returns T if a is in the angle range
		int InRange(f32 a, f32 eps = AINT_EPSILON) const
		{
		if (IsEmpty())
			return 0;

		a = angle_normalize(a);
		if (iszero(a) || istwopi(a))
			return (low > high) || iszero(low) || istwopi(high);
		else
			return (low < high) ? 
			le(low,a,eps) && le(a,high,eps) : le(a,high,eps) || ge(a,low,eps);
	}


	// returns the magnitude of the angle between low and high
		f32 Range() const;

	// returns the midpoint of the range
		f32 Mid() const;

	int IsSubsetOf(const AngleInt& a, f32 eps = AINT_BIG_EPSILON) const;
	int IsSupersetOf(const AngleInt& a, f32 eps = AINT_BIG_EPSILON) const;
	int OldIsSupersetOf(const AngleInt& a, f32 eps = AINT_BIG_EPSILON) const;

	// determines if a can be merged with *this. Returns 1 if the nodes
	// can be merged and returns the result in b


	// Returns how far a value is from being within the angle interval
	f32 Distance(f32 a) const;
};

//
// Class for iterating through angles
// 
class AngleIntIterator
{
	int count;
	int n;
	f32 x;
	f32 dx;

public:

	// Iterates through an angle range from low+eps .. high-eps num > 0 times
	// If reverse is 1 iterate outside the range
	AngleIntIterator(const AngleInt &a, int num, f32 eps, int reverse = 0);

	// Retrieves next value in iteration. Returns 0 if last value has
	// been obtained
	int Next(f32& a);
};

//
// An AngleIntList is used to store a set of AngleInts
// 
struct AngleIntListNode 
{
	AngleInt D;
	AngleIntListNode *next;
	short flag; 

	AngleIntListNode(f32 low, f32 high, AngleIntListNode *n)
	: D(low,high), next(n), flag(0) {}
};

class AngleIntList
{
	AngleIntListNode *head, *tail;
	friend class AngleIntListIterator;

	void remove(AngleIntListNode *t); 
	void add(f32 l, f32 h);

public:
	AngleIntList() : head(0), tail(0) {}

	void Clear()
	{
	while (head)
	{
		AngleIntListNode *temp = head;
		head = head->next;
		delete temp;
	}
	head = tail = 0;
	}

	~AngleIntList() { Clear(); }

	void AddList(AngleIntList &dest, f32 eps = AINT_BIG_EPSILON) const;

	void Copy(AngleIntList &dest) const;

	void Add(f32 l, f32 h, f32 eps = AINT_BIG_EPSILON);

	void Map(void (*f)(AngleInt &a, void *), void *data = 0) const
	{
	for (AngleIntListNode *t = head; t; t = t->next)
		f(t->D, data);
	}

	int IsEmpty() const { return !head; }

	AngleInt * Largest() const;

	// returns T if a is in the angle range of any of the entries
	int InRange(f32 a, f32 eps = AINT_BIG_EPSILON) const
	{
	for (AngleIntListNode *t = head; t; t = t->next)
		if (t->D.InRange(a, eps))
		return 1;
	return 0;
	}

	f32 Distance(f32 a) const;
	
	void wrap(f32 eps = AINT_BIG_EPSILON);

	int NumIntervals() const;
};

void Union(const AngleIntList &a,
	   const AngleIntList &b,
	   AngleIntList &c);

void Intersect(const AngleIntList &a,
		   const AngleIntList &b,
		   AngleIntList &c);


//
// Corresponding iterator
//
class AngleIntListIterator
{
	AngleIntListNode *a;

public:
	AngleIntListIterator() { a = 0; } 

	void Start(const AngleIntList &A)
	{ a = A.head; } 

	AngleIntListIterator(const AngleIntList &A)
	{ Start(A); }

	AngleInt *Next()
	{
	AngleIntListNode *t = a;
	if (a)
		a = a->next;
	return t ? &t->D : 0;
	}
};

#endif
