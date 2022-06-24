// drawdoc.cpp : implementation of the CDrawDoc class
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
#include "summpage.h"
#include "statpage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc

IMPLEMENT_DYNCREATE(CDrawDoc, COleDocument)
//IMPLEMENT_DYNCREATE(): CObject �Ļ� Ŭ������ ��ü�� DECLARE_DYNCREATE ��ũ�ο� �Բ� ����� �� ��Ÿ�ӿ� �������� ���� �� �ֽ��ϴ�.

BEGIN_MESSAGE_MAP(CDrawDoc, COleDocument)
	//{{AFX_MSG_MAP(CDrawDoc)
	ON_COMMAND(ID_VIEW_PAPERCOLOR, OnViewPaperColor)
	//ON_COMMAND(ID_FILE_SUMMARYINFO, OnFileSummaryInfo)

	//}}AFX_MSG_MAP
	// Enable default OLE container implementation// �⺻ OLE �����̳� ���� ���
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleDocument::OnUpdateObjectVerbMenu)
		// MAPI support
	//ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)

	//ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc construction/destruction


//Ŭ���̾�Ʈ �� 
CDrawDoc::CDrawDoc()
{
	EnableCompoundFile();
	//EnableCompoundFile(): OLE ������ Storage ���� ������ ����Ͽ� ������ �����մϴ�.

	m_nMapMode = MM_ANISOTROPIC;
	m_paperColor = RGB(255, 255, 255);
	m_pSummInfo = NULL;
	ComputePageSize();
	//ComputePageSize(): ����� ���� ũ���� ĵ������ �߾ӿ� ��ġ
}

CDrawDoc::~CDrawDoc()
{
	POSITION pos = m_objects.GetHeadPosition();
	//GetHeadPosition(): �� ����� ��� ��� ��ġ�� �����ɴϴ�.


	while (pos != NULL)	//1. pos�� null �� �ƴϸ� while �Ͽ�
		delete m_objects.GetNext(pos);  //2. ���� pos �����
	//GetNext(): �ĺ��Ǵ� ��� ��Ҹ� �������� ��Ͽ��� ���� �׸��� POSITION ������ pos�� �����մϴ�.
	delete m_pSummInfo;   // 3. m_pSummInfo �� �����
}

//BOOL CDrawDoc::OnNewDocument()
//{
//	if (!COleDocument::OnNewDocument())
//		//OnNewDocument():�� ���� ����� �Ϻη� �����ӿ�ũ���� ȣ��˴ϴ�.
//		return FALSE;
//
//	// reinitialization code
//	// (SDI documents will reuse this document)
//	if(m_pSummInfo != NULL)
//		delete m_pSummInfo;
//	m_pSummInfo = new CSummInfo;
//	// Title, Subject, Author, Keywords default to empty string
//	// Comments, Template, SavedBy default to empty string
//	// LastSave, LastPrint, EditTime, RevNum default to 0
//	m_pSummInfo->StartEditTimeCount();
//	m_pSummInfo->RecordCreateDate();
//	m_pSummInfo->SetNumPages(1);
//	// NumWords, NumChars default to 0
//	m_pSummInfo->SetAppname( _T("DrawCli") );
//	// Security defaults to 0
//	
//	//SetAppname():
//	//Watson �� �ٸ� ��ġ���� ó�� �� ���� ���� �� �� �̸����� ��Ŷ �Ű� ������ ���� �մϴ�.
//	//  �̴� ��κ��� ��� �ڵ����� ���� �˴ϴ�. �� ���� ���� �м��� ������ �߻� �� ���� �ƴ� �ſ� Ư�� �� ��쿡�� ���� �� �� �̸��� ���� �ؾ� �մϴ�.
//	return TRUE;
//}

//BOOL CDrawDoc::OnOpenDocument(LPCTSTR lpszPathName)
////OnOpenDocument():���� ���� ����� �Ϻη� �����ӿ�ũ���� ȣ��˴ϴ�.
//{
//	if( m_pSummInfo != NULL)
//		delete m_pSummInfo;
//	m_pSummInfo = new CSummInfo;
//	m_pSummInfo->StartEditTimeCount();
//	return COleDocument::OnOpenDocument(lpszPathName);
//}

//BOOL CDrawDoc::OnSaveDocument(LPCTSTR lpszPathName)
////OnSaveDocument: ���� ���� �Ǵ� ���� �ٸ� �̸����� ���� ����� �Ϻη� �����ӿ�ũ���� ȣ��˴ϴ�.
//{
//	m_pSummInfo->RecordSaveDate();
//	m_pSummInfo->IncrRevNum();
//	m_pSummInfo->SetLastAuthor(m_pSummInfo->GetAuthor());
//	m_pSummInfo->AddCountToEditTime();
//	m_pSummInfo->StartEditTimeCount();
//	return COleDocument::OnSaveDocument(lpszPathName);
//}

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc serialization

//void CDrawDoc::Serialize(CArchive& ar) // ���� ���� �� ����°� ����?
//{
	//if (ar.IsStoring())//IsStoring(): ����� IsStoring ����Ͽ� ��ü�� �����͸� ����
	//{
	//	ar << m_paperColor;
	//	m_objects.Serialize(ar);
	//	//Serialize(): �� �Լ��� ��Ʈ�� ���������� ����ȭ�ؾ� �ϴ� �ν��Ͻ� ����� ������
	//	m_pSummInfo->WriteToStorage(m_lpRootStg);
	//	//WriteToStorage(): ����ҿ� �� ���Դϴ�.
	//}
	//else
	//{
	//	ar >> m_paperColor;
	//	m_objects.Serialize(ar);
	//	//Serialize(): �� �Լ��� ��Ʈ�� ���������� ����ȭ�ؾ� �ϴ� �ν��Ͻ� ����� ������
	//	
	//	m_pSummInfo->ReadFromStorage(m_lpRootStg);
	//	//ReadFromStorage(): The clause to read from the storage.
	//}
	//// By calling the base class COleDocument, we enable serialization
	////  of the container document's COleClientItem objects automatically.
	//COleDocument::Serialize(ar);

//}


/////////////////////////////////////////////////////////////////////////////
// CDrawDoc implementation

void CDrawDoc::Draw(CDC* pDC, CDrawView* pView)
{
	POSITION pos = m_objects.GetHeadPosition();  
	//GetHeadPosition(): �� ����� ��� ��� ��ġ�� �����ɴϴ�.
	while (pos != NULL)   // 1. pos�� ���� �ƴѵ��� while�� ������
	{
		CDrawObj* pObj = m_objects.GetNext(pos);  // 2.  pos�� ���� ��ġ ��� ��ü�� ���
		//GetNext(): ���� ��ġ�� ��� ��ü�� ���, 
		//���� ��ġ�� �̵� (��, ���� ��ġ�� ��´�.�� ������ �����ͱ��� ���� NULL�� �ȴ�.)
		pObj->Draw(pDC);//Draw(): �׸��� �Լ��� ���� ȣ��  //3.  
		if (pView->m_bActive && !pDC->IsPrinting() && pView->IsSelected(pObj));
			// 4.  
			// IsPrinting(): DC�� ������ DC���� �ƴ����� BOOL������ ��ȯ
			//IsSelected(): ���� �Ӽ��� ���õǾ����� ���θ� ��Ÿ���ϴ�.
			pObj->DrawTracker(pDC, CDrawObj::selected);
	}
}

void CDrawDoc::Add(CDrawObj* pObj)
{
	m_objects.AddTail(pObj); //m_
	//AddTail()�� ����� �޺κп� �� ��� �Ǵ� ��� ����� �߰��մϴ�.
	pObj->m_pDocument = this;
	SetModifiedFlag();
	//SetModifiedFlag(): ������ ������ �� �� �Լ��� ȣ���մϴ�.

	// 
}

void CDrawDoc::Remove(CDrawObj* pObj)
{
	// Find and remove from document
	POSITION pos = m_objects.Find(pObj);  // m_objects�� pObj�� ã�� ��
	if (pos != NULL)					  // pos�� null�� �ƴϸ�
		m_objects.RemoveAt(pos);		  // �ش� pos ����
	// set document modified flag
	SetModifiedFlag();					  // ������ �� �����ֱ�

	// call remove for each view so that the view can remove from m_selection
	pos = GetFirstViewPosition();
	//GetFirstViewPosition(): �� �Լ��� ȣ���Ͽ� ������ ����� �� ��Ͽ��� ù ��° ������ ��ġ�� �����ɴϴ�.
	while (pos != NULL)																	// pos�� ���� �ƴ� ����
		((CDrawView*)GetNextView(pos))->Remove(pObj);									// �ش� pObj�� ����
	//GetNextView(): �� �Լ��� ȣ���Ͽ� ������ ��� ���⸦ �ݺ��մϴ�.
	//Remove(): ������ Ư�� ���ڸ� ��� �����Ѵ�.
}

// point is in logical coordinates  //���� ���� ��ǥ
CDrawObj* CDrawDoc::ObjectAt(const CPoint& point)
{
	CRect rect(point, CSize(1, 1));													// 1. ����,���ΰ� 1�� CSize ��ü ���� ��
	POSITION pos = m_objects.GetTailPosition();										// 	
	//GetTailPosition(): ����� tail ��� ��ġ�� ��ȯ�մϴ�.
	while (pos != NULL)																// pos�� ���� �ƴ� ����
	{
		CDrawObj* pObj = m_objects.GetPrev(pos);									// ���� pos�� ��������
		//GetPrev(): �ݺ��� ���� ��Ҹ� �����ɴϴ�. 
		if (pObj->Intersects(rect))													// ����  1���� �����ϸ� pObj�� ����
			//Intersects(): 2 ���� �簢���� �����ϴ��� üũ�Ͽ� �����ϴ� ��� 
						//: �����ϴ� ������ ���� �� ����
			return pObj;
	}

	return NULL;
}

void CDrawDoc::ComputePageSize()  // ������ �����ϱ�
{
	CSize new_size(850, 1100);  // 8.5" x 11" default

	CPrintDialog dlg(FALSE);
	if (AfxGetApp()->GetPrinterDeviceDefaults(&dlg.m_pd))
		//AfxGetApp(): �Ļ� Ŭ������ �ν��Ͻ� �����͸� ��ȯ�� �ִ� ������ ��.
		//GetPrinterDeviceDefaults(): �μ��� ������ ����̽� ���ؽ�Ʈ�� �غ��Ϸ��� �� ��� �Լ��� ȣ���մϴ�.


	{
		// GetPrinterDC returns a HDC so attach it
		CDC dc;
		HDC hDC= dlg.CreatePrinterDC();
		//CreatePrinterDC(): ���� ���� ������ ����̽� ���ؽ�Ʈ�� ���� �ڵ��Դϴ�.
		ASSERT(hDC != NULL);
		dc.Attach(hDC);
		//Attach(): ������ �ڵ��� �̿��� ������ ��ü�� �����츦 ����

		// Get the size of the page in loenglish
		new_size.cx = MulDiv(dc.GetDeviceCaps(HORZSIZE), 1000, 254);
		//MulDiv(): ȭ���� �����̳� DPI ���� �κ��� ����ϴµ� ���
		//GetDeviceCaps(): ������ Device Context�� ���� ������ ���ϴ� �Լ�
		new_size.cy = MulDiv(dc.GetDeviceCaps(VERTSIZE), 1000, 254);
	}

	// if size changed then iterate over views and reset
	// ũ�Ⱑ ����� ��� �信 �ݺ��Ͽ� �缳��
	if (new_size != m_size)
	{
		m_size = new_size;
		POSITION pos = GetFirstViewPosition();
		//GetFirstViewPosition(): �� ��Ͽ��� ù ��° ��ġ�� ��ġ�� ��ȯ
		while (pos != NULL)
			((CDrawView*)GetNextView(pos))->SetPageSize(m_size);
		//GetNextView(): ������ ����� �� ����� �ݺ�
		//SetPageSize(): �����̴� ��Ʈ���� ������ ũ�⸦ ����
	}
}

void CDrawDoc::OnViewPaperColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() != IDOK)
		//DoModal(): �� ��� �Լ��� ȣ���Ͽ� ��� ��ȭ ���ڸ� ȣ���ϰ� �Ϸ�Ǹ� ��ȭ ���� ����� ��ȯ�մϴ�.
		return;

	m_paperColor = dlg.GetColor();
	//GetColor(): ȣ�� �� �� �Լ��� ȣ�� DoModal �Ͽ� ����ڰ� ������ ���� ���� ������ �˻��մϴ�.
	SetModifiedFlag();
	//SetModifiedFlag(): ������ ���������� ������ ���� ���������� ��Ÿ���� �÷��׸� ����
	UpdateAllViews(NULL);
	//UpdateAllViews(): ������ ������ �� �� �Լ��� ȣ���մϴ�.
}

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc diagnostics

#ifdef _DEBUG
void CDrawDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CDrawDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrawDoc commands

void CDrawDoc::OnFileSummaryInfo()	
{
	ASSERT_VALID(this);

	CPropertySheet sheet( _T("Document Properties") );
	CSummPage summ;
	CStatPage stat;
	sheet.AddPage( &summ );
	//AddPage(): ������ �������� �Ӽ� ��Ʈ�� �� ������ �ǿ� �߰��մϴ�.
	sheet.AddPage( &stat );

	//summinfo.cpp �����ϱ� //summary dialog �� ����// IDD_SUMM_PAGE
	summ.m_strAppname = m_pSummInfo->GetAppname();
	summ.m_strTitle   = m_pSummInfo->GetTitle();
	summ.m_strSubj    = m_pSummInfo->GetSubject();
	summ.m_strAuthor  = m_pSummInfo->GetAuthor();
	summ.m_strKeywd   = m_pSummInfo->GetKeywords();
	summ.m_strCmt     = m_pSummInfo->GetComments();
	summ.m_strTempl   = m_pSummInfo->GetTemplate();

	//statpage.cpp //Statistics dialog  // IDD_STAT_PAGE ���̷α׿� ����
	stat.m_strSavedBy    = m_pSummInfo->GetLastAuthor();
	stat.m_strRevNum     = m_pSummInfo->GetRevNum();
	stat.m_strEditTime   = m_pSummInfo->GetEditTime();
	stat.m_strLastPrint  = m_pSummInfo->GetLastPrintDate();
	stat.m_strCreateDate = m_pSummInfo->GetCreateDate();
	stat.m_strLastSave   = m_pSummInfo->GetLastSaveDate();
	stat.m_strNumPages   = m_pSummInfo->GetNumPages();
	stat.m_strNumWords   = m_pSummInfo->GetNumWords();
	stat.m_strNumChars   = m_pSummInfo->GetNumChars();
	stat.m_strSecurity   = m_pSummInfo->GetSecurity();

	if (sheet.DoModal() != IDOK)
		//DoModal(): �� ��� �Լ��� ȣ���Ͽ� ��� ��ȭ ���ڸ� ȣ���ϰ� �Ϸ�Ǹ� ��ȭ ���� ����� ��ȯ�մϴ�.
		return;

	//summinfo.cpp �����ϱ� //summary dialog �� ����// IDD_SUMM_PAGE
	m_pSummInfo->SetAuthor(summ.m_strAuthor);
	m_pSummInfo->SetKeywords(summ.m_strKeywd);
	m_pSummInfo->SetSubject(summ.m_strSubj);
	m_pSummInfo->SetComments(summ.m_strCmt);
	m_pSummInfo->SetTemplate(summ.m_strTempl);
	m_pSummInfo->SetTitle(summ.m_strTitle);

	SetModifiedFlag();
	//SetModifiedFlag(): ������ ���������� ������ ���� ���������� ��Ÿ���� �÷��׸� ����

}
