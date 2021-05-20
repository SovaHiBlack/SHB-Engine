#ifndef noiseH
#define noiseH

float	noise3(const Fvector3& vec);
float	fractalsum3(const Fvector3& v, float freq, int octaves);
float	turbulence3(const Fvector3& v, float freq, int octaves);

#endif
