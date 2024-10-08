#ifndef noiseH
#define noiseH

F32	noise3(const Fvector& vec);
F32	fractalsum3(const Fvector& v, F32 freq, int octaves);
F32	turbulence3(const Fvector& v, F32 freq, int octaves);

#endif
