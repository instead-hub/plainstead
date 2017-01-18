// LauncherDialog.cpp: ���� ����������
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

// ���������� ���� LauncherDialog

IMPLEMENT_DYNAMIC(LauncherDialog, CDialog)
//���� �����
#define ID_PAGE_INSTALLED 0 //������������� ����
#define ID_PAGE_NEW       1 //����� (�� �����������)
//��������� �����
#define SEL_FILTER_ALL           0 //��� ����
#define SEL_FILTER_VALID         1 //���������
#define SEL_FILTER_VALID_AND_UNK 2 //��������� � �������������

LauncherDialog::LauncherDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LAUNCHERDIALOG, pParent)
{

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

//�������� ��� � ������ �� LUA-�����
static CString get_game_name_ru(std::wstring inp) //������� �����
{
	//��������� ����������� �������� �����
	const std::wregex regex_name(L"\\$\\s*Name\\s*\\(\\s*ru\\s*\\)\\s*:([^\\$]*)\\$");
	std::wsregex_iterator next(inp.begin(), inp.end(), regex_name);
	std::wsregex_iterator end;
	while (next != end) {
		std::wsmatch match = *next;
		if (match.size() == 2)
		{
			CString nameStr(match[1].str().data());
			nameStr = nameStr.Trim(); //����������
			return nameStr;
		}
		next++;
	}
	return CString();
}

static CString get_game_name_en(std::wstring inp)
{
	//��������� ����������� �������� �����
	const std::wregex regex_name(L"\\s*Name\\s*:([^\\$]*)\\$");
	std::wsregex_iterator next(inp.begin(), inp.end(), regex_name);
	std::wsregex_iterator end;
	while (next != end) {
		std::wsmatch match = *next;
		if (match.size() == 2)
		{
			CString nameStr(match[1].str().data());
			nameStr = nameStr.Trim(); //����������
			return nameStr;
		}
		next++;
	}
	return CString();
}

static CString get_game_version(std::wstring inp) //������� �����
{
	//��������� ����������� �������� �����
	const std::wregex regex_name(L"\\$\\s*Version\\s*:([^\\$]*)\\$");
	std::wsregex_iterator next(inp.begin(), inp.end(), regex_name);
	std::wsregex_iterator end;
	while (next != end) {
		std::wsmatch match = *next;
		if (match.size() == 2)
		{
			CString nameStr(match[1].str().data());
			nameStr = nameStr.Trim(); //����������
			return nameStr;
		}
		next++;
	}
	return CString();
}

// ����������� ��������� LauncherDialog


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

	// TODO:  �������� �������������� �������������
	TC_ITEM TabItem;
	TabItem.mask = TCIF_TEXT;
	TabItem.pszText = L"������������� ����";
	m_tab.InsertItem(0, &TabItem);
	TabItem.pszText = L"����������� ���";
	m_tab.InsertItem(1, &TabItem);
	m_comboFiler.AddString(L"��� ����");
	m_comboFiler.AddString(L"������ ���������");
	m_comboFiler.AddString(L"������ ��������� � �������������");

	CIniFile mainSettings;
	m_lastSelFilter = mainSettings.GetInt(L"main", L"mRepoFilter", SEL_FILTER_VALID);
	if (m_lastSelFilter > (m_comboFiler.GetCount() - 1)) m_lastSelFilter = SEL_FILTER_VALID;
	m_comboFiler.SetCurSel(m_lastSelFilter);

	showInstalledTabControls();

	//Set the style to listControl
	ListView_SetExtendedListViewStyle(::GetDlgItem(m_hWnd, IDC_LIST_INSTALLED), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	ListView_SetExtendedListViewStyle(::GetDlgItem(m_hWnd, IDC_LIST_NEW), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CreateColumns();

	//AddInstalledGame(L"���������� ���������� ����", true, L"1.0");
	//AddNewGame(L"�����!", true, L"1.0",L"1 ��",L"http://some.page");
	//��������� ������ ��������� ���
	UpdateApprovedFromFile();

	RescanInstalled();

	m_tab.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // ����������: �������� ������� OCX ������ ���������� �������� FALSE
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
	std::vector<std::pair<CString, CString> > filePathsAndNames;
	installedGamePath.clear();
	ListFilesGamInDirectory(dir, filePathsAndNames);

	for (int i = 0; i < filePathsAndNames.size(); i++)
	{
		CString string;
		if (PathFileExists(filePathsAndNames[i].first + L"\\main.lua"))
		{
			CStdioFileEx gameFile(filePathsAndNames[i].first + L"\\main.lua", CFile::modeRead);

			gameFile.SetCodePage(CP_UTF8);
			CString game_name;
			CString game_name_en;
			CString game_version;
			const int MAX_STR_CNT = 30; //�� ������ ����� ���������� ����� �� ������
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
				//������ � ���������� ������ ������ � ��������
				curr_str++;
				if (curr_str > MAX_STR_CNT) break;
			}

			//�� ����� ���� ����������
			if (game_name.IsEmpty() && !game_name_en.IsEmpty() && !game_version.IsEmpty()) //���� ������ ����. ���
			{
				AddInstalledGame(game_name_en, game_version, filePathsAndNames[i]);
			}
			else if (!game_name.IsEmpty() && game_version.IsEmpty()) //���� ��� ������ ������
			{
				AddInstalledGame(game_name, L"", filePathsAndNames[i]);
			}
			else if (!game_name_en.IsEmpty() && game_version.IsEmpty()) //���� ��� ������ ������ � ������ ����
			{
				AddInstalledGame(game_name_en, L"", filePathsAndNames[i]);
			}
		}
	}
}


void LauncherDialog::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �������� ���� ��� ����������� �����������
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
}

// This function inserts the default values into the listControl
void LauncherDialog::CreateColumns()
{
	// Set the LVCOLUMN structure with the required column information
	LVCOLUMN list;
	list.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;
	list.cx = 320;
	list.pszText = L"��������";
	list.iSubItem = 0;
	//Inserts the column
	m_listInstalled.InsertColumn(0, &list);

	list.cx = 100;
	list.pszText = L"���������";
	list.iSubItem = 1;
	m_listInstalled.InsertColumn(1, &list);

	list.cx = 100;
	list.pszText = L"������";
	list.iSubItem = 2;
	m_listInstalled.InsertColumn(2, &list);

	list.cx = 200;
	list.pszText = L"������";
	list.iSubItem = 3;
	m_listInstalled.InsertColumn(3, &list);

	/////// ��� ����� ���
	list.cx = 270;
	list.pszText = L"��������";
	list.iSubItem = 0;
	m_listNew.InsertColumn(0, &list);

	list.cx = 50;
	list.pszText = L"���������";
	list.iSubItem = 1;
	m_listNew.InsertColumn(1, &list);

	list.cx = 50;
	list.pszText = L"������";
	list.iSubItem = 2;
	m_listNew.InsertColumn(2, &list);

	list.cx = 50;
	list.pszText = L"������";
	list.iSubItem = 3;
	m_listNew.InsertColumn(3, &list);

	list.cx = 300;
	list.pszText = L"������";
	list.iSubItem = 4;
	m_listNew.InsertColumn(4, &list);
}

void LauncherDialog::AddInstalledGame(CString name, CString version, std::pair<CString, CString> path)
{
	CString mark = L"���";
	CString info = L"";
	if (approveInfo.count(path.second)) {
		mark = approveInfo[path.second].first;
		info = approveInfo[path.second].second;
	}

	int cnt = m_listInstalled.GetItemCount();
	int col = 0;
	SetCell(m_listInstalled, name, cnt, col++);
	SetCell(m_listInstalled, mark, cnt, col++);
	SetCell(m_listInstalled, version, cnt, col++);
	SetCell(m_listInstalled, info, cnt, col++);

	installedGamePath.push_back(path.first);
	installedGameName.insert(path.second);
}

void LauncherDialog::AddNewGame(CString name, CString version, CString sz, CString page, std::pair<CString, CString> downloadPageAndInstallName)
{
	CString mark = L"���";
	CString info = L"";
	if (approveInfo.count(downloadPageAndInstallName.second)) {
		mark = approveInfo[downloadPageAndInstallName.second].first;
		info = approveInfo[downloadPageAndInstallName.second].second;
	}

	int cnt = m_listNew.GetItemCount();
	int col = 0;
	SetCell(m_listNew, name, cnt, col++);
	SetCell(m_listNew, mark, cnt, col++);
	SetCell(m_listNew, version, cnt, col++);
	SetCell(m_listNew, sz, cnt, col++);
	SetCell(m_listNew, info, cnt, col++);
	//�������� ��� ��������
	//SetCell(m_listNew, page, cnt, col++);

	pageInfo.push_back(page);
	networkGameDWPageAndName.push_back(downloadPageAndInstallName);
	networkGameName.insert(downloadPageAndInstallName.second);
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
	// TODO: �������� ���� ��� ����������� �����������
	*pResult = 0;
}


BOOL LauncherDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �������� ������������������ ��� ��� ����� �������� ������
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
	// TODO: �������� ������������������ ��� ��� ����� �������� ������

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
		AfxMessageBox(L"�� ������ �������� �� �������!");
		return;
	}
	else if (sel > installedGamePath.size()) {
		AfxMessageBox(L"��������� ���� ��� ��������� ��� ��������!");
		return;
	}
	CString gameTitle = m_listInstalled.GetItemText(sel, 0);
	//����� ������������� ��� �������� ����
	int want_del = AfxMessageBox(L"�� ������������� ������ ������� ���� "+ gameTitle+L"?", MB_YESNOCANCEL | MB_ICONQUESTION);
	if (want_del == IDYES)
	{		
		DeleteDirectory(installedGamePath[sel]);
		AfxMessageBox(L"���� �������");
		RescanInstalled();
	}
}

void LauncherDialog::ClearNewList()
{
	networkGameName.clear();
	networkGameDWPageAndName.clear();
	m_listNew.DeleteAllItems();
	pageInfo.clear();
}

void LauncherDialog::OnBnClickedBtnUpdate()
{
	ClearNewList();

	bool ok_appr = UpdateApprovedGamesFromUrl(L"http://dialas.ru/instead_games_approved.xml",
							   L"games\\instead_games_approved.xml");
	if (ok_appr) UpdateApprovedFromFile();

	CString strURL = L"http://instead.sf.net/pool/game_list.xml";
	UpdateNewGamesFromUrl(strURL, L"temp.xml");
	CString strURL2 = L"http://dialas.ru/instead_game_list.xml";
	UpdateNewGamesFromUrl(strURL2, L"temp2.xml");
}

void LauncherDialog::UpdateApprovedFromFile()
{
	//������ xml � ���������
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
		//��������: ��������� �� ������� ��������� � ����� xml
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

void LauncherDialog::UpdateNewGamesFromUrl(CString url, CString temp_xmlfile)
{
	// TODO: �������� ���� ��� ����������� �����������
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

		ReadNewGamesFromXMLAndAdd(temp_xmlfile);
	}
}

void LauncherDialog::ReadNewGamesFromXMLAndAdd(CString temp_xmlfile)
{
	//������ xml � ���������
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
		//��������: ��������� �� ������� ��������� � ����� xml
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

		CString approved_mark;
		if (approveInfo.count(csSN)) approved_mark = approveInfo[csSN].first;
		bool ok_filter = ((m_lastSelFilter == SEL_FILTER_ALL) ||
			( (m_lastSelFilter == SEL_FILTER_VALID) && approved_mark==L"��" ) ||
			((m_lastSelFilter == SEL_FILTER_VALID_AND_UNK) && approved_mark != L"����������")
			);

		//��������� ������������� ���� � �����, ���� � ��� � ������������
		if ((csLang == L"ru") && 
			(installedGameName.count(csSN) == 0) && 
			(networkGameName.count(csSN) == 0) &&
			ok_filter
			)
		{
			CString megabytes_num;
			megabytes_num.Format(L"%.1f ��. ", csSizeBytes / (1024.0f*1024.0f));
			AddNewGame(csTitle, csVersion, megabytes_num, csDescUrl, std::make_pair(csDownloadUrl, csSN));
		}

		xml.OutOfElem();
	}
}


void LauncherDialog::OnBnClickedBtnOpenLink()
{
	int sel = m_listNew.GetSelectionMark();
	if (sel == -1)
	{
		AfxMessageBox(L"�� ������ �������� �� �������!");
		return;
	}
	else if (sel > pageInfo.size())
	{
		AfxMessageBox(L"��������� ���� ��� ��������� ��� �������� ������!");
		return;
	}
	CString url_open = pageInfo[sel];
	ShellExecute(0, 0, url_open, 0, 0, SW_SHOW);
}


void LauncherDialog::OnBnClickedBtnInstall()
{
	int sel = m_listNew.GetSelectionMark();
	if (sel == -1)
	{
		AfxMessageBox(L"�� ������ �������� �� �������!");
		return;
	}
	else if (sel > networkGameDWPageAndName.size())
	{
		AfxMessageBox(L"��������� ���� ��� ��������� ��� ��������!");
		return;
	}
	else if (installedGameName.count(networkGameDWPageAndName[sel].second))
	{
		AfxMessageBox(L"��������� ���� ��� �����������");
		return;
	}

	CUrlFileDlg dlg(networkGameDWPageAndName[sel].first, L"games\\" + networkGameDWPageAndName[sel].second + L".zip");
	dlg.DoModal();
	//����� ������� �������� ��������� ������
	if (dlg.isGoodLoad())
	{
		RescanInstalled();
	}
}


void LauncherDialog::OnBnClickedBtnPlayGamem()
{
	// TODO: �������� ���� ��� ����������� �����������
	int sel = m_listInstalled.GetSelectionMark();
	if (sel == -1)
	{
		AfxMessageBox(L"�� ������ �������� �� �������!");
		return;
	}
	else if (sel > installedGamePath.size())
	{
		AfxMessageBox(L"��������� ���� ��� ��������� ��� �������!");
		return;
	}

	m_wantPlay = true;
	m_stGamePath = installedGamePath[sel];
	m_stGameTitle = m_listInstalled.GetItemText(sel, 0);

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
	//��������� �������
	if (m_lastSelFilter != m_comboFiler.GetCurSel())
	{
		CIniFile mainSettings;
		m_lastSelFilter = m_comboFiler.GetCurSel();
		mainSettings.WriteNumber(L"main", L"mRepoFilter", m_lastSelFilter);
		//������� ������ ����� ���
		ClearNewList();
		//��������� ������ ����� ���
		ReadNewGamesFromXMLAndAdd(L"temp.xml");
		ReadNewGamesFromXMLAndAdd(L"temp2.xml");
	}
}
