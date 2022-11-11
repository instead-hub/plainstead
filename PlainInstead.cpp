// PlainInstead.cpp : ���������� ��������� ������� ��� ����������.
//

#include "stdafx.h"
#include <locale.h>
#include <iostream>
#include "PlainInstead.h"
#include "MainFrm.h"

#include "PlainInsteadDoc.h"
#include "PlainInsteadView.h"
#include "InterpreterController.h"
#include "GlobalManager.h"
#include "MultiSpeech.h"
#include "SelectNewGameDialog.h"
#include "LauncherDialog.h"
#include "CPCBTESTDlg.h"
#include "IniFile.h"
#include "afxwin.h"
#include "global.h"
#include "Tolk.h"
#include "unzip.h"
#pragma comment( lib, "Version.lib" )
#include "bass.h"
#pragma comment( lib, "bass.lib" )
#include "bassmidi.h"
#pragma comment( lib, "bassmidi.lib" )
extern "C" {
	#include "instead\instead.h"
	extern int instead_metaparser_init(void);
	extern int instead_sound_init(void);
	extern int instead_timer_init(void);
	extern void setGlobalSoundLevel(int volume);
	extern int getGlobalSoundLevel();
	extern void stopAllSound();
	extern int gBassInit;
void restart() {
	AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_RESTART_MENU, 0L);
};
void save() {
	AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_FILE_SAVE_GAME, 0L);
};
void load() {
	AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_FILE_OPEN, 0L);
};
void playSound(char* sound,int isLooping) {
	Wave::Play(sound,isLooping);
}
void onNewInsteadCommand(char* cmd,char* p) {
	CPlainInsteadView::GetCurrentView()->onNewInsteadCommand(cmd, p, L"������ ����������");
}
	static int tiny_init(void)
	{
		int rc;
		rc = instead_loadfile("tiny.lua");
		if (rc)
			return rc;
		return 0;
	}

	static struct instead_ext ext;
		//= {
	//	.init = tiny_init,
	//};
	}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CPlainInsteadApp

BEGIN_MESSAGE_MAP(CPlainInsteadApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CPlainInsteadApp::OnAppAbout)
	// ����������� ������� �� ������ � ������� ����������
	//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_NEW_GAME, OnFileNewGame)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_GAME, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CPlainInsteadApp::OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_GAME, &CPlainInsteadApp::OnUpdateFileSaveGame)
	ON_COMMAND(ID_VIEW_FULL_HIST, &CPlainInsteadApp::OnViewFullHist)
	ON_COMMAND(ID_VIEW_PREV_HIST, &CPlainInsteadApp::OnViewPrevHist)
	ON_COMMAND(ID_VIEW_NEXT_HIST, &CPlainInsteadApp::OnViewNextHist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_HIST, &CPlainInsteadApp::OnUpdateViewFullHist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREV_HIST, &CPlainInsteadApp::OnUpdateViewPrevHist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT_HIST, &CPlainInsteadApp::OnUpdateViewNextHist)
	ON_UPDATE_COMMAND_UI(ID_MENU_DEBUG, &CPlainInsteadApp::OnUpdateDebug)
	ON_COMMAND(ID_NEW_GAME_FROM_FILE, &CPlainInsteadApp::OnNewGameFromFile)
	ON_COMMAND(ID_NEW_GAME, &CPlainInsteadApp::OnNewGameFromLib)
	ON_COMMAND(ID_RESTART_MENU, &CPlainInsteadApp::OnRestartMenu)
	ON_UPDATE_COMMAND_UI(ID_RESTART_MENU, &CPlainInsteadApp::OnUpdateRestartMenu)
	ON_COMMAND(ID_ENTER_SETUP, &CPlainInsteadApp::OnEnterSetup)
	ON_COMMAND(ID_RESET_ALL_SETTINGS, &CPlainInsteadApp::OnResetAllSettings)
	ON_COMMAND(ID_APP_EXIT, &CPlainInsteadApp::OnAppExit)
	ON_COMMAND(ID_VOLUME_DOWN, &CPlainInsteadApp::OnVolumeDown)
	ON_COMMAND(ID_VOLUME_UP, &CPlainInsteadApp::OnVolumeUp)
	ON_COMMAND(ID_VOLUME_OFF, &CPlainInsteadApp::OnVolumeOff)
	ON_COMMAND(ID_VOLUME_ON, &CPlainInsteadApp::OnVolumeOn)
	ON_UPDATE_COMMAND_UI(ID_VOLUME_UP, &CPlainInsteadApp::OnUpdateVolumeUp)
	ON_UPDATE_COMMAND_UI(ID_VOLUME_DOWN, &CPlainInsteadApp::OnUpdateVolumeDown)
	ON_UPDATE_COMMAND_UI(ID_VOLUME_ON, &CPlainInsteadApp::OnUpdateVolumeOn)
	ON_UPDATE_COMMAND_UI(ID_VOLUME_OFF, &CPlainInsteadApp::OnUpdateVolumeOff)
	ON_COMMAND(ID_ADD_GAME_TO_LIB, &CPlainInsteadApp::OnAddGameToLib)
	ON_COMMAND(ID_OPEN_MANAGER, &CPlainInsteadApp::OnOpenManager)
	ON_COMMAND(ID_LISTSND_ON, &CPlainInsteadApp::OnListsndOn)
	ON_COMMAND(ID_LISTSND_DOWN, &CPlainInsteadApp::OnListsndDown)
	ON_COMMAND(ID_LISTSND_UP, &CPlainInsteadApp::OnListsndUp)
	ON_UPDATE_COMMAND_UI(ID_LISTSND_ON, &CPlainInsteadApp::OnUpdateListsndOn)
	ON_UPDATE_COMMAND_UI(ID_LISTSND_DOWN, &CPlainInsteadApp::OnUpdateListsndDown)
	ON_UPDATE_COMMAND_UI(ID_LISTSND_UP, &CPlainInsteadApp::OnUpdateListsndUp)
END_MESSAGE_MAP()


// �������� CPlainInsteadApp

CPlainInsteadApp::CPlainInsteadApp()
{
	isMute = false;
	//m_openGame = FALSE;
	// TODO: �������� ��� ��������,
	// ��������� ���� ������ ��� ������������� � InitInstance
}


// ������������ ������ CPlainInsteadApp

CPlainInsteadApp theApp;

// ������������� CPlainInsteadApp


static void RecursiveDelete(CString szPath)
{
	CFileFind ff;
	CString path = szPath;
	
	if(path.Right(1) != "\\")
		path += "\\";

	path += "*.*";

	BOOL res = ff.FindFile(path);

	while(res)
	{
		res = ff.FindNextFile();
		if (!ff.IsDots() && !ff.IsDirectory())
			DeleteFile(ff.GetFilePath());
	}
}

BOOL CPlainInsteadApp::InitInstance()
{
	// InitCommonControlsEx() ��������� ��� Windows XP, ���� ��������
	// ���������� ���������� ComCtl32.dll ������ 6 ��� ����� ������� ������ ��� ���������
	// ������ �����������. � ��������� ������ ����� ��������� ���� ��� �������� ������ ����.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �������� ���� �������� ��� ��������� ���� ����� ������� ����������, ������� ���������� ������������
	// � ����� ����������.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	//hInst = (HINSTANCE)CWinThread::m_pMainWnd;

	// ������������� ��������� OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	//������������� BASS.dll
	gBassInit = 1;
	if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
		gBassInit = 0;
	}

	if (BASS_Init(-1, 44100, BASS_DEVICE_DEFAULT | BASS_DEVICE_FREQ, 0, NULL) == 0) {
		gBassInit = 0;
	}

	//����������� ��������
	BASS_PluginLoad("bassmidi.dll", 0);
	//����� ��� ����
	HSOUNDFONT newfont = BASS_MIDI_FontInit("chorium.sf2", 0);
	if (newfont) {
		BASS_MIDI_FONT sf;
		sf.font = newfont;
		sf.preset = -1; // use all presets
		sf.bank = 0; // use default bank(s)
		BASS_MIDI_StreamSetFonts(0, &sf, 1);    // set default soundfont
												//BASS_MIDI_FontFree(font);             // free old soundfont
	}

	// ����������� �������������
	// ���� ��� ����������� �� ������������ � ���������� ��������� ������
	// ��������� ������������ �����, ���������� ������� �� ����������
	// ���������� ��������� �������������, ������� �� ���������
	// �������� ������ �������, � ������� �������� ���������
	// TODO: ������� �������� ��� ������ �� ���-������ ����������,
	// �������� �� �������� �����������
	//SetRegistryKey(_T("��������� ����������, ��������� � ������� ������� ����������"));
	//LoadStdProfileSettings(4);  // ��������� ����������� ��������� INI-����� (������� MRU)
	// ��������������� ������� ���������� ����������. ������� ����������
	//  ��������� � ���� ���������� ����� �����������, ������ ������� � ���������������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPlainInsteadDoc),
		RUNTIME_CLASS(CMainFrame),       // �������� ���� ������ SDI
		RUNTIME_CLASS(CPlainInsteadView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// �������������� ������ ��������� ������ �� ����������� ������� ��������, DDE, �������� ������
	CCommandLineInfo cmdInfoEmpty;
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ������� ���������������, ��������� � ��������� ������. �������� FALSE ����� ����������, ����
	// ���������� ���� �������� � ���������� /RegServer, /Register, /Unregserver ��� /Unregister.
	if (!ProcessShellCommand(cmdInfoEmpty))
		return FALSE;

	//��������� ������� ������
    LPCTSTR lpsz = _wsetlocale(LC_CTYPE , _T("rus"));
    if (0 == lpsz)
    {
        std::cerr << "Failed set locale" << std::endl;
    }
	ext.init = tiny_init;
	if (instead_extension(&ext)) {
		std::cerr << "Failed set tiny" << std::endl;
		return false;
	}
	instead_err_msg_max(0);

	//�������� ���������� LUA
	instead_sound_init();
	instead_metaparser_init();
	instead_timer_init();
		currFilePath=L"";
	currFileName=L"";
	bool useAutosave = false;
	//��������� ��� ����� ��� ��������
	if (!cmdInfo.m_strFileName.IsEmpty())
	{
		currFilePath = cmdInfo.m_strFileName;
		currFileName = currFilePath.Mid(currFilePath.ReverseFind('\\')+1);		
		StartNewGameFile(currFilePath,currFileName);
	}
	else
	{
		//��������� ��������� �����
		OnOpenManager();
		//CSelectNewGameDialog selNewGameDialog(currFilePath,currFileName,useAutosave);
		//selNewGameDialog.DoModal();
		//if (currFilePath.GetLength()>0)
		//{
		//	StartNewGameFile(currFilePath,currFileName);

			/*
			if (useAutosave)
			{
				CPlainInsteadView::GetCurrentView()->TryInsteadCommand(L"load autosave");
				CPlainInsteadView::GetCurrentView()->TryInsteadCommand(L"");
				CPlainInsteadView::GetCurrentView()->InitFocusLogic();
				GlobalManager::lastString = 0;
			}
			*/
		//}
	}

	// ���� � ������ ���� ���� ���� ����������������, ������� ���������� � �������� ���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// ����� DragAcceptFiles ������ ��� ������� ��������
	//  � ���������� SDI ��� ������ ��������� ����� ProcessShellCommand
	return TRUE;
}



// ���������� ���� CAboutDlg ������������ ��� �������� �������� � ����������

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(CString dictors);

// ������ ����������� ����
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV
	BOOL OnInitDialog();
	CString m_textVoice;
// ����������
protected:
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_usedVoiseHelpers;
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	CStatic mProgramVersion;
};

CAboutDlg::CAboutDlg(CString dictors) : CDialog(CAboutDlg::IDD),
	m_textVoice(dictors)
{
	EnableActiveAccessibility();	
}

static bool GetProductVersion(CString &strProductVersion)
{
    // get the filename of the executable containing the version resource
    TCHAR szFilename[MAX_PATH + 1] = {0};
	DWORD               dwSize              = 0;
    BYTE                *pbVersionInfo      = NULL;
    VS_FIXEDFILEINFO    *pFileInfo          = NULL;
    UINT                puLenFileInfo       = 0;

    if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
    {
        TRACE("GetModuleFileName failed with error %d\n", GetLastError());
        return false;
    }

    // get the version info for the file requested
    dwSize = GetFileVersionInfoSize( szFilename, NULL );
    if ( dwSize == 0 )
    {
        TRACE( "Error in GetFileVersionInfoSize: %d\n", GetLastError() );
        return false;
    }

    pbVersionInfo = new BYTE[ dwSize ];

    if ( !GetFileVersionInfo( szFilename, 0, dwSize, pbVersionInfo ) )
    {
        TRACE( "Error in GetFileVersionInfo: %d\n", GetLastError() );
        delete[] pbVersionInfo;
        return false;
    }

    if ( !VerQueryValue( pbVersionInfo, TEXT("\\"), (LPVOID*) &pFileInfo, &puLenFileInfo ) )
    {
        TRACE( "Error in VerQueryValue: %d\n", GetLastError() );
        delete[] pbVersionInfo;
        return false;
    }

	/* ������ ������ (4-� �������)
	strProductVersion.Format( L"%d.%d.%d.%d",
        ( pFileInfo->dwProductVersionMS >> 16 ) & 0xffff,
        ( pFileInfo->dwProductVersionMS >> 0  ) & 0xffff,
        ( pFileInfo->dwProductVersionLS >> 16 ) & 0xffff,
        ( pFileInfo->dwProductVersionLS >>  0 ) & 0xffff
        );
		*/

	// �������� ������ (2-� �������)
	strProductVersion.Format( L"%d.%d",
        ( pFileInfo->dwProductVersionMS >> 16 ) & 0xffff,
        ( pFileInfo->dwProductVersionMS >> 0  ) & 0xffff
        );

    return true;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_usedVoiseHelpers.SetWindowTextW(m_textVoice);
	CString ver;
	if (GetProductVersion(ver)) mProgramVersion.SetWindowTextW(ver);
	else mProgramVersion.SetWindowTextW(L"?.?.?");
	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VOICE_HELP, m_usedVoiseHelpers);
	DDX_Control(pDX, IDC_PROG_VER, mProgramVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// ������� ���������� ��� ������� �������
void CPlainInsteadApp::OnAppAbout()
{
	CString textVoice = L"������� ������: ";
	textVoice += MultiSpeech::getInstance().GetCurrentReader();
	CAboutDlg aboutDlg(textVoice);
	aboutDlg.DoModal();
}


// ����������� ��������� CPlainInsteadApp
void CPlainInsteadApp::OnFileOpen()
{
	//C��������� �����
	if (Tolk_IsSpeaking()) Tolk_Silence();
	for (int i = 0; i < 3; i++) //3 ������� ����������
	{
		CFileDialog fileDialog(TRUE, NULL, saveDir + L"\\1.sav", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Instead save file (*.sav)|*.sav");	//������ ������ ������ �����
		int result = fileDialog.DoModal();	//��������� ���������� ����
		if (result == IDOK)	//���� ���� ������
		{
			CString userFilePath = fileDialog.GetFolderPath();
			if (userFilePath == saveDir)
			{
				CString userFileName = saveGameNameDir + L"/" + fileDialog.GetFileName();
				InterpreterController::loadSave(userFileName);
				CPlainInsteadView::GetCurrentView()->TryInsteadCommand(L"load " + userFileName);
				CPlainInsteadView::GetCurrentView()->TryInsteadCommand(L"",L"��������");
				AfxMessageBox(L"�������������!");
				return;
			}
			else
			{
				AfxMessageBox(L"��������� ������ �����. ���������� ��������� ��� ���.");
			}
		}
		else
		{
			return;
		}
	}
}


void CPlainInsteadApp::StartNewGameFile(CString file, CString name)
{
	CIniFile mainSettings;
	int needAutoLog = mainSettings.GetInt(L"main", L"mCheckAutoLog", 0 );
	int savedVol = mainSettings.GetInt(L"main", L"mSavedVol", 80 );
	soundBeforeMute = savedVol;
	isMute = (bool)mainSettings.GetInt(L"main", L"mMuteMusic", 0);
	
	int savedEffectsVol = mainSettings.GetInt(L"main", L"mEffectsVol", 80);
	effectsBeforeMute = savedEffectsVol;
	isMuteEffects = (bool)mainSettings.GetInt(L"main", L"mMuteEffects", 0);
	if (!isMuteEffects) Wave::SetVolume(savedEffectsVol);
	else Wave::SetVolume(0);

	stopAllSound();
	if (!isMute) setGlobalSoundLevel(savedVol);
	else setGlobalSoundLevel(0);
	//��������� ��������� ��� ��������������
	mainSettings.WriteString(L"main", L"lastGameFile", file);
	mainSettings.WriteString(L"main", L"lastGameName", name);
	//���������� ���� ��� ����������
	TCHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString baseDir = buff;
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	CString gameName = file.Right(file.GetLength() - file.ReverseFind(_T('\\')) - 1);
	saveGameNameDir = L"../../saves/" + gameName;
	saveDir = baseDir + L"saves\\" + gameName;

	//closeAllChannels();
InterpreterController::startGameFile(file,name,needAutoLog);
    CPlainInsteadView::GetCurrentView()->SetOutputText(L"");

	CFrameWnd *pFrame = (CFrameWnd *)(m_pMainWnd);
	if (pFrame)
	{
		pFrame->GetActiveDocument()->SetTitle(name);
	}

	GlobalManager::getInstance().userStartGame();
	CPlainInsteadView::GetCurrentView()->TryInsteadCommand(L"", L"������ ���� "+ name);
	CPlainInsteadView::GetCurrentView()->InitFocusLogic();
	GlobalManager::lastString = 0;
}

void CPlainInsteadApp::OnFileSave()
{
	//C��������� �����
	if (Tolk_IsSpeaking()) Tolk_Silence();
	//������� ������� ��� ���������� (���� ��� ��� �� ����)
	if (GetFileAttributes(saveDir) == INVALID_FILE_ATTRIBUTES) {
		SHCreateDirectoryEx(NULL, saveDir, NULL);
		if (GetFileAttributes(saveDir) == INVALID_FILE_ATTRIBUTES) {
			AfxMessageBox(L"�� ���� ������� ���������� ��� ����������!");
		}
	}
	for (int i = 0; i < 3; i++) //3 ������� ����������
	{
		CFileDialog fileDialog(FALSE, L"sav", saveDir + L"\\1.sav", OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Instead save file (*.sav)|*.sav");	//������ ������ ������ �����
		int result = fileDialog.DoModal();	//��������� ���������� ����
		if (result == IDOK)	//���� ���� ������
		{
			CString userFilePath = fileDialog.GetFolderPath();
			if (userFilePath == saveDir)
			{
				CString userFileName = saveGameNameDir + L"/" + fileDialog.GetFileName();
				int save = CPlainInsteadView::GetCurrentView()->TryInsteadCommand(L"save " + userFileName);
								CPlainInsteadView::GetCurrentView()->TryInsteadCommand(L"",L"���������� ����");
				AfxMessageBox(save?L"�� ������� ��������� ����!":L"���������!");
				return;
			}
			else
			{
				AfxMessageBox(L"��������� ������ �����. ���������� ��������� ��� ���.");
			}
		}
		else
		{
			return;
		}
	}
	//CPlainInsteadView::GetCurrentView()->SetOutputText(L"��� ���������� ������� save <���>. ��������: save 1");
}

void CPlainInsteadApp::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: �������� ���� ��� ����������� �� ���������� ������
	//pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && fThreadRunning);
}

void CPlainInsteadApp::OnUpdateFileSaveGame(CCmdUI *pCmdUI)
{
	// TODO: �������� ���� ��� ����������� �� ���������� ������
	//pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && fThreadRunning);
}

void CPlainInsteadApp::OnViewFullHist()
{
	if (GlobalManager::getInstance().isUserStartGame())
	{
		//CString result = InterpreterController::RunInterpreter(L"START_COMMAND");
        //CPlainInsteadView::GetCurrentView()->SetOutputText(result);
	}
}

void CPlainInsteadApp::OnViewPrevHist()
{
	CPlainInsteadView::GetCurrentView()->SetOutputText(GlobalManager::getInstance().previosHistoryData());
}

void CPlainInsteadApp::OnViewNextHist()
{
	CPlainInsteadView::GetCurrentView()->SetOutputText(GlobalManager::getInstance().nextHistoryData());
}

void CPlainInsteadApp::OnUpdateViewFullHist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && (GlobalManager::getInstance().nextHistoryHave() || GlobalManager::getInstance().previosHistoryHave()) );
}

void CPlainInsteadApp::OnUpdateViewPrevHist(CCmdUI* pCmdUI) {
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && GlobalManager::getInstance().previosHistoryHave());
}



void CPlainInsteadApp::OnUpdateViewNextHist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && GlobalManager::getInstance().nextHistoryHave());
}
void CPlainInsteadApp::OnUpdateDebug(CCmdUI* pCmdUI) {
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame());
}
void CAboutDlg::OnClose()
{
	//AfxMessageBox(L"���� � ��������� �������");
	CDialog::OnClose();
}

void CAboutDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}

void CPlainInsteadApp::OnNewGameFromFile()
{
	if (!GlobalManager::getInstance().isUserSaveLastFile())
	{
		int res = MessageBox(NULL,L"��������� ������� ����?",L"���� �� ��������", MB_YESNOCANCEL);
		if (res == IDYES)
		{
			OnFileSave();
		}
		else if (res == IDCANCEL)
		{
			return;
		}
	}

	//AfxMessageBox(L"OnFileNewGame");
	if (Tolk_IsSpeaking()) Tolk_Silence();
    CFileDialog fileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Tads game (*.gam)|*.gam");	//������ ������ ������ �����
	//AfxMessageBox(L"fileDialog constr");
	int result = fileDialog.DoModal();	//��������� ���������� ����
    //AfxMessageBox(L"fileDialog modal end");
	if (result==IDOK)	//���� ���� ������
	{
		currFilePath=fileDialog.GetPathName();
		currFileName=fileDialog.GetFileName();
		//closeAllChannels();
        StartNewGameFile(fileDialog.GetPathName(),fileDialog.GetFileName());
	} 
}

void CPlainInsteadApp::OnNewGameFromLib()
{
	// TODO: �������� ���� ��� ����������� ������
	if (!GlobalManager::getInstance().isUserSaveLastFile())
	{
		int res = MessageBox(NULL,L"��������� ������� ����?",L"���� �� ��������", MB_YESNOCANCEL);
		if (res == IDYES)
		{
			OnFileSave();
		}
		else if (res == IDCANCEL)
		{
			return;
		}
	}


	bool useAutosave = false;
	CSelectNewGameDialog selNewGameDialog(currFilePath,currFileName,useAutosave);
	selNewGameDialog.DoModal();
	if (currFilePath.GetLength()>0)
	{
		StartNewGameFile(currFilePath,currFileName);
		/*
		if (useAutosave)
		{
			CString result = InterpreterController::RunInterpreter(L"LAST_SAVE");
			CPlainInsteadView::GetCurrentView()->SetOutputText(result);
		}
		*/
	}
}

void CPlainInsteadApp::OnRestartMenu()
{
	// TODO: �������� ���� ��� ����������� ������
	if (!GlobalManager::getInstance().isUserSaveLastFile())
	{
		int res = MessageBox(NULL, L"��������� ������� ����?", L"���� �� ��������", MB_YESNOCANCEL);
		if (res == IDYES)
		{
			OnFileSave();
		}
		else if (res == IDNO)
		{

		}
		else if (res == IDCANCEL)
		{
			return;
		}

		StartNewGameFile(currFilePath, currFileName);
		return;
	}
	else StartNewGameFile(currFilePath, currFileName);
}

void CPlainInsteadApp::OnUpdateRestartMenu(CCmdUI *pCmdUI)
{
	// TODO: �������� ���� ��� ����������� �� ���������� ������
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame());
}

void CPlainInsteadApp::OnEnterSetup()
{
	// TODO: �������� ���� ��� ����������� ������
	CCPCBTESTDlg dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//���������� ������� ������ � ��. ���������
		CPlainInsteadView::GetCurrentView()->UpdateSettings();
	}

}

void CPlainInsteadApp::OnResetAllSettings()
{
	// TODO: �������� ���� ��� ����������� ������
	if (MessageBox(NULL,L"�� ������� ��� ������ �������� ��� �������� ����������?",L"������������� ������ ��������", MB_YESNOCANCEL) == IDYES)
	{
		DeleteFile(L"settings.ini");
		MessageBox(NULL,L"��� ������� ������ ��������, ������������� ���������",L"���������� ������", MB_OK);
	}
}

void CPlainInsteadApp::OnAppExit()
{
	// TODO: �������� ���� ��� ����������� ������
	if (!GlobalManager::getInstance().isUserSaveLastFile())
	{
		int res = MessageBox(NULL,L"���� �� ��������. �� ����� �����?",L"���� �� ��������", MB_YESNO);
		if (res == IDYES)
		{
			GlobalManager::getInstance().isIgnoreExitDialog = true;
			instead_done();
			if (gBassInit) BASS_Free();

			CWinApp::CloseAllDocuments(FALSE);
			ASSERT(AfxGetApp()->m_pMainWnd != NULL);
			AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
		}
		else if (res == IDNO)
		{
			return;
		}
	}
	else
	{
		instead_done();
		if (gBassInit) BASS_Free();
		
		CWinApp::CloseAllDocuments(FALSE);
		ASSERT(AfxGetApp()->m_pMainWnd != NULL);
		AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
	}
}

/*
void CPlainInsteadApp::OnSysCommand(UINT nID, LPARAM lParam)
{
    if(nID == SC_CLOSE)
    {
        if(MessageBox(_T("Really"), _T("What"), MB_YESNO) == IDYES);
            //Do What you want here.
        else
            //Do something else
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}
*/

void CPlainInsteadApp::OnVolumeDown()
{
	if (getGlobalSoundLevel()>0){
		int newLevel = getGlobalSoundLevel()-10;
		soundBeforeMute = newLevel;
		CIniFile mainSettings;
		mainSettings.WriteNumber(L"main", L"mSavedVol", newLevel );
		setGlobalSoundLevel(newLevel);
	}
}

void CPlainInsteadApp::OnVolumeUp()
{
	if (getGlobalSoundLevel()<100){
		int newLevel = getGlobalSoundLevel()+10;
		soundBeforeMute = newLevel;
		CIniFile mainSettings;
		mainSettings.WriteNumber(L"main", L"mSavedVol", newLevel );
		setGlobalSoundLevel(newLevel);
	}
}

void CPlainInsteadApp::OnVolumeOff()
{
	//setGlobalSoundLevel(0);
	//isMute = true;
}

void CPlainInsteadApp::OnVolumeOn()
{
	CMenu *pMenu = m_pMainWnd->GetMenu();
	if (pMenu != NULL)
	{
		CIniFile mainSettings;
		if (isMute) {
			setGlobalSoundLevel(soundBeforeMute);
			isMute = false;
			mainSettings.WriteNumber(L"main", L"mMuteMusic", 0);
			pMenu->CheckMenuItem(ID_VOLUME_ON, MF_CHECKED | MF_BYCOMMAND);
		}
		else
		{
			setGlobalSoundLevel(0);
			isMute = true;
			mainSettings.WriteNumber(L"main", L"mMuteMusic", 1);
			pMenu->CheckMenuItem(ID_VOLUME_ON, MF_UNCHECKED | MF_BYCOMMAND);
		}
	}
}

void CPlainInsteadApp::OnUpdateVolumeUp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && !isMute && (getGlobalSoundLevel()<100));
}

void CPlainInsteadApp::OnUpdateVolumeDown(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && !isMute && (getGlobalSoundLevel()>0));
}

void CPlainInsteadApp::OnUpdateVolumeOn(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame());
}

void CPlainInsteadApp::OnUpdateVolumeOff(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && !isMute);
}

//////////////////////////// ������� �������� ���������� �������

void CPlainInsteadApp::OnListsndOn()
{
	CMenu *pMenu = m_pMainWnd->GetMenu();
	if (pMenu != NULL)
	{
		CIniFile mainSettings;
		if (isMuteEffects) {
			Wave::SetVolume(effectsBeforeMute);
			isMuteEffects = false;
			mainSettings.WriteNumber(L"main", L"mMuteEffects", 0);
			pMenu->CheckMenuItem(ID_LISTSND_ON, MF_CHECKED | MF_BYCOMMAND);
		}
		else
		{
			Wave::SetVolume(0);
			isMuteEffects = true;
			mainSettings.WriteNumber(L"main", L"mMuteEffects", 1);
			pMenu->CheckMenuItem(ID_LISTSND_ON, MF_UNCHECKED | MF_BYCOMMAND);
		}
	}
}


void CPlainInsteadApp::OnListsndDown()
{
	if (Wave::GetVolume()>0) {
		int newLevel = Wave::GetVolume() - 10;
		effectsBeforeMute = newLevel;
		CIniFile mainSettings;
		mainSettings.WriteNumber(L"main", L"mEffectsVol", newLevel);
		Wave::SetVolume(newLevel);
	}
}


void CPlainInsteadApp::OnListsndUp()
{
	if (Wave::GetVolume()<100) {
		int newLevel = Wave::GetVolume() + 10;
		effectsBeforeMute = newLevel;
		CIniFile mainSettings;
		mainSettings.WriteNumber(L"main", L"mEffectsVol", newLevel);
		Wave::SetVolume(newLevel);
	}
}


void CPlainInsteadApp::OnUpdateListsndOn(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame());
}


void CPlainInsteadApp::OnUpdateListsndDown(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && !isMuteEffects && (Wave::GetVolume()>0));
}


void CPlainInsteadApp::OnUpdateListsndUp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GlobalManager::getInstance().isUserStartGame() && !isMuteEffects && (Wave::GetVolume()<100));
}

////////////////////////////

static int DeleteDirectory(const CString &refcstrRootDirectory,
	bool              bDeleteSubdirectories = true)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
												 // subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	CString     strFilePath;                 // Filepath
	CString     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + L"\\*.*";
	hFile = ::FindFirstFile(strPattern, &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath = refcstrRootDirectory + L"\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath,
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory,
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}

void CPlainInsteadApp::OnAddGameToLib()
{
	//AfxMessageBox(L"OnFileNewGame");
	if (Tolk_IsSpeaking()) Tolk_Silence();
	CFileDialog fileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"����� � ����� (*.zip)|*.zip");	//������ ������ ������ �����
																											//AfxMessageBox(L"fileDialog constr");
	int result = fileDialog.DoModal();	//��������� ���������� ����
										//AfxMessageBox(L"fileDialog modal end");
	if (result == IDOK)	//���� ���� ������
	{

		TCHAR buff[MAX_PATH];
		::GetModuleFileName(NULL, buff, sizeof(buff));
		CString baseDir = buff;
		baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
		SetCurrentDirectory(baseDir);

		//���������� ����� zip utils
		HZIP hz = OpenZip(fileDialog.GetPathName(), 0);
		ZIPENTRY ze;
		GetZipItem(hz, -1, &ze);
		int numitems = ze.index;
		SetUnzipBaseDir(hz, L"games");
		bool have_main_lua = false;
		CString game_name;
		for (int i = 0; i < numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			if (i == 0) game_name = ze.name; //������ ����-�����, �� ������ �� �����
			if (ze.name == game_name+L"main.lua") {
				have_main_lua = true;
				break;
			}
			else if (ze.name == game_name + L"main3.lua") {
				have_main_lua = true;
				break;
			}
			//UnzipItem(hz, i, ze.name);
		}
		//������� ���� � �����
		game_name.Truncate(game_name.GetLength() - 1);
		//�������� ��� �� ���� �������
		if (!have_main_lua)
		{
			CloseZip(hz);
			AfxMessageBox(L"����� �� �������� ����� INSTEAD.");
			return;
		}
		//��������� ���� �� ����� ����� ���
		if (GetFileAttributes(L"games\\" + game_name) != INVALID_FILE_ATTRIBUTES) {
			int res = MessageBox(NULL, L"������ ���� ��� ���� � ����������. �� ������ � ��������?", L"������", MB_YESNO);
			if (res == IDYES)
			{
				DeleteDirectory(L"games\\" + game_name);
			}
			else
			{
				CloseZip(hz);
				return;
			}
		}
		//�������� ����������
		GetZipItem(hz, -1, &ze);
		numitems = ze.index;
		for (int i = 0; i < numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			UnzipItem(hz, i, ze.name);
		}

		CloseZip(hz);
		AfxMessageBox(L"���� ����������� � ����������");
	}
}


void CPlainInsteadApp::OnOpenManager()
{
	LauncherDialog dlg;
	dlg.DoModal();
	if (dlg.isWantStartGame())
	{
		currFilePath = dlg.getStartGamePath();
		currFileName = dlg.getStartGameTitle();
		StartNewGameFile(currFilePath, currFileName);
	}
}