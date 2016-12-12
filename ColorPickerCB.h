/*|*\
|*|  File:      FocusEditCtrl.h
|*|  
|*|  By:        James R. Twine
|*|             Copyright 1998, James R. Twine
|*|             Copyright 1999-2000, TransactionWorks, Inc.
|*|  Date:      xxxx, xxxx, 1998
|*|             
|*|             This implementes a ComboBox control that can be
|*|             used to display and provide selection for a specific
|*|             set of colors.  The standard set of colors provided
|*|             by the control are a subset of the X11 colorset, and
|*|             are the ones available in (and the color names 
|*|				recognized by) Internet Explorer.
|*|             
|*|				The ComboBox that is subclassed must have the 
|*|             Owner Draw Fixed and Has Strings styles.
|*|             
|*|             This is based on code that was originally found on
|*|             CodeGuru, and was © 1997 Baldvin Hansson.
|*|             
|*|             A bug fix by Marcel Galem was added to provide
|*|             correct masking of the COLORREF values.
|*|             
|*|             DDX routines were provided by Paul Wardle
|*|             (paul.wardle@siemenscomms.co.uk)
|*|             
|*|             This Code May Be Freely Incorporated Into 
|*|             Projects Of Any Type Subject To The Following 
|*|             Conditions:
|*|             
|*|             o This Header Must Remain In This File, And Any
|*|               Files Derived From It
|*|             o Do Not Misrepresent The Origin Of Any Parts Of 
|*|               This Code (IOW, Do Not Claim You Wrote It)
|*|             
|*|             A "Mention In The Credits", Or Similar Acknowledgement,
|*|             Is *NOT* Required.  It Would Be Nice, Though! :)
|*|	
\*|*/
#if !defined(AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_)
#define AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
//	Constants...
//
#define		CCB_MAX_COLOR_NAME		32							// Max Chars For Color Name - 1


//
//	Macros...
//
#if	!defined( COUNTOF )
#define		COUNTOF( Array )	( ( sizeof( Array ) / sizeof( Array[ 0 ] ) ) )
#endif



//
//	Internal Structure For Color/Name Storage...
//
struct	SColorAndName
{
	/**/	SColorAndName()									// Default Constructor
	{
		memset( this, 0, sizeof( SColorAndName ) );			// Init Structure
	};
	/**/	SColorAndName( COLORREF crColor, 
					LPCTSTR cpColor ) : m_crColor( crColor )// Smart Constructor
	{
		_tcsncpy( m_cColor, cpColor, CCB_MAX_COLOR_NAME );	// Set Color Name
		m_cColor[ CCB_MAX_COLOR_NAME - 1 ] = _T( '\0' );	// Just To Make Sure...
	};
	COLORREF	m_crColor;									// Actual Color RGB Value
	TCHAR		m_cColor[ CCB_MAX_COLOR_NAME ];				// Actual Name For Color
};


class CColorPickerCB : public CComboBox
{
public:
	/**/	CColorPickerCB();								// Constructor
	virtual	~CColorPickerCB();								// Destructor

public:
	void			InitializeDefaultColors( void );		// Initialize Control With Default Colors

	COLORREF		GetSelectedColorValue( void );			// Get Selected Color Value
	LPCTSTR			GetSelectedColorName( void );			// Get Selected Color Name

	void			SetSelectedColorValue( COLORREF crClr );// Set Selected Color Value
	void			SetSelectedColorName( LPCTSTR cpColor );// Set Selected Color Name

	bool			RemoveColor( LPCTSTR cpColor );			// Remove Color From List
	bool			RemoveColor( COLORREF crClr );			// Remove Color From List
	
	int				AddColor( LPCTSTR cpName, 
							COLORREF crColor );				// Add A New Color


	void			DDX_ColorPicker( CDataExchange *pDX, 
							int iIDC, COLORREF &crColor );	// DDX Function For COLORREF Value

	void			DDX_ColorPicker( CDataExchange *pDX, 
							int iIDC, CString &sName );		// DDX Function For Color Name

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerCB)
	protected:
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);


	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPickerCB)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static
	SColorAndName	ms_pColors[];							// Array Of Colors And Names

	TCHAR			m_cColorName[ CCB_MAX_COLOR_NAME ];		// Name Of Selected Color

	//
	//	Prevent Misuse Of Copies...
	//
	/**/			CColorPickerCB( const CColorPickerCB& rSrc );
	CColorPickerCB	&operator=( const CColorPickerCB& rSrc );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_)
