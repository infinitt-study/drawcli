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
//IMPLEMENT_DYNCREATE(): CObject 파생 클래스의 개체를 DECLARE_DYNCREATE 매크로와 함께 사용할 때 런타임에 동적으로 만들 수 있습니다.

BEGIN_MESSAGE_MAP(CDrawDoc, COleDocument)
	//{{AFX_MSG_MAP(CDrawDoc)
	ON_COMMAND(ID_VIEW_PAPERCOLOR, OnViewPaperColor)
	//ON_COMMAND(ID_FILE_SUMMARYINFO, OnFileSummaryInfo)

	//}}AFX_MSG_MAP
	// Enable default OLE container implementation// 기본 OLE 컨테이너 구현 사용
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


//클라이언트 뷰 
CDrawDoc::CDrawDoc()
{
	EnableCompoundFile();
	//EnableCompoundFile(): OLE 구조적 Storage 파일 형식을 사용하여 문서를 저장합니다.

	m_nMapMode = MM_ANISOTROPIC;
	m_paperColor = RGB(255, 255, 255);
	m_pSummInfo = NULL;
	ComputePageSize();
	//ComputePageSize(): 사용자 지정 크기의 캔버스를 중앙에 위치
}

CDrawDoc::~CDrawDoc()
{
	POSITION pos = m_objects.GetHeadPosition();
	//GetHeadPosition(): 이 목록의 헤드 요소 위치를 가져옵니다.


	while (pos != NULL)	//1. pos가 null 이 아니면 while 하여
		delete m_objects.GetNext(pos);  //2. 다음 pos 지우기
	//GetNext(): 식별되는 목록 요소를 가져오고 목록에서 다음 항목의 POSITION 값으로 pos을 설정합니다.
	delete m_pSummInfo;   // 3. m_pSummInfo 도 지우기
}

//BOOL CDrawDoc::OnNewDocument()
//{
//	if (!COleDocument::OnNewDocument())
//		//OnNewDocument():새 파일 명령의 일부로 프레임워크에서 호출됩니다.
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
//	//Watson 및 다른 위치에서 처리 된 오류 보고 된 앱 이름으로 버킷 매개 변수를 설정 합니다.
//	//  이는 대부분의 경우 자동으로 설정 됩니다. 앱 보고 원격 분석도 오류가 발생 한 앱이 아닌 매우 특수 한 경우에만 보고 된 앱 이름을 변경 해야 합니다.
//	return TRUE;
//}

//BOOL CDrawDoc::OnOpenDocument(LPCTSTR lpszPathName)
////OnOpenDocument():파일 열기 명령의 일부로 프레임워크에서 호출됩니다.
//{
//	if( m_pSummInfo != NULL)
//		delete m_pSummInfo;
//	m_pSummInfo = new CSummInfo;
//	m_pSummInfo->StartEditTimeCount();
//	return COleDocument::OnOpenDocument(lpszPathName);
//}

//BOOL CDrawDoc::OnSaveDocument(LPCTSTR lpszPathName)
////OnSaveDocument: 파일 저장 또는 파일 다른 이름으로 저장 명령의 일부로 프레임워크에서 호출됩니다.
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

//void CDrawDoc::Serialize(CArchive& ar) // 파일 저장 및 입출력과 관련?
//{
	//if (ar.IsStoring())//IsStoring(): 결과를 IsStoring 사용하여 개체의 데이터를 삽입
	//{
	//	ar << m_paperColor;
	//	m_objects.Serialize(ar);
	//	//Serialize(): 이 함수는 루트를 성공적으로 직렬화해야 하는 인스턴스 목록을 컴파일
	//	m_pSummInfo->WriteToStorage(m_lpRootStg);
	//	//WriteToStorage(): 저장소에 쓸 절입니다.
	//}
	//else
	//{
	//	ar >> m_paperColor;
	//	m_objects.Serialize(ar);
	//	//Serialize(): 이 함수는 루트를 성공적으로 직렬화해야 하는 인스턴스 목록을 컴파일
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
	//GetHeadPosition(): 이 목록의 헤드 요소 위치를 가져옵니다.
	while (pos != NULL)   // 1. pos가 널이 아닌동안 while문 돌려서
	{
		CDrawObj* pObj = m_objects.GetNext(pos);  // 2.  pos의 현재 위치 노드 객체를 얻고
		//GetNext(): 현재 위치의 노드 객체를 얻고, 
		//다음 위치로 이동 (즉, 다음 위치를 얻는다.맨 마지막 데이터까지 가면 NULL이 된다.)
		pObj->Draw(pDC);//Draw(): 그리기 함수에 대한 호출  //3.  
		if (pView->m_bActive && !pDC->IsPrinting() && pView->IsSelected(pObj));
			// 4.  
			// IsPrinting(): DC가 프린터 DC인지 아닌지를 BOOL형으로 반환
			//IsSelected(): 현재 속성이 선택되었는지 여부를 나타냅니다.
			pObj->DrawTracker(pDC, CDrawObj::selected);
	}
}

void CDrawDoc::Add(CDrawObj* pObj)
{
	m_objects.AddTail(pObj); //m_
	//AddTail()이 목록의 뒷부분에 새 요소 또는 요소 목록을 추가합니다.
	pObj->m_pDocument = this;
	SetModifiedFlag();
	//SetModifiedFlag(): 문서를 수정한 후 이 함수를 호출합니다.

	// 
}

void CDrawDoc::Remove(CDrawObj* pObj)
{
	// Find and remove from document
	POSITION pos = m_objects.Find(pObj);  // m_objects의 pObj를 찾은 후
	if (pos != NULL)					  // pos가 null이 아니면
		m_objects.RemoveAt(pos);		  // 해당 pos 제거
	// set document modified flag
	SetModifiedFlag();					  // 수정된 후 보여주기

	// call remove for each view so that the view can remove from m_selection
	pos = GetFirstViewPosition();
	//GetFirstViewPosition(): 이 함수를 호출하여 문서와 연결된 뷰 목록에서 첫 번째 보기의 위치를 가져옵니다.
	while (pos != NULL)																	// pos가 널이 아닌 동안
		((CDrawView*)GetNextView(pos))->Remove(pObj);									// 해당 pObj를 제거
	//GetNextView(): 이 함수를 호출하여 문서의 모든 보기를 반복합니다.
	//Remove(): 지정한 특정 문자를 모두 제거한다.
}

// point is in logical coordinates  //점의 논리적 좌표
CDrawObj* CDrawDoc::ObjectAt(const CPoint& point)
{
	CRect rect(point, CSize(1, 1));													// 1. 가로,세로가 1인 CSize 객체 생성 후
	POSITION pos = m_objects.GetTailPosition();										// 	
	//GetTailPosition(): 목록의 tail 요소 위치를 반환합니다.
	while (pos != NULL)																// pos가 널이 아닌 동안
	{
		CDrawObj* pObj = m_objects.GetPrev(pos);									// 이전 pos를 가져오기
		//GetPrev(): 반복할 이전 요소를 가져옵니다. 
		if (pObj->Intersects(rect))													// 만약  1번이 교차하면 pObj로 리턴
			//Intersects(): 2 개의 사각형이 교차하는지 체크하여 교차하는 경우 
						//: 교차하는 영역을 얻을 수 있음
			return pObj;
	}

	return NULL;
}

void CDrawDoc::ComputePageSize()  // 교수님 질문하기
{
	CSize new_size(850, 1100);  // 8.5" x 11" default

	CPrintDialog dlg(FALSE);
	if (AfxGetApp()->GetPrinterDeviceDefaults(&dlg.m_pd))
		//AfxGetApp(): 파생 클래스의 인스턴스 포인터를 반환해 주는 역할을 함.
		//GetPrinterDeviceDefaults(): 인쇄할 프린터 디바이스 컨텍스트를 준비하려면 이 멤버 함수를 호출합니다.


	{
		// GetPrinterDC returns a HDC so attach it
		CDC dc;
		HDC hDC= dlg.CreatePrinterDC();
		//CreatePrinterDC(): 새로 만든 프린터 디바이스 컨텍스트에 대한 핸들입니다.
		ASSERT(hDC != NULL);
		dc.Attach(hDC);
		//Attach(): 윈도우 핸들을 이용해 윈도우 객체와 윈도우를 연결

		// Get the size of the page in loenglish
		new_size.cx = MulDiv(dc.GetDeviceCaps(HORZSIZE), 1000, 254);
		//MulDiv(): 화면의 비율이나 DPI 관련 부분을 계산하는데 사용
		//GetDeviceCaps(): 지정된 Device Context의 여러 정보를 구하는 함수
		new_size.cy = MulDiv(dc.GetDeviceCaps(VERTSIZE), 1000, 254);
	}

	// if size changed then iterate over views and reset
	// 크기가 변경된 경우 뷰에 반복하여 재설정
	if (new_size != m_size)
	{
		m_size = new_size;
		POSITION pos = GetFirstViewPosition();
		//GetFirstViewPosition(): 뷰 목록에서 첫 번째 위치의 위치를 반환
		while (pos != NULL)
			((CDrawView*)GetNextView(pos))->SetPageSize(m_size);
		//GetNextView(): 문서와 연결된 뷰 목록을 반복
		//SetPageSize(): 슬라이더 컨트롤의 페이지 크기를 설정
	}
}

void CDrawDoc::OnViewPaperColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() != IDOK)
		//DoModal(): 이 멤버 함수를 호출하여 모달 대화 상자를 호출하고 완료되면 대화 상자 결과를 반환합니다.
		return;

	m_paperColor = dlg.GetColor();
	//GetColor(): 호출 후 이 함수를 호출 DoModal 하여 사용자가 선택한 색에 대한 정보를 검색합니다.
	SetModifiedFlag();
	//SetModifiedFlag(): 문서를 마지막으로 저장한 이후 수정했음을 나타내는 플래그를 설정
	UpdateAllViews(NULL);
	//UpdateAllViews(): 문서가 수정된 후 이 함수를 호출합니다.
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
	//AddPage(): 제공된 페이지를 속성 시트의 맨 오른쪽 탭에 추가합니다.
	sheet.AddPage( &stat );

	//summinfo.cpp 참고하기 //summary dialog 와 관련// IDD_SUMM_PAGE
	summ.m_strAppname = m_pSummInfo->GetAppname();
	summ.m_strTitle   = m_pSummInfo->GetTitle();
	summ.m_strSubj    = m_pSummInfo->GetSubject();
	summ.m_strAuthor  = m_pSummInfo->GetAuthor();
	summ.m_strKeywd   = m_pSummInfo->GetKeywords();
	summ.m_strCmt     = m_pSummInfo->GetComments();
	summ.m_strTempl   = m_pSummInfo->GetTemplate();

	//statpage.cpp //Statistics dialog  // IDD_STAT_PAGE 다이로그와 관련
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
		//DoModal(): 이 멤버 함수를 호출하여 모달 대화 상자를 호출하고 완료되면 대화 상자 결과를 반환합니다.
		return;

	//summinfo.cpp 참고하기 //summary dialog 와 관련// IDD_SUMM_PAGE
	m_pSummInfo->SetAuthor(summ.m_strAuthor);
	m_pSummInfo->SetKeywords(summ.m_strKeywd);
	m_pSummInfo->SetSubject(summ.m_strSubj);
	m_pSummInfo->SetComments(summ.m_strCmt);
	m_pSummInfo->SetTemplate(summ.m_strTempl);
	m_pSummInfo->SetTitle(summ.m_strTitle);

	SetModifiedFlag();
	//SetModifiedFlag(): 문서를 마지막으로 저장한 이후 수정했음을 나타내는 플래그를 설정

}
