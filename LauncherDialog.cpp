// LauncherDialog.cpp: файл реализации
//

#include "stdafx.h"
#include "PlainInstead.h"
#include "LauncherDialog.h"
#include "afxdialogex.h"
#include "afxinet.h"
#include <vector>
#include <regex>
#include <codecvt>
#include "StdioFileEx.h"
#include "Markup.h"
#include "urlfileDlg.h"
#include "IniFile.h"

// диалоговое окно LauncherDialog

IMPLEMENT_DYNAMIC(LauncherDialog, CDialog)
//виды табов
#define ID_PAGE_INSTALLED 0 //установленные игры
#define ID_PAGE_NEW       1 //новые (из репозитория)
//Выбранный фильт
#define SEL_FILTER_ALL           0 //все игры
#define SEL_FILTER_VALID         1 //доступные
#define SEL_FILTER_VALID_AND_UNK 2 //доступные и непроверенные

#define N_SUBITEM_LIST_INSTALLED_CAPTION 0 //название
#define N_SUBITEM_LIST_INSTALLED_ACCESSABLE 1 //доступна
#define N_SUBITEM_LIST_INSTALLED_VERSION    2 //версия
#define N_SUBITEM_LIST_INSTALLED_DESC       3 //кратко
#define N_SUBITEM_LIST_INSTALLED_DATE       4 //кратко
#define N_SUBITEM_LIST_INSTALLED_GNAME      5 //игровое имя

#define N_SUBITEM_LIST_NEW_CAPTION 0 //название
#define N_SUBITEM_LIST_NEW_ACCESSABLE 1 //доступна
#define N_SUBITEM_LIST_NEW_VERSION    2 //версия
#define N_SUBITEM_LIST_NEW_SIZE       3 //размер
#define N_SUBITEM_LIST_NEW_DESC       4 //кратко
#define N_SUBITEM_LIST_NEW_DATE       5 //Дата публикации
#define N_SUBITEM_LIST_NEW_URL        6 //ссылка URL
#define N_SUBITEM_LIST_NEW_GNAME      7 //игровое имя
#define N_SUBITEM_LIST_NEW_DWN_URL    8 //ссылка для прямой загрузки
#define N_SUBITEM_LIST_NEW_IS_SANDER  9 //является ли игра из песочницы


LauncherDialog::LauncherDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LAUNCHERDIALOG, pParent),
	m_sortInstalledUp(true),
	m_sortNewUp(true)
{
	m_sortInstalledLastItem = -1;
	m_sortNewLastItem = -1;
}

LauncherDialog::~LauncherDialog()
{
}

void LauncherDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_LIST_INSTALLED, m_listInstalled);
	DDX_Control(pDX, IDC_LIST_NEW, m_listNew);
	DDX_Control(pDX, IDC_BTN_DEL_GAME, m_btnDelete);
	DDX_Control(pDX, IDC_BTN_UPDATE, m_btnUpdate);
	DDX_Control(pDX, IDC_BTN_INSTALL, m_btnInstall);
	DDX_Control(pDX, IDC_BTN_OPEN_LINK, m_btnOpenLink);
	DDX_Control(pDX, IDC_BTN_PLAY_GAMEM, m_btnPlayGame);
	DDX_Control(pDX, IDC_BTN_RESUMEOLD_GAME2, m_btnResumeGame);
	DDX_Control(pDX, IDC_COMBO_FILTER, m_comboFiler);
	DDX_Control(pDX, IDC_CHECK_SANDER, m_CheckSander);
}


BEGIN_MESSAGE_MAP(LauncherDialog, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &LauncherDialog::OnTcnSelchangeTab1)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB1, &LauncherDialog::OnTcnSelchangingTab1)
	ON_BN_CLICKED(IDC_BTN_DEL_GAME, &LauncherDialog::OnBnClickedBtnDelGame)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &LauncherDialog::OnBnClickedBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_OPEN_LINK, &LauncherDialog::OnBnClickedBtnOpenLink)
	ON_BN_CLICKED(IDC_BTN_INSTALL, &LauncherDialog::OnBnClickedBtnInstall)
	ON_BN_CLICKED(IDC_BTN_PLAY_GAMEM, &LauncherDialog::OnBnClickedBtnPlayGamem)
	ON_BN_CLICKED(IDC_BTN_RESUMEOLD_GAME2, &LauncherDialog::OnBnClickedBtnResumeoldGame2)
	ON_CBN_SELCHANGE(IDC_COMBO_FILTER, &LauncherDialog::OnCbnSelchangeComboFilter)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &LauncherDialog::OnHdnItemclickListInstalled)
END_MESSAGE_MAP()


static void ListFilesGamInDirectory(LPCTSTR dirName, std::vector<std::pair<CString/*full path*/, CString/*name*/> > & filepaths)
{
	// Check input parameters
	ASSERT(dirName != NULL);
	// Clear filename list
	filepaths.clear();
	// Object to enumerate files
	CFileFind finder;
	// Build a string using wildcards *.*,
	// to enumerate content of a directory
	CString wildcard(dirName);
	wildcard += _T("\\*.*");
	// Init the file finding job
	BOOL working = finder.FindFile(wildcard);
	// For each file that is found:
	while (working)
	{
		// Update finder status with new file
		working = finder.FindNextFile();
		// Skip '.' and '..'
		if (finder.IsDots())
		{
			continue;
		}
		// Skip sub-directories
		if (finder.IsDirectory())
		{
			// Add file path to container
			filepaths.push_back(std::make_pair(finder.GetFilePath(), finder.GetFileName()) );
			continue;
		}
	}
	// Cleanup file finder
	finder.Close();
}

static std::wstring utf8_to_wstring(std::wstring& str)
{
	std::string s(str.begin(), str.end());
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> myconv;
	return myconv.from_bytes(s);
}

//получить имя и версию из LUA-файла
static CString get_game_name_ru(std::wstring inp) //входной текст
{
	//Обработка отображения объектов сцены
	const std::wregex regex_name(L"\\$\\s*Name\\s*\\(\\s*ru\\s*\\)\\s*:([^\\$]*)\\$");
	std::wsregex_iterator next(inp.begin(), inp.end(), regex_name);
	std::wsregex_iterator end;
	while (next != end) {
		std::wsmatch match = *next;
		if (match.size() == 2)
		{
			CString nameStr(match[1].str().data());
			nameStr = nameStr.Trim(); //триммируем
			return nameStr;
		}
		next++;
	}
	return CString();
}

static CString get_game_name_en(std::wstring inp)
{
	//Обработка отображения объектов сцены
	const std::wregex regex_name(L"\\s*Name\\s*:([^\\$]*)\\$");
	std::wsregex_iterator next(inp.begin(), inp.end(), regex_name);
	std::wsregex_iterator end;
	while (next != end) {
		std::wsmatch match = *next;
		if (match.size() == 2)
		{
			CString nameStr(match[1].str().data());
			nameStr = nameStr.Trim(); //триммируем
			return nameStr;
		}
		next++;
	}
	return CString();
}

static CString get_game_version(std::wstring inp) //входной текст
{
	//Обработка отображения объектов сцены
	const std::wregex regex_name(L"\\$\\s*Version\\s*:([^\\$]*)\\$");
	std::wsregex_iterator next(inp.begin(), inp.end(), regex_name);
	std::wsregex_iterator end;
	while (next != end) {
		std::wsmatch match = *next;
		if (match.size() == 2)
		{
			CString nameStr(match[1].str().data());
			nameStr = nameStr.Trim(); //триммируем
			return nameStr;
		}
		next++;
	}
	return CString();
}

// обработчики сообщений LauncherDialog


BOOL LauncherDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetCurrentDirectory(MAX_PATH, CStrBuf(currDir, MAX_PATH));
	TCHAR buff[MAX_PATH];
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString baseDir = buff;
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	SetCurrentDirectory(baseDir);

	m_wantPlay = false;

	// TODO:  Добавить дополнительную инициализацию
	TC_ITEM TabItem;
	TabItem.mask = TCIF_TEXT;
	TabItem.pszText = L"Установленные игры";
	m_tab.InsertItem(0, &TabItem);
	TabItem.pszText = L"Репозиторий игр";
	m_tab.InsertItem(1, &TabItem);
	m_comboFiler.AddString(L"Все игры");
	m_comboFiler.AddString(L"Только доступные");
	m_comboFiler.AddString(L"Только доступные и непроверенные");

	CIniFile mainSettings;
	m_lastSelFilter = mainSettings.GetInt(L"main", L"mRepoFilter", SEL_FILTER_VALID);
	if (m_lastSelFilter > (m_comboFiler.GetCount() - 1)) m_lastSelFilter = SEL_FILTER_VALID;
	m_comboFiler.SetCurSel(m_lastSelFilter);

	showInstalledTabControls();

	//Set the style to listControl
	ListView_SetExtendedListViewStyle(::GetDlgItem(m_hWnd, IDC_LIST_INSTALLED), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	ListView_SetExtendedListViewStyle(::GetDlgItem(m_hWnd, IDC_LIST_NEW), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CreateColumns();

	//Обновляем список доступных игр
	UpdateApprovedFromFile();
	//Обновляем доп инфу, если есть xml
	if (PathFileExists(L"rss1.xml")) ReadAdditionalInfoFromXMLRss(L"rss1.xml");
	if (PathFileExists(L"rss2.xml")) ReadAdditionalInfoFromXMLRss(L"rss2.xml");

	RescanInstalled();

	//m_tab.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}

void LauncherDialog::RescanInstalled()
{
	m_listInstalled.DeleteAllItems();
	TCHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString baseDir = buff;
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	CString dir = baseDir + L"\\games";
	m_gameBaseDir = dir;
	std::vector<std::pair<CString, CString> > filePathsAndNames;
	installedGameNameCache.clear();
	ListFilesGamInDirectory(dir, filePathsAndNames);

	for (int i = 0; i < filePathsAndNames.size(); i++)
	{
		CString string;
		CStdioFileEx gameFile;
		bool have_file = false;
		if (PathFileExists(filePathsAndNames[i].first + L"\\main.lua"))
		{
			gameFile.Open(filePathsAndNames[i].first + L"\\main.lua", CFile::modeRead);
			have_file = true;
		}
		else if (PathFileExists(filePathsAndNames[i].first + L"\\main3.lua"))
		{
			gameFile.Open(filePathsAndNames[i].first + L"\\main3.lua", CFile::modeRead);
			have_file = true;
		}

		if (have_file)
		{
			gameFile.SetCodePage(CP_UTF8);
			CString game_name;
			CString game_name_en;
			CString game_version;
			const int MAX_STR_CNT = 30; //не больше этого количества строк от начала
			int curr_str = 0;
			while (gameFile.ReadString(string))
			{
				if (game_name.IsEmpty()) game_name = get_game_name_ru(string.GetBuffer());
				if (game_name_en.IsEmpty()) game_name_en = get_game_name_en(string.GetBuffer());
				if (game_version.IsEmpty()) game_version = get_game_version(string.GetBuffer());

				if (!game_name.IsEmpty() && !game_version.IsEmpty())
				{
					AddInstalledGame(game_name, game_version, filePathsAndNames[i]);
					break;
				}
				//Обычно в заголовках только версия и название
				curr_str++;
				if (curr_str > MAX_STR_CNT) break;
			}

			//Не нашли всей информации
			if (game_name.IsEmpty() && !game_name_en.IsEmpty() && !game_version.IsEmpty()) //Если только англ. имя
			{
				AddInstalledGame(game_name_en, game_version, filePathsAndNames[i]);
			}
			else if (!game_name.IsEmpty() && game_version.IsEmpty()) //Если нет номера версии
			{
				AddInstalledGame(game_name, L"", filePathsAndNames[i]);
			}
			else if (!game_name_en.IsEmpty() && game_version.IsEmpty()) //Если нет номера версии и только англ
			{
				AddInstalledGame(game_name_en, L"", filePathsAndNames[i]);
			}
		}
	}
}


void LauncherDialog::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: добавьте свой код обработчика уведомлений
	int nTab = m_tab.GetCurSel();
	
	if (nTab == ID_PAGE_INSTALLED) {
		showInstalledTabControls();
	}
	else
	{
		showNewTabControls();
	}

	*pResult = 0;
}

void LauncherDialog::showInstalledTabControls()
{
	m_listInstalled.ShowWindow(SW_SHOW);
	m_btnDelete.ShowWindow(SW_SHOW);
	m_btnPlayGame.ShowWindow(SW_SHOW);
	m_btnResumeGame.ShowWindow(SW_SHOW);

	m_listNew.ShowWindow(SW_HIDE);
	m_btnUpdate.ShowWindow(SW_HIDE);
	m_btnInstall.ShowWindow(SW_HIDE);
	m_btnOpenLink.ShowWindow(SW_HIDE);
	m_comboFiler.ShowWindow(SW_HIDE);
	m_CheckSander.ShowWindow(SW_HIDE);

	m_listInstalled.SetFocus();
}

void LauncherDialog::showNewTabControls()
{
	m_listInstalled.ShowWindow(SW_HIDE);
	m_btnDelete.ShowWindow(SW_HIDE);
	m_btnPlayGame.ShowWindow(SW_HIDE);
	m_btnResumeGame.ShowWindow(SW_HIDE);

	m_listNew.ShowWindow(SW_SHOW);
	m_btnUpdate.ShowWindow(SW_SHOW);
	m_btnInstall.ShowWindow(SW_SHOW);
	m_btnOpenLink.ShowWindow(SW_SHOW);
	m_comboFiler.ShowWindow(SW_SHOW);
	m_CheckSander.ShowWindow(SW_SHOW);

	m_listNew.SetFocus();
}

struct PARAMSORT
{
	PARAMSORT(HWND hWnd, int columnIndex, bool ascending)
	:m_hWnd(hWnd)
	,m_ColumnIndex(columnIndex)
	,m_Ascending(ascending)
	{}

	HWND m_hWnd;
	int  m_ColumnIndex;
	bool m_Ascending;
};

// Comparison extracts values from the List-Control
int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	PARAMSORT& ps = *(PARAMSORT*)lParamSort;

	TCHAR left[256] = _T(""), right[256] = _T("");
	ListView_GetItemText(ps.m_hWnd, lParam1,
		ps.m_ColumnIndex, left, sizeof(left));
	ListView_GetItemText(ps.m_hWnd, lParam2,
		ps.m_ColumnIndex, right, sizeof(right));

	if (ps.m_Ascending)
		return _tcscmp(left, right);
	else
		return _tcscmp(right, left);
}

void LauncherDialog::SortColumn(CListCtrl* ctrl,int columnIndex, bool ascending)
{
	PARAMSORT paramsort(ctrl->m_hWnd, columnIndex, ascending);
	ctrl->SortItemsEx(SortFunc, (DWORD_PTR)&paramsort);
}


// This function inserts the default values into the listControl
void LauncherDialog::CreateColumns()
{
	// Set the LVCOLUMN structure with the required column information
	LVCOLUMN list;
	list.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;
	list.cx = 320;
	list.pszText = L"Название";
	list.iSubItem = N_SUBITEM_LIST_INSTALLED_CAPTION;
	//Inserts the column
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_INSTALLED_CAPTION, &list);

	list.cx = 100;
	list.pszText = L"Доступна";
	list.iSubItem = N_SUBITEM_LIST_INSTALLED_ACCESSABLE;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_INSTALLED_ACCESSABLE, &list);

	list.cx = 100;
	list.pszText = L"Версия";
	list.iSubItem = N_SUBITEM_LIST_INSTALLED_VERSION;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_INSTALLED_VERSION, &list);

	list.cx = 200;
	list.pszText = L"Кратко";
	list.iSubItem = N_SUBITEM_LIST_INSTALLED_DESC;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_INSTALLED_DESC, &list);

	list.cx = 70;
	list.pszText = L"Дата";
	list.iSubItem = N_SUBITEM_LIST_INSTALLED_DATE;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_INSTALLED_DATE, &list);

	list.cx = 0;
	list.pszText = L"Игровое имя";
	list.iSubItem = N_SUBITEM_LIST_INSTALLED_GNAME;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_INSTALLED_GNAME, &list);

	/////// Для новых игр
	list.cx = 270;
	list.pszText = L"Название";
	list.iSubItem = N_SUBITEM_LIST_NEW_CAPTION;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_CAPTION, &list);

	list.cx = 50;
	list.pszText = L"Доступна";
	list.iSubItem = N_SUBITEM_LIST_NEW_ACCESSABLE;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_ACCESSABLE, &list);

	list.cx = 50;
	list.pszText = L"Версия";
	list.iSubItem = N_SUBITEM_LIST_NEW_VERSION;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_VERSION, &list);

	list.cx = 50;
	list.pszText = L"Размер";
	list.iSubItem = N_SUBITEM_LIST_NEW_SIZE;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_SIZE, &list);

	list.cx = 300;
	list.pszText = L"Кратко";
	list.iSubItem = N_SUBITEM_LIST_NEW_DESC;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_DESC, &list);

	list.cx = 70;
	list.pszText = L"Дата";
	list.iSubItem = N_SUBITEM_LIST_NEW_DATE;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_DATE, &list);

	list.cx = 0;
	list.pszText = L"URL";
	list.iSubItem = N_SUBITEM_LIST_NEW_URL;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_URL, &list);

	list.cx = 0;
	list.pszText = L"Игровое имя";
	list.iSubItem = N_SUBITEM_LIST_NEW_GNAME;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_GNAME, &list);

	list.cx = 0;
	list.pszText = L"Ссылка на скачивание";
	list.iSubItem = N_SUBITEM_LIST_NEW_DWN_URL;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_DWN_URL, &list);

	list.cx = 0;
	list.pszText = L"Ссылка на скачивание";
	list.iSubItem = N_SUBITEM_LIST_NEW_IS_SANDER;
	m_listNew.InsertColumn(N_SUBITEM_LIST_NEW_IS_SANDER, &list);
}

void LauncherDialog::AddInstalledGame(CString name, CString version, std::pair<CString, CString> path)
{
	CString mark = L"Неизвестно";
	CString info = L"";
	CString date = L"";
	if (approveInfo.count(path.second)) {
		mark = approveInfo[path.second].first;
		info = approveInfo[path.second].second;
	}
	if (rssInfo.count(name)) {
		date = rssInfo[name].first;
		info.Append(rssInfo[name].second);
	}

	int cnt = m_listInstalled.GetItemCount();
	int col = 0;
	SetCell(m_listInstalled, name, cnt, col++);
	SetCell(m_listInstalled, mark, cnt, col++);
	SetCell(m_listInstalled, version, cnt, col++);
	SetCell(m_listInstalled, info, cnt, col++);
	SetCell(m_listInstalled, date, cnt, col++);
	SetCell(m_listInstalled, path.second, cnt, col++);

	installedGameNameCache.insert(path.second);
}

void LauncherDialog::AddNewGame(CString name, CString version, CString sz, CString page, std::pair<CString, CString> downloadPageAndInstallName)
{
	CString mark = L"Неизвестно";
	CString info = L"";
	CString date = L"";
	if (approveInfo.count(downloadPageAndInstallName.second)) {
		mark = approveInfo[downloadPageAndInstallName.second].first;
		info = approveInfo[downloadPageAndInstallName.second].second;
	}
	if (rssInfo.count(name)) {
		date = rssInfo[name].first;
		info.Append(rssInfo[name].second);
	}

	int cnt = m_listNew.GetItemCount();
	int col = 0;
	SetCell(m_listNew, name, cnt, col++);
	SetCell(m_listNew, mark, cnt, col++);
	SetCell(m_listNew, version, cnt, col++);
	SetCell(m_listNew, sz, cnt, col++);
	SetCell(m_listNew, info, cnt, col++);
	SetCell(m_listNew, date, cnt, col++);
	SetCell(m_listNew, page, cnt, col++);
	SetCell(m_listNew, downloadPageAndInstallName.second, cnt, col++);
	SetCell(m_listNew, downloadPageAndInstallName.first, cnt, col++);

	//networkGameDWPageAndName.push_back(downloadPageAndInstallName);
	//networkGameName.insert(downloadPageAndInstallName.second);
}

// This function set the text in the specified SubItem depending on the Row and Column values
void LauncherDialog::SetCell(CListCtrl& ctrl, CString value, int nRow, int nCol)
{
	if (nCol > 0)
	{
		//set the value of listItem
		ctrl.SetItemText(nRow, nCol, value);
	}
	else
	{
		//Fill the LVITEM structure with the values given as parameters.
		TCHAR     szString[256];
		wsprintf(szString, value, 0);
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = nRow;
		lvItem.pszText = szString;
		lvItem.iSubItem = nCol;
		ctrl.InsertItem(&lvItem);
	}
}


void LauncherDialog::OnTcnSelchangingTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: добавьте свой код обработчика уведомлений
	*pResult = 0;
}


BOOL LauncherDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: добавьте специализированный код или вызов базового класса
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_DELETE &&
		(m_tab.GetCurSel() == ID_PAGE_INSTALLED) &&
		(GetFocus() == &m_listInstalled) &&
		(m_listInstalled.GetSelectedCount()>0)
		)
	{
		OnBnClickedBtnDelGame();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_F5 &&
		(m_tab.GetCurSel() == ID_PAGE_NEW) )
	{
		OnBnClickedBtnUpdate();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == 'V' &&
		(m_tab.GetCurSel() == ID_PAGE_NEW) &&
		(GetFocus() == &m_listNew) &&
		(m_listNew.GetSelectedCount()>0)
		)
	{
		OnBnClickedBtnOpenLink();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		(m_tab.GetCurSel() == ID_PAGE_NEW) &&
		(GetFocus() == &m_listNew) &&
		(m_listNew.GetSelectedCount()>0)
		)
	{
		OnBnClickedBtnInstall();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		(m_tab.GetCurSel() == ID_PAGE_INSTALLED) &&
		(GetFocus() == &m_listInstalled) &&
		(m_listInstalled.GetSelectedCount()>0)
		)
	{
		OnBnClickedBtnPlayGamem();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_F3 &&
		(m_tab.GetCurSel() == ID_PAGE_INSTALLED)
		)
	{
		OnBnClickedBtnResumeoldGame2();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		(::GetKeyState(VK_CONTROL) < 0) &&
		pMsg->wParam == '2' &&
		(m_tab.GetCurSel() == ID_PAGE_INSTALLED)
		)
	{
		m_tab.SetCurSel(ID_PAGE_NEW);
		showNewTabControls();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		(::GetKeyState(VK_CONTROL) < 0) &&
		pMsg->wParam == '1' &&
		(m_tab.GetCurSel() == ID_PAGE_NEW)
		)
	{
		m_tab.SetCurSel(ID_PAGE_INSTALLED);
		showInstalledTabControls();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void LauncherDialog::OnOK()
{
	// TODO: добавьте специализированный код или вызов базового класса

	//CDialog::OnOK();
}

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

void LauncherDialog::OnBnClickedBtnDelGame()
{
	int sel = m_listInstalled.GetSelectionMark();
	if (sel == -1)
	{
		AfxMessageBox(L"Ни одного элемента не выбрано!");
		return;
	}

	CString gameName = m_listInstalled.GetItemText(sel, N_SUBITEM_LIST_INSTALLED_GNAME);
	if (gameName == L"mirror" || gameName == L"tutorial3") {
		AfxMessageBox(L"Выбранная игра входит в поставку плеера, и отличается от той что в репозитории. Операция отменена.");
		return;
	}
	CString gameTitle = m_listInstalled.GetItemText(sel, 0);
	//Выдаём подтверждение для удаления игры
	int want_del = AfxMessageBox(L"Вы действительно хотите удалить игру "+ gameTitle+L"?", MB_YESNOCANCEL | MB_ICONQUESTION);
	if (want_del == IDYES)
	{		
		DeleteDirectory(m_gameBaseDir + L"\\" + gameName);
		AfxMessageBox(L"Игра удалена");
		RescanInstalled();
	}
}

void LauncherDialog::ClearNewList()
{
	m_listNew.DeleteAllItems();
}

void LauncherDialog::OnBnClickedBtnUpdate()
{
	ClearNewList();

	bool ok_appr = UpdateApprovedGamesFromUrl(L"http://dialas.ru/instead_games_approved.xml",
							   L"games\\instead_games_approved.xml");
	if (ok_appr) UpdateApprovedFromFile();

	//Обновляем доп. информацию из rss для списка из репозитория
	rssInfo.clear();
	CString strURL_RSS1 = L"http://dialas.ru/rss_game_list.xml";
	UpdateNewGamesRssAdditionalInfoFromUrl(strURL_RSS1, L"rss1.xml");
	CString strURL_RSS2 = L"http://dialas.ru/rss_game_list_sander.xml";
	UpdateNewGamesRssAdditionalInfoFromUrl(strURL_RSS2, L"rss2.xml");

	/*CString strURL = L"http://instead.sf.net/pool/game_list.xml";
	UpdateNewGamesFromUrl(strURL, L"temp.xml");*/
	CString strURL2 = L"http://dialas.ru/instead_game_list.xml";
	UpdateNewGamesFromUrl(strURL2, L"temp2.xml");
	CString strURL3 = L"http://dialas.ru/instead_game_list_sander.xml";
	UpdateNewGamesFromUrl(strURL3, L"temp3.xml",true);
}

void LauncherDialog::UpdateApprovedFromFile()
{
	//Читаем xml и разбираем
	CStdioFileEx gameFile(L"games\\instead_games_approved.xml", CFile::modeRead);
	gameFile.SetCodePage(CP_UTF8);
	CString xmlDoc;
	CString str;
	while (gameFile.ReadString(str)) xmlDoc.Append(str);

	approveInfo.clear();
	CMarkup xml;
	xml.SetDoc(xmlDoc);
	while (xml.FindChildElem(L"game"))
	{
		xml.IntoElem();
		//ВНИМАНИЕ: считываем по порядку заданному в файле xml
		xml.FindChildElem(L"name");
		CString csName = xml.GetChildData();
		xml.FindChildElem(L"approve");
		CString csApproveMark = xml.GetChildData();
		xml.FindChildElem(L"info");
		CString csInfo = xml.GetChildData();
		
		approveInfo[csName] = std::make_pair(csApproveMark, csInfo);

		xml.OutOfElem();
	}
}

bool LauncherDialog::UpdateApprovedGamesFromUrl(CString url, CString res_path)
{
	CInternetSession session;
	CHttpFile *pFile = (CHttpFile *)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);

	// Determine file size:
	DWORD dwBytesInFile = (DWORD)pFile->Seek(0, FILE_END);
	pFile->Seek(0, FILE_BEGIN);	// reposition file pointer at the start
	if (dwBytesInFile > 0)
	{
		CString stLine;
		char buf[2000];
		int numread;
		CFile xmlWithGames(res_path,
			CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		while ((numread = pFile->Read(buf, sizeof(buf) - 1)) > 0)
		{
			buf[numread] = '\0';
			xmlWithGames.Write(buf, numread);
		}
		xmlWithGames.Close();
		return true;
	}
	return false;
}

void LauncherDialog::UpdateNewGamesFromUrl(CString url, CString temp_xmlfile, bool is_sander)
{
	// TODO: добавьте свой код обработчика уведомлений
	CInternetSession session;
	CHttpFile *pFile = (CHttpFile *)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);

	// Determine file size:
	DWORD dwBytesInFile = (DWORD)pFile->Seek(0, FILE_END);
	pFile->Seek(0, FILE_BEGIN);	// reposition file pointer at the start
	if (dwBytesInFile > 0)
	{
		CString stLine;
		char buf[2000];
		int numread;
		CFile xmlWithGames(temp_xmlfile,
			CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		while ((numread = pFile->Read(buf, sizeof(buf) - 1)) > 0)
		{
			buf[numread] = '\0';
			xmlWithGames.Write(buf, numread);
			//pWebFileDlg->m_ctrlProgress.StepIt();
			//PeekAndPump();
		}
		xmlWithGames.Close();

		ReadNewGamesFromXMLAndAdd(temp_xmlfile, is_sander);
	}
}

void LauncherDialog::UpdateNewGamesRssAdditionalInfoFromUrl(CString url, CString temp_xmlfile)
{
	// TODO: добавьте свой код обработчика уведомлений
	CInternetSession session;
	CHttpFile *pFile = (CHttpFile *)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);

	// Determine file size:
	DWORD dwBytesInFile = (DWORD)pFile->Seek(0, FILE_END);
	pFile->Seek(0, FILE_BEGIN);	// reposition file pointer at the start
	if (dwBytesInFile > 0)
	{
		CString stLine;
		char buf[2000];
		int numread;
		CFile xmlWithGames(temp_xmlfile, CFile::modeCreate | CFile::modeWrite | CStdioFileEx::modeWriteUnicode);
		while ((numread = pFile->Read(buf, sizeof(buf) - 1)) > 0)
		{
			buf[numread] = '\0';
			xmlWithGames.Write(buf, numread);
			//pWebFileDlg->m_ctrlProgress.StepIt();
			//PeekAndPump();
		}
		xmlWithGames.Close();

		ReadAdditionalInfoFromXMLRss(temp_xmlfile);
	}
}

void LauncherDialog::ReadNewGamesFromXMLAndAdd(CString temp_xmlfile, bool is_sander)
{
	//не выбрана галочка песочница, ничего не добавляем
	if (is_sander && m_CheckSander.GetCheck() == BST_UNCHECKED) return;
	//Читаем xml и разбираем
	CStdioFileEx gameFile(temp_xmlfile, CFile::modeRead);
	gameFile.SetCodePage(CP_UTF8);
	CString xmlDoc;
	CString str;
	while (gameFile.ReadString(str)) xmlDoc.Append(str);

	CMarkup xml;
	xml.SetDoc(xmlDoc);
	while (xml.FindChildElem(L"game"))
	{
		xml.IntoElem();
		//ВНИМАНИЕ: считываем по порядку заданному в файле xml
		xml.FindChildElem(L"name");
		CString csSN = xml.GetChildData();
		xml.FindChildElem(L"title");
		CString csTitle = xml.GetChildData();
		xml.FindChildElem(L"version");
		CString csVersion = xml.GetChildData();
		xml.FindChildElem(L"url");
		CString csDownloadUrl = xml.GetChildData();
		xml.FindChildElem(L"size");
		float csSizeBytes = _wtof(xml.GetChildData());
		xml.FindChildElem(L"lang");
		CString csLang = xml.GetChildData();
		xml.FindChildElem(L"descurl");
		CString csDescUrl = xml.GetChildData();

		//Начинаем фильтровать
		CString approved_mark;
		if (approveInfo.count(csSN)) approved_mark = approveInfo[csSN].first;
		bool ok_filter = ((m_lastSelFilter == SEL_FILTER_ALL) ||
			( (m_lastSelFilter == SEL_FILTER_VALID) && approved_mark==L"Да" ) ||
			((m_lastSelFilter == SEL_FILTER_VALID_AND_UNK) && approved_mark != L"Нет")
			);

		//Добавляем русскоязычную игру в новые, если её нет в существующих
		if ((csLang == L"ru") && 
			(installedGameNameCache.count(csSN) == 0) &&
			ok_filter
			)
		{
			CString megabytes_num;
			megabytes_num.Format(L"%.1f МБ. ", csSizeBytes / (1024.0f*1024.0f));
			AddNewGame(csTitle, csVersion, megabytes_num, csDescUrl, std::make_pair(csDownloadUrl, csSN));
		}

		xml.OutOfElem();
	}
}

void LauncherDialog::ReadAdditionalInfoFromXMLRss(CString temp_xmlfile)
{
	//Читаем xml и разбираем
	CStdioFileEx gameFile(temp_xmlfile, CFile::modeRead | CFile::typeText);
	gameFile.SetCodePage(CP_UTF8);
	CString xmlDoc;
	CString str;
	bool firstStr = true;
	while (gameFile.ReadString(str)) {
		//FIXE: костыль из-за того что неправильно читает xml и пропадает начало тега в первой строчке
		if (firstStr)
		{
			str = L"<?x" + str;
			firstStr = false;
		}
		xmlDoc.Append(str);
	}

	CMarkup xml;
	xml.SetDoc(xmlDoc);

	if (xml.FindChildElem(L"channel"))
	{
		xml.IntoElem();
		while (xml.FindChildElem(L"item"))
		{
			xml.IntoElem();
			//ВНИМАНИЕ: считываем по порядку заданному в файле xml
			xml.FindChildElem(L"title");
			CString csTitle = xml.GetChildData();
			xml.FindChildElem(L"description");
			CString csDescription = xml.GetChildData();
			xml.FindChildElem(L"pubDate");
			CString csDate = xml.GetChildData();

			//Добавляем в карту
			rssInfo[csTitle] = std::make_pair(csDate, csDescription);

			xml.OutOfElem();
		}
		xml.OutOfElem();
	}
}


void LauncherDialog::OnBnClickedBtnOpenLink()
{
	int sel = m_listNew.GetSelectionMark();
	if (sel == -1)
	{
		AfxMessageBox(L"Ни одного элемента не выбрано!");
		return;
	}
	//CString url_open = pageInfo[sel];
	CString url_open = m_listNew.GetItemText(sel, N_SUBITEM_LIST_NEW_URL);
	ShellExecute(0, 0, url_open, 0, 0, SW_SHOW);
}


void LauncherDialog::OnBnClickedBtnInstall()
{
	int sel = m_listNew.GetSelectionMark();
	if (sel == -1)
	{
		AfxMessageBox(L"Ни одного элемента не выбрано!");
		return;
	}
	
	CString gameName =  m_listNew.GetItemText(sel, N_SUBITEM_LIST_NEW_GNAME);
	CString gameTitle = m_listNew.GetItemText(sel, N_SUBITEM_LIST_NEW_CAPTION);
	CString gameDwnUrl =   m_listNew.GetItemText(sel, N_SUBITEM_LIST_NEW_DWN_URL);
	//TODO: добавить проверку по списку установленных
	
	bool foundInInstalled = false;
	for (int i = 0; i < m_listInstalled.GetItemCount(); i++)
	{
		CString installedGameName = m_listInstalled.GetItemText(i, N_SUBITEM_LIST_INSTALLED_GNAME);
		if (gameName == installedGameName)
		{
			foundInInstalled = true;
			break;
		}
	}

	if (foundInInstalled)
	{
		//AfxMessageBox(L"Выбранная игра уже установлена");
		int want_run = AfxMessageBox(L"Выбранная игра уже установлена. Хотите её запустить?", MB_YESNOCANCEL | MB_ICONQUESTION);
		if (want_run == IDYES)
		{
			m_wantPlay = true;
			m_stGamePath = m_gameBaseDir + L"\\"+ gameName;
			m_stGameTitle = gameTitle;
			EndDialog(-1);
		}
		//
		return;
	}
	

	CUrlFileDlg dlg(gameDwnUrl, L"games\\" + gameName + L".zip");
	dlg.DoModal();
	//После хорошей загрузки обновляем списки
	if (dlg.isGoodLoad())
	{
		RescanInstalled();
	}
}


void LauncherDialog::OnBnClickedBtnPlayGamem()
{
	// TODO: добавьте свой код обработчика уведомлений
	int sel = m_listInstalled.GetSelectionMark();
	if (sel == -1)
	{
		AfxMessageBox(L"Ни одного элемента не выбрано!");
		return;
	}
	CString gameName = m_listInstalled.GetItemText(sel, N_SUBITEM_LIST_INSTALLED_GNAME);
	m_wantPlay = true;
	m_stGamePath = m_gameBaseDir + L"\\" + gameName;//installedGamePath[gameName];
	m_stGameTitle = m_listInstalled.GetItemText(sel, N_SUBITEM_LIST_INSTALLED_CAPTION);

	EndDialog(-1);
}

bool LauncherDialog::isWantStartGame()
{
	return m_wantPlay;
}

CString LauncherDialog::getStartGamePath()
{
	return m_stGamePath;
}

CString LauncherDialog::getStartGameTitle()
{
	return m_stGameTitle;
}


void LauncherDialog::OnBnClickedBtnResumeoldGame2()
{
	CIniFile mainSettings;

	m_wantPlay = true;
	mainSettings.GetString(L"main", L"lastGameFile", m_stGamePath, L"");
	mainSettings.GetString(L"main", L"lastGameName", m_stGameTitle, L"");

	EndDialog(-1);
}


void LauncherDialog::OnCbnSelchangeComboFilter()
{
	//Изменение фильтра
	if (m_lastSelFilter != m_comboFiler.GetCurSel())
	{
		CIniFile mainSettings;
		m_lastSelFilter = m_comboFiler.GetCurSel();
		mainSettings.WriteNumber(L"main", L"mRepoFilter", m_lastSelFilter);
		if (PathFileExists(L"temp.xml") && PathFileExists(L"temp2.xml") && PathFileExists(L"temp3.xml") )
		{
			//Очищаем список новых игр
			ClearNewList();
			//Обновляем список новых игр
			ReadNewGamesFromXMLAndAdd(L"temp.xml",false);
			ReadNewGamesFromXMLAndAdd(L"temp2.xml",false);
			ReadNewGamesFromXMLAndAdd(L"temp3.xml", true);
		}
	}
}


void LauncherDialog::OnHdnItemclickListInstalled(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: добавьте свой код обработчика уведомлений
	int nTab = m_tab.GetCurSel();

	if (nTab == ID_PAGE_INSTALLED)
	{
		TRACE(_T("InstalledList HeaderClick %d\n"), phdr->iItem);
		if (m_sortInstalledLastItem != phdr->iItem) {
			m_sortInstalledLastItem = phdr->iItem;
			m_sortInstalledUp = true;
		}
		SortColumn(&m_listInstalled, phdr->iItem, m_sortInstalledUp);
		m_sortInstalledUp = !m_sortInstalledUp;
	}
	else if (nTab == ID_PAGE_NEW)
	{
		TRACE(_T("NewList HeaderClick %d\n"), phdr->iItem);
		if (m_sortNewLastItem != phdr->iItem) {
			m_sortNewLastItem = phdr->iItem;
			m_sortNewUp = true;
		}
		SortColumn(&m_listNew, phdr->iItem, m_sortNewUp);
		m_sortNewUp = !m_sortNewUp;
	}
	else
	{
		TRACE(_T("Unknown List HeaderClick %d\n"), phdr->iItem);
	}
	*pResult = 0;
}
