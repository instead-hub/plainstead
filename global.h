/*
** global.h -- All of the globally defined variables.  To define them "for
**             real" simply "#define extern" before you #include this file,
**             then "#undef extern" after you include it.
*/

#ifndef CCHMAXPATH
#define CCHMAXPATH 500
#endif

#define PREFS_VER "1.2.2"	// Don't forget to change PROGRAM_VER in oem.h

// Preferences structure
typedef struct _PREFS {
	CHAR	version[10];			// Preferences version
	RECT	rectFrame;
	LOGFONT	lfNormal, lfBold,
			lfStatus;				// Status font
	BOOL	fStatusSeparated,
			fAskedAboutRegistry,	// Have we asked the user about Registry?
			fMaximized,				// Is WinTADS maximized when we quit?
			paging,					// Use "[MORE]" and pause while paging?
			fulljustify,			// Full-justify the text?
			clearbyscroll,			// Should we clear window by scrolling?
			doublespace,			// 2 spaces after a period?
			fBoldInput,				// Is the input bold?
			fStickyPaths,			// Do I remember the last path?
			fStickyMacros,			// Do I remember macros?
			fCloseOnEnd,			// Do I close WinTADS when quitting a game?
			fCloseAborts;			// Does the 'x' button abort a game?
	short	marginx, marginy,		// Size of the margins
			historylength;			// How many command lines should we save?
	long	buffersize,				// The minimum amount to save
			bufferslack;			// How much to save before trimming
	ULONG	ulFore,					// Foreground color
			ulBack;					// Background color
	CHAR	szGamePath[CCHMAXPATH],	// Last path to .gam files
			szSavePath[CCHMAXPATH],	// Last path to .sav files
			szStatusFont[CCHMAXPATH],		// Name of the status font
			szMacroText[12][255],	// Defined macros
			szRecentGames[10][CCHMAXPATH];	// Recently-opened games
} PREFS, *PPREFS;

/* "Find" dialog data structure */
typedef struct _FINDDATA {
	CHAR	findText[256];
	BOOL	fCase,
			fBack;
} FINDDATA;

/* Window data struct which threads may use for communication w/the mother ship */
typedef struct _SHARED {
	CHAR	szTADS[CCHMAXPATH];
	CHAR	szData[CCHMAXPATH];
	CHAR	szOutput[CCHMAXPATH];
} SHARED, *PSHARED;

extern HINSTANCE	hInst;			// The current instance
extern HDC		hdcClient;			// HPS to the client
extern HWND		hwndFrame,			// Frame window
				hwndClient,			// Main client window
				hwndMessageBox,		// The message box
				hwndStatus,			// The status window
				hwndStatusFrame;	// The frame around the status window
extern LONG		lMessageBoxWidth,	// Width of the message box
				lMessageBoxHeight,	// Height of the message box
				lStatusWidth,		// Width of the status bar
				lStatusHeight,		// Height of the status bar
				lClientHeight;		// Height of the client window
extern RECT		clientRect;			// Size of the client window
extern CHAR		szStatusLeft[256],	// Text on the left side of the status bar
				szStatusRight[256];	// Text on the right side of the status bar
extern LONG		lStatusLeftWidth,	// Width (in pels) of the left status text
				lStatusRightWidth;	// "                    " right status text
extern HCURSOR	hptrArrow,			// Storage for arrow ptr
				hptrWait,			// Wait pointer
				hptrText,			// I-beam text pointer
				hptrOld,			// Old pointer
				hptrCurrent;		// Current pointer
extern LONG		cursorX, cursorY,	// Cursor (x, y) position
				cursorWidth,		// Cursor width
				cursorHeight;		// Cursor height
extern HFONT	hfNorm,				// Normal text font
				hfBold,				// Bold text font
				hfInput,			// Input text font
				hfHTML4Norm[4],		// The 4 fonts I use for HTML 4 chars
				hfHTML4Bold[4];		// The 4 fonts I use for bold-faced HTML 4 chars
extern PREFS	prefsTADS;			// Preferences for the program
extern BOOL		fScrollVisible,		// Is the scroll bar visible?
				fWaitCursor,		// Should we display the wait cursor?
				fCursorOn,			// Is the cursor on?
				fMinimized,			// Are we minimizing?
				fBreakSignalled;	// Does the user want to break?
extern LONG		lineheight_story,	// Height of lines in the client window
				lineheightoff_story;	// Vertical offset bet. lines
extern SHORT	iNormSpace,			// # of pixels a space takes up in normal font
				iBoldSpace;			// # of pixels ' ' takes up in bold
extern CHAR		zcodename[CCHMAXPATH],		// Path & filename of the game to play
				pszExecutable[CCHMAXPATH],	// Path & filename of the executable
				pszHomePath[CCHMAXPATH];	// Path of the executable
extern SHARED	sShare;				// Data shared w/window & binding threads
extern LONG		zcodepos;			// Position of any bound data
extern BOOL		validBinding;		// TRUE if there's data at the end of TADS/2
extern DWORD	tidMachine;			// The thread ID of the TADS virt. machine
extern HANDLE	hMachine;			// Handle to the virt. machine thread
extern FINDDATA	fd;					// Info for the "Find" dialog
extern DWORD	timeZero;			// The zero time for os_get_sys_clock_ms

// Semaphores
extern BOOL		fWantsExtendedKeys;