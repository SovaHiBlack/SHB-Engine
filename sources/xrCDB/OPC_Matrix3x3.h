///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for 3x3 matrices.
 *	\file		IceMatrix3x3.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEMATRIX3X3_H__
#define __ICEMATRIX3X3_H__

	// Forward declarations
	class Quat;

	#define	MATRIX3X3_EPSILON		(1.0e-7f)
	#define	ROW	*(*this)

	class ICEMATHS_API Matrix3x3
	{
		public:
		//! Empty constructor
		inline_					Matrix3x3()									{}
		//! Constructor from 9 values
		inline_					Matrix3x3(f32 m00, f32 m01, f32 m02, f32 m10, f32 m11, f32 m12, f32 m20, f32 m21, f32 m22)
								{
									m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
									m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
									m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
								}
		//! Copy constructor
		inline_					Matrix3x3(const Matrix3x3& mat)				{ CopyMemory(m, &mat.m, 9*sizeof(f32));	}
		//! Destructor
		inline_					~Matrix3x3()								{}

		//! Assign values
		inline_	void			Set(f32 m00, f32 m01, f32 m02, f32 m10, f32 m11, f32 m12, f32 m20, f32 m21, f32 m22)
				{
					m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
					m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
					m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
				}

		//! Sets the scale from a Point. The point is put on the diagonal.
		inline_	void			SetScale(const Point& p)					{ m[0][0] = p.x;	m[1][1] = p.y;	m[2][2] = p.z;	}

		//! Sets the scale from floats. Values are put on the diagonal.
		inline_	void			SetScale(f32 sx, f32 sy, f32 sz)		{ m[0][0] = sx;		m[1][1] = sy;	m[2][2] = sz;	}

		//! Scales from a Point. Each row is multiplied by a component.
		inline_	void			Scale(const Point& p)
				{
					m[0][0] *= p.x;	m[0][1] *= p.x;	m[0][2] *= p.x;
					m[1][0] *= p.y;	m[1][1] *= p.y;	m[1][2] *= p.y;
					m[2][0] *= p.z;	m[2][1] *= p.z;	m[2][2] *= p.z;
				}

		//! Scales from floats. Each row is multiplied by a value.
		inline_	void			Scale(f32 sx, f32 sy, f32 sz)
				{
					m[0][0] *= sx;	m[0][1] *= sx;	m[0][2] *= sx;
					m[1][0] *= sy;	m[1][1] *= sy;	m[1][2] *= sy;
					m[2][0] *= sz;	m[2][1] *= sz;	m[2][2] *= sz;
				}

		//! Copy from a Matrix3x3
		inline_	void			Copy(const Matrix3x3& source)				{ CopyMemory(m, source.m, 9*sizeof(f32));			}

		// Row-column access
		//! Returns a row.
		inline_	void			GetRow(const udword r, Point& p)	const	{ p.x = m[r][0];	p.y = m[r][1];	p.z = m[r][2];	}
		//! Sets a row.
		inline_	void			SetRow(const udword r, const Point& p)		{ m[r][0] = p.x;	m[r][1] = p.y;	m[r][2] = p.z;	}
		//! Returns a column.
		inline_	void			GetCol(const udword c, Point& p)	const	{ p.x = m[0][c];	p.y = m[1][c];	p.z = m[2][c];	}
		//! Sets a column.
		inline_	void			SetCol(const udword c, const Point& p)		{ m[0][c] = p.x;	m[1][c] = p.y;	m[2][c] = p.z;	}

		//! Computes the trace. The trace is the sum of the 3 diagonal components.
		inline_	f32			Trace()					const				{ return m[0][0] + m[1][1] + m[2][2];				}
		//! Clears the matrix.
		inline_	void			Zero()										{ ZeroMemory(&m, sizeof(m));						}
		//! Sets the identity matrix.
		inline_	void			Identity()									{ Zero(); m[0][0] = m[1][1] = m[2][2] = 1.0f; 		}
		//! Checks for identity
		inline_	bool			IsIdentity()			const
				{
					if(IR(m[0][0])!=IEEE_1_0)	return false;
					if(IR(m[0][1])!=0)			return false;
					if(IR(m[0][2])!=0)			return false;

					if(IR(m[1][0])!=0)			return false;
					if(IR(m[1][1])!=IEEE_1_0)	return false;
					if(IR(m[1][2])!=0)			return false;

					if(IR(m[2][0])!=0)			return false;
					if(IR(m[2][1])!=0)			return false;
					if(IR(m[2][2])!=IEEE_1_0)	return false;

					return true;
				}

		//! Makes a skew-symmetric matrix (a.k.a. Star(*) Matrix)
		//!	[  0.0  -a.z   a.y ]
		//!	[  a.z   0.0  -a.x ]
		//!	[ -a.y   a.x   0.0 ]
		//! This is also called a "cross matrix" since for any vectors A and B,
		//! A^B = Skew(A) * B = - B * Skew(A);
		inline_	void			SkewSymmetric(const Point& a)
				{
					m[0][0] = 0.0f;
					m[0][1] = -a.z;
					m[0][2] = a.y;

					m[1][0] = a.z;
					m[1][1] = 0.0f;
					m[1][2] = -a.x;

					m[2][0] = -a.y;
					m[2][1] = a.x;
					m[2][2] = 0.0f;
				}

		//! Negates the matrix
		inline_	void			Neg()
				{
					m[0][0] = -m[0][0];	m[0][1] = -m[0][1];	m[0][2] = -m[0][2];
					m[1][0] = -m[1][0];	m[1][1] = -m[1][1];	m[1][2] = -m[1][2];
					m[2][0] = -m[2][0];	m[2][1] = -m[2][1];	m[2][2] = -m[2][2];
				}

		//! Neg from another matrix
		inline_	void			Neg(const Matrix3x3& mat)
				{
					m[0][0] = -mat.m[0][0];	m[0][1] = -mat.m[0][1];	m[0][2] = -mat.m[0][2];
					m[1][0] = -mat.m[1][0];	m[1][1] = -mat.m[1][1];	m[1][2] = -mat.m[1][2];
					m[2][0] = -mat.m[2][0];	m[2][1] = -mat.m[2][1];	m[2][2] = -mat.m[2][2];
				}

		//! Add another matrix
		inline_	void			Add(const Matrix3x3& mat)
				{
					m[0][0] += mat.m[0][0];	m[0][1] += mat.m[0][1];	m[0][2] += mat.m[0][2];
					m[1][0] += mat.m[1][0];	m[1][1] += mat.m[1][1];	m[1][2] += mat.m[1][2];
					m[2][0] += mat.m[2][0];	m[2][1] += mat.m[2][1];	m[2][2] += mat.m[2][2];
				}

		//! Sub another matrix
		inline_	void			Sub(const Matrix3x3& mat)
				{
					m[0][0] -= mat.m[0][0];	m[0][1]	-= mat.m[0][1];	m[0][2] -= mat.m[0][2];
					m[1][0] -= mat.m[1][0];	m[1][1] -= mat.m[1][1];	m[1][2] -= mat.m[1][2];
					m[2][0] -= mat.m[2][0];	m[2][1] -= mat.m[2][1];	m[2][2] -= mat.m[2][2];
				}
		//! Mac
		inline_	void			Mac(const Matrix3x3& a, const Matrix3x3& b, f32 s)
				{
					m[0][0] = a.m[0][0] + b.m[0][0] * s;
					m[0][1] = a.m[0][1] + b.m[0][1] * s;
					m[0][2] = a.m[0][2] + b.m[0][2] * s;

					m[1][0] = a.m[1][0] + b.m[1][0] * s;
					m[1][1] = a.m[1][1] + b.m[1][1] * s;
					m[1][2] = a.m[1][2] + b.m[1][2] * s;

					m[2][0] = a.m[2][0] + b.m[2][0] * s;
					m[2][1] = a.m[2][1] + b.m[2][1] * s;
					m[2][2] = a.m[2][2] + b.m[2][2] * s;
				}
		//! Mac
		inline_	void			Mac(const Matrix3x3& a, f32 s)
				{
					m[0][0] += a.m[0][0] * s;	m[0][1] += a.m[0][1] * s;	m[0][2] += a.m[0][2] * s;
					m[1][0] += a.m[1][0] * s;	m[1][1] += a.m[1][1] * s;	m[1][2] += a.m[1][2] * s;
					m[2][0] += a.m[2][0] * s;	m[2][1] += a.m[2][1] * s;	m[2][2] += a.m[2][2] * s;
				}

		//! this = A * s
		inline_	void			Mult(const Matrix3x3& a, f32 s)
				{
					m[0][0] = a.m[0][0] * s;	m[0][1] = a.m[0][1] * s;	m[0][2] = a.m[0][2] * s;
					m[1][0] = a.m[1][0] * s;	m[1][1] = a.m[1][1] * s;	m[1][2] = a.m[1][2] * s;
					m[2][0] = a.m[2][0] * s;	m[2][1] = a.m[2][1] * s;	m[2][2] = a.m[2][2] * s;
				}

		inline_	void			Add(const Matrix3x3& a, const Matrix3x3& b)
				{
					m[0][0] = a.m[0][0] + b.m[0][0];	m[0][1] = a.m[0][1] + b.m[0][1];	m[0][2] = a.m[0][2] + b.m[0][2];
					m[1][0] = a.m[1][0] + b.m[1][0];	m[1][1] = a.m[1][1] + b.m[1][1];	m[1][2] = a.m[1][2] + b.m[1][2];
					m[2][0] = a.m[2][0] + b.m[2][0];	m[2][1] = a.m[2][1] + b.m[2][1];	m[2][2] = a.m[2][2] + b.m[2][2];
				}

		inline_	void			Sub(const Matrix3x3& a, const Matrix3x3& b)
				{
					m[0][0] = a.m[0][0] - b.m[0][0];	m[0][1] = a.m[0][1] - b.m[0][1];	m[0][2] = a.m[0][2] - b.m[0][2];
					m[1][0] = a.m[1][0] - b.m[1][0];	m[1][1] = a.m[1][1] - b.m[1][1];	m[1][2] = a.m[1][2] - b.m[1][2];
					m[2][0] = a.m[2][0] - b.m[2][0];	m[2][1] = a.m[2][1] - b.m[2][1];	m[2][2] = a.m[2][2] - b.m[2][2];
				}

		//! this = a * b
		inline_	void			Mult(const Matrix3x3& a, const Matrix3x3& b)
				{
					m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0];
					m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1];
					m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2];
					m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0];
					m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1];
					m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2];
					m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0];
					m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1];
					m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2];
				}

		//! this = transpose(a) * b
		inline_	void			MultAtB(const Matrix3x3& a, const Matrix3x3& b)
				{
					m[0][0] = a.m[0][0] * b.m[0][0] + a.m[1][0] * b.m[1][0] + a.m[2][0] * b.m[2][0];
					m[0][1] = a.m[0][0] * b.m[0][1] + a.m[1][0] * b.m[1][1] + a.m[2][0] * b.m[2][1];
					m[0][2] = a.m[0][0] * b.m[0][2] + a.m[1][0] * b.m[1][2] + a.m[2][0] * b.m[2][2];
					m[1][0] = a.m[0][1] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[2][1] * b.m[2][0];
					m[1][1] = a.m[0][1] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[2][1] * b.m[2][1];
					m[1][2] = a.m[0][1] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[2][1] * b.m[2][2];
					m[2][0] = a.m[0][2] * b.m[0][0] + a.m[1][2] * b.m[1][0] + a.m[2][2] * b.m[2][0];
					m[2][1] = a.m[0][2] * b.m[0][1] + a.m[1][2] * b.m[1][1] + a.m[2][2] * b.m[2][1];
					m[2][2] = a.m[0][2] * b.m[0][2] + a.m[1][2] * b.m[1][2] + a.m[2][2] * b.m[2][2];
				}

		//! this = a * transpose(b)
		inline_	void			MultABt(const Matrix3x3& a, const Matrix3x3& b)
				{
					m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[0][1] + a.m[0][2] * b.m[0][2];
					m[0][1] = a.m[0][0] * b.m[1][0] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[1][2];
					m[0][2] = a.m[0][0] * b.m[2][0] + a.m[0][1] * b.m[2][1] + a.m[0][2] * b.m[2][2];
					m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[0][1] + a.m[1][2] * b.m[0][2];
					m[1][1] = a.m[1][0] * b.m[1][0] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[1][2];
					m[1][2] = a.m[1][0] * b.m[2][0] + a.m[1][1] * b.m[2][1] + a.m[1][2] * b.m[2][2];
					m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[0][1] + a.m[2][2] * b.m[0][2];
					m[2][1] = a.m[2][0] * b.m[1][0] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[1][2];
					m[2][2] = a.m[2][0] * b.m[2][0] + a.m[2][1] * b.m[2][1] + a.m[2][2] * b.m[2][2];
				}

		//! Makes a rotation matrix mapping vector3 "from" to vector3 "to".
				Matrix3x3&		FromTo(const Point& from, const Point& to);

		//! Set a rotation matrix around the X axis.
				void			RotX(f32 angle)	{ f32 Cos = _cos(angle), Sin = _sin(angle); Identity(); m[1][1] = m[2][2] = Cos; m[2][1] = -Sin;	m[1][2] = Sin;	}
		//! Set a rotation matrix around the Y axis.
				void			RotY(f32 angle)	{ f32 Cos = _cos(angle), Sin = _sin(angle); Identity(); m[0][0] = m[2][2] = Cos; m[2][0] = Sin;	m[0][2] = -Sin;	}
		//! Set a rotation matrix around the Z axis.
				void			RotZ(f32 angle)	{ f32 Cos = _cos(angle), Sin = _sin(angle); Identity(); m[0][0] = m[1][1] = Cos; m[1][0] = -Sin;	m[0][1] = Sin;	}

		//! Make a rotation matrix about an arbitrary angle
				Matrix3x3&		Rot(f32 angle, const Point& axis);

		//! Transpose the matrix.
				void			Transpose()
				{
					IR(m[1][0]) ^= IR(m[0][1]);	IR(m[0][1]) ^= IR(m[1][0]);	IR(m[1][0]) ^= IR(m[0][1]);
					IR(m[2][0]) ^= IR(m[0][2]);	IR(m[0][2]) ^= IR(m[2][0]);	IR(m[2][0]) ^= IR(m[0][2]);
					IR(m[2][1]) ^= IR(m[1][2]);	IR(m[1][2]) ^= IR(m[2][1]);	IR(m[2][1]) ^= IR(m[1][2]);
				}

		//! this = Transpose(a)
				void			Transpose(const Matrix3x3& a)
				{
					m[0][0] = a.m[0][0];	m[0][1] = a.m[1][0];	m[0][2] = a.m[2][0];
					m[1][0] = a.m[0][1];	m[1][1] = a.m[1][1];	m[1][2] = a.m[2][1];
					m[2][0] = a.m[0][2];	m[2][1] = a.m[1][2];	m[2][2] = a.m[2][2];
				}

		//! Compute the determinant of the matrix. We use the rule of Sarrus.
				f32			Determinant()					const
				{
					return (m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0] + m[0][2]*m[1][0]*m[2][1])
						-  (m[2][0]*m[1][1]*m[0][2] + m[2][1]*m[1][2]*m[0][0] + m[2][2]*m[1][0]*m[0][1]);
				}
/*
		//! Compute a cofactor. Used for matrix inversion.
				f32			CoFactor(ubyte row, ubyte column)	const
				{
					static sdword gIndex[3+2] = { 0, 1, 2, 0, 1 };
					return	(m[gIndex[row+1]][gIndex[column+1]]*m[gIndex[row+2]][gIndex[column+2]] - m[gIndex[row+2]][gIndex[column+1]]*m[gIndex[row+1]][gIndex[column+2]]);
				}
*/
		//! Invert the matrix. Determinant must be different from zero, else matrix can't be inverted.
				Matrix3x3&		Invert()
				{
					f32 Det = Determinant();	// Must be !=0
					Matrix3x3 Temp;

					f32 OneOverDet = 1.0f / Det;

					Temp.m[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDet;
					Temp.m[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDet;
					Temp.m[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDet;
					Temp.m[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDet;
					Temp.m[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDet;
					Temp.m[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDet;
					Temp.m[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDet;
					Temp.m[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDet;
					Temp.m[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDet;

					*this = Temp;

					return	*this;
				}

				Matrix3x3&		Normalize()
				{
					Point RowX, RowY, RowZ;
					GetRow(0, RowX);
//							GetRow(1, RowY);
					GetRow(2, RowZ);
					RowZ.Normalize();
					RowY = (RowZ ^ RowX).Normalize();
					RowX = RowY ^ RowZ;
					SetRow(0, RowX);
					SetRow(1, RowY);
					SetRow(2, RowZ);
//							this->eZ() = this->eZ().Unit();
//							this->eY() = (this->eZ() * this->eX()).Unit();
//							this->eX() = this->eY() * this->eZ();
					return *this;
				}

		//! this = exp(a)
				Matrix3x3&		Exp(const Matrix3x3& a)
				{
					const int NbTerms = 100;
					Matrix3x3 Term;	// next term in series
					Matrix3x3 Temp;

					Identity();
					Term.Identity();

					for(udword div=1; div<=NbTerms; div++)
					{
						Temp.Mult(Term, a);
						Term.Mult(Temp, 1.0f / f32(div));
						// find next Term = Term * a / div
						Add(Term);
					}
					return *this;
				}

void FromQuat(const Quat &q);
void FromQuatL2(const Quat &q, f32 l2);

		// Arithmetic operators
		//! Operator for Matrix3x3 Plus = Matrix3x3 + Matrix3x3;
		inline_	Matrix3x3		operator+(const Matrix3x3& mat)	const
				{
					return Matrix3x3(
					m[0][0] + mat.m[0][0],	m[0][1] + mat.m[0][1],	m[0][2] + mat.m[0][2],
					m[1][0] + mat.m[1][0],	m[1][1] + mat.m[1][1],	m[1][2] + mat.m[1][2],
					m[2][0] + mat.m[2][0],	m[2][1] + mat.m[2][1],	m[2][2] + mat.m[2][2]);
				}

		//! Operator for Matrix3x3 Minus = Matrix3x3 - Matrix3x3;
		inline_	Matrix3x3		operator-(const Matrix3x3& mat)	const
				{
					return Matrix3x3(
					m[0][0] - mat.m[0][0],	m[0][1] - mat.m[0][1],	m[0][2] - mat.m[0][2],
					m[1][0] - mat.m[1][0],	m[1][1] - mat.m[1][1],	m[1][2] - mat.m[1][2],
					m[2][0] - mat.m[2][0],	m[2][1] - mat.m[2][1],	m[2][2] - mat.m[2][2]);
				}

		//! Operator for Matrix3x3 Mul = Matrix3x3 * Matrix3x3;
		inline_	Matrix3x3		operator*(const Matrix3x3& mat)	const
				{
					return Matrix3x3(
					m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0],
					m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1],
					m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2],

					m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0],
					m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1],
					m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2],

					m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0],
					m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1],
					m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2]);
				}

		//! Operator for Point Mul = Matrix3x3 * Point;
		inline_	Point			operator*(const Point& v)		const		{ return Point(ROW[0]|v, ROW[1]|v, ROW[2]|v); }

		//! Operator for Matrix3x3 Mul = Matrix3x3 * float;
		inline_	Matrix3x3		operator*(f32 s)				const
				{
					return Matrix3x3(
					m[0][0]*s,	m[0][1]*s,	m[0][2]*s,
					m[1][0]*s,	m[1][1]*s,	m[1][2]*s,
					m[2][0]*s,	m[2][1]*s,	m[2][2]*s);
				}

		//! Operator for Matrix3x3 Mul = float * Matrix3x3;
		inline_	friend Matrix3x3 operator*(f32 s, const Matrix3x3& mat)
				{
					return Matrix3x3(
					s*mat.m[0][0],	s*mat.m[0][1],	s*mat.m[0][2],
					s*mat.m[1][0],	s*mat.m[1][1],	s*mat.m[1][2],
					s*mat.m[2][0],	s*mat.m[2][1],	s*mat.m[2][2]);
				}

		//! Operator for Matrix3x3 Div = Matrix3x3 / float;
		inline_	Matrix3x3		operator/(f32 s)				const
				{
					if (s)	s = 1.0f / s;
					return Matrix3x3(
					m[0][0]*s,	m[0][1]*s,	m[0][2]*s,
					m[1][0]*s,	m[1][1]*s,	m[1][2]*s,
					m[2][0]*s,	m[2][1]*s,	m[2][2]*s);
				}

		//! Operator for Matrix3x3 Div = float / Matrix3x3;
		inline_	friend Matrix3x3 operator/(f32 s, const Matrix3x3& mat)
				{
					return Matrix3x3(
					s/mat.m[0][0],	s/mat.m[0][1],	s/mat.m[0][2],
					s/mat.m[1][0],	s/mat.m[1][1],	s/mat.m[1][2],
					s/mat.m[2][0],	s/mat.m[2][1],	s/mat.m[2][2]);
				}

		//! Operator for Matrix3x3 += Matrix3x3
		inline_	Matrix3x3&		operator+=(const Matrix3x3& mat)
				{
					m[0][0] += mat.m[0][0];		m[0][1] += mat.m[0][1];		m[0][2] += mat.m[0][2];
					m[1][0] += mat.m[1][0];		m[1][1] += mat.m[1][1];		m[1][2] += mat.m[1][2];
					m[2][0] += mat.m[2][0];		m[2][1] += mat.m[2][1];		m[2][2] += mat.m[2][2];
					return	*this;
				}

		//! Operator for Matrix3x3 -= Matrix3x3
		inline_	Matrix3x3&		operator-=(const Matrix3x3& mat)
				{
					m[0][0] -= mat.m[0][0];		m[0][1] -= mat.m[0][1];		m[0][2] -= mat.m[0][2];
					m[1][0] -= mat.m[1][0];		m[1][1] -= mat.m[1][1];		m[1][2] -= mat.m[1][2];
					m[2][0] -= mat.m[2][0];		m[2][1] -= mat.m[2][1];		m[2][2] -= mat.m[2][2];
					return	*this;
				}

		//! Operator for Matrix3x3 *= Matrix3x3
		inline_	Matrix3x3&		operator*=(const Matrix3x3& mat)
				{
					Point TempRow;

					GetRow(0, TempRow);
					m[0][0] = TempRow.x*mat.m[0][0] + TempRow.y*mat.m[1][0] + TempRow.z*mat.m[2][0];
					m[0][1] = TempRow.x*mat.m[0][1] + TempRow.y*mat.m[1][1] + TempRow.z*mat.m[2][1];
					m[0][2] = TempRow.x*mat.m[0][2] + TempRow.y*mat.m[1][2] + TempRow.z*mat.m[2][2];

					GetRow(1, TempRow);
					m[1][0] = TempRow.x*mat.m[0][0] + TempRow.y*mat.m[1][0] + TempRow.z*mat.m[2][0];
					m[1][1] = TempRow.x*mat.m[0][1] + TempRow.y*mat.m[1][1] + TempRow.z*mat.m[2][1];
					m[1][2] = TempRow.x*mat.m[0][2] + TempRow.y*mat.m[1][2] + TempRow.z*mat.m[2][2];

					GetRow(2, TempRow);
					m[2][0] = TempRow.x*mat.m[0][0] + TempRow.y*mat.m[1][0] + TempRow.z*mat.m[2][0];
					m[2][1] = TempRow.x*mat.m[0][1] + TempRow.y*mat.m[1][1] + TempRow.z*mat.m[2][1];
					m[2][2] = TempRow.x*mat.m[0][2] + TempRow.y*mat.m[1][2] + TempRow.z*mat.m[2][2];
					return	*this;
				}

		//! Operator for Matrix3x3 *= float
		inline_	Matrix3x3&		operator*=(f32 s)
				{
					m[0][0] *= s;	m[0][1] *= s;	m[0][2] *= s;
					m[1][0] *= s;	m[1][1] *= s;	m[1][2] *= s;
					m[2][0] *= s;	m[2][1] *= s;	m[2][2] *= s;
					return	*this;
				}

		//! Operator for Matrix3x3 /= float
		inline_	Matrix3x3&		operator/=(f32 s)
				{
					if (s)	s = 1.0f / s;
					m[0][0] *= s;	m[0][1] *= s;	m[0][2] *= s;
					m[1][0] *= s;	m[1][1] *= s;	m[1][2] *= s;
					m[2][0] *= s;	m[2][1] *= s;	m[2][2] *= s;
					return	*this;
				}

		// Cast operators
		//! Cast a Matrix3x3 to a Matrix4x4.
								operator Matrix4x4()	const;
		//! Cast a Matrix3x3 to a Quat.
								operator Quat()			const;

		inline_	const Point*	operator[](int nRow)	const	{ return (const Point*)&m[nRow][0];	}
		inline_	Point*			operator[](int nRow)			{ return (Point*)&m[nRow][0];		}

		public:

			f32			m[3][3];
	};

#endif // __ICEMATRIX3X3_H__

