#ifndef xrEPropsH
#define xrEPropsH

#ifdef XR_EPROPS_EXPORTS
#define XR_EPROPS_API __declspec(dllexport)
#else
#	ifndef XR_EPROPS_API
#		define XR_EPROPS_API __declspec(dllimport)
#	endif
#endif

#include "PropertiesListTypes.h"
#include "ItemListTypes.h"
										
enum EItemType{
	TYPE_INVALID= -1,
	TYPE_FOLDER	= 0,
	TYPE_OBJECT	= 1
};

typedef fastdelegate::FastDelegate1<ListItemsVec&> 				TOnILItemsFocused;
typedef fastdelegate::FastDelegate0<>							TOnILCloseEvent;      
typedef fastdelegate::FastDelegate3<const char*, const char*, EItemType>	TOnItemRename;
typedef fastdelegate::FastDelegate3<const char*, EItemType, bool&>	TOnItemRemove;
typedef fastdelegate::FastDelegate0<>							TOnItemAfterRemove;
typedef fastdelegate::FastDelegate0<> 							TOnCloseEvent;
typedef fastdelegate::FastDelegate0<>		  					TOnModifiedEvent;

//------------------------------------------------------------------------------
// Prepare Key
//------------------------------------------------------------------------------
inline xr_string FolderAppend	(const char* val)
{
	xr_string	tmp 	= (val&&val[0])?val:"";
	if (val&&val[0])tmp	+= "\\";
	return 	tmp;
}
inline CSharedString PrepareKey	(const char* pref, const char* key)
{
	R_ASSERT	(key);
	return 		CSharedString(xr_string(FolderAppend(pref)+key).c_str());
}
inline CSharedString PrepareKey (const char* pref0, const char* pref1, const char* key)
{
	R_ASSERT	(key);
	return 		CSharedString(xr_string(FolderAppend(pref0)+FolderAppend(pref1)+key).c_str());
}
inline CSharedString PrepareKey (const char* pref0, const char* pref1, const char* pref2, const char* key)
{
	R_ASSERT	(key);
	return 		CSharedString(xr_string(FolderAppend(pref0)+FolderAppend(pref1)+FolderAppend(pref2)+key).c_str());
}
//------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------
class XR_EPROPS_API IPropHelper{
public:
	virtual PropItem* 			__stdcall	FindItem			(PropItemsVec& items, CSharedString key, EPropType type=PROP_UNDEF)=0;
public:
//------------------------------------------------------------------------------
// predefind event routines
	virtual bool 				__stdcall 	FvectorRDOnAfterEdit(PropValue* sender, Fvector3& edit_val)=0;
	virtual void 				__stdcall 	FvectorRDOnBeforeEdit(PropValue* sender, Fvector3& edit_val)=0;
	virtual void 				__stdcall 	FvectorRDOnDraw		(PropValue* sender, xr_string& draw_val)=0;
	virtual bool 				__stdcall 	floatRDOnAfterEdit	(PropValue* sender,	float& edit_val)=0;
	virtual void 				__stdcall 	floatRDOnBeforeEdit	(PropValue* sender,	float& edit_val)=0;
	virtual void 				__stdcall 	floatRDOnDraw		(PropValue* sender, xr_string& draw_val)=0;    
// R-name edit
	virtual void				__stdcall  	NameBeforeEdit		(PropValue* sender, CSharedString& edit_val)=0;
	virtual bool 				__stdcall  	NameAfterEdit		(PropValue* sender, CSharedString& edit_val)=0;
	virtual void 				__stdcall  	NameDraw			(PropValue* sender, xr_string& draw_val)=0;
// C-name edit
	virtual void				__stdcall  	CNameBeforeEdit		(PropValue* sender, xr_string& edit_val)=0;
	virtual bool 				__stdcall  	CNameAfterEdit		(PropValue* sender, xr_string& edit_val)=0;
	virtual void 				__stdcall  	CNameDraw			(PropValue* sender, xr_string& draw_val)=0;
public:
	virtual CaptionValue*  		__stdcall	CreateCaption	    (PropItemsVec& items, CSharedString key, CSharedString val)=0;
	virtual CanvasValue*		__stdcall	CreateCanvas	    (PropItemsVec& items, CSharedString key, CSharedString val, int height)=0;
	virtual ButtonValue*		__stdcall	CreateButton	    (PropItemsVec& items, CSharedString key, CSharedString val, u32 flags)=0;
	virtual ChooseValue*		__stdcall	CreateChoose	    (PropItemsVec& items, CSharedString key, CSharedString* val, u32 mode, const char* path=0, void* fill_param=0, u32 sub_item_count=1, u32 choose_flags=cfAllowNone)=0;
	virtual S8Value* 			__stdcall	CreateS8		    (PropItemsVec& items, CSharedString key, signed char* val, signed char mn=0, signed char mx=100, signed char inc=1)=0;
	virtual S16Value* 			__stdcall	CreateS16		    (PropItemsVec& items, CSharedString key, S16* val, S16 mn=0, S16 mx=100, S16 inc=1)=0;
	virtual S32Value* 	 		__stdcall	CreateS32		    (PropItemsVec& items, CSharedString key, int* val, int mn=0, int mx=100, int inc=1)=0;
	virtual U8Value* 			__stdcall	CreateU8		    (PropItemsVec& items, CSharedString key, unsigned char* val, unsigned char mn=0, unsigned char mx=100, unsigned char inc=1)=0;
	virtual U16Value* 			__stdcall	CreateU16		    (PropItemsVec& items, CSharedString key, U16* val, U16 mn=0, U16 mx=100, U16 inc=1)=0;
	virtual U32Value* 	  		__stdcall	CreateU32		    (PropItemsVec& items, CSharedString key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1)=0;
	virtual FloatValue* 		__stdcall	CreateFloat		    (PropItemsVec& items, CSharedString key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)=0;
	virtual BOOLValue* 	  		__stdcall	CreateBOOL		    (PropItemsVec& items, CSharedString key, BOOL* val)=0;
	virtual VectorValue*  	 	__stdcall	CreateVector	    (PropItemsVec& items, CSharedString key, Fvector3* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)=0;
	virtual Flag8Value*			__stdcall	CreateFlag8		    (PropItemsVec& items, CSharedString key, Flags8* val, unsigned char mask, const char* c0=0, const char* c1=0, u32 flags=0)=0;
	virtual Flag16Value*		__stdcall	CreateFlag16	    (PropItemsVec& items, CSharedString key, Flags16* val, U16 mask, const char* c0=0, const char* c1=0, u32 flags=0)=0;
	virtual Flag32Value*		__stdcall	CreateFlag32	    (PropItemsVec& items, CSharedString key, Flags32* val, u32 mask, const char* c0=0, const char* c1=0, u32 flags=0)=0;
	virtual Token8Value*		__stdcall	CreateToken8	    (PropItemsVec& items, CSharedString key, unsigned char* val, xr_token* token)=0;
	virtual Token16Value*		__stdcall	CreateToken16	    (PropItemsVec& items, CSharedString key, U16* val, xr_token* token)=0;
	virtual Token32Value*		__stdcall	CreateToken32	    (PropItemsVec& items, CSharedString key, u32* val, xr_token* token)=0;
	virtual RToken8Value* 		__stdcall	CreateRToken8	    (PropItemsVec& items, CSharedString key, unsigned char* val, xr_rtoken* token, u32 t_cnt)=0;
	virtual RToken16Value* 		__stdcall	CreateRToken16	    (PropItemsVec& items, CSharedString key, U16* val, xr_rtoken* token, u32 t_cnt)=0;
	virtual RToken32Value* 		__stdcall	CreateRToken32	    (PropItemsVec& items, CSharedString key, u32* val, xr_rtoken* token, u32 t_cnt)=0;
	virtual RListValue* 	 	__stdcall	CreateRList		    (PropItemsVec& items, CSharedString key, CSharedString* val, CSharedString* lst, u32 cnt)=0;
	virtual U32Value*  			__stdcall	CreateColor		    (PropItemsVec& items, CSharedString key, u32* val)=0;
	virtual ColorValue*			__stdcall	CreateFColor	    (PropItemsVec& items, CSharedString key, Fcolor* val)=0;
	virtual VectorValue*		__stdcall	CreateVColor	    (PropItemsVec& items, CSharedString key, Fvector3* val)=0;
	virtual RTextValue* 		__stdcall	CreateRText		    (PropItemsVec& items, CSharedString key, CSharedString* val)=0;
	virtual STextValue* 		__stdcall	CreateSText		    (PropItemsVec& items, CSharedString key, xr_string* val)=0;
	virtual WaveValue* 			__stdcall	CreateWave		    (PropItemsVec& items, CSharedString key, WaveForm* val)=0;
	virtual FloatValue* 		__stdcall	CreateTime		    (PropItemsVec& items, CSharedString key, float* val, float mn=0.f, float mx=86400.f)=0;
	virtual ShortcutValue*		__stdcall	CreateShortcut		(PropItemsVec& items, CSharedString key, xr_shortcut* val)=0;

	virtual FloatValue* 		__stdcall	CreateAngle		    (PropItemsVec& items, CSharedString key, float* val, float mn=flt_min, float mx=flt_max, float inc=0.01f, int decim=2)=0;
	virtual VectorValue* 		__stdcall	CreateAngle3	    (PropItemsVec& items, CSharedString key, Fvector3* val, float mn=flt_min, float mx=flt_max, float inc=0.01f, int decim=2)=0;
	virtual RTextValue* 		__stdcall	CreateName		    (PropItemsVec& items, CSharedString key, CSharedString* val, ListItem* owner)=0;
	virtual RTextValue* 		__stdcall	CreateNameCB		(PropItemsVec& items, CSharedString key, CSharedString* val, TOnDrawTextEvent=0, RTextValue::TOnBeforeEditEvent=0, RTextValue::TOnAfterEditEvent=0)=0;

	// obsolette    
	virtual CTextValue* 		__stdcall	CreateCText			(PropItemsVec& items, CSharedString key, char* val, u32 sz)=0;
	virtual CListValue* 	 	__stdcall	CreateCList		    (PropItemsVec& items, CSharedString key, char* val, u32 sz, xr_string* lst, u32 cnt)=0;
	virtual CTextValue* 		__stdcall	CreateCName		    (PropItemsVec& items, CSharedString key, char* val, u32 sz, ListItem* owner)=0;
	virtual TokenValueSH*   	__stdcall	CreateTokenSH	    (PropItemsVec& items, CSharedString key, u32* val, const TokenValueSH::SItem* lst, u32 cnt)=0;
	virtual CTextValue* 		__stdcall	CreateTexture		(PropItemsVec& items, CSharedString key, char* val, u32 sz)=0;
};
//---------------------------------------------------------------------------
extern IPropHelper &PHelper();

//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
// List
//------------------------------------------------------------------------------
class XR_EPROPS_API IListHelper{
public:
	virtual ListItem* 			__stdcall	FindItem			(ListItemsVec& items, const char* key)=0;
	virtual bool 				__stdcall	NameAfterEdit		(ListItem* sender, const char* value, CSharedString& edit_val)=0;
public:
	virtual ListItem*			__stdcall	CreateItem			(ListItemsVec& items, const char* key, int type, u32 item_flags=0, void* object=0)=0;
};

#endif
 