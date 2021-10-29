#pragma once

enum ETelekineticState
{
	TS_None,
	TS_Raise,
	TS_Keep,
	TS_Fire,
};

class CGameObject;
class CPHShellHolder;
class CTelekineticObject;
class CPHUpdateObject;
class CTelekinesis;

class CTelekineticObject
{
	ETelekineticState							state;

public:
	CPHShellHolder*								object;
	CTelekinesis*								telekinesis;
	float										target_height;

	unsigned int								time_keep_started;
	unsigned int								time_keep_updated;
	unsigned int								time_raise_started;
	unsigned int								time_to_keep;
	unsigned int								time_fire_started;

	float										strength;

	bool										m_rotate;

	ref_sound									sound_hold;
	ref_sound									sound_throw;

						CTelekineticObject		( );
						~CTelekineticObject		( );

	virtual bool		init					(CTelekinesis* tele, CPHShellHolder* obj, float s, float h, unsigned int ttk, bool rot = true);
	void				set_sound				(const ref_sound& snd_hold, const ref_sound& snd_throw);

	virtual void		raise					(float step);
	virtual void		raise_update			( );

	void				prepare_keep			( );
	virtual void		keep					( );
	virtual void		keep_update				( );
	virtual void		release					( );
	virtual void		fire					(const Fvector3& target, float power);
	void				fire_t					(const Fvector3& target, float time);
	virtual void		fire_update				( );
	virtual void		update_state			( );
	virtual bool		can_activate			(CPHShellHolder* obj);
	bool				is_released				( )
	{
		return state == TS_None;
	}
	ETelekineticState	get_state				( )
	{
		return state;
	}
	virtual void		switch_state			(ETelekineticState new_state);
	CPHShellHolder*		get_object				( )
	{
		return object;
	}

	bool				check_height			( );
	bool				check_raise_time_out	( );

	bool				time_keep_elapsed		( );
	bool				time_fire_elapsed		( );

	void				enable					( );

	bool				operator ==				(const CPHShellHolder* obj)
	{
		return (object == obj);
	}

	void				rotate					( );

private:
	void				update_hold_sound		( );
};
