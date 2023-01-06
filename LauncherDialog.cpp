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
#define N_SUBITEM_LIST_CAPTION 0 //название
#define N_SUBITEM_LIST_AUTHOR       1 //автор
#define N_SUBITEM_LIST_ACCESSABLE 2 //доступна
#define N_SUBITEM_LIST_ACCESSABLE_COMMENT 3 //дополнительный комментарий по поводу доступности
#define N_SUBITEM_LIST_VERSION    4 //версия
#define N_SUBITEM_LIST_SIZE       5 //размер
#define N_SUBITEM_LIST_DESC 6 //кратко
#define N_SUBITEM_LIST_DATE       7 //дата
#define N_SUBITEM_LIST_GNAME      8 //игровое имя
#define N_SUBITEM_LIST_URL        9 //ссылка URL
#define N_SUBITEM_LIST_DWN_URL    10 //ссылка для прямой загрузки
#define N_SUBITEM_LIST_IS_SANDER  11 //является ли игра                                   из песочницы
#define columnCount N_SUBITEM_LIST_IS_SANDER
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


static void ListFilesGamInDirectory(LPCTSTR dirName, std::vector<std::pair<CString/*full path*/, CString/*name*/> >& filepaths)
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
	wildcard += _T("*.*");
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
			filepaths.push_back(std::make_pair(finder.GetFilePath(), finder.GetFileName()));
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
//Получить тег из lua файла.
static CString get_game_property(std::wstring property, std::wstring inp) //входной текст
{
	//Обработка отображения объектов сцены
	const std::wregex regex_name(L"\\$\\s*?" + property + L"\\s*?:\\s*?([^\\$]*?)\\$", std::regex_constants::icase);
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
void LauncherDialog::updateAllGames(bool updateInstalledGames) {
	if (PathFileExists(L"temp.xml")) ReadNewGamesFromXMLAndAdd(L"temp.xml", false,updateInstalledGames);
	//if (mainSettings.GetInt(L"main", L"beta", 0) == 1 && PathFileExists(L"temp2.xml")) ReadNewGamesFromXMLAndAdd(L"temp2.xml", true);
	if (PathFileExists(L"temp2.xml"))ReadNewGamesFromXMLAndAdd(L"temp2.xml", true,false);
	if(updateInstalledGames)RescanInstalled();
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
	//Обновляем доп инфу, если есть xml
	updateAllGames();
	//m_tab.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}
static CString getGamesDir() {
	TCHAR buff[MAX_PATH];
		::GetModuleFileName(NULL, buff, sizeof(buff));
	CString baseDir = buff;
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	return baseDir + L"\\games\\";
}
void LauncherDialog::RescanInstalled()
{
	std::vector<std::pair<CString, CString> > filePathsAndNames;
	/*installedGameNameCache.clear();
	m_listInstalled.DeleteAllItems();*/
	ListFilesGamInDirectory(getGamesDir(), filePathsAndNames);

	for (int i = 0; i < filePathsAndNames.size(); i++)
	{
		if (installedGameNameCache.count(filePathsAndNames[i].second) > 0) continue;
		//Если игра установлена вручную,обновляем недостающие данные.
		CString string;
		FILE* fStream;
		bool have_file = false;
		if (PathFileExists(filePathsAndNames[i].first + L"\\main.lua"))
		{
			_tfopen_s(&fStream, filePathsAndNames[i].first + L"\\main.lua", L"rt,ccs=UTF-8");
			have_file = true;
		}
		else if (PathFileExists(filePathsAndNames[i].first + L"\\main3.lua"))
		{
			_tfopen_s(&fStream, filePathsAndNames[i].first + L"\\main3.lua", L"rt,ccs=UTF-8");
			have_file = true;
		}
		if (have_file && installedGameNameCache.count(filePathsAndNames[i].second) == 0)installedGameNameCache.insert(filePathsAndNames[i].second);
		if (have_file)
		{
			CStdioFile gameFile(fStream);
			CString game_date;
			CString game_name;
			CString game_name_en;
CString game_author;
CString game_author_en;
			CString game_description;
			CString game_language;
			CString game_version;
			const int MAX_STR_CNT =15; //не больше этого количества строк от начала
			int curr_str = 0;
			while (gameFile.ReadString(string))
			{
				//MessageBox(string.GetBuffer(), L"Тест");
				if (game_date.IsEmpty()) game_date = get_game_property(L"date", string.GetBuffer());
				if (game_name.IsEmpty()) game_name = get_game_property(L"name\\(ru\\s*?\\)", string.GetBuffer());
				if (game_name_en.IsEmpty()) game_name_en = get_game_property(L"name", string.GetBuffer());
				if (game_author_en.IsEmpty()) game_author = get_game_property(L"author", string.GetBuffer());
				if (game_author.IsEmpty()) game_name = get_game_property(L"author\\(ru\\s*?\\)", string.GetBuffer());
				if (game_description.IsEmpty()) game_description = get_game_property(L"info", string.GetBuffer());
				if (game_language.IsEmpty()) game_language = get_game_property(L"lang", string.GetBuffer());
				if (game_version.IsEmpty()) game_version = get_game_property(L"version", string.GetBuffer());
				if (!game_date.IsEmpty() && !game_name.IsEmpty() && !game_description.IsEmpty() && !game_version.IsEmpty() && !game_author.IsEmpty() && !game_language.IsEmpty())
				{
					AddNewGame(game_date, game_name, game_author, game_description, game_version, game_language, filePathsAndNames[i], m_listInstalled);
					break;
				}
				//Обычно в заголовках только версия и название
				curr_str++;
				if (curr_str > MAX_STR_CNT) break;
			}
			//Не нашли всей информации
			if (game_date.IsEmpty() ||game_name.IsEmpty() ||game_description.IsEmpty() || game_author.IsEmpty() ||game_language.IsEmpty()  || game_version.IsEmpty() )
			{
				AddNewGame(game_date, game_name.IsEmpty()?game_name_en:game_name, game_author.IsEmpty()?game_author_en:game_author, game_description, game_language, game_version, filePathsAndNames[i], m_listInstalled);
			}
			gameFile.Close();
		}
	}
}

void LauncherDialog::updateDataAfterInstalationOfGame(CString gameName, int sel, bool gameWasRemoved) {
	if (gameWasRemoved) {
		installedGameNameCache.erase(gameName);
		m_listInstalled.DeleteItem(sel);
	}
	else {
		installedGameNameCache.insert(gameName);
		int cnt = m_listInstalled.GetItemCount();
		for (int a = 0; a <= columnCount; a++) {
			SetCell(m_listInstalled, m_listNew.GetItemText(sel, a), cnt, a);
		}
	}
}
void LauncherDialog::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
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
	//m_btnUpdate.ShowWindow(SW_HIDE);
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
	//m_btnUpdate.ShowWindow(SW_SHOW);
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
		, m_ColumnIndex(columnIndex)
		, m_Ascending(ascending)
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

void LauncherDialog::SortColumn(CListCtrl* ctrl, int columnIndex, bool ascending)
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
	list.iSubItem = N_SUBITEM_LIST_CAPTION;
	//Inserts the column
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_CAPTION, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_CAPTION, &list);
	list.cx = 320;
	list.pszText = L"Автор";
	list.iSubItem = N_SUBITEM_LIST_AUTHOR;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_AUTHOR, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_AUTHOR, &list);
	list.cx = 100;
	list.pszText = L"Доступна";
	list.iSubItem = N_SUBITEM_LIST_ACCESSABLE;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_ACCESSABLE, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_ACCESSABLE, &list);
	list.cx = 256;
	list.pszText = L"Комментарий о доступности";
	list.iSubItem = N_SUBITEM_LIST_ACCESSABLE_COMMENT;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_ACCESSABLE_COMMENT, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_ACCESSABLE_COMMENT, &list);
	list.cx = 100;
	list.pszText = L"Версия";
	list.iSubItem = N_SUBITEM_LIST_VERSION;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_VERSION, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_VERSION, &list);
	list.cx = 50;
	list.pszText = L"Размер";
	list.iSubItem = N_SUBITEM_LIST_SIZE;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_SIZE, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_SIZE, &list);
	list.cx = 200;
	list.pszText = L"Кратко";
	list.iSubItem = N_SUBITEM_LIST_DESC;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_DESC, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_DESC, &list);
	list.cx = 70;
	list.pszText = L"Дата";
	list.iSubItem = N_SUBITEM_LIST_DATE;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_DATE, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_DATE, &list);
	list.cx = 0;
	list.pszText = L"Игровое имя";
	list.iSubItem = N_SUBITEM_LIST_GNAME;
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_GNAME, &list);
	m_listNew.InsertColumn(N_SUBITEM_LIST_GNAME, &list);
	list.cx = 0;
	list.pszText = L"URL";
	list.iSubItem = N_SUBITEM_LIST_URL;
	m_listNew.InsertColumn(N_SUBITEM_LIST_URL, &list);
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_URL, &list);
	list.cx = 0;
	list.pszText = L"Игровое имя";
	list.iSubItem = N_SUBITEM_LIST_GNAME;
	m_listNew.InsertColumn(N_SUBITEM_LIST_GNAME, &list);
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_GNAME, &list);
	list.cx = 0;
	list.pszText = L"Ссылка на скачивание";
	list.iSubItem = N_SUBITEM_LIST_DWN_URL;
	m_listNew.InsertColumn(N_SUBITEM_LIST_DWN_URL, &list);
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_DWN_URL, &list);
	list.cx = 0;
	list.pszText = L"Скачена ли игра из песочницы";
	list.iSubItem = N_SUBITEM_LIST_IS_SANDER;
	m_listNew.InsertColumn(N_SUBITEM_LIST_IS_SANDER, &list);
	m_listInstalled.InsertColumn(N_SUBITEM_LIST_IS_SANDER, &list);
}

void LauncherDialog::AddNewGame(CString date, CString name, CString author, CString description, CString version, CString language, std::pair<CString, CString> downloadPageAndInstallName, CListCtrl& games, CString sz, CString page, bool is_sander)
{
	CString mark = L"Неизвестно";
	CString accComment = L"";
	if (approveInfo.count(downloadPageAndInstallName.second)) {
		mark = approveInfo[downloadPageAndInstallName.second].first;
		accComment = approveInfo[downloadPageAndInstallName.second].second;
		//Приобразуем доступность игр.
		mark = mark == L"-2" ? L"Неизвестно" : mark == L"-1" ? L"Нет (не проходимо)" : mark == L"0" ? L"Нет" : mark == L"1" ? L"Частично" : L"Да";
	}
	int cnt = games.GetItemCount();
	int col = 0;
	SetCell(games, name, cnt, col++);
	SetCell(games, author, cnt, col++);
	SetCell(games, mark, cnt, col++);
	SetCell(games, accComment, cnt, col++);
	SetCell(games, version, cnt, col++);
	SetCell(games, sz, cnt, col++);
	SetCell(games, description, cnt, col++);
	SetCell(games, date, cnt, col++);
	SetCell(games, downloadPageAndInstallName.second, cnt, col++);
	SetCell(games, page, cnt, col++);
	SetCell(games, downloadPageAndInstallName.first, cnt, col++);
	SetCell(games, is_sander ? L"0" : L"1", cnt, col++);
	//installedGameNameCache.insert(downloadPageAndInstallName.second);
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
		ZeroMemory(&lvItem, sizeof(LVITEM));
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = nRow;
		lvItem.pszText = szString;
		//free(szString);
		lvItem.iSubItem = nCol;
		ctrl.InsertItem(&lvItem);
	}
}


void LauncherDialog::OnTcnSelchangingTab1(NMHDR* pNMHDR, LRESULT* pResult)
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
		(m_listInstalled.GetSelectedCount() > 0)
		)
	{
		OnBnClickedBtnDelGame();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_F5)
	{
		OnBnClickedBtnUpdate();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==VK_F4  &&
		(m_tab.GetCurSel() == ID_PAGE_NEW) &&
		(GetFocus() == &m_listNew) &&
		(m_listNew.GetSelectedCount() > 0)
		)
	{
		OnBnClickedBtnOpenLink();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		(m_tab.GetCurSel() == ID_PAGE_NEW) &&
		(GetFocus() == &m_listNew) &&
		(m_listNew.GetSelectedCount() > 0)
		)
	{
		OnBnClickedBtnInstall();
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		(m_tab.GetCurSel() == ID_PAGE_INSTALLED) &&
		(GetFocus() == &m_listInstalled) &&
		(m_listInstalled.GetSelectedCount() > 0)
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

static int DeleteDirectory(const CString& refcstrRootDirectory,
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

	CString gameName = m_listInstalled.GetItemText(sel, N_SUBITEM_LIST_GNAME);
	/*if (gameName == L"mirror" || gameName == L"tutorial3") {
		AfxMessageBox(L"Выбранная игра входит в поставку плеера, и отличается от той что в репозитории. Операция отменена.");
		return;
	}*/
	CString gameTitle = m_listInstalled.GetItemText(sel, 0);
	//Выдаём подтверждение для удаления игры
	int want_del = AfxMessageBox(L"Вы действительно хотите удалить игру " + gameTitle + L"?", MB_YESNOCANCEL | MB_ICONQUESTION);
	if (want_del == IDYES)
	{
		updateDataAfterInstalationOfGame(gameName, sel, true);
		DeleteDirectory(getGamesDir() + gameName);
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
	if (m_tab.GetCurSel() == ID_PAGE_INSTALLED) {
		updateAllGames(true);
}
	else {
	//ClearNewList();

	CString strURL = L"http://instead-games.ru/xml.php";
	UpdateGamesFromUrl(strURL, L"temp.xml");
	CString strURL2 = L"http://instead-games.ru/xml2.php";
	UpdateGamesFromUrl(strURL2, L"temp2.xml", true);
	RescanInstalled();
	}
}

bool LauncherDialog::UpdateGamesFromUrl(CString url, CString res_path, bool is_sander)
{
	CInternetSession session;
	CHttpFile* pFile = (CHttpFile*)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
	CString stLine;
	char buf[2000];
	int numread, counter = 0;
	CFile xmlWithGames(res_path,
		CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	while ((numread = pFile->Read(buf, sizeof(buf) - 1)) > 0)
	{
		buf[numread] = '\0';
		xmlWithGames.Write(buf, numread);
		counter += numread;
	}
	xmlWithGames.Close();
	ReadNewGamesFromXMLAndAdd(res_path, is_sander,!is_sander);
	return counter > 0;
}

void LauncherDialog::ReadNewGamesFromXMLAndAdd(CString temp_xmlfile, bool is_sander, bool updateInstalledGames /*Если true,очищаем список установленных игр. */)
{
	FILE* fStream;
	_tfopen_s(&fStream, temp_xmlfile, L"rt,ccs=UTF-8");
	CStdioFile gameFile(fStream);
	CString xmlDoc;
	CString str;
	while (gameFile.ReadString(str)) xmlDoc.Append(str);
	if (updateInstalledGames) {
		installedGameNameCache.clear();
		m_listInstalled.DeleteAllItems();
	}
	if (!is_sander) {
		approveInfo.clear();
		m_listNew.DeleteAllItems();
	}
	CMarkup xml;
	xml.SetDoc(xmlDoc);
	while (xml.FindChildElem(L"game"))
	{
		xml.IntoElem();
		//ВНИМАНИЕ: считываем по порядку заданному в файле xml
		xml.FindChildElem(L"name");
		CString csSN = xml.GetChildData();
		xml.FindChildElem(L"date");
		CString csDate = xml.GetChildData();
		xml.FindChildElem(L"title");
		CString csTitle = xml.GetChildData();
		xml.FindChildElem(L"author");
		CString csAuthor = xml.GetChildData();
		xml.FindChildElem(L"description");
		CString csDescription = xml.GetChildData();
		xml.FindChildElem(L"image");
		CString csImageUrl = xml.GetChildData();
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
		xml.FindChildElem(L"accessible");
		CString csApproveMark = xml.GetChildData();
		xml.FindChildElem(L"accessibleComment");
		CString csInfo = xml.GetChildData();
		CString megabytes_num;
		megabytes_num.Format(L"%.1f МБ. ", csSizeBytes / (1024.0f * 1024.0f));
		approveInfo[csSN] = std::make_pair(csApproveMark, csInfo);
		//Начинаем фильтровать
		if (GetFileAttributes(getGamesDir() + csSN) != INVALID_FILE_ATTRIBUTES && installedGameNameCache.count(csSN) == 0) {
			installedGameNameCache.insert(csSN);
						AddNewGame(csDate, csTitle, csAuthor, csDescription, csVersion, csLang, std::make_pair(csDownloadUrl, csSN), m_listInstalled, megabytes_num, csDescUrl, is_sander);
					}
		if ((is_sander && m_CheckSander.GetCheck() == BST_CHECKED) || !is_sander)AddNewGame(csDate, csTitle, csAuthor, csDescription, csVersion, csLang, std::make_pair(csDownloadUrl, csSN), m_listNew, megabytes_num, csDescUrl, is_sander);
		xml.OutOfElem();
	}
	gameFile.Close();
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
	CString url_open = m_listNew.GetItemText(sel, N_SUBITEM_LIST_URL);
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

	CString gameName = m_listNew.GetItemText(sel, N_SUBITEM_LIST_GNAME);
	CString gameTitle = m_listNew.GetItemText(sel, N_SUBITEM_LIST_CAPTION);
	CString gameDwnUrl = m_listNew.GetItemText(sel, N_SUBITEM_LIST_DWN_URL);
	//TODO: добавить проверку по списку установленных

	bool foundInInstalled = false;
	for (int i = 0; i < m_listInstalled.GetItemCount(); i++)
	{
		CString installedGameName = m_listInstalled.GetItemText(i, N_SUBITEM_LIST_GNAME);
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
			m_stGamePath = getGamesDir() + gameName;
			m_stGameTitle = gameTitle;
			EndDialog(-1);
		}
		//
		return;
	}
	CString baseDir = getGamesDir();
	if (GetFileAttributes(baseDir) == INVALID_FILE_ATTRIBUTES) SHCreateDirectoryEx(NULL, baseDir, NULL);
	CUrlFileDlg dlg(gameDwnUrl, L"games\\" + gameName + L".zip");
	dlg.DoModal();
	//После хорошей загрузки обновляем списки
	if (dlg.isGoodLoad())
	{
		updateDataAfterInstalationOfGame(gameName, sel);
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
	CString gameName = m_listInstalled.GetItemText(sel, N_SUBITEM_LIST_GNAME);
	m_wantPlay = true;
	m_stGamePath = getGamesDir() + +gameName;//installedGamePath[gameName];
	m_stGameTitle = m_listInstalled.GetItemText(sel, N_SUBITEM_LIST_CAPTION);

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
		if (PathFileExists(L"temp.xml") && PathFileExists(L"temp2.xml"))
		{
			//Очищаем список новых игр
			ClearNewList();
			//Обновляем список новых игр
			updateAllGames(false);
		}
	}
}


void LauncherDialog::OnHdnItemclickListInstalled(NMHDR* pNMHDR, LRESULT* pResult)
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
