#ifndef noiseH
#define noiseH

f32	noise3(const Fvector& vec);
f32	fractalsum3(const Fvector& v, f32 freq, int octaves);
f32	turbulence3(const Fvector& v, f32 freq, int octaves);

#endif
