#pragma once

namespace PAPI
{
	// refs
	struct SParticleEffect;
	struct PARTICLES_API SParticleAction
	{
		enum
		{
			ALLOW_ROTATE = (1 << 1)
		};
		Flags32			m_Flags;
		EParticleAction		type;	// Type field
										SParticleAction( )
		{
			m_Flags.zero( );
		}

		virtual void					Execute(SParticleEffect* pe, float dt) = 0;
		virtual void					Transform(const Fmatrix& m) = 0;

		virtual void					Load(IReader& F) = 0;
		virtual void					Save(IWriter& F) = 0;
	};

	using ParticleActionVec = xr_vector<SParticleAction*>;
	using ParticleActionVec_it = ParticleActionVec::iterator;

	class CParticleActions
	{
		ParticleActionVec			actions;

	public:
										CParticleActions( )
		{
			actions.reserve(4);
		}
										~CParticleActions( )
		{
			clear( );
		}
		inline void						clear( )
		{
			for (ParticleActionVec_it it = actions.begin( ); it != actions.end( ); it++)
			{
				xr_delete(*it);
			}

			actions.clear( );
		}
		inline void						append(SParticleAction* pa)
		{
			actions.push_back(pa);
		}
		inline bool						empty( )
		{
			return actions.empty( );
		}
		inline ParticleActionVec_it		begin( )
		{
			return actions.begin( );
		}
		inline ParticleActionVec_it		end( )
		{
			return actions.end( );
		}
		inline int						size( )
		{
			return actions.size( );
		}
		inline void						resize(int cnt)
		{
			actions.resize(cnt);
		}
		void							copy(CParticleActions* src);
	};
};
