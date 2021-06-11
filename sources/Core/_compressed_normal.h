#pragma once

// A Unit Vector to 16-bit word conversion algorithm
// based on work of Rafael Baptista (rafael@oroboro.com)
// Accuracy improved by O.D. (punkfloyd@rocketmail.com)

// a compressed unit vector3. reasonable fidelty for unit vectors in a 16 bit
// package. Good enough for surface normals we hope.

CORE_API U16		pvCompress			(const Fvector3& vec);
CORE_API void		pvDecompress		(Fvector3& vec, U16 mVec);
CORE_API void		pvInitializeStatics	( );
