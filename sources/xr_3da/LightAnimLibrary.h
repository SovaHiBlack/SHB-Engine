#pragma once

class ENGINE_API CLAItem				{
public:
	shared_str		cName;
	f32				fFPS;
	DEFINE_MAP		(int,u32,KeyMap,KeyPairIt);
	KeyMap			Keys;
	int				iFrameCount;

public:
					CLAItem				();
					
	void			InitDefault			();
	void			Load				(IReader& F);
	void			Save				(IWriter& F);
	f32			Length_sec			(){return f32(iFrameCount)/fFPS;}
	u32				Length_ms			(){return iFloor(Length_sec()*1000.f);}
	u32				InterpolateRGB		(int frame);
	u32				InterpolateBGR		(int frame);
	u32				CalculateRGB		(f32 T, int& frame);
	u32				CalculateBGR		(f32 T, int& frame);
	void		    Resize				(int new_len);
	void		    InsertKey			(int frame, u32 color);
	void		    DeleteKey			(int frame);
	void		    MoveKey				(int from, int to);
	bool		    IsKey				(int frame){return (Keys.end()!=Keys.find(frame));}
	int		 	    PrevKeyFrame		(int frame);
	int		 	    NextKeyFrame 		(int frame);
	int		 	    FirstKeyFrame		(){return Keys.rend()->first;}
	int		 	    LastKeyFrame		(){return Keys.rbegin()->first;}
	u32*			GetKey				(int frame){KeyPairIt it=Keys.find(frame); return (it!=Keys.end())?&(it->second):0;}
};

DEFINE_VECTOR(CLAItem*,LAItemVec,LAItemIt);

class ENGINE_API ELightAnimLibrary{
public:
	LAItemVec       Items;
	LAItemIt		FindItemI			(pcstr name);
	CLAItem*		FindItem			(pcstr name);

public:
					ELightAnimLibrary	();
					~ELightAnimLibrary	();

	void			OnCreate			();
	void			OnDestroy			();
	void			Load				();
	void			Save				();
	void			Reload				();
	void			Unload				();
	CLAItem*		AppendItem			(pcstr name, CLAItem* src);
	LAItemVec&		Objects				(){return Items;}
};

extern ENGINE_API ELightAnimLibrary LALib;
