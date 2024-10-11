#pragma once

#ifdef  XRGAME_EXPORTS

#ifdef	dSqrt
#undef	dSqrt
#define dSqrt(x) ((f32)_sqrt(x))		/* square root */
#endif

#ifdef  dRecipSqrt
#undef  dRecipSqrt
#define dRecipSqrt(x) ((f32)(1.0f/_sqrt(x)))	/* reciprocal square root */
#endif

#ifdef	dSin
#undef  dSin
#define dSin(x) ((f32)_sin(x))			/* sine */
#endif

#ifdef  dCos
#undef  dCos
#define dCos(x) ((f32)_cos(x))			/* cosine */
#endif

#ifdef  dFabs
#undef  dFabs
#define dFabs(x) ((f32)_abs(x))		/* absolute value */
#endif

#endif//XRGAME_EXPORTS
