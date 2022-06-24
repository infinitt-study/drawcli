// drawcli.cpp : Defines the class behaviors for the application.

#include "stdafx.h"
#include "drawcli.h"

#include "mainfrm.h"
#include "drawobj.h"
#include "drawdoc.h"
#include "drawvw.h"
#include "splitfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawApp

BEGIN_MESSAGE_MAP(CDrawApp, CWinApp)
	//{{AFX_MSG_MAP(CDrawApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//OnAppAbout: ���̷α� ���࿡ ���� �� �ڸ�Ʈ 
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//CWinApp::OnFileNew(): ID_FILE_NEW ����� �����մϴ�.
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	//ID_FILE_OPEN ����� �����մϴ�.
	//CWinApp::OnFileOpen():  Standard print setup command
	//ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	// CWinApp::OnFilePrintSetup(): ID_FILE_PRINT_SETUP ����� �����մϴ�.

END_MESSAGE_MAP() //�޽��� ���� ���Ǹ� �����մϴ�.

/////////////////////////////////////////////////////////////////////////////
// CDrawApp construction

CDrawApp::CDrawApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	//InitInstance�� ��� �߿��� �ʱ�ȭ ��ġ
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDrawApp object

CDrawApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDrawApp initialization

BOOL CDrawApp::InitInstance()
{
		SetRegistryKey(_T("YourCompany"));   // 1. setRegistrykey �� �ϰ�
		//SetRegistryKey(): ���ø����̼� ������ INI ���� ��� ������Ʈ���� ����ǵ��� �մϴ�.
	// Initialize OLE libraries
	if (!AfxOleInit())//OLE ���̺귯���� �ʱ�ȭ  // 2. ���� !AfxOleInit() �ƴ� ��
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED); // 3. �˸�â ����
		//AfxMessageBox(): �˸�â ����
		//IDP_OLE_INIT_FAILED: OLE �ʱ�ȭ�� �����߽��ϴ�. OLE ���̺귯���� �ùٸ� �������� Ȯ���մϴ�.
		return FALSE;  // 4. ���� ����
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	//LoadStdProfileSettings():
	//InitInstance ��� �Լ� ������ �� ��� 
	//�Լ��� ȣ���Ͽ� ���� �ֱٿ� �����(MRU) ���� ��
	//������ �̸� ���� ���� ����� ����ϵ��� �����ϰ� �ε��մϴ�

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_DRAWCLTYPE,
		RUNTIME_CLASS(CDrawDoc),
		RUNTIME_CLASS(CSplitFrame),
		RUNTIME_CLASS(CDrawView));
	pDocTemplate->SetContainerInfo(IDR_DRAWCLTYPE_CNTR_IP);
	//SetContainerInfo(): ���� ��ġ OLE �׸��� ������ �� OLE �����̳ʿ� ���� ���ҽ��� �����մϴ�.
	AddDocTemplate(pDocTemplate);
	//AddDocTemplate(): �� ��� �Լ��� ȣ���Ͽ� ���� ���α׷����� ���� �����ϴ� ��� ������ ���� ���� ���� ��Ͽ� ���� ���� ������ �߰��մϴ�.

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;  

	if(!pMainFrame)  // 1. pMainFrame �� �ƴϸ� ���� ����
		return FALSE;

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) //1. pMainFrame�� LoadFrame(IDR_MAINFRAME) �� �����ϸ�
	{
		delete pMainFrame;  //2. pMainFrame �����
		return FALSE;   // 3. ���� ����
	}
	m_pMainWnd = pMainFrame; 

	// Enable DDE Execute open
	EnableShellOpen();
	//Ŭ������ ������ �� �� �ְ� �ϴ� �Լ�.
	RegisterShellFileTypes(TRUE);
	//��ť��Ʈ ���ø��� ã�Ƴ� ��ť��Ʈ Ÿ���� ������Ʈ���� ����ϴ� �Լ�

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo); 
	//�� ��� �Լ��� ȣ���Ͽ� ������� ���� �м��ϰ�
	//�Ű� ������ �� ���� �ϳ��� cmdInfo�� �����ϴ�.

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
//ProcessShellCommand(): �� ��� �Լ��� rCmdInfo�� �ĺ��� ��ü���� CCommandLineInfo ���޵� �Ű� ������ �����ϰ� ǥ�õ� �۾��� �����ϱ� ���� InitInstance���� ȣ��˴ϴ�.
		return FALSE;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	//ProcessShellCommand(): ���� ���α׷��� CWinApp::InitInstance �Լ����� �����͸� CWnd ����Ͽ� â ������ �� ��� �Լ��� ȣ���Ͽ� â�� Windows ���� ������ �Ǵ� ���� Ž���� ������ ������ ������� ��Ÿ���ϴ�.

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	//ShowWindow(): ��ü�� ����ų� ǥ���Ϸ��� �� �� CWnd �����ӿ�ũ���� �� ��� �Լ��� ȣ���մϴ�.
	pMainFrame->UpdateWindow();
	//UpdateWindow(): ������Ʈ ������ ��� ���� ���� ��� �޽����� ���� WM_PAINT Ŭ���̾�Ʈ ������ ������Ʈ�մϴ�.
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX }; //IDD_ABOUTBOX: �� ���α׷� ���� â
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//DoDataExchange(): ��ȭ ���� �����͸� ��ȯ�ϰ� ��ȿ���� �˻��ϱ� ���� �����ӿ�ũ���� ȣ��˴ϴ�.
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//DECLARE_MESSAGE_MAP(): Ŭ������ �޽��� ���� �����Ѵٰ� �����մϴ�. ���α׷��� �� CCmdTarget�Ļ� Ŭ������ �޽����� ó���� �޽��� ���� �����ؾ� �մϴ�.
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//��ȭ ���� �����͸� ��ȯ�ϰ� ��ȿ���� �˻��ϱ� ���� �����ӿ�ũ���� ȣ��˴ϴ�.
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDrawApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
	//DoModal(): �� ��� �Լ��� ȣ���Ͽ� ��� ��ȭ ���ڸ� ȣ���ϰ� �Ϸ�Ǹ� ��ȭ ���� ����� ��ȯ�մϴ�.
}

/////////////////////////////////////////////////////////////////////////////
// CDrawApp commands
