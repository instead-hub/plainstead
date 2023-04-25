// SelectNewGameDialog.cpp: файл реализации
//

#include "stdafx.h"
#include "PlainInstead.h"
#include "SelectNewGameDialog.h"
#include "IniFile.h"


// диалоговое окно CSelectNewGameDialog

IMPLEMENT_DYNAMIC(CSelectNewGameDialog, CDialog)

CSelectNewGameDialog::CSelectNewGameDialog(CString& selGameFile, CString& selName, bool& selLastGame, CWnd* pParent /*=NULL*/)
	: CDialog(CSelectNewGameDialog::IDD, pParent),
	m_selGameFile(selGameFile),
	m_selName(selName),
	m_bSelectLastGame(selLastGame)
{

}

CSelectNewGameDialog::~CSelectNewGameDialog()
{
}

void CSelectNewGameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GAMES, m_ListGames);
	DDX_Control(pDX, IDC_EDIT_ABOUT, m_GamesDescr);
}


BEGIN_MESSAGE_MAP(CSelectNewGameDialog, CDialog)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_LIST_GAMES, &CSelectNewGameDialog::OnLbnSelchangeListGames)
	ON_BN_CLICKED(IDOK, &CSelectNewGameDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_START_LAST_GAME, &CSelectNewGameDialog::OnBnClickedButtonStartLastGame)
END_MESSAGE_MAP()


// обработчики сообщений CSelectNewGameDialog

void CSelectNewGameDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// Автомасштабирование компонентов
	const int contHeight = 20;
	if (m_ListGames.m_hWnd) m_ListGames.SetWindowPos(NULL, 0, contHeight, cx, (cy-contHeight*2)/2, SWP_NOACTIVATE | SWP_NOZORDER);
    if (m_GamesDescr.m_hWnd) m_GamesDescr.SetWindowPos(NULL, 0, (cy-contHeight*2)/2, cx, cy/2-contHeight, SWP_NOACTIVATE | SWP_NOZORDER);
}

static void ListFilesGamInDirectory(LPCTSTR dirName,std::vector<CString> & filepaths )
{
	// Check input parameters
	ASSERT( dirName != NULL );
	// Clear filename list
	filepaths.clear();
	// Object to enumerate files
	CFileFind finder;
	// Build a string using wildcards *.*,
	// to enumerate content of a directory
	CString wildcard( dirName );
	wildcard += _T("\\*.*");
	// Init the file finding job
	BOOL working = finder.FindFile( wildcard );
	// For each file that is found:
	while ( working )
	{
		// Update finder status with new file
		working = finder.FindNextFile();
		// Skip '.' and '..'
		if ( finder.IsDots() )
		{
		continue;
		}
		// Skip sub-directories
		if ( finder.IsDirectory() )
		{
			// Add file path to container
			filepaths.push_back(finder.GetFileName());
			continue;
		}
	}
	// Cleanup file finder
	finder.Close();
}


BOOL CSelectNewGameDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::vector< CString > filePaths;
	//GetCurrentDirectory( MAX_PATH, CStrBuf(dir, MAX_PATH) );
	TCHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(NULL, buff, std::size(buff));
	baseDir = buff;
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	CString dir = baseDir + L"\\games";
	ListFilesGamInDirectory( dir, filePaths );
	//Обновление поля выбора игр
	CIniFile iniFile(baseDir+L"\\games\\info.ini", 1024);
	for ( size_t i = 0; i < filePaths.size(); i++ )
	{
		CString strName;
		iniFile.GetString(L"game_name", filePaths[i], strName, L"");
		if (strName.GetLength()>0)
		{
			m_ListGames.AddString(strName);
			CString strDescr;
			iniFile.GetString(L"game_desc", filePaths[i], strDescr, L"Нет данных");
			fileNameDescr.push_back(std::make_pair(filePaths[i],strDescr));
			if (i==0)
			{
				m_GamesDescr.SetWindowTextW(strDescr);
			}

		}
		else
		{
			m_ListGames.AddString(filePaths[i]);
			fileNameDescr.push_back(std::make_pair(filePaths[i],L"Нет данных"));
		}
	}

	m_ListGames.SetCurSel(0);
	m_ListGames.SetFocus();

	return FALSE;   // Возвратить TRUE, если Вы не устанавливаете фокус ввода к элементу управления
}

void CSelectNewGameDialog::OnLbnSelchangeListGames()
{
	// TODO: добавьте свой код обработчика уведомлений
	if (m_ListGames.GetCurSel()<fileNameDescr.size())
		m_GamesDescr.SetWindowTextW(fileNameDescr[m_ListGames.GetCurSel()].second);
	else
		m_GamesDescr.SetWindowTextW(L"");
}

void CSelectNewGameDialog::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	if (m_ListGames.GetCurSel() >= 0 && m_ListGames.GetCurSel()<fileNameDescr.size())
	{
		m_selGameFile = baseDir + L"games\\";
		m_selGameFile.Append(fileNameDescr[m_ListGames.GetCurSel()].first);
		m_ListGames.GetText(m_ListGames.GetCurSel(),m_selName);
		m_bSelectLastGame = false;
	}
	else
	{
		m_selGameFile = L"";
		m_selName = L"";
	}
	OnOK();
}

void CSelectNewGameDialog::OnBnClickedButtonStartLastGame()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_bSelectLastGame = true;
	CIniFile mainSettings;
	mainSettings.GetString(L"main", L"lastGameFile", m_selGameFile, L"");
	mainSettings.GetString(L"main", L"lastGameName", m_selName, L"");
	OnOK();
}
