#ifndef noiseH
#define noiseH

f32	noise3(const fVector3& vec);
f32	fractalsum3(const fVector3& v, f32 freq, int octaves);
f32	turbulence3(const fVector3& v, f32 freq, int octaves);

#endif
