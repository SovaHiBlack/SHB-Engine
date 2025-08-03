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


#ifndef _EQNH
#define _EQNH


#include "aint.h"

//
// Evaluate alpha*cos(x) + beta*sin(x) efficiently
// 

inline f32 sin_and_cos(f32 x, f32 alpha, f32 beta)
{
	while (x < 0)
	x += 2*M_PI;
	while (x > 2*M_PI)
	x -= 2*M_PI;

	f32 c = _cos(x);
	return (x > M_PI) ?
	(alpha*c - beta*_sqrt(1-c*c)) : 
	(alpha*c + beta*_sqrt(1-c*c));
}


//
// A class representing the equation alpha*cos(psi) + beta*sin(psi) + xi
// where alpha,beta,xi are such that
//	-1 <= alpha*cos(psi) + beta*sin(psi) + xi <= 1
// and
//      -Pi <= psi <= Pi 
//


struct PsiEquation
{
	unsigned char  status, num_crits, num_roots;
	unsigned char *status_ptr, *num_roots_ptr, *num_crits_ptr;
	f32 alpha;
	f32 beta;
	f32 xi;
	f32 crit_pts[2];
	f32 root_pts[2];

	// store temporary computations
	f32 a2b2;
	f32 atan2ba;

	void Reset(f32 a, f32 b, f32 x)
	{ 
		alpha = a; beta = b; xi = x;  
		a2b2 = a*a + b*b;
		atan2ba = atan2(b,a);
		num_crits = num_roots = status = 0;
		status_ptr = &status;
		num_roots_ptr = &num_roots;
		num_crits_ptr = &num_crits;
	}

	PsiEquation() {}
	PsiEquation(f32 a, f32 b, f32 x)
	{
		Reset(a,b,x);
	}


	f32 eval(f32 psi) const
	{
		return sin_and_cos(psi, alpha, beta) + xi;
		// return alpha*cos(psi) + beta*sin(psi) + xi;
	}

	f32 deriv(f32 psi) const
	{
		return sin_and_cos(psi, beta, -alpha);
		// return -alpha*sin(psi) + beta*cos(psi); 
	}


	//
	// Returns the critical points of the equation (1 or 2) 
	//
	s32 crit_points(f32* c) const;

	//
	// Return the roots of the equation (1 or 2)
	// 
	s32 roots(f32* c) const;

	//
	// Returns the solns of alpha*cos(v) + beta*sin(v) + xi = v
	// where -1 <= v <= 1 
	//
	s32 solve(f32 v, f32* c) const;

	//
	// Calculates the set of intersections of psi such that 
	//		
	//  -1 <= low <=  alpha*cos(psi) + beta*sin(psi) + xi <=  high <= 1
	//
	// Returns the number of these regions between 0..3
	//
	// For eg, if the routine returns 1 then all angles between psi[0] and psi[1]
	// satisfy the constraints above.
	//
	//    s32 clip(f32 low, f32 high, AngleIntList &a) const;
		
	//
	// Calculates the range of psi above and below the specified value y
	//
	// returns the number of times the curve intersects y from 0 to 2  
	//
	// s32 partition(f32 y, AngleIntList &above,  AngleIntList &below) const;

	
};

#endif
