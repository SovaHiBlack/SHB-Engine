#pragma once

#ifdef DEBUG

class CLevelDebug
{
public:

	template <typename T>
	class CItemBase
	{

		xr_vector<T>		m_data;
		typedef typename xr_vector<T>::iterator		ITEM_STORAGE_VEC_IT;

		struct remove_text_pred
		{
			pcstr text;
			remove_text_pred(pcstr t) : text(t)
			{ }
			bool operator () (const T& item)
			{
				return (item.text == text);
			}
		};

		struct remove_id_pred
		{
			u32 id;
			remove_id_pred(u32 i) : id(i)
			{ }
			bool operator () (const T& item)
			{
				return (item.id == id);
			}
		};

		struct sort_id_pred
		{
			bool operator() (const T& item1, const T& item2)
			{
				return (item1.id < item2.id);
			}
		};

	public:
		IC	void	add_item(T data)
		{
			m_data.push_back(data);
			std::sort(m_data.begin( ), m_data.end( ), sort_id_pred( ));
		}

		IC	void	remove_item(pcstr text)
		{
			ITEM_STORAGE_VEC_IT it = std::remove_if(m_data.begin( ), m_data.end( ), remove_text_pred(text));
			m_data.erase(it, m_data.end( ));

			std::sort(m_data.begin( ), m_data.end( ), sort_id_pred( ));
		}
		IC	void	remove_item(u32 id)
		{
			ITEM_STORAGE_VEC_IT it = std::remove_if(m_data.begin( ), m_data.end( ), remove_id_pred(id));
			m_data.erase(it, m_data.end( ));

			std::sort(m_data.begin( ), m_data.end( ), sort_id_pred( ));
		}
		IC	void	clear( )
		{
			m_data.clear( );
		}

		template<class T>
		IC	void	process(T& process_pred)
		{
			for (ITEM_STORAGE_VEC_IT it = m_data.begin( ); it != m_data.end( ); ++it)
			{
				process_pred(*it);
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////

	struct SInfoItem
	{
		shared_str	text;
		u32			color;
		u32			id;

		SInfoItem(pcstr str, u32 col, u32 i) : text(str), color(col), id(i)
		{ }
	};

	class CObjectInfo : public CItemBase<SInfoItem>
	{

		typedef CItemBase<SInfoItem> inherited;

#define	DELTA_HEIGHT_DEFAULT	16.f
#define	SHIFT_POS_DEFAULT		fVector3().set(0.0f,2.0f,0.0f)

		fVector3			m_shift_pos;
		f32			m_delta_height;

	public:

		CObjectInfo( )
		{
			setup( );
		}

		void	add_item(pcstr text, u32 color, u32 id = u32(-1));

		void	draw_info(f32 x, f32& y);
		IC	void	setup(const fVector3& shift = SHIFT_POS_DEFAULT, f32 delta = DELTA_HEIGHT_DEFAULT)
		{
			m_shift_pos.set(shift); m_delta_height = delta;
		}

		IC	fVector3& get_shift_pos( )
		{
			return m_shift_pos;
		}
	};

	//////////////////////////////////////////////////////////////////////////

	struct STextItem
	{
		shared_str	text;

		f32		x;
		f32		y;

		u32			color;
		u32			id;

		STextItem(pcstr str, f32 coord_x, f32 coord_y, u32 col, u32 i) : text(str), x(coord_x), y(coord_y), color(col), id(i)
		{ }
	};

	class CTextInfo : public CItemBase<STextItem>
	{
		typedef CItemBase<STextItem> inherited;

	public:
		void	add_item(pcstr text, f32 x, f32 y, u32 color, u32 id = u32(-1));
		void	draw_text( );
	};

	//////////////////////////////////////////////////////////////////////////

	struct SLevelItem
	{
		fVector3		position1;
		fVector3		position2;
		f32		radius;

		enum
		{
			ePoint = u32(0),
			eLine,
			eBox
		} ptype;

		u32			color;
		u32			id;

		SLevelItem(const fVector3& p, u32 col, u32 i)
		{
			set(p, col, i);
			ptype = ePoint;
		}

		SLevelItem(const fVector3& p, const fVector3& p2, u32 col, u32 i)
		{
			set(p, col, i);
			ptype = eLine;
			position2 = p2;
		}

		SLevelItem(const fVector3& p, f32 r, u32 col, u32 i)
		{
			set(p, col, i);
			ptype = eBox;
			radius = r;
		}

		void	set(const fVector3& p, u32 col, u32 i)
		{
			position1 = p;
			color = col;
			id = i;
		}
	};

	class CLevelInfo : public CItemBase<SLevelItem>
	{
		typedef CItemBase<SLevelItem> inherited;
	public:
		void	add_item(const fVector3& pos, u32 color, u32 id = u32(-1));
		void	add_item(const fVector3& pos1, const fVector3& pos2, u32 color, u32 id = u32(-1));
		void	add_item(const fVector3& pos, f32 radius, u32 color, u32 id = u32(-1));
		void	draw_info( );
	};

public:
	CLevelDebug( );
	~CLevelDebug( );

	template<class T>
	CObjectInfo& object_info(CObject* obj, T typed_class)
	{
		return object_info(obj, typeid((*typed_class)).name( ));
	}

	template<class T>
	CObjectInfo& object_info(T typed_class)
	{
		return object_info(typed_class, typeid((*typed_class)).name( ));
	}

	template<class T>
	CTextInfo& text(T typed_class)
	{
		return text(typed_class, typeid((*typed_class)).name( ));
	}

	template<class T>
	CLevelInfo& level_info(T typed_class)
	{
		return level_info(typed_class, typeid((*typed_class)).name( ));
	}

	void		draw_object_info( );
	void		draw_text( );
	void		draw_level_info( );

	void		on_destroy_object(CObject* obj);

private:
	void		free_mem( );

	CObjectInfo& object_info(CObject* obj, pcstr class_name);
	CTextInfo& text(pvoid class_ptr, pcstr class_name);
	CLevelInfo& level_info(pvoid class_ptr, pcstr class_name);

private:
	struct SKey
	{
		void* class_ptr;
		pcstr	class_name;

		SKey(void* ptr, pcstr name)
		{
			class_ptr = ptr; class_name = name;
		}

		bool	operator <	(const SKey& val) const
		{
			return (class_ptr < val.class_ptr);
		}
	};

	DEFINE_MAP(pcstr, CObjectInfo*, CLASS_INFO_MAP, CLASS_INFO_MAP_IT);
	DEFINE_MAP(CObject*, CLASS_INFO_MAP, OBJECT_INFO_MAP, OBJECT_INFO_MAP_IT);
	DEFINE_MAP(SKey, CTextInfo*, TEXT_INFO_MAP, TEXT_INFO_MAP_IT);
	DEFINE_MAP(SKey, CLevelInfo*, LEVEL_INFO_MAP, LEVEL_INFO_MAP_IT);

	OBJECT_INFO_MAP		m_objects_info;
	TEXT_INFO_MAP		m_text_info;
	LEVEL_INFO_MAP		m_level_info;
};

#endif
