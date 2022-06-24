// drawobj.cpp - implementation for drawing objects
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include "drawcli.h"

#include "drawdoc.h"
#include "drawvw.h"
#include "drawobj.h"

#include "cntritem.h"
#include "rectdlg.h"

IMPLEMENT_SERIAL(CDrawObj, CObject, 0)
//IMPLEMENT_SERIAL(): ���� ���� ������ Ŭ���� �̸� �� ��ġ�� ���� ��Ÿ�� �׼��� ������ �ִ� ���� CObject �Ļ� Ŭ������ �ʿ��� C++ �ڵ带 �����մϴ�.

CDrawObj::CDrawObj()
{
}

CDrawObj::~CDrawObj()
{
}

CDrawObj::CDrawObj(const CRect& position)
{
	m_position = position;
	m_pDocument = NULL;

	m_bPen = TRUE;
	m_logpen.lopnStyle = PS_INSIDEFRAME;
	//PS_INSIDEFRAME: �� ���� ��� �簢���� ����ϴ� GDI �׸��� �Լ����� ����ϸ� ���� �ʺ� ����Ͽ� ��� �簢���� ������ �µ��� �׸��� ġ���� ��ҵ˴ϴ�. �̴� �������� �濡�� ����˴ϴ�.
	m_logpen.lopnWidth.x = 1;
	m_logpen.lopnWidth.y = 1;
	m_logpen.lopnColor = RGB(0, 0, 0);

	m_bBrush = TRUE;
	m_logbrush.lbStyle = BS_SOLID;
	m_logbrush.lbColor = RGB(192, 192, 192);
	m_logbrush.lbHatch = HS_HORIZONTAL;
	//HS_HORIZONTAL: ���� ���� ����???
}

void CDrawObj::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
		//IsStoring(): �������� �����͸� �����ϴ��� ���θ� Ȯ��
	{
		ar << m_position;
		ar << (WORD)m_bPen;
		ar.Write(&m_logpen, sizeof(LOGPEN));
		//Write():������ ����Ʈ ���� ���� ���Ͽ� ���ϴ�.
		ar << (WORD)m_bBrush;
		ar.Write(&m_logbrush, sizeof(LOGBRUSH));
		//Write():������ ����Ʈ ���� ���� ���Ͽ� ���ϴ�.

	}
	else
	{
		// get the document back pointer from the archive
		m_pDocument = (CDrawDoc*)ar.m_pDocument;
		ASSERT_VALID(m_pDocument);
		ASSERT_KINDOF(CDrawDoc, m_pDocument);

		WORD wTemp;
		ar >> m_position;
		ar >> wTemp; m_bPen = (BOOL)wTemp;
		ar.Read(&m_logpen,sizeof(LOGPEN));
		//Read(): ���� ���Ͽ��� ������ ����Ʈ ���� �н��ϴ�.
		ar >> wTemp; m_bBrush = (BOOL)wTemp;
		ar.Read(&m_logbrush, sizeof(LOGBRUSH));
		//Read(): ���� ���Ͽ��� ������ ����Ʈ ���� �н��ϴ�.

	}
}

void CDrawObj::Remove()
{
	delete this;
}

void CDrawObj::Draw(CDC*)
{
}


//�׷��� ���� ���� �� �׵α� ����
void CDrawObj::DrawTracker(CDC* pDC, TrackerState state)
{
	ASSERT_VALID(this);

	switch (state)
	{
	case normal:
		break;

	case selected:
	case active:
		{
			int nHandleCount = GetHandleCount();
			//GetHandleCount(): ���μ����� Ȱ�� �ڵ� ���� �����ɴϴ�.
			for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
			{
				CPoint handle = GetHandle(nHandle);
				//GetHandle(): ��ҿ� �� �� HWND�� ��ȯ �մϴ�.
				pDC->PatBlt(handle.x - 3, handle.y - 3, 7, 7, DSTINVERT);
				//PatBlt (): �ش� DC�� ��Ʈ�� ������ �������·� �ʱ�ȭ�ϴµ� ���
				//DSTINVERT: ��� �簢���� ������ŵ�ϴ�.
			}
		break;
		}
	}
}

// position is in logical
void CDrawObj::MoveTo(const CRect& position, CDrawView* pView)
// ��ġ ����!!
{
	ASSERT_VALID(this);
	//ASSERT_VALID(): ��ü�� �޸� ������ ��������� ���� ��ȿ���� �˻��ϴµ� ���Ǵ� ��ũ�� �Լ� 

	// ���� ���� ��ġ�� ���� ��ġ�� ���ٸ� �׳� ����
	if (position == m_position)
		return;
	

	if (pView == NULL)
	{
		Invalidate();
		//Invalidate(): CWnd�� ��ü Ŭ���̾�Ʈ ������ ��ȿȭ //ȭ�� ��ǥ��
		m_position = position;
		Invalidate();
		//Invalidate(): CWnd�� ��ü Ŭ���̾�Ʈ ������ ��ȿȭ //ȭ�� ��ǥ��

	}
	else
	{    // �ذ��
		pView->InvalObj(this);
		m_position = position;
		pView->InvalObj(this);
	}
	m_pDocument->SetModifiedFlag();
	//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.
}

// Note: if bSelected, hit-codes start at one for the top-left
// and increment clockwise, 0 means no hit.
// If !bSelected, 0 = no hit, 1 = hit (anywhere)

// point is in logical coordinates

// ���� ���� �� ������  ���õ� �׵θ� 
int CDrawObj::HitTest(CPoint point, CDrawView* pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	if (bSelected)
	{
		int nHandleCount = GetHandleCount();
		//GetHandleCount(): ���μ����� Ȱ�� �ڵ� ���� �����ɴϴ�.
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// GetHandleRect returns in logical coords
			CRect rc = GetHandleRect(nHandle,pView);
			if (point.x >= rc.left && point.x < rc.right && point.y <= rc.top && point.y > rc.bottom)
				return nHandle;
		}
	}
	else
	{
		if (point.x >= m_position.left && point.x < m_position.right &&
			point.y <= m_position.top && point.y > m_position.bottom)
			return 1;
	}
	return 0;
}

// rect must be in logical coordinates
// // �巡���ؼ� �����ؼ� �� �����ȿ� �̰� �ֳ� ����
BOOL CDrawObj::Intersects(const CRect& rect)
{
	ASSERT_VALID(this);

	CRect fixed = m_position;
	fixed.NormalizeRect();
	//NormalizeRect():CRect���̿� �ʺ� ��� ����� �ǵ��� ����ȭ �մϴ�.
	CRect rectT = rect;
	rectT.NormalizeRect();
	//NormalizeRect():CRect���̿� �ʺ� ��� ����� �ǵ��� ����ȭ �մϴ�.
	return !(rectT & fixed).IsRectEmpty();
	//IsRectEmpty(): ��� �ִ��� ���θ� CRect Ȯ��
}

// ��� ���� Ŭ�� �� �׵θ�  �� 8��
int CDrawObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 8;
}

// returns logical coords of center of handle
// �� 8�� ��ġ�� 
CPoint CDrawObj::GetHandle(int nHandle)
{
	ASSERT_VALID(this);
	int x, y, xCenter, yCenter;

	// this gets the center regardless of left/right and top/bottom ordering
	// ���� 
	xCenter = m_position.left + m_position.Width() / 2;
	yCenter = m_position.top + m_position.Height() / 2;

	// �� 8�� ��ġ��
	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
		x = m_position.left;
		y = m_position.top;
		break;

	case 2:
		x = xCenter;
		y = m_position.top;
		break;

	case 3:
		x = m_position.right;
		y = m_position.top;
		break;

	case 4:
		x = m_position.right;
		y = yCenter;
		break;

	case 5:
		x = m_position.right;
		y = m_position.bottom;
		break;

	case 6:
		x = xCenter;
		y = m_position.bottom;
		break;

	case 7:
		x = m_position.left;
		y = m_position.bottom;
		break;

	case 8:
		x = m_position.left;
		y = yCenter;
		break;
	}

	return CPoint(x, y);
}

// return rectange of handle in logical coords
// ������
// ����
CRect CDrawObj::GetHandleRect(int nHandleID, CDrawView* pView)
{
	ASSERT_VALID(this);
	ENSURE(pView != NULL);
	//ENSURE(): ������ ��Ȯ���� ��ȿ���� �˻��ϴ� �� ����մϴ�.

	CRect rect;
	// get the center of the handle in logical coords
	CPoint point = GetHandle(nHandleID);
	//GetHandle(): ��ҿ� �� �� HWND�� ��ȯ �մϴ�.
	// convert to client/device coords
	pView->DocToClient(point);  
	// return CRect of handle in device coords
	
	//drawtracker ���Ʒ��� ���콺 ���õǰ�
	rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);
	// SetRect(): ũ�⸦ CRect ������ ��ǥ�� ����
	pView->ClientToDoc(rect);

	return rect;
}


// ���� ���� �� 4�� �������� ������ ���콺 Ŀ����
HCURSOR CDrawObj::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);

	LPCTSTR id;
	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
	case 5:
		id = IDC_SIZENWSE; //�ϵ��ʰ� �������� ����Ű�� ���� ������ ȭ��ǥ Ŀ��.
		break;

	case 2:
	case 6:
		id = IDC_SIZENS; //���ʰ� ������ ����Ű�� ���� ������ ȭ��ǥ Ŀ��.
		break;

	case 3:
	case 7:
		id = IDC_SIZENESW; // �ϼ��ʰ� �������� ����Ű�� ���� ������ ȭ��ǥ Ŀ��.
		break;

	case 4:
	case 8:
		id = IDC_SIZEWE; //���ʰ� ������ ����Ű�� ���� ������ ȭ��ǥ Ŀ��.
		break;
	}

	return AfxGetApp()->LoadStandardCursor(id);
	//AfxGetApp():	�� �Լ����� ��ȯ�� �����͸� ����Ͽ� �⺻ �޽��� ����ġ �ڵ� �Ǵ� �� �� â�� ���� ���� ���α׷� ������ �׼����� �� �ֽ��ϴ�.
	//LoadStandardCursor(id): id���� �����ϴ� �̸� ���ǵ� Windows Ŀ�� ���ҽ��� �ε��մϴ�.
}

// point must be in logical

// ���� ���� �� �ű� �� �ٲ����� ��ǥ��
void CDrawObj::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
	ASSERT_VALID(this);

	CRect position = m_position;
	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
		position.left = point.x;
		position.top = point.y;
		break;

	case 2:
		position.top = point.y;
		break;

	case 3:
		position.right = point.x;
		position.top = point.y;
		break;

	case 4:
		position.right = point.x;
		break;

	case 5:
		position.right = point.x;
		position.bottom = point.y;
		break;

	case 6:
		position.bottom = point.y;
		break;

	case 7:
		position.left = point.x;
		position.bottom = point.y;
		break;

	case 8:
		position.left = point.x;
		break;
	}

	MoveTo(position, pView);
	//MoveTo(): ���� �������� Ư���� ��ǥ�� �̵�
}

//  // �信 ���ǵǾ� ������ �� �丸 ����ϰ���
void CDrawObj::Invalidate()
{
	ASSERT_VALID(this);
	m_pDocument->UpdateAllViews(NULL, HINT_UPDATE_DRAWOBJ, this);
	//UpdateAllViews(): ������ ������ �� �� �Լ��� ȣ��
}
// �� ����Ǿ����°�
CDrawObj* CDrawObj::Clone(CDrawDoc* pDoc)
{
	ASSERT_VALID(this);

	CDrawObj* pClone = new CDrawObj(m_position);
	pClone->m_bPen = m_bPen;
	pClone->m_logpen = m_logpen;
	pClone->m_bBrush = m_bBrush;
	pClone->m_logbrush = m_logbrush;
	ASSERT_VALID(pClone);

	if (pDoc != NULL)
		pDoc->Add(pClone);

	return pClone;
}

// ���� ���� �� ���콺 ������ ��ư �׸��� properties ()
void CDrawObj::OnEditProperties()
{
	ASSERT_VALID(this);

	CPropertySheet sheet( _T("Shape Properties") );
	CRectDlg dlg;
	dlg.m_bNoFill = !m_bBrush;
	dlg.m_penSize = m_bPen ? m_logpen.lopnWidth.x : 0;
	sheet.AddPage( &dlg );
	//AddPage(): ������ �������� �Ӽ� ��Ʈ�� �� ������ �ǿ� �߰��մϴ�.

	if (sheet.DoModal() != IDOK)
		//DoModal(): �� ��� �Լ��� ȣ���Ͽ� ��� ��ȭ ���ڸ� ȣ���ϰ� �Ϸ�Ǹ� ��ȭ ���� ����� ��ȯ�մϴ�.

		return;

	m_bBrush = !dlg.m_bNoFill;
	m_bPen = dlg.m_penSize > 0;
	if (m_bPen)
	{
		m_logpen.lopnWidth.x = dlg.m_penSize;
		m_logpen.lopnWidth.y = dlg.m_penSize;
	}

	Invalidate();
	m_pDocument->SetModifiedFlag();
	//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.

}

void CDrawObj::OnOpen(CDrawView* /*pView*/ )
{
	OnEditProperties();
	//�� ��Ʈ���� �������Ͽ� �������� ������ ��Ʈ�ѷ� ��Ŀ���� ������ �� �ֽ�
}

// ������ �� �� �ٲٴ� ��
void CDrawObj::SetLineColor(COLORREF color)
{
	ASSERT_VALID(this);

	m_logpen.lopnColor = color;
	Invalidate();
	m_pDocument->SetModifiedFlag();
	//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.
}

// ���� �ȿ� ä��� �� �ٲٴ� ��
void CDrawObj::SetFillColor(COLORREF color)
{
	ASSERT_VALID(this);

	m_logbrush.lbColor = color;
	Invalidate();
	m_pDocument->SetModifiedFlag();
	//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.

}

#ifdef _DEBUG
void CDrawObj::AssertValid()
{
	ASSERT(m_position.left <= m_position.right);
	ASSERT(m_position.bottom <= m_position.top);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CDrawRect

IMPLEMENT_SERIAL(CDrawRect, CDrawObj, 0)

CDrawRect::CDrawRect()
{
}

// �ձ� �׸� 
CDrawRect::CDrawRect(const CRect& position)
	: CDrawObj(position)
{
	ASSERT_VALID(this);

	m_nShape = rectangle;
	m_roundness.x = 16;
	m_roundness.y = 16; // �� �ձ� �κ��� ����
}

void CDrawRect::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CDrawObj::Serialize(ar);
	if (ar.IsStoring())
		//IsStoring(): �������� �����͸� �����ϴ��� ���θ� Ȯ��

	{
		ar << (WORD) m_nShape;
		ar << m_roundness;
	}
	else
	{
		WORD wTemp;
		ar >> wTemp; m_nShape = (Shape)wTemp;
		ar >> m_roundness;
	}
}


// ���� ���鶧 ��& �ȿ� ���� �ֵ�
void CDrawRect::Draw(CDC* pDC)
{
	ASSERT_VALID(this);

	CBrush brush;
	if (!brush.CreateBrushIndirect(&m_logbrush))
		//CreateBrushIndirect(): ����ü�� ������ ��Ÿ��, �� �� ������ ����Ͽ� �귯�ø�  �ʱ�ȭ�մϴ�.
		return;
	CPen pen;
	if (!pen.CreatePenIndirect(&m_logpen))
		//CreatePenIndirect(): ����Ű�� ����ü�� ������ ��Ÿ��, �ʺ� �� ���� �ִ� ���� �ʱ�ȭ
		return;

	CBrush* pOldBrush;
	CPen* pOldPen;

	if (m_bBrush)
		pOldBrush = pDC->SelectObject(&brush);
	// SelectObject(): ��� ���� GDI �׸��� ��ü�� ����
	else
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	//SelectStockObject(): Windows ������ �̸� ���ǵ� ���� ��, �귯�� �Ǵ� �۲� �� �ϳ��� ����

	if (m_bPen)
		pOldPen = pDC->SelectObject(&pen);
	// SelectObject(): ��� ���� GDI �׸��� ��ü�� ����
	else
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	//SelectStockObject(): Windows ������ �̸� ���ǵ� ���� ��, �귯�� �Ǵ� �۲� �� �ϳ��� ����
	CRect rect = m_position;
	switch (m_nShape)
	{
	case rectangle:
		pDC->Rectangle(rect);
		//Rectangle(): ���� ���� ����Ͽ� �簢���� �׸��� ���� �귯�ø� ����Ͽ� ä��ϴ�.
		break;

	case roundRectangle:
		pDC->RoundRect(rect, m_roundness);
		//RoundRect():	���� ���� ����Ͽ� �𼭸��� �ձ� �簢���� �׸��� ���� �귯�ø� ����Ͽ� ä�� �簢���� �׸��ϴ�.
		break;

	case ellipse:
		pDC->Ellipse(rect);
		//Ellipse(): Ÿ���� �׸��ϴ�.
		break;


		// ���� ���� �� ��ǥ ����
	case line:
		if (rect.top > rect.bottom)
		{
			rect.top -= m_logpen.lopnWidth.y / 2;
			rect.bottom += (m_logpen.lopnWidth.y + 1) / 2;
		}
		else
		{
			rect.top += (m_logpen.lopnWidth.y + 1) / 2;
			rect.bottom -= m_logpen.lopnWidth.y / 2;
		}

		if (rect.left > rect.right)
		{
			rect.left -= m_logpen.lopnWidth.x / 2;
			rect.right += (m_logpen.lopnWidth.x + 1) / 2;
		}
		else
		{
			rect.left += (m_logpen.lopnWidth.x + 1) / 2;
			rect.right -= m_logpen.lopnWidth.x / 2;
		}

		pDC->MoveTo(rect.TopLeft());
		//TopLeft(): �簢���� ���� �� �������� ��ǥ�� ����
		pDC->LineTo(rect.BottomRight());
		// BottomRight(): �簢���� ������ �Ʒ� �������� ��ǥ�� ����
		break;
	}

	pDC->SelectObject(pOldBrush); //SelectObject(): ��� ���� GDI �׸��� ��ü�� �����մϴ�
	pDC->SelectObject(pOldPen);//SelectObject(): ��� ���� GDI �׸��� ��ü�� �����մϴ�
}

// ��
int CDrawRect::GetHandleCount()
{
	ASSERT_VALID(this);
	// line�� 2��
	return m_nShape == line ? 2 :
		CDrawObj::GetHandleCount() + (m_nShape == roundRectangle);
	//GetHandleCount(): ���μ����� Ȱ�� �ڵ� ���� �����ɴϴ�.
}

// returns center of handle in logical coordinates
CPoint CDrawRect::GetHandle(int nHandle)
{
	ASSERT_VALID(this);

	if (m_nShape == line && nHandle == 2)
		nHandle = 5;
	else if (m_nShape == roundRectangle && nHandle == 9)
	{
		CRect rect = m_position;
		rect.NormalizeRect();
		//NormalizeRect(): CRect���̿� �ʺ� ��� ����� �ǵ��� ����ȭ 
		CPoint point = rect.BottomRight();
		//BottomRight(): �簢���� ������ �Ʒ� �������� ��ǥ�� ���� �մϴ�.
		point.x -= m_roundness.x / 2;
		point.y -= m_roundness.y / 2;
		return point;
	}

	return CDrawObj::GetHandle(nHandle);
}

HCURSOR CDrawRect::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);

	if (m_nShape == line && nHandle == 2)
		nHandle = 5;
	else if (m_nShape == roundRectangle && nHandle == 9)
		return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
	//AfxGetApp():	�� �Լ����� ��ȯ�� �����͸� ����Ͽ� �⺻ �޽��� ����ġ �ڵ� �Ǵ� �� �� â�� ���� ���� ���α׷� ������ �׼����� �� �ֽ��ϴ�.
	//LoadStandardCursor(): ������ ��� Ŀ���� ���� �ڵ��Դϴ�. �׷��� ������ NULL 
	//IDC_SIZEALL():  �� ���� ������ ȭ��ǥ�Դϴ�. â ũ�⸦ �����ϴ� �� ����� Ŀ���Դϴ�.

	return CDrawObj::GetHandleCursor(nHandle);
}

// point is in logical coordinates

//�׸� ���� ������ ��ǥ �ٲ�� �� 
void CDrawRect::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
	ASSERT_VALID(this);
	//ASSERT_VALID(): ��ü�� ���� ������ ��ȿ���� ���� ������ �׽�Ʈ�ϴ� �� ���
	if (m_nShape == line && nHandle == 2)
		nHandle = 5;
	else if (m_nShape == roundRectangle && nHandle == 9)
	{
		CRect rect = m_position;
		rect.NormalizeRect();
		//NormalizeRect(): ���̿� �ʺ� ��� ����� �ǵ��� ����ȭ
		if (point.x > rect.right - 1)
			point.x = rect.right - 1;
		else if (point.x < rect.left + rect.Width() / 2)
			point.x = rect.left + rect.Width() / 2;
		if (point.y > rect.bottom - 1)
			point.y = rect.bottom - 1;
		else if (point.y < rect.top + rect.Height() / 2)
			point.y = rect.top + rect.Height() / 2;
		m_roundness.x = 2 * (rect.right - point.x);
		m_roundness.y = 2 * (rect.bottom - point.y);
		m_pDocument->SetModifiedFlag();
		//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.
		if (pView == NULL)
			Invalidate();
		else
			pView->InvalObj(this);
		return;
	}

	CDrawObj::MoveHandleTo(nHandle, point, pView);
}

// rect must be in logical coordinates
// rect ���� ��ǥ�� �־�� �մϴ�.
BOOL CDrawRect::Intersects(const CRect& rect) // �簢�� ����鼭 �ϴ� ���
//ù ��° ������ �� ��° ������ ���� �ϴ��� ���θ� Ȯ�� �մϴ�. ��ģ �� ������ ������ ����!!
{
	ASSERT_VALID(this);

	CRect rectT = rect;
	rectT.NormalizeRect();

	CRect fixed = m_position;
	fixed.NormalizeRect();
	if ((rectT & fixed).IsRectEmpty())
		//IsRectEmpty(): ��� �ִ��� ���θ� Ȯ��
		return FALSE;

	CRgn rgn;
	switch (m_nShape)
	{
	case rectangle:
		return TRUE;

	case roundRectangle:
		rgn.CreateRoundRectRgn(fixed.left, fixed.top, fixed.right, fixed.bottom,
			//CreateRoundRectRgn(): ��ü�� ����� �ձ� �𼭸��� �ִ� �簢�� ������ ����ϴ�
			m_roundness.x, m_roundness.y);
		break;

	case ellipse:
		rgn.CreateEllipticRgnIndirect(fixed);
		//CreateEllipticRgnIndirect(): Ÿ���� ������ ����
		break;

	case line:  // ������ ����� ��ó�� �簢���� ����°Ŷ� �¿� �ٲ�� �޿� ���� �� �ٲ�� ����
		{
			int x = (m_logpen.lopnWidth.x + 5) / 2;
			int y = (m_logpen.lopnWidth.y + 5) / 2;
			POINT points[4];
			points[0].x = fixed.left;
			points[0].y = fixed.top;
			points[1].x = fixed.left;
			points[1].y = fixed.top;
			points[2].x = fixed.right;
			points[2].y = fixed.bottom;
			points[3].x = fixed.right;
			points[3].y = fixed.bottom;

			if (fixed.left < fixed.right)
			{
				points[0].x -= x;   // ��
				points[1].x += x;	// ��
				points[2].x += x;   // ��
				points[3].x -= x;	// ��
			}
			else
			{
				points[0].x += x;	// ��
				points[1].x -= x;	// ��
				points[2].x -= x;	//��
				points[3].x += x;	//��
			}

			if (fixed.top < fixed.bottom)
			{
				points[0].y -= y;	//��
				points[1].y += y;	//��
				points[2].y += y;	//�Ʒ�
				points[3].y -= y;	//�Ʒ�
			}
			else
			{
				points[0].y += y;	//��
				points[1].y -= y;	//��
				points[2].y -= y;	//�Ʒ�
				points[3].y += y;	//�Ʒ�
			}
			rgn.CreatePolygonRgn(points, 4, ALTERNATE);
			//ALTERNATE: ��ü �ɼ��� ��Ÿ���ϴ�.
			//CreatePolygonRgn(): 	�ٰ��� ������ ����Ͽ� CRgn ��ü�� �ʱ�ȭ�մϴ�. 
			//�ý����� �ʿ��� ��� ������ ���������� ù ��° ���������� ���� �׷� �ٰ����� �ڵ����� �ݽ��ϴ�.
		}
		break;
	}
	return rgn.RectInRegion(fixed);
}	//RectInRegion(): fixed�� ������ �簢���� �Ϻΰ� ��ü�� ����� rgn ������ ��� ���� �ִ��� ���θ� Ȯ���մϴ�


CDrawObj* CDrawRect::Clone(CDrawDoc* pDoc)
{
	ASSERT_VALID(this);

	CDrawRect* pClone = new CDrawRect(m_position);
	pClone->m_bPen = m_bPen;
	pClone->m_logpen = m_logpen;
	pClone->m_bBrush = m_bBrush;
	pClone->m_logbrush = m_logbrush;
	pClone->m_nShape = m_nShape;
	pClone->m_roundness = m_roundness;
	ASSERT_VALID(pClone);

	if (pDoc != NULL)
		pDoc->Add(pClone);

	ASSERT_VALID(pClone);
	return pClone;
}

////////////////////////////////////////////////////////////////////////////
// CDrawPoly

IMPLEMENT_SERIAL(CDrawPoly, CDrawObj, 0)
//IMPLEMENT_SERIAL(): ���� ���� ������ Ŭ���� �̸� �� ��ġ�� ���� ��Ÿ�� �׼��� ������ �ִ� ���� CObject �Ļ� Ŭ������ �ʿ��� C++ �ڵ带 ����

CDrawPoly::CDrawPoly()
{
	m_points = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;
}

CDrawPoly::CDrawPoly(const CRect& position)
	: CDrawObj(position)
{
	m_points = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;
	m_bPen = TRUE;
	m_bBrush = FALSE;
}

CDrawPoly::~CDrawPoly()
{
	if (m_points != NULL)
		delete[] m_points;
}

void CDrawPoly::Serialize( CArchive& ar )
{
	int i;
	CDrawObj::Serialize( ar );
	if( ar.IsStoring() )
		//IsStoring(): �������� �����͸� �����ϴ��� ���θ� Ȯ��

	{
		ar << (WORD) m_nPoints;
		ar << (WORD) m_nAllocPoints;
		for (i = 0;i< m_nPoints; i++)
			ar << m_points[i];
	}
	else
	{
		WORD wTemp;
		ar >> wTemp; m_nPoints = wTemp;
		ar >> wTemp; m_nAllocPoints = wTemp;
		m_points = new CPoint[m_nAllocPoints];
		for (i = 0;i < m_nPoints; i++)
			ar >> m_points[i];
	}
}


// ���� ���� �� ������ �� ����
void CDrawPoly::Draw(CDC* pDC)
{
	ASSERT_VALID(this);

	CBrush brush;
	if (!brush.CreateBrushIndirect(&m_logbrush))
		//CreateBrushIndirect(): ����ü�� ������ ��Ÿ��, �� �� ������ ����Ͽ� �귯�ø� LOGBRUSH �ʱ�ȭ�մϴ�.
		return;
	CPen pen;
	if (!pen.CreatePenIndirect(&m_logpen))
		//CreatePenIndirect(): ����Ű�� ����ü�� ������ ��Ÿ��, �ʺ� �� ���� �ִ� ���� �ʱ�ȭ
		return;

	CBrush* pOldBrush;
	CPen* pOldPen;

	if (m_bBrush)
		pOldBrush = pDC->SelectObject(&brush);
	//SelectObject(): ����̽� ���ؽ�Ʈ�� ��ü�� ����
	else
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

	if (m_bPen)
		pOldPen = pDC->SelectObject(&pen);
	//SelectObject(): ����̽� ���ؽ�Ʈ�� ��ü�� ����
	else
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	//SelectStockObject(): �̸� ���ǵ� ���� ��, �귯�� �Ǵ� �۲� �� �ϳ��� �ش��ϴ� ��ü�� ����

	pDC->Polygon(m_points, m_nPoints);
	//Polygon(): ���� ���� ����Ͽ� ������ ����� �� �̻��� ��(������)���� ������ �ٰ����� �׸��ϴ�.

	pDC->SelectObject(pOldBrush);
	//SelectObject(): ����̽� ���ؽ�Ʈ�� ��ü�� ����
	pDC->SelectObject(pOldPen);
	//SelectObject(): ����̽� ���ؽ�Ʈ�� ��ü�� ����

}

// position must be in logical coordinates
// �ϼ��� ���� �����̰� �ϴ� ��
void CDrawPoly::MoveTo(const CRect& position, CDrawView* pView)
{
	ASSERT_VALID(this);
	if (position == m_position)
		return;

	if (pView == NULL)
		Invalidate();
	else
		pView->InvalObj(this);

	for (int i = 0; i < m_nPoints; i += 1)
	{
		m_points[i].x += position.left - m_position.left;
		m_points[i].y += position.top - m_position.top;
	}

	m_position = position;

	if (pView == NULL)
		Invalidate();
	else
		pView->InvalObj(this);
	m_pDocument->SetModifiedFlag();
	//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.
}

int CDrawPoly::GetHandleCount()
{
	return m_nPoints;
}

CPoint CDrawPoly::GetHandle(int nHandle)
{
	ASSERT_VALID(this);

	ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
	return m_points[nHandle - 1];
}

HCURSOR CDrawPoly::GetHandleCursor(int )
{
	return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

// point is in logical coordinates
// poly ���� ���� ���� ��ǥ
void CDrawPoly::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
	ASSERT_VALID(this);
	ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
	//ASSERT: �μ��� ��
	if (m_points[nHandle - 1] == point)
		return;

	m_points[nHandle - 1] = point;
	RecalcBounds(pView);

	if (pView == NULL)
		Invalidate();
	else
		pView->InvalObj(this);
	m_pDocument->SetModifiedFlag();
	//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.

}

// rect must be in logical coordinates
BOOL CDrawPoly::Intersects(const CRect& rect)
{
	ASSERT_VALID(this);
	CRgn rgn;
	rgn.CreatePolygonRgn(m_points, m_nPoints, ALTERNATE);
	return rgn.RectInRegion(rect);
	//RectInRegion(): rect���� ������ �簢���� �Ϻΰ� ��ü�� ����� ������ ��� ���� �ִ��� ���� Ȯ��
}

CDrawObj* CDrawPoly::Clone(CDrawDoc* pDoc)
{
	ASSERT_VALID(this);

#pragma warning(suppress:6211)
	CDrawPoly* pClone = new CDrawPoly(m_position);
	pClone->m_bPen = m_bPen;
	pClone->m_logpen = m_logpen;
	pClone->m_bBrush = m_bBrush;
	pClone->m_logbrush = m_logbrush;
	pClone->m_points = new CPoint[m_nAllocPoints];
	
	memcpy_s(pClone->m_points, sizeof(CPoint) * m_nAllocPoints, m_points, sizeof(CPoint) * m_nPoints);
	//memcpy_s(): ���� ���� ����Ʈ�� ����
	pClone->m_nAllocPoints = m_nAllocPoints;
	pClone->m_nPoints = m_nPoints;
	ASSERT_VALID(pClone);

	if (pDoc != NULL)
		pDoc->Add(pClone);

	ASSERT_VALID(pClone);
	return pClone;
}

// point is in logical coordinates
// ���� ��ǥ �߰��Ҷ� ����ϴ� ����
void CDrawPoly::AddPoint(const CPoint& point, CDrawView* pView)
{
	ASSERT_VALID(this);
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		if (m_points != NULL)
		{
			memcpy_s(newPoints, sizeof(CPoint) * (m_nAllocPoints + 10), m_points, sizeof(CPoint) * m_nAllocPoints);
			//memcpy_s(): ���� ���� ����Ʈ�� ����
			delete[] m_points;
		}
		m_points = newPoints;
		m_nAllocPoints += 10;
	}

	if (m_nPoints == 0 || m_points[m_nPoints - 1] != point)
	{
		m_points[m_nPoints++] = point;
		if (!RecalcBounds(pView))
		{
			if (pView == NULL)
				Invalidate();
			else
				pView->InvalObj(this);
		}
		m_pDocument->SetModifiedFlag();
		//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.

	}
}


// poly �� ��� �� �ٷ� ������ �ϴ� �� (������ ���� ȭ���� �ٲ�� �������� ����)
BOOL CDrawPoly::RecalcBounds(CDrawView* pView)
{
	ASSERT_VALID(this);

	if (m_nPoints == 0)
		return FALSE;

	CRect bounds(m_points[0], CSize(0, 0));
	for (int i = 1; i < m_nPoints; ++i)
	{
		if (m_points[i].x < bounds.left)
			bounds.left = m_points[i].x;
		if (m_points[i].x > bounds.right)
			bounds.right = m_points[i].x;
		if (m_points[i].y < bounds.top)
			bounds.top = m_points[i].y;
		if (m_points[i].y > bounds.bottom)
			bounds.bottom = m_points[i].y;
	}

	if (bounds == m_position)
		return FALSE;

	if (pView == NULL)
		Invalidate();
	else
		pView->InvalObj(this);

	m_position = bounds;

	if (pView == NULL)
		Invalidate();
	else
		pView->InvalObj(this);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CDrawOleObj, CDrawObj, 0)

BOOL CDrawOleObj::c_bShowItems = TRUE;

CDrawOleObj::CDrawOleObj() : m_extent(0,0)
{
	m_pClientItem = NULL;
}

CDrawOleObj::CDrawOleObj(const CRect& position)
	: CDrawObj(position), m_extent(0, 0)
{
	m_pClientItem = NULL;
}

CDrawOleObj::~CDrawOleObj()
{
	if (m_pClientItem != NULL)
	{
		m_pClientItem->Release();
		//Release(): �� �Լ��� ȣ���Ͽ� OLE �׸񿡼� ����ϴ� ���ҽ��� ����
		m_pClientItem = NULL;
	}
}

// ole ���� ���� �ΰ� ������ ���߿� ���� ã�ƺ�����
void CDrawOleObj::Remove()
{
	if (m_pClientItem != NULL)
	{
		m_pClientItem->Delete();
		//Delete(): �����̳� �������� OLE �׸��� �����Ϸ��� �� �Լ��� ȣ��
		m_pClientItem = NULL;
	}
	CDrawObj::Remove();
}

void CDrawOleObj::Serialize( CArchive& ar )
{
	ASSERT_VALID(this);

	CDrawObj::Serialize(ar);

	if (ar.IsStoring())
		//IsStoring(): �������� �����͸� �����ϴ��� ���θ� Ȯ��
	{
		ar << m_extent;
		ar << m_pClientItem;
	}
	else
	{
		ar >> m_extent;
		ar >> m_pClientItem;
		m_pClientItem->m_pDrawObj = this;
	}
}

CDrawObj* CDrawOleObj::Clone(CDrawDoc* pDoc)
{
	ASSERT_VALID(this);

	AfxGetApp()->BeginWaitCursor();
	//BeginWaitCursor(): ����� ������ �� ���� ��� �ð� ������ �ҿ�Ǵ� ��� Ŀ���� �� �ð�� ǥ���Ϸ��� �� �Լ��� ȣ���մϴ�.

	CDrawOleObj* pClone = NULL;
	CDrawItem* pItem = NULL;
	TRY
	{
		// perform a "deep copy" -- need to copy CDrawOleObj and the CDrawItem
		//  that it points to.
		pClone = new CDrawOleObj(m_position);
#pragma warning(suppress:6014)
		pItem = new CDrawItem(m_pDocument, pClone);
		if (!pItem->CreateCloneFrom(m_pClientItem))
			//CreateCloneFrom(): �� �Լ��� ȣ���Ͽ� ������ OLE �׸��� ���纻�� ����ϴ�
		{
			AfxThrowMemoryException();
			//AfxThrowMemoryException(): �޸� ���ܸ� throw�մϴ�.
		}

		pClone->m_pClientItem = pItem;
		pClone->m_bPen = m_bPen;
		pClone->m_logpen = m_logpen;
		pClone->m_bBrush = m_bBrush;
		pClone->m_logbrush = m_logbrush;
		ASSERT_VALID(pClone);

		if (pDoc != NULL)
			pDoc->Add(pClone);
	}
	CATCH_ALL(e)
	{
		if(pItem)
			pItem->Delete();

		if(pClone)
		{
			pClone->m_pClientItem = NULL;
			pClone->Remove();
		}
		AfxGetApp()->EndWaitCursor();
		//EndWaitCursor(): �� �ð� Ŀ������ ���� Ŀ���� ��ȯ�ϵ��� ��� �Լ��� ȣ���� �� �� �Լ��� ȣ�� BeginWaitCursor �մϴ�. 

		THROW_LAST();
		//THROW_LAST(): ���ܸ� ���� �ܺ� CATCH ������� �ٽ� throw�մϴ�.
	}
	END_CATCH_ALL//������ CATCH_ALL �Ǵ� ����� ��AND_CATCH_ALL ǥ���մϴ�.

	AfxGetApp()->EndWaitCursor();
	//EndWaitCursor(): �� �ð� Ŀ������ ���� Ŀ���� ��ȯ�ϵ��� ��� �Լ��� ȣ���� �� �� �Լ��� ȣ�� BeginWaitCursor �մϴ�. 
	return pClone;
}

// ole ���� �� ��
void CDrawOleObj::Draw(CDC* pDC)
{
	ASSERT_VALID(this);

	CDrawItem* pItem = m_pClientItem;
	if (pItem != NULL)
	{
		// draw the OLE item itself
		pItem->Draw(pDC, m_position);

		// don't draw tracker in print preview or on printer
		if (!pDC->IsPrinting())
			//IsPrinting():����̽� ���ؽ�Ʈ�� �μ⿡ ���Ǵ��� ���θ� Ȯ��
		{
			// use a CRectTracker to draw the standard effects
			CRectTracker tracker;
			tracker.m_rect = m_position;
			pDC->LPtoDP(tracker.m_rect);
			//LPtoDP():�� ������ ����̽� ������ ��ȯ

			if (c_bShowItems)
			{
				// put correct border depending on item type
				if (pItem->GetType() == OT_LINK)
					//GetType(): �� �Լ��� ȣ���Ͽ� OLE �׸��� ���� �Ǵ� ����Ǿ����� �Ǵ� ���� �׸����� Ȯ���մϴ�.
					tracker.m_nStyle |= CRectTracker::dottedLine;
				else
					tracker.m_nStyle |= CRectTracker::solidLine;
			}

			// put hatching over the item if it is currently open
			if (pItem->GetItemState() == COleClientItem::openState ||
				pItem->GetItemState() == COleClientItem::activeUIState)
				//GetItemState():  �Լ��� ȣ���Ͽ� OLE �׸��� ���� ���¸� �����ɴϴ�.
			{
				tracker.m_nStyle |= CRectTracker::hatchInside;
			}
			tracker.Draw(pDC);
		}
	}
}

// ole ������ �� �ٲ�� ��
void CDrawOleObj::OnOpen(CDrawView* pView)
{
	AfxGetApp()->BeginWaitCursor();
	//BeginWaitCursor(): ����� ������ �� ���� ��� �ð� ������ �ҿ�Ǵ� ��� Ŀ���� �� �ð�� ǥ���Ϸ��� �� �Լ��� ȣ���մϴ�.
	//DoVerb(): ������ ���縦 �����ϱ� ���� ȣ��
	//OLEIVERB_OPEN: ������ â���� �׸� ����
	//OLEIVERB_PRIMARY: �⺻ ����
	//VK_CONTROL: control Ű
	//GetKeyState():������ ���� Ű�� ���¸� �˻��մϴ�. ���´� Ű�� ��, �Ʒ� �Ǵ� ���(����, ����, Ű�� ���� ������ ������ ����)���� ���θ� ����
	m_pClientItem->DoVerb(
		GetKeyState(VK_CONTROL) < 0 ? OLEIVERB_OPEN : OLEIVERB_PRIMARY,
		pView);
	AfxGetApp()->EndWaitCursor();
	//EndWaitCursor(): �� �ð� Ŀ������ ���� Ŀ���� ��ȯ�ϵ��� ��� �Լ��� ȣ���� �� �� �Լ��� ȣ�� BeginWaitCursor �մϴ�.
}

// ole ���� (���콺 ������ ��ư)
void CDrawOleObj::OnEditProperties()
{
	// using COlePropertiesDialog directly means no scaling
	COlePropertiesDialog dlg(m_pClientItem, 100, 100, NULL);

	dlg.DoModal();
	//DoModal(): �� ��� �Լ��� ȣ���Ͽ� ��� ��ȭ ���ڸ� ȣ���ϰ� �Ϸ�Ǹ� ��ȭ ���� ����� ��ȯ�մϴ�.
}

// position is in logical  //ole �����̶� ���ص� ��	// ole �����̰� �ϴ� ��
void CDrawOleObj::MoveTo(const CRect& position, CDrawView* pView)
{
	ASSERT_VALID(this);

	if (position == m_position)
		return;

	//// call base class to update position
	CDrawObj::MoveTo(position, pView);

	//// update position of in-place editing session on position change
	if (m_pClientItem->IsInPlaceActive())
	//	//IsInPlaceActive(): �� �Լ��� ȣ���Ͽ� OLE �׸��� ���� ��ġ Ȱ�� �������� Ȯ����
		m_pClientItem->SetItemRects();
	//SetItemRects():�� �Լ��� ȣ���Ͽ� ��� �簢�� �Ǵ� OLE �׸��� ǥ�õǴ� �簢���� �����մϴ�.
}

/////////////////////////////////////////////////////////////////////////////
