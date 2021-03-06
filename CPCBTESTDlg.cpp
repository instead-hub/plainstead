// CPCBTESTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlainInstead.h"
#include "ColorPickerCB.h"
#include "CPCBTESTDlg.h"
#include "IniFile.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define START_FONT 9

/////////////////////////////////////////////////////////////////////////////
// CCPCBTESTDlg dialog

CCPCBTESTDlg::CCPCBTESTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCPCBTESTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCPCBTESTDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCPCBTESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCPCBTESTDlg)
	DDX_Control(pDX, IDC_FOCUSEDCB, m_cbOutBack);
	DDX_Control(pDX, IDC_INACTIVECB, m_cbInBack);
	DDX_Control(pDX, IDC_DROPPEDCB, m_cbInFont);
	DDX_Control(pDX, IDC_DISABLEDCB, m_cbOutFont);
	//}}AFX_DATA_MAP
	//DDX_Control(pDX, IDC_EDIT_SIZE_FONT, m_editFontHeight);
	//DDX_Text(pDX, IDC_EDIT_SIZE_FONT, m_valueEdit);
	DDX_Control(pDX, IDC_STATIC_EXAMPLE_OUT_FONT, m_textExampleOut);
	DDX_Control(pDX, IDC_STATIC_EXAMPLE_IN_FONT, m_textExampleIn);
	DDX_Control(pDX, IDC_CHECK_AUTOSAY, m_CheckAutosay);
	DDX_Control(pDX, IDC_CHECK_SET_FOCUS_OUT, m_CheckSetFocusToOut);
	DDX_Control(pDX, IDC_COMBO_FONT_SIZE, m_ComboFontSize);
	DDX_Control(pDX, IDC_CHECK_AUTO_LOG, mCheckAutoLog);
	DDX_Control(pDX, IDC_CHECK_SOUND_LIST, mCheckSoundList);
	DDX_Control(pDX, IDC_COMBO_STYLE_ANNOUNCE, m_ComboStyleAnnounce);
}

BEGIN_MESSAGE_MAP(CCPCBTESTDlg, CDialog)
	//{{AFX_MSG_MAP(CCPCBTESTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCPCBTESTDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_SIZE_FONT, &CCPCBTESTDlg::OnEnChangeEditSizeFont)
	ON_CBN_SELCHANGE(IDC_FOCUSEDCB, &CCPCBTESTDlg::OnCbnSelchangeFocusedcb)
	ON_CBN_SELCHANGE(IDC_INACTIVECB, &CCPCBTESTDlg::OnCbnSelchangeInactivecb)
	ON_CBN_SELCHANGE(IDC_DISABLEDCB, &CCPCBTESTDlg::OnCbnSelchangeDisabledcb)
	ON_CBN_SELCHANGE(IDC_DROPPEDCB, &CCPCBTESTDlg::OnCbnSelchangeDroppedcb)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_FONT_SIZE, &CCPCBTESTDlg::OnCbnSelchangeComboFontSize)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE_ANNOUNCE, &CCPCBTESTDlg::OnCbnSelchangeComboStyleAnnounce)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_ANNOUNCE, &CCPCBTESTDlg::OnBnClickedButtonCheckAnnounce)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCPCBTESTDlg message handlers

BOOL CCPCBTESTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	m_cbOutFont.InitializeDefaultColors();
	m_cbInBack.InitializeDefaultColors();
	m_cbOutBack.InitializeDefaultColors();
	m_cbInFont.InitializeDefaultColors();

	CIniFile mainSettings;
	CString fontH;
	mainSettings.GetString(L"main", L"fontHeight", fontH, L"20" );
	
	m_cbOutFont.SetCurSel( mainSettings.GetInt(L"main", L"m_cbOutFont", 7 ) );
	m_cbInBack.SetCurSel( mainSettings.GetInt(L"main", L"m_cbInBack", 137 ) );//white
	m_cbOutBack.SetCurSel( mainSettings.GetInt(L"main", L"m_cbOutBack", 128 ) );//standard back
	m_cbInFont.SetCurSel( mainSettings.GetInt(L"main", L"m_cbInFont", 7 ) );
	m_CheckAutosay.SetCheck( mainSettings.GetInt(L"main", L"m_CheckAutosay", 1 ) );
	m_CheckSetFocusToOut.SetCheck( mainSettings.GetInt(L"main", L"m_CheckSetFocusToOut", 0 ) );
	mCheckAutoLog.SetCheck( mainSettings.GetInt(L"main", L"mCheckAutoLog", 0 ) );
	//mCheckSoundList.SetCheck(mainSettings.GetInt(L"main", L"mCheckSoundList", 1));

	//m_editFontHeight.SetWindowTextW(fontH);

	m_valueEdit=_wtoi(fontH);
	UpdateFontSize(_wtoi(fontH));

	//���������� ����������
	for (int fnt=START_FONT;fnt<=35;fnt++)
	{
		CString fntText;
		fntText.Format(L"%d",fnt);
		m_ComboFontSize.InsertString(fnt-START_FONT,fntText);
	}
	m_ComboFontSize.SetCurSel(m_valueEdit-START_FONT);

	TCHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString baseDir = buff;
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	SetCurrentDirectory(baseDir);
	//���������� ������ ��� ����������
	if (PathFileExists(baseDir + L"\\sounds\\scene1.wav") &&
		PathFileExists(baseDir + L"\\sounds\\inventory1.wav") &&
		PathFileExists(baseDir + L"\\sounds\\ways1.wav")
		)
	{
		m_ComboStyleAnnounce.AddString(L"1");
	}
	if (PathFileExists(baseDir + L"\\sounds\\scene2.wav") &&
		PathFileExists(baseDir + L"\\sounds\\inventory2.wav") &&
		PathFileExists(baseDir + L"\\sounds\\ways2.wav")
		)
	{
		m_ComboStyleAnnounce.AddString(L"2");
	}
	if (PathFileExists(baseDir + L"\\sounds\\scene3.wav") &&
		PathFileExists(baseDir + L"\\sounds\\inventory3.wav") &&
		PathFileExists(baseDir + L"\\sounds\\ways3.wav")
		)
	{
		m_ComboStyleAnnounce.AddString(L"3");
	}
	if (PathFileExists(baseDir + L"\\sounds\\scene4.wav") &&
		PathFileExists(baseDir + L"\\sounds\\inventory4.wav") &&
		PathFileExists(baseDir + L"\\sounds\\ways4.wav")
		)
	{
		m_ComboStyleAnnounce.AddString(L"4");
	}
	if (PathFileExists(baseDir + L"\\sounds\\scene5.wav") &&
		PathFileExists(baseDir + L"\\sounds\\inventory5.wav") &&
		PathFileExists(baseDir + L"\\sounds\\ways5.wav")
		)
	{
		m_ComboStyleAnnounce.AddString(L"5");
	}
	m_currWaveStyle = mainSettings.GetInt(L"main", L"m_ComboStyleAnnounce", 0);
	m_ComboStyleAnnounce.SetCurSel(m_currWaveStyle);
	temp_scene_wave = 0;
	temp_inv_wave = 0;
	temp_ways_wave = 0;
	UpdateTempWave();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCPCBTESTDlg::UpdateTempWave()
{
	char wave_pos[30];
	
	sprintf(wave_pos, "sounds\\scene%d.wav", m_currWaveStyle + 1);
	if (temp_scene_wave) delete temp_scene_wave;
	temp_scene_wave = new Wave(wave_pos);

	sprintf(wave_pos, "sounds\\inventory%d.wav", m_currWaveStyle + 1);
	if (temp_inv_wave) delete temp_inv_wave;
	temp_inv_wave = new Wave(wave_pos);
	
	sprintf(wave_pos, "sounds\\ways%d.wav", m_currWaveStyle + 1);
	if (temp_ways_wave) delete temp_ways_wave;
	temp_ways_wave = new Wave(wave_pos);
}


void CCPCBTESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCPCBTESTDlg::OnPaint() 
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
HCURSOR CCPCBTESTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCPCBTESTDlg::OnBnClickedOk()
{
	// TODO: ���������� � �������� ���������
	CIniFile mainSettings;
	//CString fontH;
	//m_editFontHeight.GetWindowTextW(fontH);
	//mainSettings.WriteNumber(L"main", L"fontHeight", _wtoi(fontH) );
	mainSettings.WriteNumber(L"main", L"fontHeight", m_ComboFontSize.GetCurSel()+START_FONT );
	mainSettings.WriteNumber(L"main", L"m_cbOutBack", m_cbOutBack.GetCurSel() );
	mainSettings.WriteNumber(L"main", L"m_cbInBack",  m_cbInBack.GetCurSel() );
	mainSettings.WriteNumber(L"main", L"m_cbInFont", m_cbInFont.GetCurSel() );
	mainSettings.WriteNumber(L"main", L"m_cbOutFont",  m_cbOutFont.GetCurSel() );

	mainSettings.WriteNumber(L"main", L"OutBackCol", (INT)m_cbOutBack.GetSelectedColorValue() );
	mainSettings.WriteNumber(L"main", L"InBackCol",  (INT)m_cbInBack.GetSelectedColorValue() );
	mainSettings.WriteNumber(L"main", L"InFontCol", (INT)m_cbInFont.GetSelectedColorValue() );
	mainSettings.WriteNumber(L"main", L"OutFontCol",  (INT)m_cbOutFont.GetSelectedColorValue() );
	mainSettings.WriteNumber(L"main", L"m_CheckAutosay",  m_CheckAutosay.GetCheck() );
	mainSettings.WriteNumber(L"main", L"m_CheckSetFocusToOut",  m_CheckSetFocusToOut.GetCheck() );
	mainSettings.WriteNumber(L"main", L"mCheckAutoLog",  mCheckAutoLog.GetCheck() );
	//mainSettings.WriteNumber(L"main", L"mCheckSoundList", mCheckSoundList.GetCheck());

	mainSettings.WriteNumber(L"main", L"m_ComboStyleAnnounce", m_ComboStyleAnnounce.GetCurSel());

	OnOK();
}

HBRUSH CCPCBTESTDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �������� ����� �������� DC
	if(pWnd->GetDlgCtrlID() == IDC_STATIC_EXAMPLE_OUT_FONT)   
	{      
		pDC->SetTextColor(m_cbOutFont.GetSelectedColorValue());   
		pDC->SetBkColor(m_cbOutBack.GetSelectedColorValue());   
	}
	else if(pWnd->GetDlgCtrlID() == IDC_STATIC_EXAMPLE_IN_FONT)   
	{      
		pDC->SetTextColor(m_cbInFont.GetSelectedColorValue());   
		pDC->SetBkColor(m_cbInBack.GetSelectedColorValue());   
	}

	// TODO:  ������� ������ �������� ����������� �����, ���� ��� �� ���������� �� ���������
	return hbr;
}

void CCPCBTESTDlg::UpdateFontSize(int fontH)
{
	//��������� ������� ��������
    LOGFONT lf;   
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = fontH;                // Request a fontH font
    wcscpy(lf.lfFaceName, L"Arial");    // with face name "Arial".
	m_font.DeleteObject();
	m_font.CreateFontIndirect(&lf);    // Create the font.
	m_textExampleIn.SetFont(&m_font);
	m_textExampleOut.SetFont(&m_font);
}

void CCPCBTESTDlg::OnEnChangeEditSizeFont()
{
	//UpdateData(TRUE);
	//if (m_valueEdit<1) m_valueEdit = 1;
	//if (m_valueEdit>30) m_valueEdit = 30;
	//UpdateFontSize(m_valueEdit);
	//Invalidate();
	//UpdateWindow();
}

void CCPCBTESTDlg::OnCbnSelchangeFocusedcb()
{
	// TODO: �������� ���� ��� ����������� �����������
	Invalidate();
	UpdateWindow();
}

void CCPCBTESTDlg::OnCbnSelchangeInactivecb()
{
	// TODO: �������� ���� ��� ����������� �����������
	Invalidate();
	UpdateWindow();
}

void CCPCBTESTDlg::OnCbnSelchangeDisabledcb()
{
	// TODO: �������� ���� ��� ����������� �����������
	Invalidate();
	UpdateWindow();
}

void CCPCBTESTDlg::OnCbnSelchangeDroppedcb()
{
	// TODO: �������� ���� ��� ����������� �����������
	Invalidate();
	UpdateWindow();
}

/*
void CCPCBTESTDlg::OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: �������� ���� ��� ����������� �����������
	if ( pNMUpDown->iDelta < 0 )
	{
		if (m_valueEdit>8) 
		{
			m_valueEdit--;
			UpdateFontSize(m_valueEdit);
			UpdateData(FALSE);
		}
	}
	else
	{
		if (m_valueEdit<30) 
		{
			m_valueEdit++;
			UpdateFontSize(m_valueEdit);
			UpdateData(FALSE);
		}
	}
	*pResult = 0;
}
*/

void CCPCBTESTDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_font.DeleteObject();
	if (temp_scene_wave) delete temp_scene_wave;
	if (temp_inv_wave) delete temp_inv_wave;
	if (temp_ways_wave) delete temp_ways_wave;
	// TODO: �������� ���� ��� ����������� ���������
}

void CCPCBTESTDlg::OnCbnSelchangeComboFontSize()
{
	// TODO: Add your control notification handler code here
	int currFont = m_ComboFontSize.GetCurSel()+START_FONT;
	UpdateFontSize(currFont);
	UpdateData(FALSE);
}


void CCPCBTESTDlg::OnCbnSelchangeComboStyleAnnounce()
{
	// TODO: �������� ���� ��� ����������� �����������
	if (m_ComboStyleAnnounce.GetCurSel() != m_currWaveStyle)
	{
		m_currWaveStyle = m_ComboStyleAnnounce.GetCurSel();
		UpdateTempWave();
		OnBnClickedButtonCheckAnnounce();
	}
}


void CCPCBTESTDlg::OnBnClickedButtonCheckAnnounce()
{
	// TODO: �������� ���� ��� ����������� �����������
	if (temp_scene_wave && temp_inv_wave && temp_ways_wave)
	{
		temp_scene_wave->play(true);
		temp_inv_wave->play(true);
		temp_ways_wave->play(true);
	}
}
