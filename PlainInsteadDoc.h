// PlainInsteadDoc.h : ��������� ������ CPlainInsteadDoc
//


#pragma once


class CPlainInsteadDoc : public CDocument
{
protected: // ������� ������ �� ������������
	CPlainInsteadDoc();
	DECLARE_DYNCREATE(CPlainInsteadDoc)

// ��������
public:

// ��������
public:

// ���������������
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����������
public:
	virtual ~CPlainInsteadDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ��������� ������� ����� ���������
protected:
	DECLARE_MESSAGE_MAP()
};


