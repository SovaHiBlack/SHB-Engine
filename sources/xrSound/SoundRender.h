#pragma once

#include "Sound.h"

class CSoundRender_Core;
class CSoundRender_Source;
class CSoundRender_Emitter;
class CSoundRender_Target;
class CSoundRender_Environment;
class CSoundEnvironmentLib;

const	unsigned int		sdef_target_count		= 3;									//
const	unsigned int		sdef_target_block		= 400;									// ms
const	unsigned int		sdef_target_size		= sdef_target_count*sdef_target_block;	// ms
const	unsigned int		sdef_source_footer		= 0;									// ms of silence after buffer data, just for rendering
const	unsigned int		sdef_env_version		= 4;									// current version of env-def
const	unsigned int		sdef_level_version		= 1;									// current version of level-def
const	unsigned int		sdef_event_pulse		= 500;									// ms
