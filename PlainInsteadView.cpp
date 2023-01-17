// PlainInsteadView.cpp : ���������� ������ CPlainInsteadView
//
#include "stdafx.h"

#include "PlainInstead.h"

#include "PlainInsteadDoc.h"
#include "PlainInsteadView.h"

#include "InterpreterController.h"
#include "MultiSpeech.h"
#include "GlobalManager.h"
#include "IniFile.h"
#include "global.h"
#include <regex>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" {
#include "instead\instead.h"
	extern char temp_buff[5000];
	extern BOOL WasReadLine;
	extern BOOL WasReadScreen;
	extern char line_buff[1000]; //����� ����� ������������
	extern BOOL AskSaveLoadTadsGame; //������ �� ����������/�������� ����
	extern BOOL IsNeedSaveTadsGame; //���������� ���������� ����
	extern BOOL IsOkSaveLoadTadsGame; //���������� ����������/��������
	extern char TadsFileName[200];
}


CPlainInsteadView* CPlainInsteadView::m_curView = 0;
CEdit* updateEdit = NULL;
void CALLBACK EXPORT OnTimerUpdateText(HWND, UINT, UINT, DWORD);

CPlainInsteadView* CPlainInsteadView::GetCurrentView()
{
	return m_curView;
}

// CPlainInsteadView

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);
static CString text[2];
static bool debug;
IMPLEMENT_DYNCREATE(CPlainInsteadView, CFormView)

BEGIN_MESSAGE_MAP(CPlainInsteadView, CFormView)
	ON_WM_SIZE()
	ON_EN_SETFOCUS(IDC_EDIT_OUT, &CPlainInsteadView::OnEnSetfocusEditOut)
	ON_WM_TIMER()		// ����������� �� ������
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_COMMAND(ID_FULL_HISTORY, &CPlainInsteadView::OnFullHistory)
	ON_COMMAND(ID_BACK_HIST, &CPlainInsteadView::OnBackHist)
	ON_COMMAND(ID_FORW_HIST, &CPlainInsteadView::OnForwHist)
	ON_COMMAND(ID_MANUAL_STARTER, &CPlainInsteadView::OnManualStarter)
	ON_COMMAND(ID_MANUAL_CMD_LIST, &CPlainInsteadView::OnManualCmdList)
	ON_COMMAND(ID_MANUAL_HOW_PLAY, &CPlainInsteadView::OnManualHowPlay)
	ON_COMMAND(ID_MANUAL_RUK1, &CPlainInsteadView::OnManualRuk1)
	ON_COMMAND(ID_MANUAL_RUK2, &CPlainInsteadView::OnManualRuk2)
	ON_COMMAND(ID_MANUAL_RUK3, &CPlainInsteadView::OnManualRuk3)
	ON_COMMAND(ID_MANUAL_RUK4, &CPlainInsteadView::OnManualRuk4)
	ON_COMMAND(ID_MANUAL_RUK5, &CPlainInsteadView::OnManualRuk5)
	ON_COMMAND(ID_MANUAL_RUK6, &CPlainInsteadView::OnManualRuk6)
	ON_COMMAND(ID_MANUAL_RUK7, &CPlainInsteadView::OnManualRuk7)
	ON_COMMAND(ID_MANUAL_BIG_WRAP, &CPlainInsteadView::OnManualBigWrap)
	ON_COMMAND(ID_FIND_TEXT, &CPlainInsteadView::OnFindText)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
	ON_COMMAND(ID_FIND_NEXT, &CPlainInsteadView::OnFindNext)
	ON_COMMAND(ID_HISTORY_STOP, &CPlainInsteadView::OnHistoryStop)
	ON_COMMAND(ID_HISTORY_START, &CPlainInsteadView::OnHistoryStart)
	ON_COMMAND(ID_UPDATE, &CPlainInsteadView::OnUpdateOutView)
	ON_STN_CLICKED(IDC_STATIC_SCENE, &CPlainInsteadView::OnStnClickedStaticScene)
	ON_LBN_SETFOCUS(IDC_LIST_SCENE, &CPlainInsteadView::OnLbnSetfocusListScene)
	ON_LBN_SETFOCUS(IDC_LIST_INV, &CPlainInsteadView::OnLbnSetfocusListInv)
	ON_LBN_SETFOCUS(IDC_LIST_WAYS, &CPlainInsteadView::OnLbnSetfocusListWays)
	ON_COMMAND(ID_GOTO_SCENE, &CPlainInsteadView::OnGotoScene)
	ON_COMMAND(ID_GOTO_INV, &CPlainInsteadView::OnGotoInv)
	ON_COMMAND(ID_GOTO_WAYS, &CPlainInsteadView::OnGotoWays)
	ON_COMMAND(ID_MENU_LOG, &CPlainInsteadView::OnMenuLog)
	ON_COMMAND(ID_MENU_DEBUG, &CPlainInsteadView::OnMenuDebug)
END_MESSAGE_MAP()

// ��������/����������� CPlainInsteadView

CPlainInsteadView::CPlainInsteadView()
	: CFormView(CPlainInsteadView::IDD)
{
	EnableActiveAccessibility();
	// TODO: �������� ��� ��������
	outFontCol = RGB(0, 0, 0);
	outBackCol = RGB(240, 240, 240);
	inFontCol = RGB(0, 0, 0);
	inBackCol = RGB(255, 255, 255);
	currInpHeight = 20;
	useClipboard = false;
	m_auto_say = true;
	m_jump_to_out = false;
	m_pFindDialog = NULL;
	wasFind = false;
	wave_inv = 0;
	wave_ways = 0;
	wave_scene = 0;
	isLogOn = false;
}

CPlainInsteadView::~CPlainInsteadView()
{
	if (wave_inv) delete wave_inv;
	if (wave_ways) delete wave_ways;
	if (wave_scene) delete wave_scene;
}

void CPlainInsteadView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OUT, m_OutEdit);
		DDX_Control(pDX, IDC_EDIT_INP, m_InputEdit);
	DDX_Control(pDX, IDC_LIST_SCENE, mListScene);
	DDX_Control(pDX, IDC_LIST_INV, mListInv);
	DDX_Control(pDX, IDC_LIST_WAYS, mListWays);
	DDX_Control(pDX, IDC_STATIC_SCENE, mStaticScene);
	DDX_Control(pDX, IDC_STATIC_INV, mStaticInv);
	DDX_Control(pDX, IDC_STATIC_WAYS, mStaticWays);
}

BOOL CPlainInsteadView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �������� ����� Window ��� ����� ����������� ���������
	//  CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CPlainInsteadView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_curView = this;

	m_OutEdit.SetWindowTextW(
		L"������� �������:\r\n"
		L"��� ������ ����� ���� �� ���������� ������� CTRL+M � �������� ���� � ������� ENTER.\r\n"
		L"���� �� ������� ����, ������� ��� � ����������, �� ������� CTRL+N � �������� ����� � �����.\r\n"
		L"��� ���� ����� ������ �������� ����� � ����� �� ������� - �������, ���������, ���� � ������� ������� ENTER.\r\n"
		L"����� �� ���������� � ���������, ��� ���������� ������� ������� �����, ������ ENTER, � ����� ������� ������ ����� � ������ ENTER.\r\n"
		L"���������� � �������� ��� ���������� ����������� � ��������� � ������, ������ �� ������ ������.\r\n"
		L"��� ����������, ���������� ���������� ��� ����� ���������� ������� ��� �������.\r\n"
		L"��������! ������� ������ ����� �� ��������������� ���������� ����� �������� ��� ��������� � ����������. ���������� �������������� ����������� �� � ����� � ������."
	);

	//������������� ���������� �����
	MultiSpeech::getInstance();

	//���������� ��������
	UpdateSettings();
	//������ ������� ���������� ������
	//SetTimer(ID_TIMER_2,100,OnTimerUpdateText);
	//������ ��������
	CIniFile mainSettings;
	useClipboard = mainSettings.GetInt(L"main", L"useClipboard", 0);

	updateEdit = &m_OutEdit;
}


// ����������� CPlainInsteadView

#ifdef _DEBUG
void CPlainInsteadView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPlainInsteadView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPlainInsteadDoc* CPlainInsteadView::GetDocument() const // �������� ������������ ������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlainInsteadDoc)));
	return (CPlainInsteadDoc*)m_pDocument;
}
#endif //_DEBUG


// ����������� ��������� CPlainInsteadView

void CPlainInsteadView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: �������� ���� ��� ����������� ���������
	// ������������������� �����������	
		if (m_InputEdit.m_hWnd && m_InputEdit.IsWindowVisible()) 
		{
			m_InputEdit.ShowWindow(SW_HIDE);
			m_InputEdit.EnableWindow(FALSE);
		}
	int h_static = 30;
	int dh_static_text = 15;
	int h_static_text = h_static - dh_static_text;
	int sz_list = 200;
	int h_list = (cy / 3 - h_static);
	if (m_OutEdit.m_hWnd) m_OutEdit.SetWindowPos(NULL, 0, 0, cx - sz_list, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	if (m_InputEdit.m_hWnd && !m_InputEdit.IsWindowVisible())
	{
		m_InputEdit.ShowWindow(SW_SHOW);
		m_InputEdit.EnableWindow(TRUE);
	}
	//if (mStaticScene.m_hWnd) mStaticScene.SetWindowPos(NULL, 2+cx - sz_list, dh_static_text, sz_list, h_static_text, SWP_NOACTIVATE | SWP_NOZORDER);
	if (mListScene.m_hWnd) mListScene.SetWindowPos(NULL, 2 + cx - sz_list, h_static, sz_list, h_list, SWP_NOACTIVATE | SWP_NOZORDER);

	//if (mStaticInv.m_hWnd) mStaticInv.SetWindowPos(NULL, 2 + cx - sz_list, h_static +h_list + dh_static_text, sz_list, h_static_text, SWP_NOACTIVATE | SWP_NOZORDER);
	if (mListInv.m_hWnd)   mListInv.SetWindowPos(NULL, 2 + cx - sz_list, h_static + h_list + h_static, sz_list, h_list, SWP_NOACTIVATE | SWP_NOZORDER);

	//if (mStaticWays.m_hWnd) mStaticWays.SetWindowPos(NULL, 2 + cx - sz_list, h_static + h_list+ h_static + h_list+ dh_static_text, sz_list, h_static_text, SWP_NOACTIVATE | SWP_NOZORDER);
	if (mListWays.m_hWnd)  mListWays.SetWindowPos(NULL, 2 + cx - sz_list, h_static + h_list + h_static + h_list + h_static, sz_list, h_list, SWP_NOACTIVATE | SWP_NOZORDER);
}

bool Utf8ToCString(CString& cstr, const char* utf8Str)
{
	size_t utf8StrLen = strlen(utf8Str);

	if (utf8StrLen == 0)
	{
		cstr.Empty();
		return true;
	}

	LPWSTR ptr = cstr.GetBuffer(utf8StrLen + 1);

	// CString is UNICODE string so we decode
	int newLen = MultiByteToWideChar(
		CP_UTF8, 0,
		utf8Str, utf8StrLen, ptr, utf8StrLen + 1
	);
	if (!newLen)
	{
		cstr.ReleaseBuffer(0);
		return false;
	}

	cstr.ReleaseBuffer(newLen);
	return true;
}

static std::string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}
static CString getError(CString cmd) {
	const char* er = instead_err();
	if (er) {
		CString error;
		//������������ char* � ������.
		Utf8ToCString(error, er);
		if (!cmd) cmd = ""; else if(cmd.GetLength() > 0)cmd = L"\n"+cmd+L"\n";
		instead_err_msg(NULL);
		return cmd + error + L"\n";
	}
	return L"";
}
static CString getError() {
	return getError(NULL);
}
//���� ������-��������
//��������� �������-������ � ����������� � ����
static std::wstring process_instead_text(std::wstring inp, //������� �����
	CListBox& resBox, //���� ��� ���������� ���������
	std::map<int/*list_pos*/, int/*id_obj*/>& map_action, //����� ����������� ���� �� id-�������
	bool append_num = false, //��������� ����� � ������ (��� �������)
	bool clear =false //������� �� �������� �������,����� ����������� ���� ������
) {
	//��������� ����������� �������� �����
	//const std::wregex regex(L"\\{\\s*(\\w+)\\s*\\#(\\d+)\\}"); //��� �������� ��������
	//const std::wregex regex(L"(\\w+)\\s*\\((\\d+)\\)"); //��� ������� �����
	const std::wregex regex(L"\\[a\\]([^]*?)\\#(\\d+)\\[/a\\]"); //��� ����� [a]
	std::wsmatch match;
	;
	std::wsregex_iterator next(inp.begin(), inp.end(), regex);
	std::wsregex_iterator end;
	while (next != end) {
		match = *next;
		if (match.size() == 3)
		{
			CString addStr(match[1].str().data()); //��� ������, ����
			addStr = addStr.Trim(); //����������
			CString numStr(match[2].str().data()); //id-������� ��� �������
			int obj_id = _wtoi(numStr); //id-�������
			if (append_num) addStr = addStr + L"(" + numStr + L")";
			int str_pos = resBox.GetCount(); //���������� ������ � ������
			resBox.InsertString(str_pos, addStr);
			map_action.insert(std::make_pair(str_pos, obj_id));
		}
		next++;
	}
	std::wstring result;
	std::regex_replace(std::back_inserter(result), inp.begin(), inp.end(), regex, L"$1");
	return result;
}

static std::wstring process_instead_text_act(std::wstring inp, //������� �����
	CListBox& resBox, //���� ��� ���������� ���������
	std::map<int/*list_pos*/, CString/*code*/>& map_action //����� ����������� ���� �� ��������� lua
)
{
	//��������� ����������� �������� �����
	//const std::wregex regex(L"\\{\\s*(\\w+)\\s*\\#(\\d+)\\}"); //��� �������� ��������
	//const std::wregex regex(L"(\\w+)\\s*\\((\\d+)\\)"); //��� ������� �����
	const std::wregex regex(L"\\[a\\:([^\\]]*)\\]([^\\[]*)\\[/a\\]");//��� ����� [a: code]text[\a]
	std::wsregex_iterator next(inp.begin(), inp.end(), regex);
	std::wsregex_iterator end;
	while (next != end) {
		std::wsmatch match = *next;
		if (match.size() == 3)
		{
			CString codeStr(match[1].str().data()); //��� ������, ����
			codeStr = codeStr.Trim(); //����������
			CString textStr(match[2].str().data()); //id-������� ��� �������
			int str_pos = resBox.GetCount(); //���������� ������ � ������
			resBox.InsertString(str_pos, textStr);
			map_action.insert(std::make_pair(str_pos, codeStr));
		}
		next++;
	}
	std::wstring result;
	std::regex_replace(std::back_inserter(result), inp.begin(), inp.end(), regex, L"$2");
	return result;
}
static CString getLogsDir() {
	TCHAR buff[MAX_PATH];
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString baseDir = buff;
	/*free(buff);
	delete[] buff;*/
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	return baseDir + L"logs\\";
}
void CPlainInsteadView::onNewInsteadCommand(char* cmd,char* p,CString cmdForLog) {
	std::map<int, int> prev_map;
	CString er;
	mListScene.ResetContent();
	prev_map = pos_id_scene;
	pos_id_scene.clear();
	act_on_scene.clear();
	text[0] = L"";
	text[1] = L"";
		CString tmp;
		Utf8ToCString(tmp, cmd);
		//free(cmd);
		er = getError(tmp);
		text[0].Append(er);
		text[1].Append(er);
			if (p && *p) {
Utf8ToCString(tmp, p);
		free(p);
		std::wstring buf = tmp.GetBuffer();
		text[1].Append(tmp);
		std::wstring result = process_instead_text(buf, mListScene, pos_id_scene);
		tmp.ReleaseBuffer();
		std::wstring result2 = process_instead_text_act(result, mListScene, act_on_scene);
		Utf8ToCString(tmp, cmd);
		text[0].Append(result2.data());
		//text[0].Append(tmp);
	}
	if (/*m_BeepList && */prev_map.size() != pos_id_scene.size()) {
		wave_scene->play();
	}
	mListWays.ResetContent();
	prev_map = pos_id_ways;
	pos_id_ways.clear();
	p = instead_cmd("way", NULL);
	er = getError(L"way");
	text[0].Append(er);
	text[1].Append(er);
	if (p && *p) {
		Utf8ToCString(tmp, p);
		free(p);
		//���������� ����� � ���� ������
		//text[0].Append(L">> ");
		//text[0].Append(tmp);
		//text[0].Append(L"\n");
		std::wstring buf = tmp.GetBuffer();
		std::wstring result = process_instead_text(buf, mListWays, pos_id_ways);
	}
	if (/*m_BeepList && */prev_map.size() != pos_id_ways.size()) {
		wave_ways->play();
	}
	p = instead_cmd("inv", NULL);
	mListInv.ResetContent();
	prev_map = pos_id_inv;
	pos_id_inv.clear();
	er = getError(L"inv");
	text[0].Append(er);
	text[1].Append(er);
	if (p && *p) {
		Utf8ToCString(tmp, p);
		free(p);
		//���������� ��������� � ���� ������
		//text[0].Append(L"** ");
		//text[0].Append(tmp);
		//text[0].Append(L"\n");
		std::wstring buf = tmp.GetBuffer();
		std::wstring result = process_instead_text(buf, mListInv, pos_id_inv);
		tmp.ReleaseBuffer();
	}
	if (/*m_BeepList && */prev_map.size() != pos_id_inv.size()) {
		//PlaySound(baseSoundDir+_T("inventory.wav"), NULL, SND_MEMORY | SND_FILENAME | SND_ASYNC | SND_NOSTOP);
		wave_inv->play();
	}
	text[0].Replace(L"\n", L"\r\n");
	updateText();
	if (isLogOn)
	{
		CString logsDir = getLogsDir();
		if (GetFileAttributes(logsDir) == INVALID_FILE_ATTRIBUTES) SHCreateDirectoryEx(NULL, logsDir, NULL);
		FILE* fStream;
		//errno = 0;
		int result = _tfopen_s(&fStream, logFileName, L"at,ccs=UTF-8");
				if (result )
		{
			AfxMessageBox(L"�� ���� �������� ���� ����! ����������� ��������.");
			TurnOffLogging();
			return;
		}
				CStdioFile flog(fStream);
		flog.SeekToEnd();
		flog.WriteString(L"\n\n>" + cmdForLog + L"\n");
		flog.SeekToEnd();
		flog.WriteString(text[debug]);
		flog.SeekToEnd();
		if (mListScene.GetCount() > 0)
		{
			flog.WriteString(L"\n�����: ");
			flog.SeekToEnd();
			CString itm;
			for (int i = 0; i < mListScene.GetCount(); i++) {
				mListScene.GetText(i, itm);
				flog.WriteString(itm + L"; ");
				flog.SeekToEnd();
			}
		}
		if (mListInv.GetCount() > 0)
		{
			flog.WriteString(L"\n���������: ");
			flog.SeekToEnd();
			CString itm;
			for (int i = 0; i < mListInv.GetCount(); i++) {
				mListInv.GetText(i, itm);
				flog.WriteString(itm + L"; ");
				flog.SeekToEnd();
			}
		}
		if (mListWays.GetCount() > 0)
		{
			flog.WriteString(L"\n����: ");
			flog.SeekToEnd();
			CString itm;
			for (int i = 0; i < mListWays.GetCount(); i++) {
				mListWays.GetText(i, itm);
				flog.WriteString(itm + L"; ");
				flog.SeekToEnd();
			}
		}
		flog.Close();
	}
}
void CPlainInsteadView::updateText(char* txt) {
	if (txt) {
				CString tmp;
		Utf8ToCString(tmp, txt);
		text[0] = tmp;
		text[1] = tmp;
		//free(txt);
tmp.ReleaseBuffer(tmp.GetLength());
		}
	m_OutEdit.SetWindowTextW(text[debug]);
	if (!m_jump_to_out) UpdateFocusLogic();
	if (m_jump_to_out) m_OutEdit.SetFocus();
	if (m_auto_say) MultiSpeech::getInstance().Say(text[debug]);
}
int CPlainInsteadView::TryInsteadCommand(CString textIn, CString cmdForLog, bool needSearchVariants, bool isFromEdit)
{
	CString resout;
	int rc;
	char cmd[256];
	char* p;
	bool is_saving = false;
	if (textIn.Find(L"save ") == 0)
	{
		GlobalManager::getInstance().userSavedFile();
		is_saving = true;
	}
	if (!is_saving && !textIn.IsEmpty()) GlobalManager::getInstance().userNewCommand();
	//��������� ������ � Instead
	char command[sizeof(cmd)];
	strcpy(command, utf8_encode(textIn.GetBuffer()).c_str());
	if (needSearchVariants) {
				textIn.ReleaseBuffer();
		snprintf(cmd, sizeof(cmd), "use %s", command);
		p = instead_cmd(cmd, &rc);
		if (rc) { /* try go */
			free(p);
			snprintf(cmd, sizeof(cmd), "go %s", command);
			p = instead_cmd(cmd, &rc);
		}
		if (rc) free(p);
		//free(command); //������ �� �� ����� ���������� �������,�.� ��� ������� ������������ command,� ��������� ��������� ������.
	}
	if (!needSearchVariants || rc) {
		if (isFromEdit) /*����������?*/
		{
						if(needSearchVariants) free(p);
			snprintf(cmd, sizeof(cmd), "@metaparser \"%s\"", command);
			//m_InputEdit.SetWindowTextW((LPCTSTR) cmd); //(��� �������,����� ���������,��� �������� ������������� ���������)
			p = instead_cmd(cmd, &rc);
			textIn.ReleaseBuffer(textIn.GetLength());
		}
		if (!isFromEdit ||rc) {
			/* try act */
			strcpy(cmd, utf8_encode(textIn.GetBuffer()).c_str());
			textIn.ReleaseBuffer(textIn.GetLength());
			p = instead_cmd(cmd, &rc);
			if ((textIn.Find(L"save ") == 0 || textIn.Find(L"load ") == 0) && !isFromEdit)
			{
				return rc;
			}
		}
	}
	onNewInsteadCommand(cmd,p, cmdForLog);
	return rc;
}

////////////////////////

BOOL CPlainInsteadView::PreTranslateMessage(MSG* pMsg)
{
	static bool was_enter = false;
	// TODO: �������� ������������������ ��� ��� ����� �������� ������
	if ((pMsg->message == WM_CHAR) && (GetFocus() == &m_OutEdit))
	{
		//������������� ������������� �� ���� ����� ���� �������� �������� �����
		m_InputEdit.SetFocus();
		CString str = _T("");
		m_InputEdit.GetWindowText( str );
		str.AppendChar(pMsg->wParam);
		m_InputEdit.SetWindowText(str);
		int len = str.GetLength();
		m_InputEdit.SetSel(len,len);
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &mListScene)
	{
		int sel_pos = mListScene.GetCurSel();
		if (pos_id_scene.count(sel_pos)) {
			if (pos_id_scene[sel_pos] == 0) {
				//��� ������, �� ���� ������ �������, ������ �������� ������
				MessageBeep(MB_OK);
			}
			else {
				CString res;
				int res_pos = pos_id_scene[sel_pos];
				if (res_pos > 1000) res_pos -= 1000;
				res.Format(L"%d", res_pos);
				if (!inv_save.IsEmpty()) { res = inv_save + +L"," + res;  inv_save.Empty(); }
				int total_list_sz = mListScene.GetCount();
				CString selText;
				mListScene.GetText(sel_pos, selText);
				TryInsteadCommand(res, L"����� ����� \'" + selText + L"\'");
				if (mListScene.GetCount() == total_list_sz) {
					mListScene.SetCurSel(sel_pos);
				}
				if (mListScene.GetCurSel() == LB_ERR && mListScene.GetCount() > 0)
				{
					mListScene.SetCurSel(0);
				}
			}
		}
		else if (act_on_scene.count(sel_pos)) { //��������� ������� ���� �� �����
			CString code = act_on_scene[sel_pos];
			int total_list_sz = mListScene.GetCount();
			if (!inv_save.IsEmpty()) inv_save.Empty();
			CString selText;
			mListScene.GetText(sel_pos, selText);
			TryInsteadCommand(code, L"�������� \'" + savedSelInv + L"\' �� \'" + selText + L"\'");
			if (mListScene.GetCount() == total_list_sz) {
				mListScene.SetCurSel(sel_pos);
			}
			if (mListScene.GetCurSel() == LB_ERR && mListScene.GetCount() > 0)
			{
				mListScene.SetCurSel(0);
			}
		}

	}
	else if (pMsg->message == WM_KEYDOWN && ::GetKeyState(VK_CONTROL) < 0 && (GetFocus() == &m_OutEdit ||GetFocus() ==&m_InputEdit))
	{
		CEdit* currEdit = (CEdit*)GetFocus();
		switch (pMsg->wParam)
		{
					case 'Z':
			currEdit->Undo();
			return TRUE;
					case 'X':
						currEdit->Cut();
						return TRUE;
								case 'C':
			currEdit->Copy();
			return TRUE;
		case 'V':
			currEdit->Paste();
			return TRUE;
		case 'A':
			currEdit->SetSel(0, -1);
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_UP &&
		GetFocus() == &m_InputEdit)
	{
		if (!was_enter) GlobalManager::getInstance().previosCommandMove();//) MessageBeep(MB_ICONSTOP);
		else was_enter = false;
		m_InputEdit.SetWindowText(GlobalManager::getInstance().commandData());
		m_InputEdit.SetSel(0, 0);
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_DOWN &&
		GetFocus() == &m_InputEdit)
	{
		if (!was_enter) GlobalManager::getInstance().nextCommandMove();//) MessageBeep(MB_ICONSTOP);
		else was_enter = false;
		m_InputEdit.SetWindowText(GlobalManager::getInstance().commandData());
		m_InputEdit.SetSel(0, 0);
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &m_InputEdit)
	{
		if (GlobalManager::getInstance().isUserStartGame())
		{
			was_enter = true;
			//��������� ����� �������
			CString textIn;
			m_InputEdit.GetWindowTextW(textIn);
			GlobalManager::getInstance().appendCommand(textIn);
			//��������� �� ����������� �������
			CString textCheck = textIn;
			textCheck.MakeLower();
			textCheck.Trim();
			if (textCheck == L"�����")
			{
				//AfxMessageBox(L"������� �����!!!");
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_APP_EXIT, 0L);
				m_InputEdit.SetWindowTextW(L"");
					return TRUE;
			}
			/*else if ((textCheck == L"����������") || (textCheck == L"������"))
			{
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_RESTART_MENU, 0L);
				m_InputEdit.SetWindowTextW(L"");
				//GlobalManager::getInstance().userNewCommand();
				return TRUE;
			}
			else if (textCheck == L"���������")
			{
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_FILE_SAVE_GAME, 0L);
				m_InputEdit.SetWindowTextW(L"");
				GlobalManager::getInstance().userNewCommand();
				return TRUE;
			}
			else if (textCheck == L"���������")
			{
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_FILE_OPEN, 0L);
				m_InputEdit.SetWindowTextW(L"");
				GlobalManager::getInstance().userNewCommand();
								return TRUE;
			}*/
			//m_OutEdit.SetWindowTextW(L"");
						m_InputEdit.SetWindowTextW(L"");
									//m_OutEdit.SetFocus();
			TryInsteadCommand(textIn,L"���� ������", false, true);
		}
		else
		{
			AfxMessageBox(L"������� ���� �� �������. \n����������, ������� ����� ����");
		}
		return TRUE; // this doesn't need processing anymore
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &mListWays)
	{
		int sel_pos = mListWays.GetCurSel();
		if (pos_id_ways.count(sel_pos)) {
			if (pos_id_ways[sel_pos] == 0) {
				//��� ������, �� ���� ������ �������, ������ �������� ������
				MessageBeep(MB_OK);
			}
			else {
				CString res;
				int res_pos = pos_id_ways[sel_pos];
				if (res_pos > 1000) res_pos -= 1000;
				res.Format(L"%d", res_pos);
				if (!inv_save.IsEmpty()) { res = inv_save + +L"," + res;  inv_save.Empty(); }
				inv_save.Empty(); //������ ��������� ������� �� ����
				int total_list_sz = mListWays.GetCount();
				CString selText;
				mListWays.GetText(sel_pos, selText);
				TryInsteadCommand(res, L"������� ���� \'" + selText + L"\'");
				if (mListWays.GetCount() == total_list_sz) {
					mListWays.SetCurSel(sel_pos);
				}
				if (mListWays.GetCurSel() == LB_ERR && mListWays.GetCount() > 0)
				{
					mListWays.SetCurSel(0);
				}
			}
		}
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &mListInv)
	{
		int sel_pos = mListInv.GetCurSel();
		if (pos_id_inv.count(sel_pos)) {
			if (pos_id_inv[sel_pos] == 0) {
				//��� ������, �� ���� ������ �������, ������ �������� ������
				MessageBeep(MB_OK);
			}
			else {
				CString res;
				bool isMenuItem = (pos_id_inv[sel_pos] > 1000);
				if (isMenuItem) res.Format(L"%d", pos_id_inv[sel_pos] - 1000);
				else res.Format(L"%d", pos_id_inv[sel_pos]);
				if (inv_save.IsEmpty() && !isMenuItem) {
					inv_save = res;
					CString currText;
					mListInv.GetText(sel_pos, currText);
					savedSelInv = currText;//��������� ��������� �����
					currText += L" (������)";
					mListInv.SetDlgItemTextW(sel_pos, currText);
					mListInv.DeleteString(sel_pos);
					mListInv.InsertString(sel_pos, currText);
					mListInv.SetCurSel(sel_pos);
				}
				else {
					if (res != inv_save && !inv_save.IsEmpty()) res = inv_save + L"," + res;
					inv_save.Empty();
					int total_list_sz = mListInv.GetCount();
					CString selText;
					mListInv.GetText(sel_pos, selText);
					TryInsteadCommand(res, L"�������� \'" + selText + L"\' �� '" + savedSelInv + L"\'");
					if (mListInv.GetCount() == total_list_sz) {
						mListInv.SetCurSel(sel_pos);
					}
					else if (isMenuItem && (mListInv.GetCount() > sel_pos)) {
						mListInv.SetCurSel(sel_pos);
					}

					if (mListInv.GetCurSel() == LB_ERR && mListInv.GetCount() > 0)
					{
						mListInv.SetCurSel(0);
					}
				}
			}
		}

	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CPlainInsteadView::SetOutputText(CString newText, BOOL useHistory)
{
	//�������� �������������� ����� ���� �� ������
	if (GlobalManager::getInstance().isAutoMenuDetect() && !GlobalManager::getInstance().isUseMenu())
	{
		//CArray<CString,CString> str;
		CString field;
		int index = 0;
		while (AfxExtractSubString(field, newText, index, _T(',')))
		{
			//����� �������� ����� ����
			if (field.Compare(GlobalManager::getInstance().keyMenuString()) == 0)
			{
				GlobalManager::getInstance().setUseMenu();
				break;
			}
			//str.Add(field);
			++index;
		}
	}
	if (GlobalManager::getInstance().isUseMenu())
	{
		CArray<CString, CString> str;
		CString field;
		int index = 0;
		bool findKeyVal = false;
		while (AfxExtractSubString(field, newText, index, _T(',')))
		{
			//����� �������� ����� ����, ����� ����� ����� ��������� ������
			if (field.Compare(GlobalManager::getInstance().keyMenuString()) == 0)
			{

			}
			else
			{
				if (findKeyVal == false) str.Add(field);
				else
				{

					//m_ListInput.AddString(field);
				}
			}
			++index;
		}
	}
	if (GlobalManager::getInstance().isUserStartGame() && useHistory)
	{
		//��������� ���������� ����� � �������
		GlobalManager::getInstance().appendLastRespond(newText);
	}
	text[0] = newText;
	text[1] = newText;
	m_OutEdit.SetWindowTextW(text[debug]);
	m_InputEdit.SetWindowTextW(L"");
	m_InputEdit.SetFocus();
	//300 �� �������� ��� ������ ����
	SetTimer(ID_TIMER_1,200,NULL);
}
void CPlainInsteadView::SendCommand(CString cmd)
{
	m_InputEdit.SetWindowTextW(cmd);
	m_InputEdit.SetFocus();
	MSG msg;
	msg.message = WM_KEYDOWN;
	msg.wParam = VK_RETURN;
	PreTranslateMessage(&msg);
	
}

void CPlainInsteadView::InitFocusLogic()
{
	mListScene.SetFocus();
	UpdateFocusLogic();
}

void CPlainInsteadView::UpdateFocusLogic()
{
	if (!mListScene.IsWindowEnabled() && mListScene.GetCount() > 0) mListScene.EnableWindow(true);
	if (!mListInv.IsWindowEnabled() && mListInv.GetCount() > 0) mListInv.EnableWindow(true);
	if (!mListWays.IsWindowEnabled() && mListWays.GetCount() > 0) mListWays.EnableWindow(true);

	if ((GetFocus() == &mListScene) && (mListScene.GetCount() == 0))
	{
		if (mListInv.GetCount() > 0) mListInv.SetFocus();
		else if (mListWays.GetCount() > 0) mListWays.SetFocus();
		else m_OutEdit.SetFocus();
	}
	else if ((GetFocus() == &mListInv) && (mListInv.GetCount() == 0))
	{
		if (mListScene.GetCount() > 0) mListScene.SetFocus();
		else if (mListWays.GetCount() > 0) mListWays.SetFocus();
		else m_OutEdit.SetFocus();
	}
	else if ((GetFocus() == &mListWays) && (mListWays.GetCount() == 0))
	{
		if (mListScene.GetCount() > 0) mListScene.SetFocus();
		else if (mListInv.GetCount() > 0) mListInv.SetFocus();
		else m_OutEdit.SetFocus();
	}

	if (mListScene.GetCount() == 0) mListScene.EnableWindow(false);
	if (mListInv.GetCount() == 0) mListInv.EnableWindow(false);
	if (mListWays.GetCount() == 0) mListWays.EnableWindow(false);
}

void CPlainInsteadView::OnEnSetfocusEditOut()
{
	//������� ��������� �� ����� Edit, ��� ������������
	//������� ������� �������
	int selFrom = LOWORD(m_OutEdit.GetSel());
	int selTo = HIWORD(m_OutEdit.GetSel());
	CString txtOut;
	m_OutEdit.GetWindowTextW(txtOut);
	if (selFrom == 0 && (txtOut.GetLength() == (selTo - selFrom)))
	{
		m_OutEdit.SetSel(0);
	}
}

void CPlainInsteadView::OnTimer(UINT uTime)
{
	updateText();
	KillTimer(ID_TIMER_1);
}

void CPlainInsteadView::UpdateSettings()
{

	CIniFile mainSettings;
	m_auto_say = mainSettings.GetInt(L"main", L"m_CheckAutosay", 1);
		m_jump_to_out = mainSettings.GetInt(L"main", L"m_CheckSetFocusToOut", 0);
		debug = mainSettings.GetInt(L"main", L"debug", 0);
	//m_BeepList = mainSettings.GetInt(L"main", L"mCheckSoundList", 1);

	UpdateFontSize();
	//��������� ����������� �����
	int currWaveStyle = mainSettings.GetInt(L"main", L"m_ComboStyleAnnounce", 0);
	char wave_pos[30];
	sprintf(wave_pos, "sounds\\scene%d.wav", currWaveStyle + 1);
	if (wave_scene) {
		delete wave_scene;
		exit(0);
	}
	wave_scene = new Wave(wave_pos);

	sprintf(wave_pos, "sounds\\inventory%d.wav", currWaveStyle + 1);
	if (wave_inv) delete wave_inv;
	wave_inv = new Wave(wave_pos);

	sprintf(wave_pos, "sounds\\ways%d.wav", currWaveStyle + 1);
	if (wave_ways) delete wave_ways;
	wave_ways = new Wave(wave_pos);
}

void CPlainInsteadView::UpdateFontSize()
{
	CIniFile mainSettings;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	int currFontHeight = mainSettings.GetInt(L"main", L"fontHeight", 20);
	currInpHeight = currFontHeight + 10;
	lf.lfHeight = currFontHeight;                // Request a fontH font
	wcscpy(lf.lfFaceName, L"Arial");    // with face name "Arial".
	m_fontOut.DeleteObject();
	m_fontOut.CreateFontIndirect(&lf);    // Create the font.
	m_OutEdit.SetFont(&m_fontOut);

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = currFontHeight;                // Request a fontH font
	wcscpy(lf.lfFaceName, L"Arial");    // with face name "Arial".
	m_fontIn.DeleteObject();
	m_fontIn.CreateFontIndirect(&lf);    // Create the font.
	m_InputEdit.SetFont(&m_fontIn);

	outFontCol = mainSettings.GetInt(L"main", L"OutFontCol", RGB(0, 0, 0));
	outBackCol = mainSettings.GetInt(L"main", L"OutBackCol", RGB(240, 240, 240));
	inFontCol = mainSettings.GetInt(L"main", L"InFontCol", RGB(0, 0, 0));
	inBackCol = mainSettings.GetInt(L"main", L"InBackCol", RGB(255, 255, 255));
}

HBRUSH CPlainInsteadView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �������� ����� �������� DC
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_OUT)
	{
		pDC->SetTextColor(outFontCol);
		pDC->SetBkColor(outBackCol);
	}

	// TODO:  ������� ������ �������� ����������� �����, ���� ��� �� ���������� �� ���������
	return hbr;
}

void CPlainInsteadView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: �������� ���� ��� ����������� ���������
}

void CPlainInsteadView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);

	// TODO: �������� ���� ��� ����������� ���������
}

void CPlainInsteadView::OnClose()
{
	// TODO: �������� ���� ��� ����������� ��������� ��� ����� ������������

	CFormView::OnClose();
}


static int countExacWnd = 0;
static CString lastWndText = L"";
static CString currWindText = L"";
#define MAX_STABLE_COUNT 3

static void AppendTextToEditCtrl(CEdit& edit, LPCTSTR pszText)
{
	// get the initial text length
	int nLength = edit.GetWindowTextLength();
	// put the selection at the end of text
	edit.SetSel(nLength, nLength);
	// replace the selection
	edit.ReplaceSel(pszText);
}

void CALLBACK EXPORT OnTimerUpdateText(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	//������ ���� ������ ����
	if (GlobalManager::getInstance().isUserStartGame())
	{
		/*
		if (!WasReadScreen)
		{
			CString toDisp(temp_buff);

			CPlainInsteadView::GetCurrentView()->SetOutputText(toDisp);

			//WasReadScreen = TRUE;
		}
		*/
		//��������� ���������� ����
		/*
		if (AskSaveLoadTadsGame)
		{
			KillTimer(hWnd, ID_TIMER_2); //������������� ������

			CFileDialog* dlg;
			CFileDialog fileSaveDialog(FALSE,L"*.sav",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Tads save (*.sav)|*.sav");	//������ ������ ������ �����
			CFileDialog fileOpenDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Tads save (*.sav)|*.sav");	//������ ������ ������ �����
			if (IsNeedSaveTadsGame) dlg = &fileSaveDialog;
			else dlg = &fileOpenDialog;
			CString dir;
			GetCurrentDirectory( MAX_PATH, CStrBuf(dir, MAX_PATH) );
			dir.Append(L"\\saves");
			dlg->m_ofn.lpstrInitialDir  = dir;
			int result = dlg->DoModal();	//��������� ���������� ����
			if (result==IDOK)	//���� ���� ������
			{
				//������� ���� ������
				if (IsNeedSaveTadsGame) GlobalManager::getInstance().userSavedFile();
				CPlainInsteadView::GetCurrentView()->SetOutputText(L"");
				GlobalManager::lastString = 0;
				CString filePath = dlg->GetPathName();
				CT2A ascii(filePath);
				strcpy(TadsFileName,ascii);
				IsOkSaveLoadTadsGame = TRUE;
			}
			//��������� ������
			AskSaveLoadTadsGame = FALSE;
			//����� ��������� ������ ���������� ������
			SetTimer(hWnd, ID_TIMER_2,100,OnTimerUpdateText);
		}
		*/
	}
}

void CPlainInsteadView::OnFullHistory()
{
	//�������� ���� �������, ���� ��� ����
	if (GlobalManager::getInstance().isUserStartGame())
	{
		CString fullHist = GlobalManager::getInstance().fullHistoryData();
		if (!fullHist.IsEmpty())
		{
			SetOutputText(fullHist, FALSE);
		}
	}
}

void CPlainInsteadView::OnBackHist()
{
	//��������� ����� �� �������
	if (GlobalManager::getInstance().isUserStartGame())
	{
		CString prevStep = GlobalManager::getInstance().previosHistoryData();
		if (!prevStep.IsEmpty())
		{
			SetOutputText(prevStep, FALSE);
		}
	}
}

void CPlainInsteadView::OnForwHist()
{
	//��������� ����� �� �������
	if (GlobalManager::getInstance().isUserStartGame())
	{
		CString nextStep = GlobalManager::getInstance().nextHistoryData();
		if (!nextStep.IsEmpty())
		{
			SetOutputText(nextStep, FALSE);
		}
	}
}

void CPlainInsteadView::ShowTextFromResource(LPCWSTR res_id)
{
	bool ok = false;
	HRSRC hResource = FindResource(NULL, res_id, L"Text");

	if (hResource)
	{
		HGLOBAL hLoadedResource = LoadResource(NULL, hResource);
		if (hLoadedResource)
		{
			LPVOID pLockedResource = LockResource(hLoadedResource);
			if (pLockedResource)
			{
				DWORD dwResourceSize = SizeofResource(NULL, hResource);
				if (0 != dwResourceSize)
				{
					// Use pLockedResource and dwResourceSize however you want
					LPBYTE sData = (LPBYTE)pLockedResource;
					char* text = (char*)sData;
					CString sText(text);
					SetOutputText(sText, FALSE);
					ok = true;
				}
			}
		}
	}

	if (ok == false)
	{
		SetOutputText(L"������. �� ���� ���������� ����� �������.", FALSE);
	}
}

void CPlainInsteadView::OnManualStarter()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT1));
}

void CPlainInsteadView::OnManualCmdList()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT2));
}

void CPlainInsteadView::OnManualHowPlay()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT3));
}
void CPlainInsteadView::OnManualRuk1()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT4));
}

void CPlainInsteadView::OnManualRuk2()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT5));
}

void CPlainInsteadView::OnManualRuk3()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT6));
}

void CPlainInsteadView::OnManualRuk4()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT7));
}

void CPlainInsteadView::OnManualRuk5()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT8));
}

void CPlainInsteadView::OnManualRuk6()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT9));
}

void CPlainInsteadView::OnManualRuk7()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT10));
}

void CPlainInsteadView::OnManualBigWrap()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT11));
}

void CPlainInsteadView::OnFindText()
{
	if (NULL == m_pFindDialog)
	{
		m_pFindDialog = new CFindReplaceDialog(); // Must be created on the heap.
		m_pFindDialog->Create(TRUE, _T(""), _T(""), FR_DOWN | FR_HIDEUPDOWN | FR_HIDEWHOLEWORD, this);
		m_pFindDialog->m_fr.lStructSize = sizeof(FINDREPLACE);
		m_pFindDialog->m_fr.hwndOwner = this->m_hWnd;
	}
}

LRESULT CPlainInsteadView::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pFindDialog != NULL);

	// If the FR_DIALOGTERM flag is set,
	// ��������� ����
	if (m_pFindDialog->IsTerminating())
	{
		m_pFindDialog = NULL;
		return 0;
	}

	// If the FR_FINDNEXT flag is set,
	// ���� ������
	if (m_pFindDialog->FindNext())
	{
		//read data from dialog
		CString FindName = m_pFindDialog->GetFindString();
		bool bMatchCase = m_pFindDialog->MatchCase() == TRUE;
		//bool bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
		//bool bSearchDown = m_pFindDialog->SearchDown() == TRUE;

		//with given name do search
		if (FindStringInEdit(FindName, bMatchCase))
		{
			wasFind = true;
			lastSearchStr = FindName;
			lastMatchCase = bMatchCase;

			m_pFindDialog->DestroyWindow();
			m_pFindDialog = NULL;
			return 0;
		}
		else
		{
			MessageBeep(MB_ICONSTOP);
		}
	}

	return 0;
}

bool CPlainInsteadView::FindStringInEdit(CString FindName, bool bMatchCase)
{
	//����� � editBox
	CString sEdit;
	m_OutEdit.GetWindowTextW(sEdit);
	//���� �� ���� ��������� �������, �� ��������� �����
	if (!bMatchCase)
	{
		FindName.MakeLower();
		sEdit.MakeLower();
	}
	//������� ������� �������
	int sel = LOWORD(m_OutEdit.GetSel());
	//���� ����
	int resPos = sEdit.Find(FindName, sel + 1);
	//����� - ��������
	if (resPos >= 0)
	{
		m_OutEdit.SetSel(resPos, resPos + FindName.GetLength());
		return true;
	}
	return false;
}

void CPlainInsteadView::OnFindNext()
{
	if (wasFind)
	{
		if (!FindStringInEdit(lastSearchStr, lastMatchCase)) MessageBeep(MB_ICONSTOP);
	}
}

void CPlainInsteadView::OnHistoryStop()
{
	//��������� ������ ������� � ������
	GlobalManager::getInstance().enableHistory(false);
	SetOutputText(L"������� �����������", FALSE);
}

void CPlainInsteadView::OnHistoryStart()
{
	//������������� ������ �������
	GlobalManager::getInstance().enableHistory(true);
	SetOutputText(L"������� ����������", FALSE);
}


void CPlainInsteadView::OnUpdateOutView()
{
	// TODO: �������� ���� ��� ����������� ������
	if (GlobalManager::getInstance().isUserStartGame())
	{
		int sel_pos = 0;
		CListBox* curr_box = 0;
		if (GetFocus() == &mListInv) {
			sel_pos = mListInv.GetCurSel();
			curr_box = &mListInv;
		}
		else if (GetFocus() == &mListScene) {
			sel_pos = mListScene.GetCurSel();
			curr_box = &mListScene;
		}
		else if (GetFocus() == &mListWays) {
			sel_pos = mListWays.GetCurSel();
			curr_box = &mListWays;
		}
		TryInsteadCommand(L"", L"��������",false);
		if (!m_jump_to_out && curr_box)
		{
			if (curr_box->GetCount() > sel_pos) {
				curr_box->SetCurSel(sel_pos);
			}
		}
	}
}


void CPlainInsteadView::OnStnClickedStaticScene()
{
	// TODO: �������� ���� ��� ����������� �����������
}


void CPlainInsteadView::OnLbnSetfocusListScene()
{
	if (GetFocus() == &mListScene)
	{
		mListInv.SetCurSel(-1);
		mListWays.SetCurSel(-1);
		MultiSpeech::getInstance().Say(L"�������");
	}
	if (mListScene.GetCurSel() == LB_ERR && mListScene.GetCount() > 0)
	{
		mListScene.SetCurSel(0);
	}
}


void CPlainInsteadView::OnLbnSetfocusListInv()
{
	if (GetFocus() == &mListInv) {
		mListWays.SetCurSel(-1);
		mListScene.SetCurSel(-1);
		MultiSpeech::getInstance().Say(L"���������");
	}
	// TODO: �������� ���� ��� ����������� �����������
	if (mListInv.GetCurSel() == LB_ERR && mListInv.GetCount() > 0)
	{
		mListInv.SetCurSel(0);
	}
}


void CPlainInsteadView::OnLbnSetfocusListWays()
{
	if (GetFocus() == &mListWays)
	{
		mListInv.SetCurSel(-1);
		mListScene.SetCurSel(-1);
		MultiSpeech::getInstance().Say(L"����");
	}
	// TODO: �������� ���� ��� ����������� �����������
	if (mListWays.GetCurSel() == LB_ERR && mListWays.GetCount() > 0)
	{
		mListWays.SetCurSel(0);
	}
}


void CPlainInsteadView::OnGotoScene()
{
	if (mListScene.GetCount() > 0 && GetFocus() != &mListScene)
	{
		mListScene.SetFocus();
	}
}


void CPlainInsteadView::OnGotoInv()
{
	if (mListInv.GetCount() > 0 && GetFocus() != &mListInv)
	{
		mListInv.SetFocus();
	}
}


void CPlainInsteadView::OnGotoWays()
{
	if (mListWays.GetCount() > 0 && GetFocus() != &mListWays)
	{
		mListWays.SetFocus();
	}
}

void CPlainInsteadView::OnMenuLog()
{
	// TODO: �������� ���� ��� ����������� ������
	isLogOn = !isLogOn;
	CMenu* pMenu = AfxGetApp()->GetMainWnd()->GetMenu();
	if (pMenu != NULL)
	{
		if (isLogOn) {
			pMenu->CheckMenuItem(ID_MENU_LOG, MF_CHECKED | MF_BYCOMMAND);
			// uses printf() format specifications for time
			CString t = CTime::GetCurrentTime().Format("%y%m%d_%H%M");
			logFileName= getLogsDir() + L"log_" + t + L".txt";
			AfxMessageBox(L"����������� ��������. ��� ����������� � ����� logs ��� ������: " + logFileName);
		}
		else
		{
			pMenu->CheckMenuItem(ID_MENU_LOG, MF_UNCHECKED | MF_BYCOMMAND);
		}
	}
}
void CPlainInsteadView::OnMenuDebug() {
	debug = !debug;
	CMenu* pMenu = AfxGetApp()->GetMainWnd()->GetMenu();
	if (pMenu != NULL) {
		CIniFile mainSettings;
		mainSettings.WriteNumber(L"main", L"debug", debug);
		pMenu->CheckMenuItem(ID_MENU_DEBUG, (debug?MF_CHECKED: MF_UNCHECKED) | MF_BYCOMMAND);
		updateText();
		}
}
void CPlainInsteadView::TurnOffLogging()
{
	if (isLogOn) OnMenuLog();
}
