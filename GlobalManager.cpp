#include "StdAfx.h"
#include "GlobalManager.h"

int GlobalManager::lastString=0;

GlobalManager::GlobalManager():
	m_haveFileSaved(true),
	m_userStartGame(false),
	m_useMenu(false),
	m_en_history(true)
{
}

GlobalManager::~GlobalManager()
{
}

void GlobalManager::userStartGame()
{
	m_userStartGame = true;
	//Очищаем историю
	commandHistory.clear();
	m_currHistPos = -1;
	m_currCmdPos = -1;
	//Добавляем пустую команду (текст в начале игры)
	appendCommand(L"");
}

void GlobalManager::userSavedFile()
{
	m_haveFileSaved = true;
}

void GlobalManager::userNewCommand()
{
	m_haveFileSaved = false;
}

bool GlobalManager::isUserSaveLastFile()
{
	return m_haveFileSaved;
}

bool GlobalManager::isUserStartGame()
{
	return m_userStartGame;
}

void GlobalManager::appendCommandAndRespond(CString command, CString respond)
{
	commandHistory.push_back(std::make_pair(command,respond));
	m_currHistPos = commandHistory.size()-1;
	m_currCmdPos = commandHistory.size()-1;
}

void GlobalManager::enableHistory(bool isEn)
{
	m_en_history = isEn;
}

void GlobalManager::appendCommand(CString command)
{
	if (!m_en_history) return;
	//Добавляем новую команду, с пустым откликом
	commandHistory.push_back(std::make_pair(command,L""));
	//Возвращаем на место указатель истории
	m_currHistPos = commandHistory.size()-1;
	m_currCmdPos = commandHistory.size()-1;
}

void GlobalManager::appendLastRespond(CString command)
{
	if (!m_en_history) return;
	//Добавляем недостающий отклик
	commandHistory[m_currHistPos].second = command;
}

CString GlobalManager::fullHistoryData()
{
	if (commandHistory.size() > 0)
	{
		CString hist;
		hist.Append(L"История\r\n");
		for (int i=0;i<commandHistory.size();i++)
		{
			CString res;
			res.Append(L"> ");
			res.Append(commandHistory[i].first);
			res.Append(L"\r\n");
			res.Append(commandHistory[i].second);
			//res.Append(L"\r\n");
			hist.Append(res);
		}
		return hist;
	}
	return CString();
}

CString GlobalManager::previosHistoryData()
{
	if (previosHistoryHave())
	{
		m_currHistPos--;
		CString res;
		res.Format(_T("Шаг: %d из %d\r\n"), m_currHistPos+1, commandHistory.size());
		res.Append(L"> ");
		res.Append(commandHistory[m_currHistPos].first);
		res.Append(L"\r\n");
		res.Append(commandHistory[m_currHistPos].second);
		return res;
	}
	return CString();
}

bool GlobalManager::previosHistoryHave()
{
	return (m_currHistPos > 0);
}

CString GlobalManager::nextHistoryData()
{
	if ( nextHistoryHave() )
	{
		m_currHistPos++;
		CString res;
		res.Format(_T("Шаг: %d из %d\r\n"), m_currHistPos+1, commandHistory.size());
		res.Append(L"> ");
		res.Append(commandHistory[m_currHistPos].first);
		res.Append(L"\r\n");
		res.Append(commandHistory[m_currHistPos].second);
		return res;
	}
	return CString();
}

bool GlobalManager::nextHistoryHave()
{
	return ((m_currHistPos >= 0) && (m_currHistPos<(commandHistory.size()-1)));
}

CString GlobalManager::commandData()
{
	if (commandHistory.size()>0 && m_currCmdPos>=0)
	{
		return commandHistory[m_currCmdPos].first;
	}
	return CString();
}
bool GlobalManager::previosCommandMove()
{
	if (m_currCmdPos > 1)
	{
		m_currCmdPos--;
		return true;
	}
	return false;
}

bool GlobalManager::nextCommandMove()
{
	if ((m_currCmdPos >= 0) && (m_currCmdPos<(commandHistory.size()-1)))
	{
		m_currCmdPos++;
		return true;
	}
	return false;
}


bool GlobalManager::isUseMenu()
{
	return m_useMenu;
}

bool GlobalManager::isAutoMenuDetect()
{
	return true;
}

CString GlobalManager::keyMenuString()
{
	return L"Что вы будете делать дальше?";
}

void GlobalManager::setUseMenu()
{
	m_useMenu = true;
}
