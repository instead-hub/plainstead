// PlainInsteadDoc.h : интерфейс класса CPlainInsteadDoc
//


#pragma once


class CPlainInsteadDoc : public CDocument
{
protected: // создать только из сериализации
	CPlainInsteadDoc();
	DECLARE_DYNCREATE(CPlainInsteadDoc)

// Атрибуты
public:

// Операции
public:

// Переопределение
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Реализация
public:
	virtual ~CPlainInsteadDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()
};


