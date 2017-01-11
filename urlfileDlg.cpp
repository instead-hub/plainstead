// urlfileDlg.cpp : implementation file
// Written by chensu
//

#include "stdafx.h"
#include "PlainInstead.h"
#include "urlfileDlg.h"
#include "unzip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//*****************************************************************************
//-----------------------------------------------------------------------------
#define    WM_USER_ENDDOWNLOAD           (WM_USER + 1)

#define    WM_USER_DISPLAYSTATUS         (WM_USER + 2)
//-----------------------------------------------------------------------------
//*****************************************************************************

//*****************************************************************************
//-----------------------------------------------------------------------------
CBSCallbackImpl::CBSCallbackImpl(HWND hWnd, HANDLE hEventStop)
{
	m_hWnd = hWnd;  // the window handle to display status

	m_hEventStop = hEventStop;  // the event object to signal to stop

	m_ulObjRefCount = 1;
}
//-----------------------------------------------------------------------------
// IUnknown
STDMETHODIMP CBSCallbackImpl::QueryInterface(REFIID riid, void **ppvObject)
{
	TRACE(_T("IUnknown::QueryInterface\n"));

	*ppvObject = NULL;
	
	// IUnknown
	if (::IsEqualIID(riid, __uuidof(IUnknown)))
	{
		TRACE(_T("IUnknown::QueryInterface(IUnknown)\n"));

		*ppvObject = this;
	}
	// IBindStatusCallback
	else if (::IsEqualIID(riid, __uuidof(IBindStatusCallback)))
	{
		TRACE(_T("IUnknown::QueryInterface(IBindStatusCallback)\n"));

		*ppvObject = static_cast<IBindStatusCallback *>(this);
	}

	if (*ppvObject)
	{
		(*reinterpret_cast<LPUNKNOWN *>(ppvObject))->AddRef();

		return S_OK;
	}
	
	return E_NOINTERFACE;
}                                             
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CBSCallbackImpl::AddRef()
{
	TRACE(_T("IUnknown::AddRef\n"));

	return ++m_ulObjRefCount;
}
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CBSCallbackImpl::Release()
{
	TRACE(_T("IUnknown::Release\n"));

	return --m_ulObjRefCount;
}
//-----------------------------------------------------------------------------
// IBindStatusCallback
STDMETHODIMP CBSCallbackImpl::OnStartBinding(DWORD, IBinding *)
{
	TRACE(_T("IBindStatusCallback::OnStartBinding\n"));

	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::GetPriority(LONG *)
{
	TRACE(_T("IBindStatusCallback::GetPriority\n"));

	return E_NOTIMPL;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnLowResource(DWORD)
{
	TRACE(_T("IBindStatusCallback::OnLowResource\n"));

	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnProgress(ULONG ulProgress,
										 ULONG ulProgressMax,
										 ULONG ulStatusCode,
										 LPCWSTR szStatusText)
{
#ifdef _DEBUG
	static const LPCTSTR plpszStatus[] = 
	{
		_T("BINDSTATUS_FINDINGRESOURCE"),  // 1
		_T("BINDSTATUS_CONNECTING"),
		_T("BINDSTATUS_REDIRECTING"),
		_T("BINDSTATUS_BEGINDOWNLOADDATA"),
		_T("BINDSTATUS_DOWNLOADINGDATA"),
		_T("BINDSTATUS_ENDDOWNLOADDATA"),
		_T("BINDSTATUS_BEGINDOWNLOADCOMPONENTS"),
		_T("BINDSTATUS_INSTALLINGCOMPONENTS"),
		_T("BINDSTATUS_ENDDOWNLOADCOMPONENTS"),
		_T("BINDSTATUS_USINGCACHEDCOPY"),
		_T("BINDSTATUS_SENDINGREQUEST"),
		_T("BINDSTATUS_CLASSIDAVAILABLE"),
		_T("BINDSTATUS_MIMETYPEAVAILABLE"),
		_T("BINDSTATUS_CACHEFILENAMEAVAILABLE"),
		_T("BINDSTATUS_BEGINSYNCOPERATION"),
		_T("BINDSTATUS_ENDSYNCOPERATION"),
		_T("BINDSTATUS_BEGINUPLOADDATA"),
		_T("BINDSTATUS_UPLOADINGDATA"),
		_T("BINDSTATUS_ENDUPLOADINGDATA"),
		_T("BINDSTATUS_PROTOCOLCLASSID"),
		_T("BINDSTATUS_ENCODING"),
		_T("BINDSTATUS_VERFIEDMIMETYPEAVAILABLE"),
		_T("BINDSTATUS_CLASSINSTALLLOCATION"),
		_T("BINDSTATUS_DECODING"),
		_T("BINDSTATUS_LOADINGMIMEHANDLER"),
		_T("BINDSTATUS_CONTENTDISPOSITIONATTACH"),
		_T("BINDSTATUS_FILTERREPORTMIMETYPE"),
		_T("BINDSTATUS_CLSIDCANINSTANTIATE"),
		_T("BINDSTATUS_IUNKNOWNAVAILABLE"),
		_T("BINDSTATUS_DIRECTBIND"),
		_T("BINDSTATUS_RAWMIMETYPE"),
		_T("BINDSTATUS_PROXYDETECTING"),
		_T("BINDSTATUS_ACCEPTRANGES"),
		_T("???")  // unknown
	};
#endif

	TRACE(_T("IBindStatusCallback::OnProgress\n"));

	TRACE(_T("ulProgress: %lu, ulProgressMax: %lu\n"),
		  ulProgress, ulProgressMax);
	
	TRACE(_T("ulStatusCode: %lu "), ulStatusCode);

	if (ulStatusCode < UF_BINDSTATUS_FIRST ||
		ulStatusCode > UF_BINDSTATUS_LAST)
	{
		ulStatusCode = UF_BINDSTATUS_LAST + 1;
	}
	
#ifdef _DEBUG
	TRACE(_T("(%s), szStatusText: %ls\n"),
		  plpszStatus[ulStatusCode - UF_BINDSTATUS_FIRST],
		  szStatusText);
#endif

	if (m_hWnd != NULL)
	{
		// inform the dialog box to display current status,
		// don't use PostMessage
		CUrlFileDlg::DOWNLOADSTATUS downloadStatus =
			{ ulProgress, ulProgressMax, ulStatusCode, szStatusText };
		::SendMessage(m_hWnd, WM_USER_DISPLAYSTATUS,
					  0, reinterpret_cast<LPARAM>(&downloadStatus));
	}

	if (m_hEventStop != NULL)
	{
		if (::WaitForSingleObject(m_hEventStop, 0) == WAIT_OBJECT_0)
		{
			TRACE(_T("Canceled.\n"));
			::SendMessage(m_hWnd, WM_USER_DISPLAYSTATUS, 0, NULL);
			return E_ABORT;  // canceled by the user
		}
	}

	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnStopBinding(HRESULT, LPCWSTR)
{
	TRACE(_T("IBindStatusCallback::OnStopBinding\n"));

	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::GetBindInfo(DWORD *, BINDINFO *)
{
	TRACE(_T("IBindStatusCallback::GetBindInfo\n"));

	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnDataAvailable(DWORD, DWORD,
											  FORMATETC *, STGMEDIUM *)
{
	TRACE(_T("IBindStatusCallback::OnDataAvailable\n"));

	return S_OK;
}
//-----------------------------------------------------------------------------
STDMETHODIMP CBSCallbackImpl::OnObjectAvailable(REFIID, IUnknown *)
{
	TRACE(_T("IBindStatusCallback::OnObjectAvailable\n"));

	return S_OK;
}
//-----------------------------------------------------------------------------
//*****************************************************************************


//*****************************************************************************
//-----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CUrlFileDlg dialog

IMPLEMENT_DYNAMIC(CUrlFileDlg, CDialog)

CUrlFileDlg::CUrlFileDlg(CString url, CString filename, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_URLFILE_DIALOG, pParent),
	m_strURL(url),
	m_selFile(filename)

{
	//{{AFX_DATA_INIT(CUrlFileDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pDownloadThread = NULL;

	goodLoad = false;
}

void CUrlFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUrlFileDlg)
	//DDX_Control(pDX, IDOK, m_btnDownloadStop);
	//DDX_Control(pDX, IDCANCEL, m_btnExit);
	//DDX_Control(pDX, IDC_URL, m_editURL);
	//DDX_Text(pDX, IDC_URL, m_strURL);
	//DDX_Control(pDX, IDC_PROGRESS, m_editProgress);
	//DDX_Text(pDX, IDC_PROGRESS, m_strProgress);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PROGRESS, m_progressPercent);
	DDX_Control(pDX, IDC_EDIT_BYTES_LOAD, m_bytesLoad);
}

BEGIN_MESSAGE_MAP(CUrlFileDlg, CDialog)
	//{{AFX_MSG_MAP(CUrlFileDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER_ENDDOWNLOAD, OnEndDownload)
	ON_MESSAGE(WM_USER_DISPLAYSTATUS, OnDisplayStatus)
	//ON_EN_MAXTEXT(IDC_PROGRESS, OnMaxtextProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUrlFileDlg message handlers

BOOL CUrlFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_progressPercent.SetScrollRange(0, 100, TRUE);
	m_progressPercent.SetPos(0);
	m_bytesLoad.SetWindowTextW(L"");
	//Сразу начинаем загрузку
	StartDownload();
	
	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CUrlFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUrlFileDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUrlFileDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//-----------------------------------------------------------------------------
// sent when the downloading thread ends
// wParam and lParam: not used
// return value: not used
LRESULT CUrlFileDlg::OnEndDownload(WPARAM, LPARAM)
{
	ASSERT_POINTER(m_pDownloadThread, CWinThread);
	ASSERT_KINDOF(CWinThread, m_pDownloadThread);

	TRACE(_T("OnEndDownload()\n"));

	// wait until the thread terminates
	DWORD dwExitCode;
	if (::GetExitCodeThread(m_pDownloadThread->m_hThread, &dwExitCode) &&
		dwExitCode == STILL_ACTIVE)
	{
		::WaitForSingleObject(m_pDownloadThread->m_hThread, INFINITE);
	}

	delete m_pDownloadThread;
	m_pDownloadThread = NULL;

	// display the result
	if (m_downloadParam.strFileName.IsEmpty())
	{
		::AfxMessageBox(L"Загрузка не удалась!");
	}
	else
	{
		//Загрузка успешная
		::AfxMessageBox(L"Игра загружена!",
						MB_OK | MB_ICONINFORMATION);

		//Распаковка через zip utils
		HZIP hz = OpenZip(m_selFile, 0);
		ZIPENTRY ze;
		GetZipItem(hz, -1, &ze);
		int numitems = ze.index;
		SetUnzipBaseDir(hz, L"games");
		for (int i = 0; i<numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			UnzipItem(hz, i, ze.name);
		}
		CloseZip(hz);
		//удаление архива
		DeleteFile(m_selFile);
		//Признак хорошей загрузки
		goodLoad = true;
	}

	// change the UI
	this->ChangeUIDownloading(false);

	EndDialog(-1);

	return 0;
}

bool CUrlFileDlg::isGoodLoad()
{
	return goodLoad;
}

//-----------------------------------------------------------------------------
// sent from the downloading thread to the dialog box to display the progress
// wParam: not used
// lParam: DOWNLOADSTATUS *, NULL if canceled
//		   {
//			   ULONG ulProgress - IN
//			   ULONG ulProgressMax - IN
//			   ULONG ulStatusCode - IN
//			   LPCWSTR szStatusText - IN
//		   }
// return value: not used
LRESULT CUrlFileDlg::OnDisplayStatus(WPARAM, LPARAM lParam)
{
	const DOWNLOADSTATUS *const pDownloadStatus =
		reinterpret_cast<DOWNLOADSTATUS *>(lParam);

	// form the status text
	CString strStatus;
	CString strProgress;

	if (pDownloadStatus != NULL)
	{
		ASSERT_POINTER(pDownloadStatus, DOWNLOADSTATUS);

		//VERIFY(strStatus.LoadString(pDownloadStatus->ulStatusCode -
		//							UF_BINDSTATUS_FIRST +
		//							IDS_BINDSTATUS01));
		strStatus += _T("  ");
		strStatus += pDownloadStatus->szStatusText;

		strProgress.Format(L"Загружено байт %lu из %lu",
						   pDownloadStatus->ulProgress,
						   pDownloadStatus->ulProgressMax);

	}
	else
	{
		//VERIFY(strStatus.LoadString(IDS_CANCELED));
	}

#if 1
	//Add progress
	int perc = (int)(((float)pDownloadStatus->ulProgress / (float)pDownloadStatus->ulProgressMax)*100.0f);
	m_progressPercent.SetPos(perc);
	m_bytesLoad.SetWindowTextW(strProgress);
	//const int nLen = m_editProgress.GetWindowTextLength();
	//m_editProgress.SetSel(nLen, nLen);
	//m_editProgress.ReplaceSel(strStatus);
#else
	// retrieve the status text
	VERIFY(this->UpdateData());
	
	// append the text
	m_strProgress += strStatus;

	// update the edit box
	VERIFY(this->UpdateData(FALSE));
#endif

	return 0;
}
//-----------------------------------------------------------------------------
// The current insertion has exceeded the specified number of characters
// for the edit control and has been truncated.
void CUrlFileDlg::OnMaxtextProgress() 
{
	// Simply empty the edit box
	//m_editProgress.SetWindowText(NULL);
}
//-----------------------------------------------------------------------------
// Download or Stop button pressed
void CUrlFileDlg::OnOK()
{
}

void CUrlFileDlg::StartDownload()
{
	// check if the URL is valid
	USES_CONVERSION;
	if (m_strURL.IsEmpty() ||
		::IsValidURL(NULL, T2CW(m_strURL), 0) != S_OK)
	{
		::AfxMessageBox(L"IDS_ERRINVALIDURL", MB_OK | MB_ICONSTOP);

		// change the UI
		this->ChangeUIDownloading(false);

		return;
	}

	// parameters to be passed to the thread
	m_downloadParam.hWnd = this->GetSafeHwnd();

	m_eventStop.ResetEvent();  // nonsignaled
	m_downloadParam.hEventStop = m_eventStop;

	m_downloadParam.strURL = m_strURL;

	m_downloadParam.strFileName = m_selFile;

	// create a thread to download, but don't start yet
	m_pDownloadThread = ::AfxBeginThread(CUrlFileDlg::Download,
		&m_downloadParam,
		THREAD_PRIORITY_NORMAL,
		0,
		CREATE_SUSPENDED);
	if (m_pDownloadThread != NULL)
	{
		TRACE(_T("AfxBeginThread: 0x%08lX\n"),
			m_pDownloadThread->m_nThreadID);

		/*
		Set the m_bAutoDelete data member to FALSE. This allows
		the CWinThread object to survive after the thread has been
		terminated. You can then access the m_hThread data member
		after the thread has been terminated. If you use this
		technique, however, you are responsible for destroying the
		CWinThread object as the framework will not automatically
		delete it for you.
		*/
		m_pDownloadThread->m_bAutoDelete = FALSE;

		// change the UI
		this->ChangeUIDownloading();

		// start
		VERIFY(m_pDownloadThread->ResumeThread() == 1);
	}
	else
	{
		::AfxMessageBox(L"IDS_ERRCREATETHREAD", MB_OK | MB_ICONSTOP);

		// change the UI
		this->ChangeUIDownloading(false);
	}
}
//-----------------------------------------------------------------------------
// Exit button, Alt-F4 key, Close menu or Close button
void CUrlFileDlg::OnCancel()
{
	if (m_pDownloadThread != NULL)
	{
		int want_exit = AfxMessageBox(L"Вы действительно хотите прекратить загрузку?", MB_YESNO | MB_ICONQUESTION);
		if (want_exit == IDYES)
		{
			m_eventStop.SetEvent();  // signaled
			//удаление архива
			DeleteFile(m_selFile);
			CDialog::OnCancel();
		}
		return;  // Don't exit while downloading
	}
	
	CDialog::OnCancel();
}
//-----------------------------------------------------------------------------
// change the user interface
// bDownloading: true - downloading, false - not downloading
void CUrlFileDlg::ChangeUIDownloading(bool bDownloading /* = true */)
{
	if (bDownloading)
	{
		// Progress edit box
		//m_editProgress.SetWindowText(NULL);  // clear the status text
		m_progressPercent.SetFocus();
	}
	else
	{
		// URL edit box
		//m_editURL.SetFocus();
		//m_editURL.SetSel(0, -1);  // select all the text
	}
}
//-----------------------------------------------------------------------------
// static member function - the controlling function for the worker thread
// pParam: DOWNLOADPARAM *
//		   {
//			   HWND hWnd - IN				the window handle to display status
//			   HANDLE hEventStop - IN		the event object to signal to stop
//			   CString strURL - IN			the URL of the file to be downloaded
//			   CString strFileName - OUT	the filename of the downloaded file
//		   }
// return value: not used
UINT CUrlFileDlg::Download(LPVOID pParam)
{
	DOWNLOADPARAM *const pDownloadParam = static_cast<DOWNLOADPARAM *>(pParam);
	ASSERT_POINTER(pDownloadParam, DOWNLOADPARAM);

	ASSERT(::IsWindow(pDownloadParam->hWnd));

	/*
		URLDownloadToCacheFile is a blocking function. Even though the data is
		downloaded asynchronously the function does not return until all the
		data is downloaded. If complete asynchronous downloading is desired,
		one of the other UOS functions, such as URLOpenStream, or perhaps
		general URL monikers would be more appropriate.
	*/

	CBSCallbackImpl bsc(pDownloadParam->hWnd, pDownloadParam->hEventStop);

	const HRESULT hr = ::URLDownloadToFile(NULL, 
											pDownloadParam->strURL,
											pDownloadParam->strFileName.GetBuffer(MAX_PATH),
											URLOSTRM_GETNEWESTVERSION,
											&bsc);
	/*
	const HRESULT hr = ::URLDownloadToCacheFile(NULL,
												pDownloadParam->strURL,
												pDownloadParam->strFileName.
													GetBuffer(MAX_PATH),
												URLOSTRM_GETNEWESTVERSION,
												0,
												&bsc);
	*/
	/*
		The resource from the cache is used for the second and subsequent
		calls to URLDownloadToCacheFile during the session of the program
		unless the following setting is selected, in which case, every call
		to URLDownloadToCacheFile downloads the resource from the Internet.

		Control Panel/Internet/General/Temporary Internet files/Settings/
		Check for newer versions of stored pages -> Every visit to the page
	*/
	
	// empty the filename string if failed or canceled
	pDownloadParam->strFileName.ReleaseBuffer(SUCCEEDED(hr) ? -1 : 0);

	TRACE(_T("URLDownloadToCacheFile ends: 0x%08lX\nCache file name: %s\n"),
		  hr, pDownloadParam->strFileName);

	// let the dialog box know it is done
	VERIFY(::PostMessage(pDownloadParam->hWnd, WM_USER_ENDDOWNLOAD, 0, 0));

	return 0;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
