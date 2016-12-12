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
#pragma warning (disable : 4127)	// conditional expression is constant

#include "stdafx.h"
#include "ColorPickerCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Uncomment This To Help Debugging The Color Values...
//
//#define		_INCLUDE_COLOR_INFO


//
//	Load/Create Standard Colors...
//
SColorAndName	CColorPickerCB::ms_pColors[] =
{
	/*
	SColorAndName( RGB( 0x00, 0x00, 0x00 ), _T( "Чёрный" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0xFF ), _T( "Белый" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0xFF ), _T( "Синий" ) ),
	SColorAndName( RGB( 0xFF, 0x00, 0x00 ), _T( "Красный" ) ),
	SColorAndName( RGB( 0x00, 0x80, 0x00 ), _T( "Зелёный" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0x00 ), _T( "Желтый" ) ),
	*/
	//
	//	Standard IE Colors (Subset Of X11 Colorset)...
	//
	SColorAndName( RGB( 0xF0, 0xF8, 0xFF ), _T( "AliceBlue" ) ),
	SColorAndName( RGB( 0xFA, 0xEB, 0xD7 ), _T( "AntiqueWhite" ) ),
	SColorAndName( RGB( 0x00, 0xFF, 0xFF ), _T( "Aqua" ) ),
	SColorAndName( RGB( 0x7F, 0xFF, 0xD4 ), _T( "Aquamarine" ) ),
	SColorAndName( RGB( 0xF0, 0xFF, 0xFF ), _T( "Azure" ) ),
	SColorAndName( RGB( 0xF5, 0xF5, 0xDC ), _T( "Beige" ) ),
	SColorAndName( RGB( 0xFF, 0xE4, 0xC4 ), _T( "Bisque" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0x00 ), _T( "Black" ) ),
	SColorAndName( RGB( 0xFF, 0xEB, 0xCD ), _T( "BlanchedAlmond" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0xFF ), _T( "Blue" ) ),
	SColorAndName( RGB( 0x8A, 0x2B, 0xE2 ), _T( "BlueViolet" ) ),
	SColorAndName( RGB( 0xA5, 0x2A, 0x2A ), _T( "Brown" ) ),
	SColorAndName( RGB( 0xDE, 0xB8, 0x87 ), _T( "BurlyWood" ) ),
	SColorAndName( RGB( 0x5F, 0x9E, 0xA0 ), _T( "CadetBlue" ) ),
	SColorAndName( RGB( 0x7F, 0xFF, 0x00 ), _T( "Chartreuse" ) ),
	SColorAndName( RGB( 0xD2, 0x69, 0x1E ), _T( "Chocolate" ) ),
	SColorAndName( RGB( 0xFF, 0x7F, 0x50 ), _T( "Coral" ) ),
	SColorAndName( RGB( 0x64, 0x95, 0xED ), _T( "CornflowerBlue" ) ),
	SColorAndName( RGB( 0xFF, 0xF8, 0xDC ), _T( "Cornsilk" ) ),
	SColorAndName( RGB( 0xDC, 0x14, 0x3C ), _T( "Crimson" ) ),
	SColorAndName( RGB( 0x00, 0xFF, 0xFF ), _T( "Cyan" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0x8B ), _T( "DarkBlue" ) ),
	SColorAndName( RGB( 0x00, 0x8B, 0x8B ), _T( "DarkCyan" ) ),
	SColorAndName( RGB( 0xB8, 0x86, 0x0B ), _T( "DarkGoldenrod" ) ),
	SColorAndName( RGB( 0xA9, 0xA9, 0xA9 ), _T( "DarkGray" ) ),
	SColorAndName( RGB( 0x00, 0x64, 0x00 ), _T( "DarkGreen" ) ),
	SColorAndName( RGB( 0xBD, 0xB7, 0x6B ), _T( "DarkKhaki" ) ),
	SColorAndName( RGB( 0x8B, 0x00, 0x8B ), _T( "DarkMagenta" ) ),
	SColorAndName( RGB( 0x55, 0x6B, 0x2F ), _T( "DarkOliveGreen" ) ),
	SColorAndName( RGB( 0xFF, 0x8C, 0x00 ), _T( "DarkOrange" ) ),
	SColorAndName( RGB( 0x99, 0x32, 0xCC ), _T( "DarkOrchid" ) ),
	SColorAndName( RGB( 0x8B, 0x00, 0x00 ), _T( "DarkRed" ) ),
	SColorAndName( RGB( 0xE9, 0x96, 0x7A ), _T( "DarkSalmon" ) ),
	SColorAndName( RGB( 0x8F, 0xBC, 0x8F ), _T( "DarkSeaGreen" ) ),
	SColorAndName( RGB( 0x48, 0x3D, 0x8B ), _T( "DarkSlateBlue" ) ),
	SColorAndName( RGB( 0x2F, 0x4F, 0x4F ), _T( "DarkSlateGray" ) ),
	SColorAndName( RGB( 0x00, 0xCE, 0xD1 ), _T( "DarkTurquoise" ) ),
	SColorAndName( RGB( 0x94, 0x00, 0xD3 ), _T( "DarkViolet" ) ),
	SColorAndName( RGB( 0xFF, 0x14, 0x93 ), _T( "DeepPink" ) ),
	SColorAndName( RGB( 0x00, 0xBF, 0xFF ), _T( "DeepSkyBlue" ) ),
	SColorAndName( RGB( 0x69, 0x69, 0x69 ), _T( "DimGray" ) ),
	SColorAndName( RGB( 0x1E, 0x90, 0xFF ), _T( "DodgerBlue" ) ) ,
	SColorAndName( RGB( 0xB2, 0x22, 0x22 ), _T( "FireBrick" ) ),
	SColorAndName( RGB( 0xFF, 0xFA, 0xF0 ), _T( "FloralWhite" ) ),
	SColorAndName( RGB( 0x22, 0x8B, 0x22 ), _T( "ForestGreen" ) ),
	SColorAndName( RGB( 0xFF, 0x00, 0xFF ), _T( "Fuchsia" ) ),
	SColorAndName( RGB( 0xDC, 0xDC, 0xDC ), _T( "Gainsboro" ) ),
	SColorAndName( RGB( 0xF8, 0xF8, 0xFF ), _T( "GhostWhite" ) ),
	SColorAndName( RGB( 0xFF, 0xD7, 0x00 ), _T( "Gold" ) ),
	SColorAndName( RGB( 0xDA, 0xA5, 0x20 ), _T( "Goldenrod" ) ),
	SColorAndName( RGB( 0x80, 0x80, 0x80 ), _T( "Gray" ) ),
	SColorAndName( RGB( 0x00, 0x80, 0x00 ), _T( "Green" ) ),
	SColorAndName( RGB( 0xAD, 0xFF, 0x2F ), _T( "GreenYellow" ) ),
	SColorAndName( RGB( 0xF0, 0xFF, 0xF0 ), _T( "Honeydew" ) ),
	SColorAndName( RGB( 0xFF, 0x69, 0xB4 ), _T( "HotPink" ) ),
	SColorAndName( RGB( 0xCD, 0x5C, 0x5C ), _T( "IndianRed" ) ),
	SColorAndName( RGB( 0x4B, 0x00, 0x82 ), _T( "Indigo" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0xF0 ), _T( "Ivory" ) ),
	SColorAndName( RGB( 0xF0, 0xE6, 0x8C ), _T( "Khaki" ) ),
	SColorAndName( RGB( 0xE6, 0xE6, 0xFA ), _T( "Lavender" ) ),
	SColorAndName( RGB( 0xFF, 0xF0, 0xF5 ), _T( "LavenderBlush" ) ),
	SColorAndName( RGB( 0x7C, 0xFC, 0x00 ), _T( "LawnGreen" ) ),
	SColorAndName( RGB( 0xFF, 0xFA, 0xCD ), _T( "LemonChiffon" ) ),
	SColorAndName( RGB( 0xAD, 0xD8, 0xE6 ), _T( "LightBlue" ) ),
	SColorAndName( RGB( 0xF0, 0x80, 0x80 ), _T( "LightCoral" ) ),
	SColorAndName( RGB( 0xE0, 0xFF, 0xFF ), _T( "LightCyan" ) ),
	SColorAndName( RGB( 0xFA, 0xFA, 0xD2 ), _T( "LightGoldenrodYellow" ) ),
	SColorAndName( RGB( 0x90, 0xEE, 0x90 ), _T( "LightGreen" ) ),
	SColorAndName( RGB( 0xD3, 0xD3, 0xD3 ), _T( "LightGrey" ) ),
	SColorAndName( RGB( 0xFF, 0xB6, 0xC1 ), _T( "LightPink" ) ),
	SColorAndName( RGB( 0xFF, 0xA0, 0x7A ), _T( "LightSalmon" ) ),
	SColorAndName( RGB( 0x20, 0xB2, 0xAA ), _T( "LightSeaGreen" ) ),
	SColorAndName( RGB( 0x87, 0xCE, 0xFA ), _T( "LightSkyBlue" ) ),
	SColorAndName( RGB( 0x77, 0x88, 0x99 ), _T( "LightSlateGray" ) ),
	SColorAndName( RGB( 0xB0, 0xC4, 0xDE ), _T( "LightSteelBlue" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0xE0 ), _T( "LightYellow" ) ),
	SColorAndName( RGB( 0x00, 0xFF, 0x00 ), _T( "Lime" ) ),
	SColorAndName( RGB( 0x32, 0xCD, 0x32 ), _T( "LimeGreen" ) ),
	SColorAndName( RGB( 0xFA, 0xF0, 0xE6 ), _T( "Linen" ) ),
	SColorAndName( RGB( 0xFF, 0x00, 0xFF ), _T( "Magenta" ) ),
	SColorAndName( RGB( 0x80, 0x00, 0x00 ), _T( "Maroon" ) ),
	SColorAndName( RGB( 0x66, 0xCD, 0xAA ), _T( "MediumAquamarine" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0xCD ), _T( "MediumBlue" ) ),
	SColorAndName( RGB( 0xBA, 0x55, 0xD3 ), _T( "MediumOrchid" ) ),
	SColorAndName( RGB( 0x93, 0x70, 0xDB ), _T( "MediumPurple" ) ),
	SColorAndName( RGB( 0x3C, 0xB3, 0x71 ), _T( "MediumSeaGreen" ) ),
	SColorAndName( RGB( 0x7B, 0x68, 0xEE ), _T( "MediumSlateBlue" ) ),
	SColorAndName( RGB( 0x00, 0xFA, 0x9A ), _T( "MediumSpringGreen" ) ),
	SColorAndName( RGB( 0x48, 0xD1, 0xCC ), _T( "MediumTurquoise" ) ),
	SColorAndName( RGB( 0xC7, 0x15, 0x85 ), _T( "MediumVioletRed" ) ),
	SColorAndName( RGB( 0x19, 0x19, 0x70 ), _T( "MidnightBlue" ) ),
	SColorAndName( RGB( 0xF5, 0xFF, 0xFA ), _T( "MintCream" ) ),
	SColorAndName( RGB( 0xFF, 0xE4, 0xE1 ), _T( "MistyRose" ) ),
	SColorAndName( RGB( 0xFF, 0xE4, 0xB5 ), _T( "Moccasin" ) ),
	SColorAndName( RGB( 0xFF, 0xDE, 0xAD ), _T( "NavajoWhite" ) ),
	SColorAndName( RGB( 0x00, 0x00, 0x80 ), _T( "Navy" ) ),
	SColorAndName( RGB( 0xFD, 0xF5, 0xE6 ), _T( "OldLace" ) ),
	SColorAndName( RGB( 0x80, 0x80, 0x00 ), _T( "Olive" ) ),
	SColorAndName( RGB( 0x6B, 0x8E, 0x23 ), _T( "OliveDrab" ) ),
	SColorAndName( RGB( 0xFF, 0xA5, 0x00 ), _T( "Orange" ) ),
	SColorAndName( RGB( 0xFF, 0x45, 0x00 ), _T( "OrangeRed" ) ),
	SColorAndName( RGB( 0xDA, 0x70, 0xD6 ), _T( "Orchid" ) ),
	SColorAndName( RGB( 0xEE, 0xE8, 0xAA ), _T( "PaleGoldenrod" ) ),
	SColorAndName( RGB( 0x98, 0xFB, 0x98 ), _T( "PaleGreen" ) ),
	SColorAndName( RGB( 0xAF, 0xEE, 0xEE ), _T( "PaleTurquoise" ) ),
	SColorAndName( RGB( 0xDB, 0x70, 0x93 ), _T( "PaleVioletRed" ) ),
	SColorAndName( RGB( 0xFF, 0xEF, 0xD5 ), _T( "PapayaWhip" ) ),
	SColorAndName( RGB( 0xFF, 0xDA, 0xB9 ), _T( "PeachPuff" ) ),
	SColorAndName( RGB( 0xCD, 0x85, 0x3F ), _T( "Peru" ) ),
	SColorAndName( RGB( 0xFF, 0xC0, 0xCB ), _T( "Pink" ) ),
	SColorAndName( RGB( 0xDD, 0xA0, 0xDD ), _T( "Plum" ) ),
	SColorAndName( RGB( 0xB0, 0xE0, 0xE6 ), _T( "PowderBlue" ) ),
	SColorAndName( RGB( 0x80, 0x00, 0x80 ), _T( "Purple" ) ),
	SColorAndName( RGB( 0xFF, 0x00, 0x00 ), _T( "Red" ) ),
	SColorAndName( RGB( 0xBC, 0x8F, 0x8F ), _T( "RosyBrown" ) ),
	SColorAndName( RGB( 0x41, 0x69, 0xE1 ), _T( "RoyalBlue" ) ),
	SColorAndName( RGB( 0x8B, 0x45, 0x13 ), _T( "SaddleBrown" ) ),
	SColorAndName( RGB( 0xFA, 0x80, 0x72 ), _T( "Salmon" ) ),
	SColorAndName( RGB( 0xF4, 0xA4, 0x60 ), _T( "SandyBrown" ) ),
	SColorAndName( RGB( 0x2E, 0x8B, 0x57 ), _T( "SeaGreen" ) ),
	SColorAndName( RGB( 0xFF, 0xF5, 0xEE ), _T( "Seashell" ) ),
	SColorAndName( RGB( 0xA0, 0x52, 0x2D ), _T( "Sienna" ) ),
	SColorAndName( RGB( 0xC0, 0xC0, 0xC0 ), _T( "Silver" ) ),
	SColorAndName( RGB( 0x87, 0xCE, 0xEB ), _T( "SkyBlue" ) ),
	SColorAndName( RGB( 0x6A, 0x5A, 0xCD ), _T( "SlateBlue" ) ),
	SColorAndName( RGB( 0x70, 0x80, 0x90 ), _T( "SlateGray" ) ),
	SColorAndName( RGB( 0xFF, 0xFA, 0xFA ), _T( "Snow" ) ),
	SColorAndName( RGB( 0x00, 0xFF, 0x7F ), _T( "SpringGreen" ) ),
	SColorAndName( RGB( 0xF0, 0xF0, 0xF0 ), _T( "Standard back" ) ),
	SColorAndName( RGB( 0x46, 0x82, 0xB4 ), _T( "SteelBlue" ) ),
	SColorAndName( RGB( 0xD2, 0xB4, 0x8C ), _T( "Tan" ) ),
	SColorAndName( RGB( 0x00, 0x80, 0x80 ), _T( "Teal" ) ),
	SColorAndName( RGB( 0xD8, 0xBF, 0xD8 ), _T( "Thistle" ) ),
	SColorAndName( RGB( 0xFF, 0x63, 0x47 ), _T( "Tomato" ) ),
	SColorAndName( RGB( 0x40, 0xE0, 0xD0 ), _T( "Turquoise" ) ),
	SColorAndName( RGB( 0xEE, 0x82, 0xEE ), _T( "Violet" ) ),
	SColorAndName( RGB( 0xF5, 0xDE, 0xB3 ), _T( "Wheat" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0xFF ), _T( "White" ) ),
	SColorAndName( RGB( 0xF5, 0xF5, 0xF5 ), _T( "WhiteSmoke" ) ),
	SColorAndName( RGB( 0xFF, 0xFF, 0x00 ), _T( "Yellow" ) ),
	SColorAndName( RGB( 0x9A, 0xCD, 0x32 ), _T( "YellowGreen" ) )
};


CColorPickerCB::CColorPickerCB()
{
	return;													// Done!
}


CColorPickerCB::~CColorPickerCB()
{
	return;													// Done!
}


BEGIN_MESSAGE_MAP(CColorPickerCB, CComboBox)
	//{{AFX_MSG_MAP(CColorPickerCB)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerCB message handlers

int CColorPickerCB::OnCreate( LPCREATESTRUCT pCStruct ) 
{
	if( CComboBox::OnCreate( pCStruct ) == -1 )				// If Create Failed
	{
		return( -1 );										// Return Failure
	}
	_ASSERTE( GetStyle() & CBS_OWNERDRAWFIXED );			// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_DROPDOWNLIST );				// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_HASSTRINGS );				// Assert Proper Style Set

	return( 0 );											// Done!
}


void CColorPickerCB::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();							// Subclass Control

	_ASSERTE( GetStyle() & CBS_OWNERDRAWFIXED );			// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_DROPDOWNLIST );				// Assert Proper Style Set
	_ASSERTE( GetStyle() & CBS_HASSTRINGS );				// Assert Proper Style Set

	return;													// Done!
}


void	CColorPickerCB::InitializeDefaultColors( void )
{
	_ASSERTE( m_hWnd );										// We Must Be Created First...

#if	defined( _INCLUDE_COLOR_INFO )
	COLORREF	crColor;
	TCHAR		caColor[ 256 ];
#endif
	int			iAddedItem = -1;
	int			iColors = COUNTOF( ms_pColors );			// Get Number Of Colors Set

	ResetContent();											// Clear All Colors

#if	defined( _INCLUDE_COLOR_INFO )
	SetDroppedWidth( 400 );									// Increase Drop Width
#endif

	for( int iColor = 0; iColor < iColors; iColor++ )		// For All Colors
	{
#if	defined( _INCLUDE_COLOR_INFO )
		crColor = ms_pColors[ iColor ].m_crColor;			// Store The Color Value

		_ASSERTE( _stprintf( caColor, 
				_T( "%s\t\t%02X%02X%02X" ), ms_pColors[ 
				iColor ].m_cColor, GetRValue( crColor ), 
				GetGValue( crColor ), GetBValue( crColor ) ) 
				 < 255 );									// Build The Debug String
		iAddedItem = AddString(	caColor );					// Set Color Name/Text
#else
		iAddedItem = AddString(	ms_pColors[
					iColor ].m_cColor );					// Set Color Name/Text
#endif
		if( iAddedItem == CB_ERRSPACE )						// If Not Added
		{
#pragma warning (disable : 4127)
			ASSERT( FALSE );								// Let 'Em Know What Happened...
#pragma warning (default : 4127)
			break;											// Stop
		}
		else												// If Added Successfully
		{
			SetItemData( iAddedItem, ms_pColors[
					iColor ].m_crColor );					// Set Color RGB Value
		}
	}
	return;													// Done!
}


void	CColorPickerCB::OnLButtonDown( UINT nFlags, CPoint ptPoint ) 
{
	if( GetFocus() != this )								// If Not Focused
	{
		SetFocus();											// Gain Focus
	}
	CComboBox::OnLButtonDown( nFlags, ptPoint );			// Do Default

	return;													// Done!
}


void	CColorPickerCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
{
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF	crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF	crText = GetSysColor( COLOR_WINDOWTEXT );
	CBrush		brFrameBrush;
	TCHAR		cColor[ CCB_MAX_COLOR_NAME ];				// Color Name Buffer
	CRect		rItemRect( pDIStruct -> rcItem );
	CRect		rBlockRect( rItemRect );
	CRect		rTextRect( rBlockRect );
	CDC			dcContext;
	int			iFourthWidth = 0;
	int			iItem = pDIStruct -> itemID;
	int			iState = pDIStruct -> itemState;

	if( !dcContext.Attach( pDIStruct -> hDC ) )				// Attach CDC Object
	{
		return;												// Stop If Attach Failed
	}
	iFourthWidth = ( rBlockRect.Width() / 4 );				// Get 1/4 Of Item Area
	brFrameBrush.CreateStockObject( BLACK_BRUSH );			// Create Black Brush

	if( iState & ODS_SELECTED )								// If Selected
	{														// Set Selected Attributes
		dcContext.SetTextColor( ( 0x00FFFFFF & ~( 
				crText ) ) );								// Set Inverted Text Color (With Mask)
		dcContext.SetBkColor( crSelected );					// Set BG To Highlight Color
		dcContext.FillSolidRect( &rBlockRect, crSelected );	// Erase Item
	}
	else													// If Not Selected
	{														// Set Standard Attributes
		dcContext.SetTextColor( crText );					// Set Text Color
		dcContext.SetBkColor( crNormal );					// Set BG Color
		dcContext.FillSolidRect( &rBlockRect, crNormal );	// Erase Item
	}
	if( iState & ODS_FOCUS )								// If Item Has The Focus
	{
		dcContext.DrawFocusRect( &rItemRect );				// Draw Focus Rect
	}
	//
	//	Calculate Text Area...
	//
	rTextRect.left += ( iFourthWidth + 2 );					// Set Start Of Text
	rTextRect.top += 2;										// Offset A Bit

	//
	//	Calculate Color Block Area..
	//
	rBlockRect.DeflateRect( CSize( 2, 2 ) );				// Reduce Color Block Size
	rBlockRect.right = iFourthWidth;						// Set Width Of Color Block

	//
	//	Draw Color Text And Block...
	//
	if( iItem != -1 )										// If Not An Empty Item
	{
		int		iChars = GetLBText( iItem, cColor );		// Get Color Text
		int		iaTabStops[ 1 ] = { 50 };

		_ASSERTE( iChars != LB_ERR );						// Sanity Check

		if( iState & ODS_DISABLED )							// If Disabled
		{
			crColor = ::GetSysColor( COLOR_GRAYTEXT );		// Get Inactive Text Color
			dcContext.SetTextColor( crColor );				// Set Text Color
		}
		else												// If Normal
		{
			crColor = GetItemData( iItem );					// Get Color Value
		}
		dcContext.SetBkMode( TRANSPARENT );					// Transparent Background
		dcContext.TabbedTextOut( rTextRect.left, 
				rTextRect.top, cColor, iChars, 1, 
				iaTabStops, 0 );							// Draw Color Name

		dcContext.FillSolidRect( &rBlockRect, crColor );	// Draw Color
				
		dcContext.FrameRect( &rBlockRect, &brFrameBrush );	// Draw Frame
	}
	dcContext.Detach();										// Detach DC From Object
	
	return;													// Done!
}


COLORREF	CColorPickerCB::GetSelectedColorValue( void )
{
	int		iSelectedItem = GetCurSel();					// Get Selected Item

	if( iSelectedItem == CB_ERR )							// If Nothing Selected
	{
		return( RGB( 0, 0, 0 ) );							// Return Black
	}
	return( GetItemData( iSelectedItem ) );					// Return Selected Color
}


LPCTSTR	CColorPickerCB::GetSelectedColorName( void )
{
	int		iSelectedItem = GetCurSel();					// Get Selected Item

	if( iSelectedItem != CB_ERR )							// If Something Selected
	{
		GetLBText( iSelectedItem, m_cColorName );			// Store Name Of Color
	}
	else													// If Nothing Selected
	{
		m_cColorName[ 0 ] = _T( '\0' );						// Terminate Color Name Buffer (Zero Length String)
	}
	return( m_cColorName );									// Return Selected Color Name
}


void		CColorPickerCB::SetSelectedColorValue( COLORREF crClr )
{
	int		iItems = GetCount();
	
	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		if( crClr == GetItemData( iItem ) )					// If Match Found
		{
			SetCurSel( iItem );								// Select It
			break;											// Stop Looping
		}
	}
	return;													// Done!
}


void	CColorPickerCB::SetSelectedColorName( LPCTSTR cpColor )
{
	int		iItems = GetCount();
	TCHAR	cColor[ CCB_MAX_COLOR_NAME ];

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		GetLBText( iItem, cColor );							// Get Color Name
		if( !_tcsicmp( cColor, cpColor ) )					// If Match Found
		{
			SetCurSel( iItem );								// Select It
			break;											// Stop Looping
		}
	}
	return;													// Done!
}


int		CColorPickerCB::AddColor( LPCTSTR cpColor, COLORREF crColor )
{
	int		iIndex = CB_ERR;

	_ASSERTE( cpColor );									// Need This!

#if	defined( _INCLUDE_COLOR_INFO )
	TCHAR	caColor[ 256 ];

	_ASSERTE( _stprintf( caColor, 
			_T( "%s\t\t%02X%02X%02X" ), cpColor, GetRValue( 
				crColor ), GetGValue( crColor ), GetBValue( 
				crColor ) ) < 255 );						// Build The Debug String
		iIndex = AddString(	caColor );						// Set Color Name/Text
#else
		iIndex = AddString(	cpColor );						// Insert Just The Color
#endif
	if( iIndex != CB_ERR )									// If Inserted
	{
		SetItemData( iIndex, (DWORD)crColor );				// Set The Color Value
	}
	return( iIndex );										// Return Insertion Locatiom Or Failure Code
}


bool	CColorPickerCB::RemoveColor( LPCTSTR cpColor )
{
	TCHAR	cColor[ CCB_MAX_COLOR_NAME ];
	bool	bRemoved = false;
	int		iItems = GetCount();

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		GetLBText( iItem, cColor );							// Get Color Name
		if( !_tcsicmp( cColor, cpColor ) )					// If Match Found
		{
			if( DeleteString( iItem ) != CB_ERR )			// Remove It
			{
				bRemoved = true;							// Set Flag If Removed
				break;										// Stop Checking
			}
		}
	}
	return( bRemoved );										// Done!
}


bool	CColorPickerCB::RemoveColor( COLORREF crClr )
{
	bool	bRemoved = false;
	int		iItems = GetCount();

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		if( crClr == GetItemData( iItem ) )					// If Desired Color Found
		{
			if( DeleteString( iItem ) != CB_ERR )			// Remove It
			{
				bRemoved = true;							// Set Flag If Removed
				break;										// Stop Checking
			}
		}
	}
	return( bRemoved );										// Done!
}


void	CColorPickerCB::DDX_ColorPicker( CDataExchange *pDX, int iIDC, COLORREF &crColor )
{
	CColorPickerCB	*pPicker = NULL;
	HWND			hWndCtrl = pDX -> PrepareCtrl( iIDC );
	
	_ASSERTE( hWndCtrl );									// (In)Sanity Check

	pPicker = (CColorPickerCB*)CWnd::FromHandle( hWndCtrl );// Get Actual Control
	
	_ASSERTE( pPicker );									// (In)Sanity Check

	if( !( pDX -> m_bSaveAndValidate ) )					// If Setting The Color Value
	{
		pPicker -> SetSelectedColorValue( crColor );		// Set It
	}
	else													// If Getting The Color Value
	{
		crColor = pPicker -> GetSelectedColorValue();		// Get It
	}
	return;													// Done!
}


void	CColorPickerCB::DDX_ColorPicker( CDataExchange *pDX, int iIDC, CString &sName )
{
	CColorPickerCB	*pPicker = NULL;
	HWND			hWndCtrl = pDX -> PrepareCtrl( iIDC );
	
	_ASSERTE( hWndCtrl );									// (In)Sanity Check

	pPicker = (CColorPickerCB*)CWnd::FromHandle( hWndCtrl );// Get Actual Control
	
	_ASSERTE( pPicker );									// (In)Sanity Check

	if( !( pDX -> m_bSaveAndValidate ) )					// If Setting The Color Name
	{
		pPicker -> SetSelectedColorName( sName );			// Set It
	}
	else													// If Getting The Color Name
	{
		sName = pPicker -> GetSelectedColorName();			// Get It
	}
	return;													// Done!
}


#pragma warning (disable : 4018)	// '<':  signed/unsigned mismatch
#pragma warning (disable : 4100)	// unreferenced formal parameter
#pragma warning (disable : 4127)	// conditional expression is constant
#pragma warning (disable : 4244)	// conv from X to Y, possible loss of data
#pragma warning (disable : 4310)	// cast truncates constant value
#pragma warning (disable : 4505)	// X: unreference local function removed
#pragma warning (disable : 4510)	// X: default ctor could not be generated
#pragma warning (disable : 4511)	// X: copy constructor could not be generated
#pragma warning (disable : 4512)	// assignment operator could not be generated
#pragma warning (disable : 4514)	// debug symbol exceeds 255 chars
#pragma warning (disable : 4610)	// union X can never be instantiated
#pragma warning (disable : 4663)	// to explicitly spec class template X use ...
#pragma warning (disable : 4710)	// function 'XXX' not expanded
#pragma	warning	(disable : 4786)	// X: identifier truncated to '255' chars


