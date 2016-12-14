// MainFrm.cpp : ���������� ������ CMainFrame
//

#include "stdafx.h"
#include "PlainInstead.h"

#include "MainFrm.h"
#include "GlobalManager.h"
#include "MultiSpeech.h"
#include "IniFile.h"
#include "InterpreterController.h"
#include "bass.h"

extern "C" {
	#include "instead\instead.h"
	extern int gBassInit;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ��������� ������ ���������
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// ��������/����������� CMainFrame

CMainFrame::CMainFrame()
{
	EnableActiveAccessibility();
	// TODO: �������� ��� ������������� �����
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("�� ������� ������� ������ ������������\n");
		return -1;      // �� ������� �������
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("�� ������� ������� ������ ���������\n");
		return -1;      // �� ������� �������
	}

	// TODO: ������� ��� ��� ������, ���� �� ����������� ���������� ������ ������������
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �������� ����� Window ��� ����� ����������� ���������
	//  CREATESTRUCT cs

	return TRUE;
}


// ����������� CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


void CMainFrame::OnClose()
{
	if (!GlobalManager::getInstance().isUserSaveLastFile() && !GlobalManager::getInstance().isIgnoreExitDialog)
	{
		if (MessageBox(L"�� ������� ��� ������ ����� ��� ���������� ����?",L"���� �� ��������", MB_YESNOCANCEL) != IDYES)
		{
			return;
		}
	}

	instead_done();
	if (gBassInit) BASS_Free();
	MultiSpeech::getInstance().Unload();

	WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
	CIniFile mainSettings(L".\\settings.ini", 1024);
	mainSettings.WriteNumber(L"MainFrame", L"WPlen", (INT)wp.length );
	mainSettings.WriteStruct(L"MainFrame", L"WP", &wp, wp.length );

	//��������� ���, ����� ������ ��������� ����������, ������!
	InterpreterController::endInterpreter();
	exit(0);

	CFrameWnd::OnClose();
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    CFrameWnd::OnShowWindow(bShow, nStatus);

    static bool bOnce = true;

    if(bShow && !IsWindowVisible()
        && bOnce)
    {
        bOnce = false;

        WINDOWPLACEMENT lwp;
        UINT nl;

		CIniFile mainSettings(L".\\settings.ini", 1024);
		nl = mainSettings.GetInt(L"MainFrame", L"WPlen", 0);
		if (nl)
		{
			mainSettings.GetStruct(L"MainFrame", L"WP", &lwp, nl);
			SetWindowPlacement(&lwp);
		}
    }
}


