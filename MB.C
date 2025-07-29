/*  mb.c
    Mile Bones Game
    1991 Andre Needham

other files:
   mbplayer.c:
	       void    GoPlayer(HWND)

   mbcomp.c:
	       void    GoComp(HWND)

   mbcore.c:
	       BOOL MBInit(hInstance)  
	       float   frand()
	       HBITMAP CopyBitmap(hDC, xStart, yStart)
	       void    DrawBitmap(hDC, xStart, yStart, hBitmap)
	       void    EraseCard(hDC, x, y)
	       void    SetCard(hWnd, x, y, cnum)
	       void    MoveCur(hWnd)
	       void    shuffle()
	       void    Setdata()
	       DWORD   ContextHelp(nCode, wParam, lParam)
	       void    CheckForHiScore(score0, score1)
	       void    GetHighScores(LPSCORE)
	       void    ClearHighScores()
	       void    DoHelp(HWND)
	       void    SortCards(HWND)
		   void    SetStatusBar1(LPSTR)
		   void    SetStatusBar2()

   mbdlg.c: 
	       BOOL    About(...)
	       BOOL    MBHelp(...)
	       BOOL    MBName(...)
	       BOOL    MBHiScore(...)
	       BOOL    MBOptions(...)
	       BOOL    MBSave(...)
               BOOL    MBLoad(...)
*/
#include <windows.h>
#include <stdio.h>
#include "mb.h"

void SetStatusBar1(LPSTR);

HWND hInfoWnd, hWndMain;
char    szAppName[16], szMessage[16], lpszMenuName[] = "MBMenu",
	str1[60], str2[60], UserName[40], szStatusBar[60], 
	szDash[] = "----",
	szIniFile[] = "mb.ini",
	szHelpFile[] = "mb.hlp";
char *  bmlist[19];
short   numcards[19], deck[101], hand[2][7], battle[2],
	splim[2], safety[2][4], curcard, nbattle[2], nsplim[2], ncards[2],
	cdrawn, cclicked, cango[2], winmiles, two200s[2], mileval[19],
	compturn, gameover, startgame, nsafe[2], ncoup[2], rofw[2],
	xcard=56, xgap=10, xsize=66, ydrop, yscrn, ycard, ysize, ygap,
	mousein, xcursor, ycursor, helpcode = 0;
BOOL    bHelp = FALSE, bDragDrop = TRUE, bInDrag = FALSE,
	bJustInDrag = FALSE, bButtonUp = FALSE, bSortCards = TRUE,
	bDblClick = FALSE, bTimer = FALSE, bRestart = FALSE,
	bCanRestart = FALSE, bUndo = FALSE, bNoLogon = FALSE;
int     miles[2], score[2], gscore[2], scard[8][3], htype = 104,
	GameState[74], OldGameState[74];
HANDLE  hInst;
POINT   dragpt, dragoffset;
static FARPROC lpprocAbout, lpprocHelp, lpprocScore, lpprocF1,
	       lpprocHiScore, lpprocName, lpprocOptions,
               lpprocSave, lpprocLoad;
HBITMAP hBitmap[20], hDragBitmap, hBackBitmap, hDragmap;
HDC     hDragDC, hBackDC;
HCURSOR hHelpCursor;
HMENU   hMenu;

ScoreTable structScore;

BOOL FAR PASCAL MBScore(HWND hDlg, unsigned message, WORD wParam,
		   LONG lParam)
{
 char tempnum[8], szWinner[48];
 BOOL bDisplay = TRUE;
 HDC hDC;
 LONG oldcolor;
 int i, t, gtmp, numsize, tmpscore[2];

 switch(message)
   {
    case PM_CALLHELP:
		 if (lParam == 22222) //don't just jump for any WM_USER
{
       if (helpcode == 0)
	  WinHelp(hDlg, szHelpFile, HELP_INDEX, (DWORD)NULL);
       else 
	 {
	  WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	  helpcode = 0;
	 }
	 }
    break;

    case WM_INITDIALOG:
       if ((gameover < 0) || (startgame == 1)) bDisplay = FALSE;

       helpcode = 600;
       if (gameover > 0) 
	 {
	  ncards[0] = -1;
	  bUndo = FALSE;
	  EnableMenuItem(hMenu, IDM_UNDO, MF_GRAYED);
	 }
       else
	 {
	  SetDlgItemText(hDlg, 412, " ");
	  SetDlgItemText(hDlg, IDYES, "OK");
	  SetDlgItemText(hDlg, IDNO, "Cancel");
	 }
       gtmp = gameover -1;
 // Milestones
       for(i = 0;i < 2; i++)
	 {
	  tmpscore[i] = score[i];
	  if (!bDisplay)
	     lstrcpy(tempnum, szDash);
	  else
	     sprintf(tempnum, "%d",miles[i]);
	  SetDlgItemText(hDlg, 124 + 9 * i, (LPSTR)tempnum);
	  tmpscore[i] += miles[i];

 // Each safety
	  if ((nsafe[i] == 0) || !bDisplay) lstrcpy((LPSTR)tempnum, szDash);
	  else
	     sprintf(tempnum, "%d", nsafe[i] * 100);
	  SetDlgItemText(hDlg, 125 + 9 * i, (LPSTR)tempnum);

 // All 4 safeties
	  if ((nsafe[i] == 4) && bDisplay)
	    {
	     tmpscore[i] += 300;
	     lstrcpy((LPSTR)tempnum, "300");
	    }
	  else
	     lstrcpy((LPSTR)tempnum, szDash);
	  SetDlgItemText(hDlg, 126 + 9 * i, (LPSTR)tempnum);

 // Each coup fourre
	  if ((ncoup[i] == 0) || !bDisplay) lstrcpy((LPSTR)tempnum, szDash);
	  else
	     sprintf(tempnum, "%d",ncoup[i] * 300);
	  SetDlgItemText(hDlg, 127 + 9 * i, (LPSTR)tempnum);

 // Trip completed
	  if (gameover == i+1) lstrcpy((LPSTR)tempnum , "400");
	  else
	     lstrcpy((LPSTR)tempnum, szDash);
	  SetDlgItemText(hDlg, 128 + 9 * i, (LPSTR)tempnum);
	  if (gtmp == i)tmpscore[gtmp] += 400;

 // Delayed action
	  if((curcard > 100) && (gameover == i+1))
	     lstrcpy((LPSTR)tempnum, "300");
	  else
	     lstrcpy((LPSTR)tempnum, szDash);
	  SetDlgItemText(hDlg, 129 + 9 * i, (LPSTR)tempnum);
	  if((curcard > 100) && (gtmp == i)) tmpscore[gtmp] += 300;

 // Safe trip
	  if ((gtmp == i) && (two200s[i] == 0))
	    {
	     tmpscore[i] += 300;
	     lstrcpy((LPSTR)tempnum, "300");
	    }
	  else
	     lstrcpy((LPSTR)tempnum, szDash);
	  SetDlgItemText(hDlg, 130 + 9 * i , (LPSTR)tempnum);

 // Extension
	  if ((winmiles == 1000) && (gtmp == i))
	     lstrcpy((LPSTR)tempnum, "200");
	  else
	     lstrcpy((LPSTR)tempnum, szDash);
	  SetDlgItemText(hDlg, 131 + 9 * i, (LPSTR)tempnum);
	 }
       if (winmiles == 1000) tmpscore[gtmp] += 200;

 // Shutout
       if ((miles[0] == 0) && (gtmp == 1))
	 {
	  lstrcpy((LPSTR)tempnum , "500");
	  tmpscore[1] += 500;
	 }
       else
	  lstrcpy((LPSTR)tempnum, szDash);
       SetDlgItemText(hDlg, 141, (LPSTR)tempnum);
       if ((miles[1] == 0) && (gtmp == 0))
	 {
	  lstrcpy((LPSTR)tempnum, "500");
	  tmpscore[0] += 500;
	 }
       else
	  lstrcpy((LPSTR)tempnum, szDash);
       SetDlgItemText(hDlg, 132, (LPSTR)tempnum);

 // totals
       for (i = 0; i < 2; i++)
	 {
	  if(!bDisplay)
	     sprintf(tempnum, "%d",score[i]);
	  else
	     sprintf(tempnum, "%d", tmpscore[i]);
	  SetDlgItemText(hDlg, 142 + i, (LPSTR)tempnum);
	  if(gameover > 0)
	    {
	     score[i] = tmpscore[i];
	     gscore[i] += score[i];
	     sprintf(tempnum, "%d",gscore[i]);
	     SetDlgItemText(hDlg, 144 + i, (LPSTR)tempnum);
	    }
	  else
	    {
	     if (bDisplay == FALSE)
		sprintf(tempnum, "%d", gscore[i]);
	     else
		sprintf(tempnum, "%d", gscore[i] + tmpscore[i]);
	     SetDlgItemText(hDlg, 144 + i, (LPSTR)tempnum);
	    }
	 }
       if ((gameover > 0) &&
	  ((gscore[0] >= 5000) || (gscore[1] >= 5000)))
	 {
	  if (gscore[0] > gscore[1])
	     lstrcpy(szWinner, UserName);
	  else if (gscore[1] > gscore [0])
	     lstrcpy(szWinner, "Computer");
	  if (gscore[0] == gscore[1])
	     lstrcpy(szWinner,"It's a tie!");
	  else
	     lstrcat(szWinner, " wins the round!");
	  SetDlgItemText(hDlg, 413, (LPSTR)szWinner);

	  if (bDisplay) CheckForHiScore(gscore[0], gscore[1]);
	 }
 
       return TRUE;
    break;

    case WM_COMMAND:
       switch(wParam)
	 {
	  case IDYES:
	     if (gameover > 0)
	       {
		gameover = 0;
		startgame = 1;
	       }
	  break;
 
//	      case IDHELP:
//	      WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
//		  return TRUE;
//	      break;

	  default:
	     if (gameover > 0) 
		   {
		    gameover = -1;
		    SetStatusBar1( "Game over");
		   }
	  break;
	 }
       EndDialog(hDlg, TRUE);
       helpcode = 0;
       return TRUE;
    break;

    case WM_CLOSE:
       if (gameover > 0) gameover = -1;
       EndDialog(hDlg, TRUE);
       helpcode = 0;
       return TRUE;
    break;

    default:
       return FALSE;
    break;
   }
}

void newgame(HWND hWnd)
{
 int t, i;
 if (!bRestart) shuffle();
 bRestart = FALSE;
 bUndo = FALSE; // don't allow undo on game start
 EnableMenuItem(hMenu, IDM_UNDO, MF_GRAYED);
 for(t = 0; t < 6; t++)
   {
    scard[t][0] = scard[t][2] = -1;  // don't draw safeties
    hand[0][t] = deck[t];
    hand[1][t] = deck[t+6];
    scard[t][1] = hand[0][t];
   }
 scard[0][0] = scard[0][2] = scard [1][0] = scard [1][2] = scard[6][0] = scard[6][2] = 20; 
 scard[6][2] = 23; cango[0] = 0; // 0 means no go
 scard[7][1] = -1; cango[1] = 1; // 1 means able to go (but also no go)
 scard[6][0] = 23; // mileage
 scard[7][0] = 21; // draw
 scard[7][2] = 22; // discard
 scard[6][1] = 20;
 hand[0][6] = hand[1][6] = -1;
 curcard = 12; winmiles = 700;
 compturn = cdrawn = cclicked = gameover = startgame = 0;
 for(t = 0; t < 2; t++)
   {
    nbattle[t] = nsplim[t] = splim[t] = two200s[t] = nsafe[t] = 0;
    ncoup[t] = score[t] = miles[t] = battle[t] = rofw[t] = 0;
    ncards[t] = 6;
    if (gscore[t] >= 5000) gscore[0] = gscore[1] = 0;
    for(i = 0; i < 4; i++) safety[t][i] = -1;
   }
 SetStatusBar1( "Ready");

 if (bSortCards) SortCards(hWnd);
 InvalidateRect(hWnd, NULL, TRUE);
}

void SaveGameState()
{
 int i, j, k;
 k = 0;
 if (bUndo) for(i=0; i<74; i++) OldGameState[i] = GameState[i];

 for (i=0; i<2; i++)
    for (j=0; j<7; j++)
      {
       GameState[k] = hand[i][j];
       k++;    
      }
 for (i=0; i<8; i++)
    for (j=0; j<3; j++)
      {
       GameState[k] = scard[i][j];
       k++;    
      }
 for (i=0; i<2; i++)
    for (j=0; j<4; j++)
      {
       GameState[k] = safety[i][j];
       k++;
      }    
 for (i=0; i<2; i++)
    {
    GameState[k] = battle[i];
    k++;    
    GameState[k] = splim[i];
    k++;    
    GameState[k] = nbattle[i];
    k++;    
    GameState[k] = nsplim[i];
    k++;    
    GameState[k] = ncards[i];
    k++;    
    GameState[k] = cango[i];
    k++;    
    GameState[k] = two200s[i];
    k++;    
    GameState[k] = nsafe[i];
    k++;    
    GameState[k] = ncoup[i];
    k++;    
    GameState[k] = rofw[i];
    k++;    
    GameState[k] = miles[i];
    k++;    
    GameState[k] = score[i];
    k++;    
    GameState[k] = gscore[i];
    k++;    
   }
 GameState[k] = curcard;
    k++;    
 GameState[k] = winmiles;
}

//UndoLastMove
//
//Parameters: bLoadGame - TRUE = we just loaded the game
//                        FALSE= doing Move / Undo
//
void UndoLastMove(BOOL bLoadGame)
{
 int i, j, k;

 k = 0;
 if (!bLoadGame)
    for(i=0; i<74; i++) GameState[i] = OldGameState[i];
 else   //if load game when game over, reset
    {
     compturn = 0;
     gameover = 0;
     SetStatusBar1( "Ready");
    }
     
 bUndo = FALSE;
 EnableMenuItem(hMenu, IDM_UNDO, MF_GRAYED);

 for (i=0; i<2; i++)
    for (j=0; j<7; j++)
       hand[i][j] = GameState[k++];
 for (i=0; i<8; i++)
    for (j=0; j<3; j++)
       scard[i][j] = GameState[k++];
 for (i=0; i<2; i++)
    for (j=0; j<4; j++)
       safety[i][j] = GameState[k++];
 for (i=0; i<2; i++)
   {
    battle[i] = GameState[k++];
    splim[i] = GameState[k++];
    nbattle[i] = GameState[k++];
    nsplim[i] = GameState[k++];
    ncards[i] = GameState[k++];
    cango[i] = GameState[k++];
    two200s[i] = GameState[k++];
    nsafe[i] = GameState[k++];
    ncoup[i] = GameState[k++];
    rofw[i] = GameState[k++];
    miles[i] = GameState[k++];
    score[i] = GameState[k++];
    gscore[i] = GameState[k++];
   }
 curcard = GameState[k++];
 winmiles = GameState[k++];
 cdrawn = cclicked = 0;
}

void HelpUser(HWND hWnd, int htype)
{
 if (htype > 104) MessageBeep((WORD)0);
 if (htype == 106)
    DialogBox(hInst, MAKEINTRESOURCE(MBHELP), hWnd, lpprocHelp);
}

void DrawCard(HDC hDC, int lx, int ly, int cnum)
{
 int x,y;
 x = lx * (xcard+xgap) + xgap; y = ly * (ycard+ygap) + ygap;
 DrawBitmap(hDC, (short)x, (short)y, hBitmap[cnum]);
}

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
	      LPSTR lpszCmdLine, int cmdShow)
{
    MSG   msg;
    HWND  hWnd;
    HDC   hDC;
    TEXTMETRIC tm;
    char    *tempbmname;
    short   xdraw, ydraw, i;
    WORD wTimer;

    if (!hPrevInstance) {
	if (!MBInit(hInstance))
	    return FALSE;
	}

    yscrn = GetSystemMetrics(SM_CYSCREEN);
    ygap = GetSystemMetrics(SM_CYMENU);
    mousein = (short)GetSystemMetrics(SM_MOUSEPRESENT);
    if (yscrn >= 480) /* VGA or more? */
      {
       ycard = 78;
       ygap += 2;
      }
    else    /* 350 EGA or 400 VGA extended */
      {
       ycard = 60;
      }
    ysize = ycard + ygap;
    hMenu = LoadMenu(hInstance,lpszMenuName);
    
    /* Save instance handle for DialogBox */
    hInst = hInstance;

    hWnd = CreateWindow((LPSTR)szAppName,
			(LPSTR)szMessage,
				    WS_OVERLAPPEDWINDOW,
			0,
			0,
			(xcard << 3)+10*xgap,
			7*ygap+3*ycard,
			(HWND)NULL,        // no parent
			(HMENU)hMenu,      // use my menu
			(HANDLE)hInstance, // handle to window instance
			(LPSTR)NULL);        // no params to pass on
    hWndMain = hWnd;

    hInfoWnd = CreateWindow("MBInfo",
		  NULL, WS_CHILD | WS_VISIBLE,
		  0, 4*ygap+3*ycard-3-2*GetSystemMetrics(SM_CYFRAME),
		  (xcard << 3)+10*xgap, ygap+3,
		  hWnd, NULL,
		  hInstance, NULL);

    CheckMenuItem(hMenu, IDM_NOHELP, MF_CHECKED);
    SetStatusBar1("Starting...");

    // Bind callback functions with module instance
    lpprocAbout = MakeProcInstance((FARPROC)About, hInstance);
    lpprocScore = MakeProcInstance((FARPROC)MBScore, hInstance);
    lpprocHelp = MakeProcInstance((FARPROC)MBHelp, hInstance);
    lpprocF1 = MakeProcInstance((FARPROC)ContextHelp, hInstance);
    lpprocHiScore = MakeProcInstance((FARPROC)MBHiScore, hInstance);
    lpprocName = MakeProcInstance((FARPROC)MBName, hInstance);
    lpprocOptions = MakeProcInstance((FARPROC)MBOptions, hInstance);
    lpprocSave = MakeProcInstance((FARPROC)MBSave, hInstance);
    lpprocLoad = MakeProcInstance((FARPROC)MBLoad, hInstance);

    hHelpCursor = LoadCursor(hInstance, "HelpCursor");
    lpfnOldHook = SetWindowsHook(WH_MSGFILTER, lpprocF1);

    Setdata();

    hDC=GetDC(hWnd);
    GetTextMetrics(hDC, &tm);
    ydrop = tm.tmInternalLeading + tm.tmExternalLeading;
    for(i = 0; i < 19; i++)
      {
       hBitmap[i]=LoadBitmap(hInstance, (LPSTR)bmlist[i]); 
       if (hBitmap[i] == NULL) MessageBox(hWnd,
	     (LPSTR)"Not enough memory to load bitmap!",
	     NULL, MB_OK | MB_ICONHAND);
      }
    if (yscrn >= 480) hBitmap[19] = LoadBitmap(hInstance, (LPSTR)"mbblank");

    hDragDC = CreateCompatibleDC(hDC); // make temp drag area
    hDragmap = CreateCompatibleBitmap(hDC, (xcard << 1), (ycard << 1));
    SelectObject (hDragDC, hDragmap);
    hBackDC = CreateCompatibleDC(hDC); // make backgd save area
    hBackBitmap = CreateCompatibleBitmap(hDC, xcard, ycard);
    SelectObject (hBackDC, hBackBitmap);
       if (hBackBitmap == NULL) MessageBox(hWnd,
	     (LPSTR)"Not enough memory to create bitmap!"
	     ,NULL,MB_OK | MB_ICONHAND);

    ReleaseDC(hWnd,hDC);

    GetHighScores(hWnd); 
    
    newgame(hWnd);
    if(bNoLogon)  //if no logon screen, get default user name
      {
       GetPrivateProfileString((LPSTR)"Options", (LPSTR)"DefName",
          (LPSTR)"noname", (LPSTR)UserName, 31, (LPSTR)szIniFile);               
      }
    ShowWindow(hWnd,cmdShow);
    if (mousein == 0) ShowCursor(1);
    UpdateWindow(hWnd);

    if(!bNoLogon)  //bring up logon dbox if not auto-logon
      {    
       DialogBox(hInst, MAKEINTRESOURCE(MBNAME), hWnd, lpprocName);
       InvalidateRect(hWnd, NULL, TRUE);
      }    
    SaveGameState();

    while(GetMessage((LPMSG)&msg, NULL, 0, 0))
      {
       TranslateMessage((LPMSG)&msg);
       DispatchMessage((LPMSG)&msg);

       if (gameover >0)
	  SendMessage(hWnd, WM_COMMAND, IDM_SCOREDLG, NULL);
       if (startgame == 1) 
	  {
	   newgame(hWnd);
	   SaveGameState();
	  }

       if ((!bTimer) && (compturn == 1) && (gameover == 0))
	 {
	  bTimer = TRUE;
	  SetStatusBar1("Computer is playing...");
	  wTimer = SetTimer(hWnd, 42, 650, NULL);
	  if (wTimer == 0)
	    {
	     GoComp(hWnd);
	     bTimer = FALSE;
	    }
	 }
      }
    return (int)msg.wParam;
}


void KillDrag(hWnd)
HWND hWnd;
  {
   HDC hDC;
   hDC = GetDC(hWnd);
   BitBlt(hDC, dragpt.x, dragpt.y, xcard, ycard, hBackDC, 0, 0, SRCCOPY);
   ReleaseDC(hWnd, hDC);
   SetCursor(LoadCursor(NULL, IDC_ARROW));
   bInDrag = FALSE;
   dragoffset.x = 0; dragoffset.y = 0;
  }


/* Procedures which make up the window class. */
LONG FAR PASCAL MBWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
UINT wParam;
LONG lParam;

{
    HDC hDC;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    char szBuffer[80];
    int  i, tx, ty, bx, by, tscard, tmpcard;
    static POINT oldcur;
    RECT trect;
    HPEN hPrev;

    switch (message)
    {
    case PM_CALLHELP:
       if (helpcode == 0)
	  WinHelp(hWnd, szHelpFile, HELP_INDEX, (DWORD)NULL);
       else 
	 {
	  WinHelp(hWnd, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	  helpcode = 0;
	 }
    break;

    case WM_TIMER:
       KillTimer(hWnd, 42);
       bTimer = FALSE;
       GoComp(hWnd);
       SetStatusBar1( "Ready");
       SetStatusBar2();
       bUndo = TRUE;
       SaveGameState();
       EnableMenuItem(hMenu, IDM_UNDO, MF_ENABLED);
    break;

    case WM_KEYDOWN:
       switch (wParam)
       {
	case VK_LEFT:
	   xcursor = ((xcursor + (short)7) % 8);
	   MoveCur(hWnd);
	break;

	case VK_RIGHT:
	   xcursor = ((xcursor + (short)1) % 8);
	   MoveCur(hWnd);
	break;

	case VK_UP:
	   ycursor = ((ycursor + (short)2) % 3);
	   MoveCur(hWnd);
	break;

	case VK_DOWN:
	   ycursor = ((ycursor + (short)1) % 3);
	   MoveCur(hWnd);
	break;

	case VK_F1:
	   /* If Shift-F1, turn help mode on and set help cursor */ 
	   if (GetKeyState(VK_SHIFT) < 0)
	     {
	      bHelp = TRUE;
	      SetCursor(hHelpCursor);
	      return (DefWindowProc(hWnd, message, wParam, lParam));
	     }
	   else
	     WinHelp(hWnd, szHelpFile, HELP_INDEX, (DWORD)NULL);
	break;

	case VK_ESCAPE:
	   bHelp = FALSE;

	case VK_SPACE:
	case VK_RETURN:
	   if(bInDrag)
	      KillDrag(hWnd);
	   GoPlayer(hWnd);
	break;
       }
       return DefWindowProc(hWnd, message, wParam, lParam);
    break;
   
    case WM_KILLFOCUS:
    case WM_NCLBUTTONUP:
       if (bInDrag) 
	  {
	   KillDrag(hWnd);
	   SetCard(hWnd, cclicked - 1, 1, hand[0][cclicked-1]);
	   cclicked = 0;
	   } 
       return DefWindowProc(hWnd, message, wParam, lParam);
    break;
      
    case WM_LBUTTONDBLCLK:
       bDblClick = TRUE;
       GoPlayer(hWnd);
    break;
       
    case WM_LBUTTONUP:
       if (bInDrag)
	 {
	  bButtonUp = TRUE;
	  
	      if (bJustInDrag)
		 {
		  bJustInDrag = FALSE;
		  bInDrag = FALSE;
		  SetCursor(LoadCursor(NULL, IDC_ARROW));
		 }
	      else KillDrag(hWnd);
  
	  GoPlayer(hWnd);
	 }
       return DefWindowProc(hWnd, message, wParam, lParam);  
    break;

    case WM_MOUSEMOVE:
	if(bInDrag && !bJustInDrag)
	  {
	   BITMAP bm;
	   HBITMAP hBitmap;
	   HDC hMemDC;
	   POINT pt;
	   RECT msrect;
	   POINT mspt, oldpt;
	   LPPOINT lpmspt;
	   DWORD mspos;
	   int xorigin, yorigin, xdiff, ydiff;

	   mspos = GetMessagePos();
	   mspt.x=LOWORD(mspos); 
	   mspt.y=HIWORD(mspos) ; 
	   lpmspt = (LPPOINT)&mspt;
	   ScreenToClient(hWnd, lpmspt);
	   mspt = *lpmspt;
	   mspt.x -= dragoffset.x;
	   mspt.y -= dragoffset.y;
	   oldpt = dragpt;
	   xdiff = oldpt.x - mspt.x; ydiff = oldpt.y - mspt.y;
	   xorigin = mspt.x; yorigin = mspt.y;
	   if (xdiff < 0)
	     { xdiff = 0 - xdiff; xorigin = oldpt.x; }
	   if (ydiff < 0)
	     { ydiff = 0 - ydiff; yorigin = oldpt.y; }
	   if((xdiff > xcard) || (ydiff > ycard))
	     {
	      hDC = GetDC(hWnd);
	      BitBlt(hDC, dragpt.x, dragpt.y, xcard, ycard, hBackDC, 0, 0, SRCCOPY);
	      dragpt = mspt;
	      BitBlt(hBackDC, 0, 0, xcard, ycard, hDC, dragpt.x, dragpt.y, SRCCOPY);
	      DrawBitmap(hDC, dragpt.x, dragpt.y, hDragBitmap);
	      ReleaseDC(hWnd, hDC);
	     }
	   else
	     {
	      hDC = GetDC(hWnd);
	      BitBlt(hDragDC, 0, 0, xcard + xdiff, ycard + ydiff, hDC, xorigin, yorigin, SRCCOPY);      // get the combined old & new drag areas

	      BitBlt(hDragDC, oldpt.x - xorigin, oldpt.y - yorigin, xcard, ycard, hBackDC, 0, 0, SRCCOPY);    // clean up old drag area
	      dragpt = mspt;

	      BitBlt(hBackDC, 0, 0, xcard, ycard, hDragDC, dragpt.x - xorigin, dragpt.y - yorigin, SRCCOPY);    // get the new cleanup bitmap

	      DrawBitmap(hDragDC, dragpt.x - xorigin, dragpt.y - yorigin, hDragBitmap);        // draw the new drag bitmap
	      BitBlt(hDC, xorigin, yorigin, xcard + xdiff, ycard + ydiff, hDragDC, 0, 0, SRCCOPY);   // update the screen

	      ReleaseDC(hWnd, hDC);
	     }
	   }
	 return DefWindowProc(hWnd, message, wParam, lParam);  
    break;

    case WM_LBUTTONDOWN:
       if (bHelp) 
	  {
	   DoHelp(hWnd);
	   break;
	  }
	if (bInDrag) 
	  {
	   KillDrag(hWnd);
	   SetCard(hWnd, cclicked - 1, 1, hand[0][cclicked-1]);
	   cclicked = 0;  
	   }
	  else
	if (gameover < 3)  GoPlayer(hWnd);

       return DefWindowProc(hWnd, message, wParam, lParam);
    break;

    case WM_SETCURSOR:
       /* In help mode need to reset the cursor in response
	to every WM_SETCURSOR message.Otherwise, by default, Windows will reset the cursor to that of the window class. */

       if (bHelp)
	 {
	  SetCursor(hHelpCursor);
	  break;
	 }
       if (bInDrag)
	 {
	  SetCursor(NULL);
	  break;
	 }
       return (DefWindowProc(hWnd, message, wParam, lParam));
       break;

    case WM_INITMENU:
       if (bHelp) 
	 {
	  SetCursor(hHelpCursor);
	 } 
       return (TRUE);

    case WM_ENTERIDLE:
       if ((wParam == MSGF_MENU) && (GetKeyState(VK_F1) & 0x8000)) 
	 {
	  bHelp = TRUE;
	  PostMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0L);
	 }
    break;

    case WM_COMMAND:
    switch(wParam)
      {
       case IDM_ABOUT:
	  DialogBox(hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout);
       break;

       case IDM_RESTART:
	  if(bCanRestart)
	  if(MessageBox(hWnd,"Do you want to restart the current game?", "Restart?",
MB_YESNO | MB_DEFBUTTON2) == IDYES)
	    {
	     bRestart = TRUE;
	     startgame = 1;
	     gscore[0] = 0;
	     gscore[1] = 0;
	    }
       break;

       case IDM_NEWGAME:
	 if(MessageBox(hWnd, "Do you want to quit the current game and start a new one?",
	   "New Game?", MB_YESNO | MB_DEFBUTTON2) == IDYES)
	 {
	  bRestart = FALSE;
	  startgame = 1;
	  gscore[0] = 0;
	  gscore[1] = 0;
	 }
       break;

       case IDM_LOADGAME:
	  DialogBox(hInst, MAKEINTRESOURCE(MBLOAD), hWnd, lpprocLoad);
       break;

       case IDM_SAVEGAME:
          if (gameover != -1)  //don't save completed game
	  DialogBox(hInst, MAKEINTRESOURCE(MBSAVE), hWnd, lpprocSave);
       break;

       case IDM_UNDO:
	  if(bUndo)
	    {
	     UndoLastMove(FALSE);
	     InvalidateRect(hWnd, NULL, TRUE);
	    }
       break;

       case IDM_SCORES:
	  DialogBox(hInst, MAKEINTRESOURCE(MBHISCORE), hWnd, lpprocHiScore);
       break;

       case IDM_SCOREDLG:
	  DialogBox(hInst,MAKEINTRESOURCE(MBSCORE), hWnd, lpprocScore);
       break;

       case IDM_OPTIONS:
	  DialogBox(hInst,MAKEINTRESOURCE(MBOPTIONS), hWnd, lpprocOptions);
	  if (bSortCards) SortCards(hWnd);
       break;

       case IDM_NOHELP:
       case IDM_BEEPS:
       case IDM_MSGS:
	  CheckMenuItem(hMenu, (WORD)htype, MF_UNCHECKED);
	  htype = (int)wParam;
	  CheckMenuItem(hMenu, (WORD)htype, MF_CHECKED);
       break;

       case IDM_EXIT:
	if(MessageBox(hWnd, "Do you want to exit Mile Bones?",
		"Exit?", MB_YESNO | MB_DEFBUTTON2) == IDYES)
	   {
	  DestroyWindow(hWnd);
	 }
       break;

       case IDM_INDEX:
	  WinHelp(hWnd, szHelpFile, HELP_INDEX, (DWORD)NULL);
	  break;

       default:
	  return DefWindowProc(hWnd, message, wParam, lParam);
       break;
      }
    break;

    case WM_DESTROY:
       WriteHighScores();   // save out changes in score table
       UnhookWindowsHook(WH_MSGFILTER,lpprocF1);   // rip out F1 checker

      for(i = 0;i <= 19;i++) DeleteObject(hBitmap[i]);    // pull bitmaps

      if (mousein == 0) ShowCursor(0);     // pull cursor if no mouse
      WinHelp(hWnd, szHelpFile, HELP_QUIT, (DWORD)NULL);  // pull help

      DeleteDC(hDragDC);    // kill temporary drag location
      DeleteObject(hDragmap);
      DeleteDC(hBackDC);    // kill background save DC
      DeleteObject(hBackBitmap);

      PostQuitMessage(0);
    break;

    case WM_PAINT:
       BeginPaint(hWnd, &ps);
       hDC = ps.hdc;
       SetBkMode(hDC, TRANSPARENT);
       if(lstrlen((LPSTR)UserName)==0)
	  lstrcpy((LPSTR)UserName,"noname");
       lstrcpy((LPSTR)szBuffer, (LPSTR)UserName);
       lstrcat((LPSTR)szBuffer, "'s battle:");
       TextOut(hDC, xgap, ydrop, (LPSTR)szBuffer, 
	  lstrlen((LPSTR)szBuffer));
       TextOut(hDC, xgap, ydrop + (ysize << 1), (LPSTR)szBuffer,
	  wsprintf(szBuffer, "Computer's battle:"));
       lstrcpy((LPSTR)szBuffer, (LPSTR)UserName);
       lstrcat((LPSTR)szBuffer, "'s hand:");
       TextOut(hDC, xgap, ydrop + ysize, (LPSTR)szBuffer,
	  lstrlen((LPSTR)szBuffer));
       TextOut(hDC,xgap+7*xsize,ydrop,(LPSTR)szBuffer,
	  wsprintf(szBuffer, "Draw:"));
       TextOut(hDC,xgap+7*xsize,ydrop+2*ysize,(LPSTR)szBuffer,
	  wsprintf(szBuffer, "Discard:"));

       for(ty = 0;ty < 3; ty++)
	  for(tx = 0; tx < 8; tx++)
	    {
	     bx = tx * xsize + xgap; by = ty * ysize + ygap;
	     tscard = scard[tx][ty];
	     tmpcard = tscard & 31;
	     switch(tmpcard)
	       {
		case 23:      /* mileage */
		case 22:      /* discard */
		   EraseCard(hDC, tx, ty);
		   if(tmpcard ==23) TextOut(hDC, bx, by - ygap + ydrop, (LPSTR)szBuffer,
		      wsprintf(szBuffer, (LPSTR)"Miles:"));
		break;
	       }

	     if ((tmpcard < 19) && (tscard > -1)) /* draw card */
	       {
		if (!((tscard >63) && (bInDrag))) DrawCard(hDC, tx, ty, tmpcard);
		if (tscard > 63)  /* current selection */
		  {
		   if (bJustInDrag)
		     {
		      hDragBitmap = hBitmap[tscard-64] ;
		      EraseCard(hDC, tx, ty);
		      dragoffset.x = dragpt.x - (tx * (xcard+xgap) + xgap);
		      dragoffset.y = dragpt.y - (ty * (ycard+ygap) + ygap);
		     }
		  else
		     {
		      trect.left = bx;
		      trect.right = bx + xcard;
		      trect.top = by;
		      trect.bottom = by + ycard;
		      InvertRect(hDC, &trect);
		     }
		  }
		if ((tscard > 31) && (tscard < 63))  /* coup fourre */
		  {
		   hPrev = SelectObject(hDC, CreatePen(0, 1, RGB(255,0,0)));
 
		   MoveToEx(hDC, bx-1, by-1,(LPPOINT)NULL);

		   LineTo(hDC, bx+xcard, by-1);
		   LineTo(hDC, bx+xcard, by+ycard);
		   LineTo(hDC, bx-1, by+ycard);
		   LineTo(hDC, bx-1, by-1);
		   DeleteObject(SelectObject(hDC, hPrev));
		  }
	       }
 
	     switch(tmpcard)
	       {
		case 23:     /* mileage */
		   i = ty / 2;
		   EraseCard(hDC, tx, ty);
		   TextOut(hDC, bx + xgap, by + ygap,(LPSTR)szBuffer,
		      wsprintf(szBuffer, (LPSTR)"%d",miles[i]));
		break;
			case 22:     /* discard */
			    if(yscrn >= 480) DrawCard(hDC, tx, ty, 19);
			break;
		case 21:     /* draw deck*/
		   if(curcard > 100)
			     {
		      EraseCard(hDC, tx, ty);
		      TextOut(hDC, bx + xgap, by + ygap, (LPSTR)szBuffer,
			wsprintf(szBuffer, (LPSTR)"Out"));
			      }
			   else if (yscrn >= 480) DrawCard(hDC, tx, ty, 19);
			   else EraseCard(hDC, tx, ty);
		break;
		case 20:     /* erase */
		case 19:
		   EraseCard(hDC, tx, ty);
		   scard[tx][ty] = 19;
		break;
	       }
	    }

	EndPaint(hWnd, &ps);
	if (bJustInDrag)
	  {
	   hDC = GetDC(hWnd);
	   dragpt.x -= dragoffset.x;
	   dragpt.y -= dragoffset.y;
	   if((dragoffset.x !=0) && (dragoffset.y !=0))
	     {
	      BitBlt(hBackDC, 0, 0, xcard, ycard, hDC, dragpt.x, dragpt.y, SRCCOPY);
	  
	      DrawBitmap(hDC, dragpt.x, dragpt.y, hDragBitmap);
	     }
	   ReleaseDC(hWnd, hDC);
	   bJustInDrag = FALSE;
	  } 
    break;

    default:
	return DefWindowProc( hWnd, message, wParam, lParam );
    break;
    }
    return(0L);
}

LONG FAR PASCAL MBInfoWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
UINT wParam;
LONG lParam;
{
    PAINTSTRUCT ps;
    int  i;
    HPEN hPrev;
    char szStatusBar2[40];

    switch (message)
    {
    case WM_PAINT:
       BeginPaint(hWnd, &ps);

       MoveToEx(ps.hdc, 3, ygap, (LPPOINT)NULL);

       hPrev = SelectObject(ps.hdc, CreatePen(0, 0, RGB(128, 128, 128)));

       LineTo(ps.hdc, 3, 0);
       LineTo(ps.hdc, ((xcard + xgap) << 2), 0);
       DeleteObject(SelectObject(ps.hdc,hPrev));
       hPrev = SelectObject(ps.hdc, CreatePen(0, 0, RGB(255, 255, 255)));

       LineTo(ps.hdc, ((xcard + xgap) << 2), ygap+1);
       LineTo(ps.hdc, 3, ygap+1);
       DeleteObject(SelectObject(ps.hdc,hPrev));

       MoveToEx(ps.hdc, ((xcard + xgap) << 2)+3, ygap, (LPPOINT)NULL);

       hPrev = SelectObject(ps.hdc, CreatePen(0, 0, RGB(128, 128, 128)));

       LineTo(ps.hdc, ((xcard + xgap) << 2)+3, 0);
       LineTo(ps.hdc, (xcard << 3)+9*xgap, 0);
       DeleteObject(SelectObject(ps.hdc,hPrev));
       hPrev = SelectObject(ps.hdc, CreatePen(0, 0, RGB(255, 255, 255)));

       LineTo(ps.hdc, (xcard << 3)+9*xgap, ygap+1);
       LineTo(ps.hdc, ((xcard + xgap) << 2)+3, ygap+1);
       DeleteObject(SelectObject(ps.hdc,hPrev));

       SetBkMode(ps.hdc, TRANSPARENT);
       TextOut(ps.hdc, xgap, 2, (LPSTR)szStatusBar,  lstrlen((LPSTR)szStatusBar));

       if(curcard > 100)
	  TextOut(ps.hdc, (xcard << 2)+5*xgap, 2,
	      (LPSTR)szStatusBar2, wsprintf(szStatusBar2, "Draw deck is empty."));
       else if (curcard == 100)
	      TextOut(ps.hdc, (xcard << 2)+5*xgap, 2,
	      (LPSTR)szStatusBar2, wsprintf(szStatusBar2, "One card left."));
       else
	  TextOut(ps.hdc, (xcard << 2)+5*xgap, 2,
	      (LPSTR)szStatusBar2, wsprintf(szStatusBar2, (LPSTR)"%d cards left.",101 - curcard));

       EndPaint(hWnd, &ps);
    break;

    default:
	return DefWindowProc( hWnd, message, wParam, lParam );
    break;
    }
    return(0L);
}

