#ifndef ChooseTypesH
#define ChooseTypesH

enum EChooseMode{
	smCustom = 0,
	smSoundSource,
	smSoundEnv,
	smObject,
	smGroup,
	smEShader,
	smCShader,
	smPE,
	smParticles,
	smTexture,
	smTextureRaw,
	smEntityType,
	smSpawnItem,
	smLAnim,
	smVisual,
	smSkeletonAnims,
	smSkeletonBones,  
	smGameMaterial,
	smGameAnim,
	smGameSMotions,
};

enum EChooseFlags{
	cfMultiSelect	= (1<<0),
	cfAllowNone		= (1<<1),
	cfFullExpand	= (1<<2),
};

struct SChooseItem{
	shared_str				name;
	shared_str				hint;
	SChooseItem	(pcstr nm, pcstr ht):name(nm),hint(ht){}
};
DEFINE_VECTOR(SChooseItem,ChooseItemVec,ChooseItemVecIt);

// typedef
typedef fastdelegate::FastDelegate2<ChooseItemVec&,void*>	   	TOnChooseFillItems;
typedef fastdelegate::FastDelegate2<SChooseItem*, PropItemVec&>	TOnChooseSelectItem;
typedef fastdelegate::FastDelegate3<pcstr, HDC, const iRect&> 	TOnDrawThumbnail;
typedef fastdelegate::FastDelegate0<>						   	TOnChooseClose;

typedef void (*TOnChooseFillEvents)();

struct SChooseEvents{
	enum{
		flAnimated		= (1<<0),
	};
	shared_str			caption;
	TOnChooseFillItems	on_fill;
	TOnChooseSelectItem	on_sel;
	TOnDrawThumbnail    on_thm;
	TOnChooseClose	    on_close;
	flags32				flags;
						SChooseEvents	(){caption="Select Item";flags.zero();}
						SChooseEvents	(pcstr capt, TOnChooseFillItems f, TOnChooseSelectItem s, TOnDrawThumbnail t, TOnChooseClose c, u32 fl)
	{
		Set				(capt,f,s,t,c,fl);
	}
	void				Set				(pcstr capt, TOnChooseFillItems f, TOnChooseSelectItem s, TOnDrawThumbnail t, TOnChooseClose c, u32 fl)
	{
		caption			= capt;
		on_fill			= f;
		on_sel			= s;
		on_thm			= t;
		on_close		= c;
		flags.assign	(fl);
	}
};

#define NONE_CAPTION "<none>"

#endif
