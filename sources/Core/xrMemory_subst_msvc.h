#ifdef DEBUG_MEMORY_NAME
// new(0)
template <class T>
inline	T*		xr_new		()
{
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T();
}
// new(1)
template <class T, class P1>
inline	T*		xr_new		(const P1& p1) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1);
}
// new(2)
template <class T, class P1, class P2> 
inline	T*		xr_new		(const P1& p1, const P2& p2) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2);
}
// new(3)
template <class T, class P1, class P2, class P3>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2,p3);
}
// new(4)
template <class T, class P1, class P2, class P3, class P4>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2,p3,p4);
}
// new(5)
template <class T, class P1, class P2, class P3, class P4, class P5>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2,p3,p4,p5);
}
// new(6)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2,p3,p4,p5,p6);
}
// new(7)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2,p3,p4,p5,p6,p7);
}
// new(8)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2,p3,p4,p5,p6,p7,p8);
}
// new(9)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P8& p9) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(p1,p2,p3,p4,p5,p6,p7,p8,p9);
}
#else // DEBUG_MEMORY_NAME
// new(0)
template <class T>
inline	T*		xr_new		()
{
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T();
}
// new(1)
template <class T, class P1>
inline	T*		xr_new		(const P1& p1) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1);
}
// new(2)
template <class T, class P1, class P2> 
inline	T*		xr_new		(const P1& p1, const P2& p2) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2);
}
// new(3)
template <class T, class P1, class P2, class P3>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3);
}
// new(4)
template <class T, class P1, class P2, class P3, class P4>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4);
}
// new(5)
template <class T, class P1, class P2, class P3, class P4, class P5>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5);
}
// new(6)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5,p6);
}
// new(7)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5,p6,p7);
}
// new(8)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5,p6,p7,p8);
}
// new(9)
template <class T, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
inline	T*		xr_new		(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7, const P8& p8, const P8& p9) {
	T* ptr	= (T*)Memory.mem_alloc(sizeof(T));
	return new (ptr) T(p1,p2,p3,p4,p5,p6,p7,p8,p9);
}
#endif // DEBUG_MEMORY_NAME

template <bool _is_pm, typename T>
struct xr_special_free
{
	inline void operator( )(T*& ptr)
	{
		void* _real_ptr = dynamic_cast<void*>(ptr);
		ptr->~T( );
		Memory.mem_free(_real_ptr);
	}
};

template <typename T>
struct xr_special_free<false, T>
{
	inline void operator( )(T*& ptr)
	{
		ptr->~T( );
		Memory.mem_free(ptr);
	}
};

template <class T>
inline void xr_delete(T*& ptr)
{
	if (ptr)
	{
		xr_special_free<is_polymorphic<T>::result, T>( )(ptr);
		ptr = NULL;
	}
}

template <class T>
inline void xr_delete(T* const& ptr)
{
	if (ptr)
	{
		xr_special_free<is_polymorphic<T>::result, T>(ptr);
		const_cast<T*&>(ptr) = NULL;
	}
}
