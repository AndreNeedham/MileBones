/* String table constants */
#define IDSNAME      100
#define IDSABOUT     200
#define IDSTITLE     300
//#define IDHELP       399
#define IDM_NEWGAME  103
#define IDM_NOHELP   104
#define IDM_BEEPS    105
#define IDM_MSGS     106
#define IDM_ABOUT    107
#define IDM_EXIT     108
#define IDM_INDEX    109
#define IDM_SCORES   110
#define IDM_SCOREDLG 111
#define IDM_OPTIONS  112
#define IDM_RESTART  113
#define IDM_LOADGAME 114
#define IDM_SAVEGAME 115
#define IDM_UNDO     116
#define MBSCORE      350
#define MBOPTIONS    360
#define MBHELP       370
#define MBNAME       380
#define MBHISCORE    390
#define MBSAVE       400
#define MBLOAD       410
/* 'About' dialog box resource id */
#define ABOUTBOX    1

#define PM_CALLHELP  WM_USER   /* private F1 found message */

static FARPROC lpfnOldHook;

typedef char mystring[40];
typedef struct {               /* 2/9/92 */ 
	   int       HiScore[3]; 
	   mystring  HiName[3]; 
	   int       Spread[3]; 
	   mystring  SpreadName[3]; 
	   }ScoreTable; 
typedef LPVOID LPSCORE;

void GoComp(HWND); 
long FAR PASCAL MBWndProc(HWND, unsigned, WORD, LONG); 
long FAR PASCAL MBInfoWndProc(HWND, unsigned, WORD, LONG); 

/* in mbcore.c */
float frand(); 
void outline(HDC, int, int); 
void DrawBitmap(HDC, short, short, HBITMAP); 
void EraseCard(HDC, int, int); 
void SetCard(HWND, int, int, int);  
void MoveCur(HWND);  
void shuffle(); 
void Setdata(); 
DWORD FAR PASCAL ContextHelp(int, WORD, LONG);
void WriteHighScores();
void CheckForHiScore(int, int); 
void ClearHighScores();
void GetHighScores(HWND); 
void DoHelp(HWND);
 
/* in mbdlg.c: */
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG); 
BOOL FAR PASCAL MBHelp(HWND, unsigned, WORD, LONG); 
BOOL FAR PASCAL MBHiScore(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL MBName(HWND, unsigned, WORD, LONG); 
BOOL FAR PASCAL MBOptions(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL MBSave(HWND, unsigned, WORD, LONG); 
BOOL FAR PASCAL MBLoad(HWND, unsigned, WORD, LONG); 

