//---------------------------------------------------------------------------
#ifndef ItemListTypesH
#define ItemListTypesH

//---------------------------------------------------------------------------

class ListItem
{
	friend class		CListHelper;
	friend class		TItemList;
	shared_str		  	key;
	s32					type;
	pvoid item;

public:
	typedef fastdelegate::FastDelegate1<ListItem*> 					TOnListItemFocused;
	typedef fastdelegate::FastDelegate1<ListItem*> 					TOnClick;
	TOnClick			OnClickEvent;
	TOnListItemFocused	OnItemFocused;
	TOnDrawThumbnail	OnDrawThumbnail;

public:
	s32 				tag;
	pvoid				m_Object;
	s32					icon_index;
	u32					prop_color;

public:
	enum
	{
		flShowCB = (1 << 0),
		flCBChecked = (1 << 1),
		flDrawThumbnail = (1 << 2),
		flDrawCanvas = (1 << 3),
		flSorted = (1 << 4),
		flHidden = (1 << 5)
	};
	flags32				m_Flags;

public:
	ListItem(s32 _type) : type(_type), prop_color(0), item(0), key(0), tag(0), icon_index(-1), OnDrawThumbnail(0), OnItemFocused(0), m_Object(0)
	{
		m_Flags.zero( );
	}
	virtual 			~ListItem( )
	{ }
	void				SetName(pcstr _key)
	{
		key = _key;
	}

	IC void				Visible(BOOL val)
	{
		m_Flags.set(flHidden, !val);
	}
	IC s32				Type( )
	{
		return type;
	}
	IC pvoid Item( )
	{
		return item;
	}
	IC pcstr			Key( )
	{
		return *key;
	}
	IC void				SetIcon(s32 index)
	{
		icon_index = index;
	}
};

DEFINE_VECTOR(ListItem*, ListItemsVec, ListItemsIt);
//---------------------------------------------------------------------------
#endif
