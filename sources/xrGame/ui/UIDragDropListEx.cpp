#include "stdafx.h"
#include "UIDragDropListEx.h"
#include "UIScrollBar.h"
#include "../object_broker.h"
#include "UICellItem.h"

CUIDragItem* CUIDragDropListEx::m_drag_item = NULL;

void CUICell::Clear()
{
	m_bMainItem = false;
	if(m_item)	m_item->SetOwnerList(NULL);
	m_item		= NULL; 
}

CUIDragDropListEx::CUIDragDropListEx()
{

	m_flags.zero				();
	m_container					= xr_new<CUICellContainer>(this);
	m_vScrollBar				= xr_new<CUIScrollBar>();
	m_vScrollBar->SetAutoDelete	(true);
	m_selected_item				= NULL;

	SetCellSize					(iVector2().set(50,50));
	SetCellsCapacity			(iVector2().set(0,0));

	AttachChild					(m_container);
	AttachChild					(m_vScrollBar);

	m_vScrollBar->SetWindowName	("scroll_v");
	Register					(m_vScrollBar);
	AddCallback					("scroll_v",	SCROLLBAR_VSCROLL,				CUIWndCallback::void_function				(this, &CUIDragDropListEx::OnScrollV)		);
	AddCallback					("cell_item",	DRAG_DROP_ITEM_DRAG,			CUIWndCallback::void_function			(this, &CUIDragDropListEx::OnItemStartDragging)	);
	AddCallback					("cell_item",	DRAG_DROP_ITEM_DROP,			CUIWndCallback::void_function			(this, &CUIDragDropListEx::OnItemDrop)			);
	AddCallback					("cell_item",	DRAG_DROP_ITEM_SELECTED,		CUIWndCallback::void_function		(this, &CUIDragDropListEx::OnItemSelected)			);
	AddCallback					("cell_item",	DRAG_DROP_ITEM_RBUTTON_CLICK,	CUIWndCallback::void_function	(this, &CUIDragDropListEx::OnItemRButtonClick)			);
	AddCallback					("cell_item",	DRAG_DROP_ITEM_DB_CLICK,		CUIWndCallback::void_function		(this, &CUIDragDropListEx::OnItemDBClick)			);
}

CUIDragDropListEx::~CUIDragDropListEx()
{
	DestroyDragItem		();

	delete_data					(m_container);
}

void CUIDragDropListEx::SetAutoGrow(bool b)						
{
	m_flags.set(flAutoGrow,b);
}
bool CUIDragDropListEx::IsAutoGrow()								
{
	return !!m_flags.test(flAutoGrow);
}
void CUIDragDropListEx::SetGrouping(bool b)						
{
	m_flags.set(flGroupSimilar,b);
}
bool CUIDragDropListEx::IsGrouping()
{
	return !!m_flags.test(flGroupSimilar);
}
void CUIDragDropListEx::SetCustomPlacement(bool b)
{
	m_flags.set(flCustomPlacement,b);
}

bool CUIDragDropListEx::GetCustomPlacement()
{
	return !!m_flags.test(flCustomPlacement);
}

void CUIDragDropListEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIDragDropListEx::Init(f32 x, f32 y, f32 w, f32 h)
{
	inherited::SetWndRect				(x,y,w,h);
	m_vScrollBar->Init					(w, 0, h, false);
	m_vScrollBar->SetWndPos				(m_vScrollBar->GetWndPos().x - m_vScrollBar->GetWidth(), m_vScrollBar->GetWndPos().y);
/*
//.	m_background->Init					(0,0,w,h);
//.	m_background->Init					("ui\\ui_frame_02_back",0,0,w,h);
//.	fRect rect; rect.set				(0,0,64,64);
//.	m_background->GetUIStaticItem().SetOriginalRect(rect);
//.	m_background->SetStretchTexture		(true);
*/
}

void CUIDragDropListEx::OnScrollV(CUIWindow* w, void* pData)
{
	m_container->SetWndPos		(m_container->GetWndPos().x, f32(-m_vScrollBar->GetScrollPos()));
}

void CUIDragDropListEx::CreateDragItem(CUICellItem* itm)
{
	R_ASSERT							(!m_drag_item);
	m_drag_item							= itm->CreateDragItem();
	GetParent()->SetCapture				(m_drag_item, true);
}

void CUIDragDropListEx::DestroyDragItem()
{
	if(m_selected_item && m_drag_item && m_drag_item->ParentItem()==m_selected_item)
	{
		VERIFY(GetParent()->GetMouseCapturer()==m_drag_item);
		GetParent()->SetCapture				(NULL, false);

		delete_data							(m_drag_item);
	}
}

fVector2 CUIDragDropListEx::GetDragItemPosition()
{
	return m_drag_item->GetPosition();
}

void CUIDragDropListEx::OnItemStartDragging(CUIWindow* w, void* pData)
{
	OnItemSelected						(w, pData);
	CUICellItem* itm		= smart_cast<CUICellItem*>(w);

	if(itm!=m_selected_item)	return;
	
	if(m_f_item_start_drag && m_f_item_start_drag(itm) ) return;

	CreateDragItem						(itm);
}

void CUIDragDropListEx::OnItemDrop(CUIWindow* w, void* pData)
{
	OnItemSelected						(w, pData);
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);
	VERIFY								(itm->OwnerList() == itm->OwnerList());

	if(m_f_item_drop && m_f_item_drop(itm) ){
		DestroyDragItem						();
		return;
	}

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= m_drag_item->BackList();

	bool b				= (old_owner==new_owner)&&!GetCustomPlacement();

	if(old_owner&&new_owner && !b)
	{
		CUICellItem* i					= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		while(i->ChildsCount())
		{
			CUICellItem* _chld				= i->PopChild();
			new_owner->SetItem				(_chld, old_owner->GetDragItemPosition());
		}
		new_owner->SetItem				(i,old_owner->GetDragItemPosition());
	}
	DestroyDragItem						();
}

void CUIDragDropListEx::OnItemDBClick(CUIWindow* w, void* pData)
{
	OnItemSelected						(w, pData);
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);

	if(m_f_item_db_click && m_f_item_db_click(itm) ){
		DestroyDragItem						();
		return;
	}

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	VERIFY								(m_drag_item==NULL);
	VERIFY								(old_owner == this);

	if(old_owner&&old_owner->GetCustomPlacement())
	{
		CUICellItem* i					= old_owner->RemoveItem(itm, true);
		old_owner->SetItem				(i);
	}

	DestroyDragItem						();
}

void CUIDragDropListEx::OnItemSelected(CUIWindow* w, void* pData)
{
	m_selected_item						= smart_cast<CUICellItem*>(w);
	VERIFY								(m_selected_item);
	if(m_f_item_selected)
		m_f_item_selected(m_selected_item);
}

void CUIDragDropListEx::OnItemRButtonClick(CUIWindow* w, void* pData)
{
	OnItemSelected						(w, pData);
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);
	if(m_f_item_rbutton_click) 
		m_f_item_rbutton_click(itm);
}

void CUIDragDropListEx::GetClientArea(fRect& r)
{
	GetAbsoluteRect				(r);
	if(m_vScrollBar->GetVisible())
		r.x2 -= m_vScrollBar->GetWidth	();
}

void CUIDragDropListEx::ClearAll(bool bDestroy)
{
	DestroyDragItem			();
	m_container->ClearAll	(bDestroy);
	m_selected_item			= NULL;
	m_container->SetWndPos	(0,0);
	ResetCellsCapacity		();
}

void CUIDragDropListEx::Compact()
{
	CUIWindow::WINDOW_LIST	wl		= m_container->GetChildWndList();
	ClearAll						(false);

	CUIWindow::WINDOW_LIST_it it	= wl.begin();
	CUIWindow::WINDOW_LIST_it it_e	= wl.end();
	for(;it!=it_e;++it)
	{
		CUICellItem*	itm			= smart_cast<CUICellItem*>(*it);
		SetItem						(itm);
	}
}


#include "../HUDManager.h"
void CUIDragDropListEx::Draw()
{
	inherited::Draw				();

	if(0 && bDebug){
		CGameFont* F		= UI()->Font()->pFontDI;
		F->SetAligment		(CGameFont::alCenter);
		F->SetHeightI		(0.02f);
		F->OutSetI			(0.f,-0.5f);
		F->SetColor			(0xffffffff);
		iVector2			pt = m_container->PickCell(GetUICursor()->GetCursorPosition());
		F->OutNext			("%d-%d",pt.x, pt.y);
	}
}

void CUIDragDropListEx::Update()
{
	inherited::Update			();

	if( m_drag_item ){
		fRect	wndRect;
		GetAbsoluteRect(wndRect);
		fVector2 cp			= GetUICursor()->GetCursorPosition();
		if(wndRect.in(cp)){
			if(NULL==m_drag_item->BackList())
				m_drag_item->SetBackList(this);
		}else
			if( this==m_drag_item->BackList() )
				m_drag_item->SetBackList(NULL);
	}
}

void CUIDragDropListEx::ReinitScroll()
{
	f32 h1 = m_container->GetWndSize().y;
	f32 h2 = GetWndSize().y;
		VERIFY						(_valid(h1));
		VERIFY						(_valid(h2));
		m_vScrollBar->Show			( h1 > h2 );
		m_vScrollBar->Enable		( h1 > h2 );

		m_vScrollBar->SetRange		(0, _max(0,iFloor(h1-h2)) );
		m_vScrollBar->SetScrollPos	(0);
		m_vScrollBar->SetStepSize	(CellSize().y/3);
		m_vScrollBar->SetPageSize	(iFloor(GetWndSize().y/ f32(CellSize().y)));
		m_container->SetWndPos		(0,0);
}

bool CUIDragDropListEx::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	bool b = inherited::OnMouse		(x,y,mouse_action);

	if(m_vScrollBar->IsShown())
	{
		switch(mouse_action){
		case WINDOW_MOUSE_WHEEL_DOWN:
				m_vScrollBar->TryScrollInc();
				return true;
				break;

		case WINDOW_MOUSE_WHEEL_UP:
				m_vScrollBar->TryScrollDec();
				return true;
				break;
		}
	}
	return b;
}

const iVector2& CUIDragDropListEx::CellsCapacity()
{
	return m_container->CellsCapacity();
}

void CUIDragDropListEx::SetCellsCapacity(const iVector2 c)
{
	m_container->SetCellsCapacity(c);
}

const iVector2& CUIDragDropListEx::CellSize()
{
	return m_container->CellSize();
}

void CUIDragDropListEx::SetCellSize(const iVector2 new_sz)
{
	m_container->SetCellSize(new_sz);
}

int CUIDragDropListEx::ScrollPos()
{
	return m_vScrollBar->GetScrollPos();
}

void CUIDragDropListEx::SetItem(CUICellItem* itm) //auto
{
	if(m_container->AddSimilar(itm)){
		return;
	}

	iVector2 dest_cell_pos =	m_container->FindFreeCell(itm->GetGridSize());

	SetItem						(itm,dest_cell_pos);
}

void CUIDragDropListEx::SetItem(CUICellItem* itm, fVector2 abs_pos) // start at cursor pos
{
	if(m_container->AddSimilar(itm))	return;

	const iVector2 dest_cell_pos =	m_container->PickCell		(abs_pos);

	if(m_container->ValidCell(dest_cell_pos) && m_container->IsRoomFree(dest_cell_pos,itm->GetGridSize()))
		SetItem						(itm, dest_cell_pos);
	else
		SetItem						(itm);
}

void CUIDragDropListEx::SetItem(CUICellItem* itm, iVector2 cell_pos) // start at cell
{
	if(m_container->AddSimilar(itm))	return;
	R_ASSERT						(m_container->IsRoomFree(cell_pos, itm->GetGridSize()));

	m_container->PlaceItemAtPos	(itm, cell_pos);

	itm->SetWindowName			("cell_item");
	Register					(itm);
	itm->SetOwnerList			(this);
}

bool CUIDragDropListEx::CanSetItem(CUICellItem* itm){
	if (m_container->HasFreeSpace(itm->GetGridSize()))
		return true;
	Compact();

	return m_container->HasFreeSpace(itm->GetGridSize());
}

CUICellItem* CUIDragDropListEx::RemoveItem(CUICellItem* itm, bool force_root)
{
	CUICellItem* i				= m_container->RemoveItem		(itm, force_root);
	i->SetOwnerList				((CUIDragDropListEx*)NULL);
	return						i;
}

u32 CUIDragDropListEx::ItemsCount()
{
	return m_container->GetChildWndList().size();
}

bool CUIDragDropListEx::IsOwner(CUICellItem* itm){
	return m_container->IsChild(itm);
}

CUICellItem* CUIDragDropListEx::GetItemIdx(u32 idx)
{
	R_ASSERT(idx<ItemsCount());
	WINDOW_LIST_it it = m_container->GetChildWndList().begin();
	std::advance	(it, idx);
	return smart_cast<CUICellItem*>(*it);
}

CUICellContainer::CUICellContainer(CUIDragDropListEx* parent)
{
	m_pParentDragDropList		= parent;
	hShader.create				("hud\\fog_of_war","ui\\ui_grid");
	hGeom.create				(FVF::F_TL, RCache.Vertex.Buffer(), 0);
}

CUICellContainer::~CUICellContainer()
{
}

bool CUICellContainer::AddSimilar(CUICellItem* itm)
{
	if(!m_pParentDragDropList->IsGrouping())	return false;

	CUICellItem* i		= FindSimilar(itm);
	R_ASSERT			(i!=itm);
	R_ASSERT			(0==itm->ChildsCount());
	if(i){	
		i->PushChild			(itm);
		itm->SetOwnerList		(m_pParentDragDropList);
	}
	
	return (i!=NULL);
}

CUICellItem* CUICellContainer::FindSimilar(CUICellItem* itm)
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
#ifdef DEBUG
		CUICellItem* i = smart_cast<CUICellItem*>(*it);
#else
		CUICellItem* i = (CUICellItem*)(*it);
#endif
		R_ASSERT		(i!=itm);
		if(i->EqualTo(itm))
			return i;
	}
	return NULL;
}

void CUICellContainer::PlaceItemAtPos(CUICellItem* itm, iVector2& cell_pos)
{
	iVector2 cs				= itm->GetGridSize();
	for(int x=0; x<cs.x; ++x)
		for(int y=0; y<cs.y; ++y){
			CUICell& C		= GetCellAt(iVector2().set(x,y).add(cell_pos));
			C.SetItem		(itm,(x==0&&y==0));
		}

	itm->SetWndPos			(fVector2().set( (m_cellSize.x*cell_pos.x), (m_cellSize.y*cell_pos.y))	);
	itm->SetWndSize			(fVector2().set( (m_cellSize.x*cs.x),		(m_cellSize.y*cs.y)		 )	);

	AttachChild				(itm);
	itm->OnAfterChild		();
}

CUICellItem* CUICellContainer::RemoveItem(CUICellItem* itm, bool force_root)
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		CUICellItem* i		= (CUICellItem*)(*it);
		
		if(i->HasChild(itm))
		{
			CUICellItem* iii	= i->PopChild();
			R_ASSERT			(0==iii->ChildsCount());
			return				iii;
		}
	}

	if(!force_root && itm->ChildsCount())
	{
		CUICellItem* iii	=	itm->PopChild();
		R_ASSERT			(0==iii->ChildsCount());
		return				iii;
	}

	iVector2 pos			= GetItemPos(itm);
	iVector2 cs				= itm->GetGridSize();

	for(int x=0; x<cs.x;++x)
		for(int y=0; y<cs.y;++y)
		{
			CUICell& C		= GetCellAt(iVector2().set(x,y).add(pos));
			C.Clear			();
		}

	itm->SetOwnerList		(NULL);
	DetachChild				(itm);
	return					itm;
}

iVector2 CUICellContainer::FindFreeCell	(const iVector2& size)
{
	iVector2 tmp;
	for(tmp.y=0; tmp.y<=m_cellsCapacity.y-size.y; ++tmp.y )
		for(tmp.x=0; tmp.x<=m_cellsCapacity.x-size.x; ++tmp.x )
			if(IsRoomFree(tmp,size))
				return  tmp;

	if(m_pParentDragDropList->IsAutoGrow())
	{
		Grow	();
		return							FindFreeCell	(size);
	}else{
		m_pParentDragDropList->Compact		();
		for(tmp.y=0; tmp.y<=m_cellsCapacity.y-size.y; ++tmp.y )
			for(tmp.x=0; tmp.x<=m_cellsCapacity.x-size.x; ++tmp.x )
				if(IsRoomFree(tmp,size))
					return  tmp;

		R_ASSERT2		(0,"there are no free room to place item");
	}
	return			tmp;
}

bool CUICellContainer::HasFreeSpace		(const iVector2& size){
	iVector2 tmp;
	for(tmp.y=0; tmp.y<=m_cellsCapacity.y-size.y; ++tmp.y )
		for(tmp.x=0; tmp.x<=m_cellsCapacity.x-size.x; ++tmp.x )
			if(IsRoomFree(tmp,size))
				return true;

	return false;
}

bool CUICellContainer::IsRoomFree(const iVector2& pos, const iVector2& size)
{
	iVector2 tmp;
	for(tmp.x =pos.x; tmp.x<pos.x+size.x; ++tmp.x)
		for(tmp.y =pos.y; tmp.y<pos.y+size.y; ++tmp.y)
		{
			if(!ValidCell(tmp))		return		false;

			CUICell& C				= GetCellAt(tmp);

			if(!C.Empty())			return		false;
		}
	return true;
}

void CUICellContainer::GetTexUVLT(fVector2& uv, u32 col, u32 row)
{
	uv.set(0.0f,0.0f);

//.	if( (col%2==1 && row%2==1)||(col%2==0 && row%2==0) )
//.		uv.set(0.5f,0.0f);
}


void CUICellContainer::SetCellsCapacity(const iVector2& c)
{
	m_cellsCapacity				= c;
	m_cells.resize				(c.x*c.y);
	ReinitSize					();
}

void CUICellContainer::SetCellSize(const iVector2& new_sz)
{
	m_cellSize					= new_sz;
	ReinitSize					();
}

iVector2 CUICellContainer::TopVisibleCell()
{
	return iVector2().set	(0, iFloor(m_pParentDragDropList->ScrollPos()/ f32(CellSize().y)));
}

CUICell& CUICellContainer::GetCellAt(const iVector2& pos)
{
	R_ASSERT			(ValidCell(pos));
	CUICell&	c		= m_cells[m_cellsCapacity.x*pos.y+pos.x];
	return				c;
}

iVector2 CUICellContainer::GetItemPos(CUICellItem* itm)
{
	for(int x=0; x<m_cellsCapacity.x ;++x)
		for(int y=0; y<m_cellsCapacity.y ;++y){
			iVector2 p;
			p.set(x,y);
		if(GetCellAt(p).m_item==itm)
			return p;
		}

		R_ASSERT(0);
		return iVector2().set(-1,-1);
}

u32 CUICellContainer::GetCellsInRange(const iRect& rect, UI_CELLS_VEC& res)
{
	res.clear_not_free				();
	for(int x=rect.x1;x<=rect.x2;++x)
		for(int y=rect.y1;y<=rect.y2;++y)
			res.push_back	(GetCellAt(iVector2().set(x,y)));

	std::unique				(res.begin(), res.end());
	return res.size			();
}

void CUICellContainer::ReinitSize()
{
	fVector2							sz;
	sz.set								(CellSize().x*m_cellsCapacity.x, CellSize().y*m_cellsCapacity.y);
	SetWndSize							(sz);
	m_pParentDragDropList->ReinitScroll	();
}

void CUICellContainer::Grow()
{
	SetCellsCapacity	(iVector2().set(m_cellsCapacity.x,m_cellsCapacity.y+1));
}

void CUICellContainer::Shrink()
{
}

bool CUICellContainer::ValidCell(const iVector2& pos) const
{
	return !(pos.x<0 || pos.y<0 || pos.x>=m_cellsCapacity.x || pos.y>=m_cellsCapacity.y);
}

void CUICellContainer::ClearAll(bool bDestroy)
{
	{
		UI_CELLS_VEC_IT it		= m_cells.begin();
		UI_CELLS_VEC_IT it_e	= m_cells.end();
		for(;it!=it_e;++it)
			(*it).Clear();
	}
	while( !m_ChildWndList.empty() )
	{
		CUIWindow* w			= m_ChildWndList.back();
		CUICellItem* wc			= smart_cast<CUICellItem*>(w);
		VERIFY					(!wc->IsAutoDelete());
		DetachChild				(wc);	
		
		while( wc->ChildsCount() )
		{
			CUICellItem* ci		= wc->PopChild();
			R_ASSERT			(ci->ChildsCount()==0);

			if(bDestroy)
				delete_data		(ci);
		}
		
		if(bDestroy){
			delete_data			(wc);
		}
	}
}

iVector2 CUICellContainer::PickCell(const fVector2& abs_pos)
{
	iVector2 res;
	fVector2 ap;
	GetAbsolutePos							(ap);
	ap.sub									(abs_pos);
	ap.mul									(-1);
	res.x									= iFloor(ap.x/m_cellSize.x);
	res.y									= iFloor(ap.y/m_cellSize.y);
	if(!ValidCell(res))						res.set(-1, -1);
	return res;
}

void CUICellContainer::Draw()
{
	fRect clientArea;
	m_pParentDragDropList->GetClientArea(clientArea);

	iVector2			cell_cnt = m_pParentDragDropList->CellsCapacity();
	if					(cell_cnt.x==0 || cell_cnt.y==0)	return;

	iVector2			cell_sz = CellSize();

	iRect				tgt_cells;
	tgt_cells.lt		= TopVisibleCell();
	tgt_cells.x2		= iFloor( (f32(clientArea.width())+ f32(cell_sz.x)- EPSILON_5)/ f32(cell_sz.x)) + tgt_cells.lt.x;
	tgt_cells.y2		= iFloor( (f32(clientArea.height())+ f32(cell_sz.y)- EPSILON_5)/ f32(cell_sz.y)) + tgt_cells.lt.y;

	clamp				(tgt_cells.x2, 0, cell_cnt.x-1);
	clamp				(tgt_cells.y2, 0, cell_cnt.y-1);

	fVector2			lt_abs_pos;
	GetAbsolutePos		(lt_abs_pos);

	fVector2					drawLT;
	drawLT.set					(lt_abs_pos.x+tgt_cells.lt.x*cell_sz.x, lt_abs_pos.y+tgt_cells.lt.y*cell_sz.y);
	UI()->ClientToScreenScaled	(drawLT, drawLT.x, drawLT.y);

	const fVector2 pts[6] = { {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},
								 {0.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f} };
#define ty 1.0f
#define tx 0.5f
	const fVector2 uvs[6] = { {0.0f,0.0f},{tx,0.0f},{tx,ty},
								 {0.0f,0.0f},{tx,ty},{0.0f,ty} };

	// calculate cell size in screen pixels
	fVector2 f_len;
	UI()->ClientToScreenScaled(f_len, f32(cell_sz.x), f32(cell_sz.y) );

	// fill cell buffer
	u32 vOffset					= 0;
	FVF::TL* start_pv			= (FVF::TL*)RCache.Vertex.Lock	((tgt_cells.width()+1)*(tgt_cells.height()+1)*6,hGeom.stride(),vOffset);
	FVF::TL* pv					= start_pv;
	for (int x=0; x<=tgt_cells.width(); ++x){
		for (int y=0; y<=tgt_cells.height(); ++y){
			fVector2			tp;
			GetTexUVLT			(tp,tgt_cells.x1+x,tgt_cells.y1+y);
			for (u32 k=0; k<6; ++k,++pv){
				const fVector2& p	= pts[k];
				const fVector2& uv	= uvs[k];
				pv->set			(iFloor(drawLT.x + p.x*(f_len.x) + f_len.x*x)-0.5f, 
								 iFloor(drawLT.y + p.y*(f_len.y) + f_len.y*y)-0.5f, 
								 0xFFFFFFFF,tp.x+uv.x,tp.y+uv.y);
			}
		}
	}
	std::ptrdiff_t p_cnt		= (pv-start_pv)/3;
	RCache.Vertex.Unlock		(u32(pv-start_pv),hGeom.stride());

	UI()->PushScissor					(clientArea);

	if (p_cnt!=0){
		// draw grid
		RCache.set_Shader		(hShader);
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,u32(p_cnt));
	}

	//draw shown items in range
	if( GetCellsInRange(tgt_cells,m_cells_to_draw) ){
		UI_CELLS_VEC_IT it = m_cells_to_draw.begin();
		for(;it!=m_cells_to_draw.end();++it)
			if( !(*it).Empty() && !(*it).m_item->m_b_already_drawn ){
				(*it).m_item->Draw				();
			}
	}

	UI()->PopScissor			();
}