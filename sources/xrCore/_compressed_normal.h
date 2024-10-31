#pragma once

// A Unit Vector to 16-bit word conversion algorithm
// based on work of Rafael Baptista (rafael@oroboro.com)
// Accuracy improved by O.D. (punkfloyd@rocketmail.com)

// a compressed unit vector3. reasonable fidelty for unit vectors in a 16 bit
// package. Good enough for surface normals we hope.

CORE_API u16		pvCompress(const fVector3& vec);
CORE_API void		pvDecompress(fVector3& vec, u16 mVec);
CORE_API void		pvInitializeStatics( );
