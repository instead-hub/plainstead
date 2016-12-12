// PlainInsteadDoc.cpp : реализаци€ класса CPlainInsteadDoc
//

#include "stdafx.h"
#include "PlainInstead.h"

#include "PlainInsteadDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlainInsteadDoc

IMPLEMENT_DYNCREATE(CPlainInsteadDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlainInsteadDoc, CDocument)
END_MESSAGE_MAP()


// создание/уничтожение CPlainInsteadDoc

CPlainInsteadDoc::CPlainInsteadDoc()
{
	// TODO: добавьте код дл€ одноразового вызова конструктора

}

CPlainInsteadDoc::~CPlainInsteadDoc()
{
	CDocument::OnCloseDocument();
}

BOOL CPlainInsteadDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: добавьте код повторной инициализации
	// (ƒокументы SDI будут повторно использовать этот документ)

	return TRUE;
}




// сериализаци€ CPlainInsteadDoc

void CPlainInsteadDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: добавьте код сохранени€
	}
	else
	{
		// TODO: добавьте код загрузки
	}
}


// диагностика CPlainInsteadDoc

#ifdef _DEBUG
void CPlainInsteadDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPlainInsteadDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// команды CPlainInsteadDoc
