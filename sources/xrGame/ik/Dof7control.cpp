 
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
#include "stdafx.h"
#include "dof7control.h"
#include "mathTrig.h" 

 
// #define SRSDEBUG



//
// Constructor stores the T and S matrices and the 
// lengths of the upper and lower links
// 
void SRS::init(const Matrix  T1, const Matrix  T2, const f32 a[3], const f32 p[3])
{
	cpmatrix(T, T1); 
	cpmatrix(S, T2); 
	cpvector(proj_axis, a);
	cpvector(pos_axis, p);

	f32 t[3];

	get_translation(T, t);
	upper_len = norm(t);
	reciprocal_upper_len = 1.0f / upper_len;

	get_translation(S, t);
	lower_len = norm(t);

	project_to_workspace = 1;
}

//
// Given the goal position and the position vectors s and t of 
// the S and T matrices, solve for the angle of the R joint 
// according to the formula (here g and s are row vectors)
//
// ([s,1]*Ry*T*R1)*([s,1]*Ry*T*R1)' = g*g'
// 
// which says the distance from R1 to the tip of the last link
// is equal to the distance to the goal.
//
// This equation simplifies to 
//      s*Rot(Ry)*(Rot(T))*t' = g'*g - s'*s - t'*t
// where Rot(M) is the 3x3 rotation matrix of M
// and this equation is of the form
//     alpha * cos(theta_y) * beta * sin(theta_y) = gamma
//
// Only return positive solution
//
static s32 solve_R_angle(const f32 g[3],
			 const f32 s[3],
			 const f32 t[3],
			 const Matrix  T, 
						 f32& r_angle)
{
	f32 rhs = DOT(g,g) - DOT(s,s) - DOT(t,t);

	// alpha = Rot(T)*t' NOT alpha = t*Rot(T)

	f32 alpha[3];

	for (s32 j = 0; j < 3; j++)
	{
	alpha[j] = 0;
	for (s32 i = 0; i < 3; i++)
		alpha[j] += T[j][i] * t[i]; 
	}

	f32 a = alpha[0]*s[0] + alpha[2]*s[2];
	f32 b = alpha[0]*s[2] - alpha[2]*s[0];
	f32 c = alpha[1]*s[1];

	s32 n;
	f32 temp[2];

	a += a; 
	b += b;
	c = rhs - (c+c);
	
	n = solve_trig1(a, b, c, temp);

	if (n == 2)
	{
		// Two positive solutions. choose first
		if (temp[0] < 0 && temp[1] < 0)
		{
			r_angle = temp[0];
			// printf("Two solutions: %lf %lf\n", temp[0], temp[1]);
			n = 1;
		}
		else if (temp[0] < 0)
		{
			n = 1;
			r_angle = temp[0];
		}
		else if (temp[1] < 0)
		{
			n = 1;
			r_angle = temp[1];
		}
		else
		{
			n = 1;
			r_angle = temp[1];//?
		}
	}
	else if (n == 1)
	{
	// Is solution positive ?
	if (temp[0] < 0) 
		n = 0;
	else 
		r_angle = temp[0]; 

	}

	return n;
}

//
// Computes the equation of the circle given the ee position,
// an axis to project the local 2d coordinate system {u,v} onto,
// and the upper and lower lengths of the mechanism.
//
// Outputs
//	c: center of circle
//      u: local x axis
//      v: local y axis
//      n: normal to plane of circle = cross(u,v)
// Returns radius of circle
//

f32 get_circle_equation(const f32 ee[3],
			  const f32 axis[3],
			  const f32 pos_axis[3],
						f32 upper_len,
						f32 lower_len,
						f32 c[3],
						f32 u[3],
						f32 v[3],
						f32 n[3])
{
	f32 wn = norm((f32*)ee);
	f32 radius;

	cpvector(n, ee);
	unitize(n);

	
	// Use law of cosines to get angle between first spherical joint
	// and revolute joint

	f32 alpha;

	if (!law_of_cosines(wn, upper_len, lower_len, alpha))
	return 0;

	// center of circle (origin is location of first S joint)
	vecscalarmult(c, n, _cos(alpha) * upper_len);

	
	radius = _sin(alpha) * upper_len;

	f32 temp[3];

	//
	// A little kludgy. If the goal is behind the joint instead
	// of in front of it, we reverse the angle measurement by
	// inverting the normal vector
	//

	if (DOT(n,pos_axis) < 0.0)
	vecscalarmult(n,n,-1.0);

	vecscalarmult(temp, n, DOT(axis,n));
	vecsub(u, (f32*)axis, temp);
	unitize(u);

	crossproduct(v, n, u);
#if 0
	printf("Circle equation\n");
	printf("c = [%lf,%lf,%lf]\n", c[0], c[1], c[2]);
	printf("u = [%lf,%lf,%lf]\n", u[0], u[1], u[2]);
	printf("v = [%lf,%lf,%lf]\n", v[0], v[1], v[2]);
	printf("n = [%lf,%lf,%lf]\n", n[0], n[1], n[2]);
	printf("r = %lf\n", radius);
#endif
	return radius;
}

//
// Check if the goal is longer than the combined two 
// link lengths. If it is then scale it within some
// epsilon so that it lies within the boundary of the
// outer workspace.
//
// Also check if goal is shorter than the difference
// of the two combined link lengths. If it is then
// scale it so that it lies within the boundary of
// the inner workspace
//

s32 scale_goal(const f32 l1[3],
		   const f32 l2[3],
			   f32 g[3])
{
	f32 g_len = _sqrt(DOT(g,g));
	f32 L1    = _sqrt(DOT(l1,l1));
	f32 L2    = _sqrt(DOT(l2,l2));
	f32 max_len = (L1 + L2) * 0.9999f;
	//    f32 min_len = fabs(L1 - L2);

	if (g_len > max_len)
	{
	vecscalarmult(g,g,max_len/(g_len/**1.01f*/));
	return 1;
	}	
	/*
	if (g_len < min_len)
	{
	vecscalarmult(g,g,(1.01 * min_len)/g_len);
	return 1;
	}
	*/

	return 0;
} 



//
// Given a goal position and the projectio naxis, find the
// equation of the circle that defines how the R joint can
// swivel.
//
// E is a matrix that relates the R pos to the end effector
// site according to the product E*S 
//
// Subsequent calls to SolveR1 will solve the position equation
//
//     g = [0,0,0,1]*E*S*Ry*T*R1 
//

s32 SRS::SetGoalPos(const f32 eee[3],  const Matrix  E, f32& rangle)
{
	Matrix Temp, RY;
	f32 s[3];



	// Find RY, and store the positions of the R jt and 
	// the ee in the R1 frame as p_r1 and ee_r1


	get_translation(T, p_r1);
	hmatmult(Temp,(f32(*)[4]) E,S);
	get_translation(Temp, s);
	cpvector(ee,eee);

	if (project_to_workspace)
		scale_goal(p_r1,s,ee);

	// 
	// Note instead of using the length of the lower limb
	// we use the length of the lower limb extended by E
	//
	radius = get_circle_equation(ee, 
		 proj_axis, 
		 pos_axis,
		 upper_len, 
		 norm(s), c, u, v, n);

	if (!solve_R_angle(ee, s, p_r1, T, r_angle))
	return 0; 
	rangle = r_angle;

	// Find RY, and store the positions of the R jt and
	// the ee in the R1 frame as p_r1 and ee_r1

	rotation_principal_axis_to_matrix('y', r_angle, RY);
	hmatmult(Temp, Temp, RY); 
	hmatmult(Temp, Temp, T);
	get_translation(Temp, ee_r1);


	return 1; 
}

void SRS::EvaluateCircle(const f32 p[3])
{
	 radius = get_circle_equation(p, proj_axis, pos_axis, 
				 upper_len, lower_len, c, u, v, n);

}

//
// Given the goal matrix and the projection axis, find the position
// of the end effector and the equation of the circle that defines
// how the R joint can swivel. 
//
// Also compute the matrix S*RY*T and save it for future computations
//
s32 SRS::SetGoal(const Matrix  GG, f32& rangle)
{
	Matrix RY;
	f32 s[3];

	cpmatrix(G, GG);
	get_translation(G, ee);
	get_translation(T, p_r1);
	get_translation(S, s);

	if (project_to_workspace && scale_goal(p_r1,s,ee))
		set_translation(G,ee);

	EvaluateCircle(ee);
	//radius = get_circle_equation(ee, proj_axis, pos_axis, 
	//			 upper_len, lower_len, c, u, v, n);

	//
	// Build rotation matrix about the R joint
	//

	if (!solve_R_angle(ee, s, p_r1, T, r_angle))
	return 0; 
	r_angle=-r_angle;
	rangle = r_angle;

	// Find RY, and store the positions of the R jt and 
	// the ee in the R1 frame as p_r1 and ee_r1
	// Also save matrix product S*RY*T 

	rotation_principal_axis_to_matrix('y',-r_angle, RY);

	hmatmult(SRT, S, RY);
	hmatmult(SRT, SRT, T);
	get_translation(SRT, ee_r1);


#ifdef SRSDEBUG
	printf("EE distance error is %lf\n", norm(ee_r1) - norm(ee));
#endif

	return 1; 
}

IC void evalcircle(const f32 c[3],
			   const f32 u[3],
			   const f32 v[3],
					   f32 radius,
					   f32 angle,
					   f32 p[3])
{
	// p = o + r*cos(f)*u + r*sin(f)*v

	f32 temp[3];

	cpvector(p, c);
	vecscalarmult(temp, (f32*)u, radius*_cos(angle));
	vecadd(p, p, temp);
	vecscalarmult(temp, (f32*)v, radius*_sin(angle));
	vecadd(p, p, temp);
}

//
// Evaluate a point on the circle given the swivel angle 
//
void SRS::evaluate_circle(f32 angle, f32 p[3])
{
#if 1
	evalcircle(c, u, v, radius, angle, p);
#else
	// p = o + r*cos(f)*u + r*sin(f)*v

	f32 temp[3];

	cpvector(p, c);
	vecscalarmult(temp, u, radius*cos(angle));
	vecadd(p, p, temp);
	vecscalarmult(temp, v, radius*sin(angle));
	vecadd(p, p, temp);
#endif
}


//
// Form local coordinate system {x,y} from points p,q relative to the
// implicit origin 0. pscale is the reciprocal length of the p vector
// which as it turns out is already known. If the invert flag is true
// construct the transpose of the rotation matrix instead
//
IC void make_frame(const f32 p[3],
					   f32 p_scale,
			   const f32 q[3],
			   Matrix R, 
				   s32 invert = 0)
{
	f32 x[3];
	f32 y[3];
	f32 t[3];

	// x vector is unit vector from origin to p
	vecscalarmult(x, (f32*)p, p_scale);

	// y vector is unit perpendicular projection of q onto x 
	vecscalarmult(t, x, DOT(q,x));
	vecsub(y, (f32*) q, t);
	unitize(y);

	// z vector is x cross y

	if (invert)
	{
	R[0][0] = x[0];	R[1][0] = x[1];	R[2][0] = x[2];
	R[0][1] = y[0];	R[1][1] = y[1];	R[2][1] = y[2];

	R[0][2] = x[1]*y[2] - x[2]*y[1];
	R[1][2] = x[2]*y[0] - x[0]*y[2];
	R[2][2] = x[0]*y[1] - x[1]*y[0];
	}
	else
	{
	R[0][0] = x[0];	R[0][1] = x[1];	R[0][2] = x[2];
	R[1][0] = y[0];	R[1][1] = y[1];	R[1][2] = y[2];

	R[2][0] = x[1]*y[2] - x[2]*y[1];
	R[2][1] = x[2]*y[0] - x[0]*y[2];
	R[2][2] = x[0]*y[1] - x[1]*y[0];
	}

	R[3][0] = R[3][1] = R[3][2] = 
	R[0][3] = R[1][3] = R[2][3] = 0;

	R[3][3] = 1.0;
}


static void solve_R1(f32 p[3], f32 q[3],
					 f32 p2[3], f32 q2[3],
					 f32 p_scale, Matrix  R1)
{
	Matrix T, S;

	// Construct two local coordinate systems
	// and find the transformation between them
	make_frame(p,  p_scale, q,  T, 1);
	make_frame(p2, p_scale, q2, S, 0);
	rmatmult(R1,T,S);
}


//
// R1 is the rotation matrix that takes the position of the
// R jt and the last S jt in the R1 frame to their locations
// in the global frame
//  
void SRS::SolveR1(f32 angle, Matrix  R1)
{
	f32 p[3];

	evaluate_circle(angle, p);
	solve_R1(p_r1, ee_r1, p, ee, reciprocal_upper_len, R1);

#ifdef SRSDEBUG
	f32 t1[3], t2[3];
	vecsub(t1, p_r1, ee_r1);
	vecsub(t2, p, ee);

	printf("Elbow distance error is %lf\n", 
	   DOT(p_r1, p_r1) - DOT(p,p));
	printf("EE distance error is %lf\n", 
	   DOT(ee,ee) - DOT(ee_r1,ee_r1));
	printf("Distance between elbow and wrist error is %lf\n", 
	   DOT(t1,t1) - DOT(t2,t2));
#endif
}

void SRS::SolveR1R2(f32 angle, Matrix  R1, Matrix  R2)
{
	SolveR1(angle, R1);

	Matrix Temp; 

	rmatmult(R2, SRT, R1);
	invertrmatrix(Temp, R2);
	rmatmult(R2, G, Temp);

#ifdef SRSDEBUG
	Matrix G2;
	
	hmatmult(G2, R2, SRT);
	hmatmult(G2, G2, R1); 
	printf("Displaying the error matrix\n");
	for (s32 i = 0; i < 4; i++)
	{
	for (s32 j = 0; j < 4; j++)
		printf(" %lf ", fabs(G2[i][j] - G[i][j]));
	printf("\n");
	}
#endif
}


f32 SRS::PosToAngle(const f32 p[3])
{
	// Find vector from center of circle to pos and project it onto circle
	f32 cp[3];
	f32 pp[3];

	vecsub(cp, (f32*) p, c);
	project_plane(pp , cp, n);

	// Find angle between u and pp. This is the swivel angle
	
	return angle_between_vectors(u, pp, n); 
}

void SRS::AngleToPos(f32 psi, f32 p[3])
{
	evaluate_circle(psi, p);
}

void SRS::SolveR1(const f32 p[3], Matrix  R1)
{
	SolveR1(PosToAngle(p), R1);
}

void SRS::SolveR1R2(const f32 pos[3], Matrix  R1, Matrix  R2)
{
	SolveR1R2(PosToAngle(pos), R1, R2);
}


//
// Given an axis of rotation n, construct an arbitrary rotation matrix R(n,psi)
// that represents a rotation about n by psi deccomposed into its cos,sin, 
// and constant componets. 
// ie: R(n,psi) = cos(psi) * c + sin(psi) + s + o
//  
static void rotation_matrix(const f32 n[3], Matrix c, Matrix s, Matrix o)
{
	cpmatrix(c, idmat);
	cpmatrix(s, idmat);
	cpmatrix(o, idmat); 

	f32 u1 = n[0];
	f32 u2 = n[1];
	f32 u3 = n[2];

	f32 u1u1 = u1*u1;
	f32 u1u2 = u1*u2;
	f32 u1u3 = u1*u3;
	f32 u2u2 = u2*u2;
	f32 u2u3 = u2*u3;
	f32 u3u3 = u3*u3;

	c[0][0] = 1.0f-u1u1;
	c[0][1] = -u1u2;
	c[0][2] = -u1u3;
	c[1][0] = -u1u2;
	c[1][1] = 1.0f-u2u2;
	c[1][2] = -u2u3;
	c[2][0] = -u1u3;
	c[2][1] = -u2u3;
	c[2][2] = 1.0f-u3u3;

	s[0][0] = 0.0f;
	s[0][1] = u3;
	s[0][2] = -u2;
	s[1][0] = -u3;
	s[1][1] = 0.0f;
	s[1][2] = u1;
	s[2][0] = u2;
	s[2][1] = -u1;
	s[2][2] = 0.0f;

	o[0][0] = u1u1;
	o[0][1] = u1u2;
	o[0][2] = u1u3;
	o[1][0] = u1u2;
	o[1][1] = u2u2;
	o[1][2] = u2u3;
	o[2][0] = u1u3;
	o[2][1] = u2u3;
	o[2][2] = u3u3;
}


s32 SRS::R1Psi(Matrix C, Matrix s, Matrix o)
{
	Matrix R0;

	SolveR1((f32) 0, R0);

	// 
	// R1(psi)  = R0*R(n,psi) 
	// R(n,psi) = cos(psi)*C + sin(psi)*s + o
	// 

	rotation_matrix(n, C, s, o);
	rmatmult(C,R0,C);
	rmatmult(s,R0,s);
	rmatmult(o,R0,o);

	return 1;
}


s32 SRS::R1R2Psi(Matrix C, Matrix s, Matrix o,
		 Matrix c2, Matrix s2, Matrix o2)
{
	Matrix R0, Temp;

	SolveR1((f32) 0, R0);

	// 
	// R1(psi)  = R0*R(n,psi) 
	// R(n,psi) = cos(psi)*C + sin(psi)*s + o
	// 

	rotation_matrix(n, C, s, o);
	rmatmult(C,R0,C);
	rmatmult(s,R0,s);
	rmatmult(o,R0,o);

	//
	// 
	// R2 = G*transpose(SRT*R1) 
	// where R1 = (cos(phi)*c + sin(phi)*s + o)
	//

	rmatmult(Temp, SRT, C);
	invertrmatrix(c2, Temp);
	rmatmult(c2, G, c2);
 
	rmatmult(Temp, SRT, s);
	invertrmatrix(s2, Temp);
	rmatmult(s2, G, s2);

	rmatmult(Temp, SRT, o);
	invertrmatrix(o2, Temp);
	rmatmult(o2, G, o2);
	
	return 1;
}


//
// Rewrite all this stuff 
//

static void get_aim_circle_equation(const f32 g[3],
			 const f32 a[3],
			 const f32 ta[3],
			 const f32 tb[3],
			 const f32 proj_axis[3],
									f32 theta4,
									f32 center[3],
									f32 u[3],
									f32 v[3],
									f32& radius)
{
	f32 L1 = DOT(ta,ta);
	f32 L2 = DOT(tb,tb);
	Matrix Ry, Ryt;

	rotation_principal_axis_to_matrix('y', theta4, Ry);
	invertrmatrix(Ryt, Ry);

	// Compute distance of hand to shoulder 

	f32 t1[3];
	f32 t2[3];

	vecmult(t1, (f32*) tb, Ry);
	vecmult(t2, (f32*) ta, Ryt);
	f32 L3 = _sqrt(L1 + L2 + DOT(ta,t1) + DOT(tb,t2));

	// Lengths of upper and lower arms
	L1 = _sqrt(L1);
	L2 = _sqrt(L2);

	// Compute angle between a and shoulder-to-hand vector
	// This is done assuming R1 = I since the angle does
	// not depend on the shoulder joints
	//
	// h = Ry*tb + ta
	// a = Ry*a 

	vecadd(t2, t1, (f32*) ta);
	unitize(t2);

	vecmult(t1, (f32*) a, Ry);
	f32 alpha = acos(DOT(t1,t2));


	//
	// Compute the angles of the triangle s,h,g
	//
	f32 L4 = _sqrt(DOT(g,g));
	f32 beta = M_PI - alpha;

	f32 delta = asin(_sin(beta)*L3/L4);
	if (delta < 0)
	delta = - delta;
	f32 gamma = M_PI - delta - beta;

	f32 c_gamma = _cos(gamma);
	f32 n[3];
	cpvector(n, g);
	unitize(n);
	vecscalarmult(center, n, c_gamma*L3);

	radius = _sqrt(1-c_gamma*c_gamma)*L3;

	project_plane(u, (f32*) proj_axis, n);
	unitize(u);
	crossproduct(v, n, u);
}


void SRS::SetAimGoal(const f32 goal[3],
			 const f32 ax[3],
					 f32 flex_angle)
{
	f32 s[3];

	cpvector(ee, goal);
	cpvector(axis, ax);
	get_translation(T, p_r1);
	get_translation(S, s);

	get_aim_circle_equation(goal, 
		axis, p_r1, s, proj_axis, flex_angle, c, u, v, radius);

	rotation_principal_axis_to_matrix('y', flex_angle, Ry);
	vecmult(ee_r1, (f32*)s, Ry);
	vecadd(ee_r1, ee_r1, (f32*)p_r1);
}



void SRS::SolveAim(f32 psi_angle, Matrix  R1)
{
	f32 h1[3];
	f32 N[3];
	f32 angle;
	Matrix S0, S1;

	// Get the final hand position 
	evalcircle(c, u, v, radius, psi_angle, h1);

	// Rotate ee_r1 to h1
	crossproduct(N, ee_r1, h1);
	unitize(N);
	angle = angle_between_vectors(ee_r1, h1, N);
	rotation_axis_to_matrix(N, angle, S0);

	// Now rotate a0 to a
	f32 a[3];
	f32 a0[3];

	vecsub(a, (f32*)ee, h1);
	unitize(a);
	
	hmatmult(S1,Ry,S0);
	vecmult0(a0, (f32*)axis, S1);

	cpvector(N, h1);
	unitize(N);
	angle = angle_between_vectors(a0, a, N);
	rotation_axis_to_matrix(N, angle, S1);

	hmatmult(R1, S0, S1);
}
