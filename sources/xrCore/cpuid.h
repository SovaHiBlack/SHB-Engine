#pragma once

#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2   0x0004
#define _CPU_FEATURE_3DNOW  0x0008

struct _processor_info
{
	string32	v_name;								// vendor name
	string32	model_name;							// Name of model eg. Intel_Pentium_Pro
	s32			family;								// family of the processor, eg. Intel_Pentium_Pro is family 6 processor
	s32			model;								// model of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
	s32			stepping;							// Processor revision number
	s32			feature;							// processor Feature ( same as return value).
	s32			os_support;							// does OS Support the feature
};

s32 _cpuid(_processor_info*);
