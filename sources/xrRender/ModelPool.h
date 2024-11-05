// ModelPool.h: interface for the CModelPool class.
//////////////////////////////////////////////////////////////////////
#pragma once

// refs
class ENGINE_API IRenderVisual;
namespace PS	{ 
	struct ENGINE_API SEmitter; 
};

// defs
class CModelPool
{
private:
	friend class CRender;

	struct str_pred : public std::binary_function<const shared_str&, const shared_str&, bool> 
	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;}
	};
	struct ModelDef
	{
		shared_str			name;
		IRenderVisual*		model;
        u32					refs;
        ModelDef()			{ refs=0;model=0; }
	};

	typedef xr_multimap<shared_str, IRenderVisual*,str_pred>	POOL;
	typedef POOL::iterator										POOL_IT;
	typedef xr_map<IRenderVisual*,shared_str>					REGISTRY;
	typedef REGISTRY::iterator									REGISTRY_IT;
private:
	xr_vector<ModelDef>			Models;				// Reference / Base
	xr_vector<IRenderVisual*>	ModelsToDelete;		// 
	REGISTRY					Registry;			// Just pairing of pointer / Name
	POOL						Pool;				// Unused / Inactive
	BOOL						bLogging;
    BOOL						bForceDiscard;
    BOOL						bAllowChildrenDuplicate;

	void						Destroy	();
public:
                            CModelPool			();
	virtual 				~CModelPool			();
	IRenderVisual*			Instance_Create		(u32 Type);
	IRenderVisual*			Instance_Duplicate	(IRenderVisual* V);
	IRenderVisual*			Instance_Load		(pcstr N, BOOL allow_register);
	IRenderVisual*			Instance_Load		(pcstr N, IReader* data, BOOL allow_register);
	void					Instance_Register	(pcstr N, IRenderVisual* V);
	IRenderVisual*			Instance_Find		(pcstr N);

	IRenderVisual*			CreatePE			(PS::CPEDef* source);
	IRenderVisual*			CreatePG			(PS::CPGDef* source);
	IRenderVisual*			Create				(pcstr name, IReader* data=0);
	IRenderVisual*			CreateChild			(pcstr name, IReader* data);
	void					Delete				(IRenderVisual* &V, BOOL bDiscard=FALSE);
	void					Discard				(IRenderVisual* &V, BOOL b_complete);
	void					DeleteInternal		(IRenderVisual* &V, BOOL bDiscard=FALSE);
	void					DeleteQueue			();

	void					Logging				(BOOL bEnable)	{ bLogging=bEnable; }
	
	void					Prefetch			();
	void					ClearPool			( BOOL b_complete );

	void					dump 				();
};
