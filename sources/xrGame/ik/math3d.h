 
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

#ifndef _MYVECH
#define _MYVECH



#define DTOR(x)  ((x)*M_PI/180.0)
#define DOT4(u,v)  ((u)[0]*(v)[0]+(u)[1]*(v)[1]+(u)[2]*(v)[2]+(u)[3]*((v)[3]))



typedef f32 Matrix[4][4];
typedef f32 Quaternion[4];


extern Matrix idmat; 

// u = v
//#define cpmatrix(u,v) memcpy(u,v,sizeof(Matrix))
//#define cpvector(u,v) memcpy(u,v,sizeof(f32)*3)

#define cpmatrix(u,v) CopyMemory(u,v,sizeof(Matrix))
#define cpvector(u,v) CopyMemory(u,v,sizeof(f32)*3)

void hmatmult(Matrix A,Matrix B,Matrix C);
void inverthomomatrix(Matrix N,Matrix M);
void vecmult(f32 y[], f32 x[],Matrix M);
void vecmult0(f32 y[], f32 x[],Matrix M);
void qtomatrix(Matrix m, Quaternion q);
void matrixtoq(Quaternion q, Matrix m);
void axistoq(Quaternion q, f32 angle, f32 axis[]);
void qtoaxis(f32* angle, f32 axis[], Quaternion q);
void vecinterp(f32 x[], f32 u[], f32 v[], f32 t);
f32 unitize4(f32 u[4]);
void get_translation(const Matrix M, f32 p[3]);
void set_translation(Matrix M, const f32 p[3]);
void get_translation(const Matrix M, f32& x, f32& y, f32& z);
void set_translation(Matrix M, f32 x, f32 y, f32 z);

IC void vecscalarmult(f32 u[], f32 v[], f32 f)
{
	u[0] = v[0] * f;
	u[1] = v[1] * f;
	u[2] = v[2] * f;
}

IC f32 unitize(f32 u[])
{
	f32   f,m;
	
	f = u[0]*u[0] + u[1]*u[1] + u[2]*u[2];
	
//    if (f == 0) {
//        return(0.0);
//    }

	if (f != 0.0)
	{
	m = (f32)_sqrt(f);

	u[0] = u[0] / m;
	u[1] = u[1] / m;
	u[2] = u[2] / m;
	}
	else
	m = 0.0; 
	
	return(m);
}


IC void crossproduct(f32 r[3], const f32 a[3], const f32 b[3])
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

IC void vecsub(f32 t[], f32 u[], f32 v[])
{
	t[0] = u[0] - v[0];
	t[1] = u[1] - v[1];
	t[2] = u[2] - v[2];
}

IC void vecadd(f32 t[], f32 u[], f32 v[])
{
	t[0] = u[0] + v[0];
	t[1] = u[1] + v[1];
	t[2] = u[2] + v[2];
}


#define DOT(u,v) (u[0]*v[0] + u[1]*v[1] + u[2]*v[2])


#endif

//
// Some miscellaneous useful routines 
//

f32 norm(f32 v[3]);
/********************************
//
// length of a vector
//
inline f32 norm(f32 v[3])
{
	return sqrt(DOT(v,v));
}

//
// translation component of a matrix
//

inline void get_translation(const Matrix M, f32 p[3])
{
	p[0] = M[3][0];
	p[1] = M[3][1];
	p[2] = M[3][2];
}

inline void set_translation(Matrix M, const f32 p[3])
{
	M[3][0] = p[0];
	M[3][1] = p[1];
	M[3][2] = p[2];
}

inline void get_translation(const Matrix M, f32 &x, f32 &y, f32 &z)
{
	x = M[3][0];
	y = M[3][1];
	z = M[3][2];
}

inline void set_translation(Matrix M, f32 x, f32 y, f32 z)
{
	M[3][0] = x;
	M[3][1] = y;
	M[3][2] = z;
}

inline void set_row(Matrix M, int row, const f32 v[3])
{
	M[row][0] = v[0];
	M[row][1] = v[1];
	M[row][2] = v[2];
}

inline void get_row(Matrix M, int row,  f32 v[3])
{
	v[0] = M[row][0];
	v[1] = M[row][1];
	v[2] = M[row][2];
}
**********************************************************/


//
// p = projection(u) onto v
// 
void project(f32 p[3], const f32 u[3], const f32 v[3]);

//
// p = projection(u) onto plane whose normal is n
// 
void project_plane(f32 p[3], f32 u[3], f32 n[3]);


//
// Returns the angle between u and v in the direction of n
//

f32 angle_between_vectors(f32 u[3], f32 v[3], f32 n[3]);

//
// Generates a rotation matrix given an arbitrary axis and angle
//
void rotation_axis_to_matrix(f32 axis[3], f32 angle, Matrix R);
void rotation_principal_axis_to_matrix(char axis, f32 angle, Matrix R);
void rotation_principal_axis_to_deriv_matrix(char axis, f32 angle, Matrix m);

//
// Finds the axis and angle of a rotation matrix
//
void rotation_matrix_to_axis(const Matrix R, f32 axis[], f32& angle);
 

//
// Given a vector v find another vector n orthogonal to it
//
void find_normal_vector(f32 v[3], f32 n[3]);

 
void print_matrix(Matrix M);
void print_vector(f32 v[3]);

void rmatmult(Matrix A,Matrix B,Matrix C);
void invertrmatrix(Matrix N,Matrix M);

void axisangletomatrix(Matrix m, f32 axis[], f32 theta);

/***************
inline f32 vecdist(const f32 t[], const f32 t2[])
{
	f32 t3[3];

	vecsub(t3, (f32*)t, (f32*)t2);
	return sqrt(DOT(t3,t3));
}

****************************/

