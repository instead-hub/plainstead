// PlainInsteadDoc.cpp : ���������� ������ CPlainInsteadDoc
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


// ��������/����������� CPlainInsteadDoc

CPlainInsteadDoc::CPlainInsteadDoc()
{
	// TODO: �������� ��� ��� ������������ ������ ������������

}

CPlainInsteadDoc::~CPlainInsteadDoc()
{
	CDocument::OnCloseDocument();
}

BOOL CPlainInsteadDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �������� ��� ��������� �������������
	// (��������� SDI ����� �������� ������������ ���� ��������)

	return TRUE;
}




// ������������ CPlainInsteadDoc

void CPlainInsteadDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �������� ��� ����������
	}
	else
	{
		// TODO: �������� ��� ��������
	}
}


// ����������� CPlainInsteadDoc

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


// ������� CPlainInsteadDoc
