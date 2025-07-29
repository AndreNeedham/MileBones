/*  mbcore.c
    Mile Bones Game
    1991 Andre Needham
 */

#include <windows.h>
#include <stdio.h>
#include "mb.h"

extern HWND hInfoWnd;
extern short xcard, ycard, xgap, ygap, xsize, ysize, xcursor, ycursor;
extern int scard[8][3];
extern short deck[101], numcards[19], mileval[19], hand[2][7], startgame;
extern char *bmlist[19];
extern ScoreTable structScore;
extern char szAppName[],  szMessage[], lpszMenuName[], UserName[],
	    szStatusBar[];
extern BOOL bHelp, bDragDrop, bSortCards, bCanRestart, bNoLogon;

BOOL MBInit(hInstance)
HANDLE hInstance;
{
    PWNDCLASS   pMBClass;

    LoadString(hInstance, IDSNAME, (LPSTR)szAppName, 16);
    LoadString(hInstance, IDSTITLE, (LPSTR)szMessage, 16);
    
    pMBClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

    pMBClass->hCursor        = LoadCursor(NULL, IDC_ARROW);
    pMBClass->hIcon          = LoadIcon(hInstance, "MB");
    pMBClass->lpszMenuName   = (LPSTR)lpszMenuName;
    pMBClass->lpszClassName  = (LPSTR)szAppName;
    pMBClass->hbrBackground  = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    pMBClass->hInstance      = hInstance;
    pMBClass->style          = CS_DBLCLKS;
    pMBClass->lpfnWndProc    = MBWndProc;
    pMBClass->cbClsExtra     = 0;
    pMBClass->cbWndExtra     = 0;

    if (!RegisterClass((LPWNDCLASS)pMBClass))
	     return FALSE;

    pMBClass->hIcon          = NULL;
    pMBClass->lpszMenuName   = NULL;
    pMBClass->lpszClassName  = "MBInfo";
    pMBClass->style          = NULL;
    pMBClass->lpfnWndProc    = MBInfoWndProc;

    if (!RegisterClass((LPWNDCLASS)pMBClass))
	     return FALSE;

    LocalFree((HANDLE)pMBClass);
    return TRUE;       
}

DWORD FAR PASCAL ContextHelp(nCode, wParam, lParam) 
int nCode; 
WORD wParam; 
LONG lParam; 
{ 
   char szTmp[8];
   MSG tmpmsg; 
   if (nCode < 0) return DefHookProc(nCode, wParam, lParam, 
&lpfnOldHook); 
 
   tmpmsg = *(LPMSG)lParam; 
   if(tmpmsg.message == WM_KEYDOWN)
     {
      if(tmpmsg.wParam == VK_F1) 
	{ 
	 PostMessage(GetParent(tmpmsg.hwnd),PM_CALLHELP, tmpmsg.hwnd, 22222L); 
	 return 1L; 
	}
      if(tmpmsg.wParam == VK_ESCAPE)
	{
	 GetClassName(tmpmsg.hwnd, szTmp, 7); 
	 if(lstrcmp(szTmp,"MB") != 0)
	  {
	   PostMessage(GetParent(tmpmsg.hwnd),WM_CLOSE,0,0);
	   return 1L;
	  }
         return 0L;
	}
      return 0L;
     }  
   else return 0L; 
} 

void DoHelp(hWnd)
HWND hWnd;
       {
 DWORD mousepos;
 POINT mousept, tmppt;
 LPPOINT lpmousept;
 int   tindex, tmpHelpID;

	  bHelp = FALSE;
	  mousepos = GetMessagePos();
	  mousept.x=LOWORD(mousepos);
	  mousept.y=HIWORD(mousepos) ;
	  lpmousept = (LPPOINT)&mousept;
	  ScreenToClient(hWnd, lpmousept);
	  mousept = *lpmousept;

	  tmppt = mousept;
	  tmppt.x -= xgap; tmppt.y -= ygap;
	  if ((tmppt.x < 0) || (tmppt.y < 0) 
	     || (tmppt.x % xsize >= xcard) || (tmppt.y % ysize >= ycard))
	    {tmppt.x = 7; tmppt.y = 2;}
	  else
	  {tmppt.x /= xsize; tmppt.y /= ysize;}
	  tindex = (tmppt.y << 3) + tmppt.x;
	  switch(tindex)
	     {
	      case 0:
		 tmpHelpID = 701;
	      break;
	      case 1:
		 tmpHelpID = 702;
	      break;
	      case 2:
	      case 3:
	      case 4:
	      case 5:
		 tmpHelpID = 703;
	      break;
	      case 6:
		 tmpHelpID = 704;
	      break;
	      case 7:
		 tmpHelpID = 705;
	      break;
	      case 8:
	      case 9:
	      case 10:
	      case 11:
	      case 12:
	      case 13:
	      case 14:
		 tmpHelpID = 706;
	      break;
	      case 23:
		 tmpHelpID = 707;
	      break;
	      case 16:
		 tmpHelpID = 708;
	      break;
	      case 17:
		 tmpHelpID = 709;
	      break;
	      case 18:
	      case 19:
	      case 20:
	      case 21:
		 tmpHelpID = 710;
	      break;
	      case 22:
		 tmpHelpID = 711;
	      break;
	      default:
		 tmpHelpID = 0;
	      break;              
	     }
	  if(tmpHelpID > 0)
	  WinHelp( hWnd, "mb.hlp", HELP_CONTEXT,
	     (DWORD) tmpHelpID);
	 }


float frand()
{
 return (float)rand()/(float)32768;
}

void SetStatusBar1(LPSTR szTemp)
{
 RECT rEraseArea;

 rEraseArea.left = 4;
 rEraseArea.right = ((xcard + xgap) << 2) - 1;
 rEraseArea.top = 1;
 rEraseArea.bottom = ygap;
 lstrcpy(szStatusBar, szTemp);
 InvalidateRect(hInfoWnd, (LPRECT)&rEraseArea, TRUE);
} 

void SetStatusBar2()
{
 RECT rEraseArea;

 rEraseArea.left = ((xcard + xgap) << 2) + 4;
 rEraseArea.right = (xcard << 3) + 9 * xgap - 1;
 rEraseArea.top = 1;
 rEraseArea.bottom = ygap;
 InvalidateRect(hInfoWnd, (LPRECT)&rEraseArea, TRUE);
} 

HBITMAP CopyBitmap(hDC, xStart, yStart)
HDC hDC;
short xStart,yStart;
{
 BITMAP bm;
 HBITMAP hBitmap;
 HDC hMemDC;
 POINT pt;

 hMemDC = CreateCompatibleDC(hDC);
 hBitmap = CreateCompatibleBitmap(hDC, xcard, ycard);
 SelectObject (hMemDC, hBitmap);
 BitBlt(hMemDC, 0, 0, xcard, ycard, hDC, xStart, yStart, SRCCOPY);
 DeleteDC(hMemDC);
 return (hBitmap);
}

void DrawBitmap(hDC, xStart, yStart, hBitmap)
HDC hDC;
short xStart,yStart;
HBITMAP hBitmap;
{
 BITMAP bm;
 HDC hMemDC;
 POINT pt;

 hMemDC = CreateCompatibleDC(hDC);
 SelectObject(hMemDC, hBitmap);
 SetMapMode(hMemDC, GetMapMode(hDC));
 GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
 pt.x = bm.bmWidth;
 pt.y = bm.bmHeight;
 DPtoLP(hDC, &pt,1);
 BitBlt(hDC, xStart, yStart, pt.x, pt.y, hMemDC, 0, 0, SRCCOPY);
 DeleteDC(hMemDC);
}

void EraseCard(hDC,x,y)
HDC hDC;
int x,y;
{
 int tx,ty;
 HPEN hPrev;
 tx = xgap + xsize * x;
 ty = ygap + ysize * y;
 Rectangle(hDC, tx, ty, tx + xcard,ty + ycard);
}

void SetCard(hWnd,x,y,cnum)
HWND hWnd;
int x,y,cnum;
{
 RECT trect;
 scard[x][y] = cnum;
 trect.left = xgap - 1 + xsize * x;
 trect.top = ygap - 1 + ysize * y;
 trect.right = trect.left + xcard + 2;
 trect.bottom = trect.top + ycard + 2;
 InvalidateRect(hWnd, (LPRECT)&trect, FALSE);
}

void MoveCur(hWnd)
HWND hWnd;
  {
   HDC hDC;
   WORD tempx, tempy;

   WINDOWPLACEMENT orgrect;
   GetWindowPlacement(hWnd,(WINDOWPLACEMENT *)&orgrect);
   tempx = orgrect.rcNormalPosition.left + (xgap << 1) + xcursor * xsize;
   tempy = orgrect.rcNormalPosition.top + (ygap << 2) + ycursor * ysize;

   SetCursorPos(tempx, tempy);
  }

void shuffle()
{
 short tmpdeck[101];
 int s, t, x, qq;
 s = 0;
 for(t = 0; t < 19; t++)   /* make unshuffled deck */
   {
    for(x = 0; x < numcards[t]; x++) tmpdeck[x+s] = t;
    s += numcards[t];
   }
 for(t = 0;t < 101;t++)
   {
    qq=(int)(frand()*(101 - t));   /* pick a card, any card */
    deck[t] = tmpdeck[qq];       /* put into shuffled deck */
    tmpdeck[qq] = tmpdeck[100 - t];
  /* move last card in unshuffled deck to card spot we just pulled */
   }
 bCanRestart = TRUE;  //now that we've got a deck to restart on...
}

void Setdata()
{
 char szIniFile[] = "mb.ini";

 bmlist[0]="mbxtank"; numcards[0]=1;   bmlist[1]="mbdace";   numcards[1]=1;
 bmlist[2]="mbpprf";  numcards[2]=1;   bmlist[3]="mbrway";   numcards[3]=1;
 bmlist[4]="mbgas";   numcards[4]=6;   bmlist[5]="mbrep";    numcards[5]=6;
 bmlist[6]="mbstire"; numcards[6]=6;   bmlist[7]="mbgo";     numcards[7]=14;
 bmlist[8]="mbeolim"; numcards[8]=6;   bmlist[9]="mbnogas";  numcards[9]=2;
 bmlist[10]="mbacc";  numcards[10]=2;  bmlist[11]="mbftire"; numcards[11]=2;
 bmlist[12]="mbstop"; numcards[12]=4;  bmlist[13]="mbsplim"; numcards[13]=3;
 bmlist[14]="mb200";  numcards[14]=4;  bmlist[15]="mb100";   numcards[15]=12;
 bmlist[16]="mb75";   numcards[16]=10; bmlist[17]="mb50";    numcards[17]=10;
 bmlist[18]="mb25";   numcards[18]=10; mileval[14]=200;      mileval[15]=100;
 mileval[16]=75;      mileval[17]=50;  mileval[18]=25;
 srand((unsigned)GetTickCount());

 bDragDrop = (BOOL)GetPrivateProfileInt("Options", "DragDrop", 1,
    (LPSTR)szIniFile);
 bSortCards = (BOOL)GetPrivateProfileInt("Options", "SortCards", 1,
    (LPSTR)szIniFile);
 bNoLogon = (BOOL)GetPrivateProfileInt("Options", "NoLogon", 0,
    (LPSTR)szIniFile);

 startgame = 0;
}

void CheckForHiScore(score0, score1)
int score0, score1;
   {
    int spread, tempscore, i, j;
    char szTmpName[40];
    
    spread = score0 - score1;

    if (spread > 0)    // human wins
       {
	 tempscore = score0;
	 lstrcpy(szTmpName, UserName);
	}
    else
       {
	 tempscore = score1;
	 lstrcpy(szTmpName, "Computer");
       }
    if ( tempscore >= structScore.HiScore[2])     // if you made the list
      {
       i = 0;
       while (tempscore < structScore.HiScore[i]) i++;   // look for insert point
       for(j=1; j>=i; j--)  // scroll the other entries down
	  {
	   structScore.HiScore[j+1] = structScore.HiScore[j];
	   lstrcpy(structScore.HiName[j+1], structScore.HiName[j]);
	   }
       structScore.HiScore[i]=tempscore;   // finally insert it
       lstrcpy(structScore.HiName[i], szTmpName);
      }

    if (spread < 0)  spread = -spread;

    if (spread >= structScore.Spread[2])     // if you made the list
      {
       i = 0;
       while (spread < structScore.Spread[i]) i++;   // look for insert point
       for(j=1; j>=i; j--)  // scroll the other entries down
	  {
	   structScore.Spread[j+1] = structScore.Spread[j];
	   lstrcpy(structScore.SpreadName[j+1], structScore.SpreadName[j]);
	   }
       structScore.Spread[i]=spread;   // finally insert it
       lstrcpy(structScore.SpreadName[i], szTmpName);
      }
  }                

void WriteHighScores()
{
char szIniFile[60], szKeyName[16], tempnum[8];

 lstrcpy((LPSTR)szKeyName,"High Scores");

 GetWindowsDirectory((LPSTR)szIniFile, 48); /* look for INI file */
 lstrcat((LPSTR)szIniFile, "\\mb.ini");

    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName1",
       structScore.HiName[0], (LPSTR)szIniFile);

    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName2",
       structScore.HiName[1], (LPSTR)szIniFile);

    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName3",
       structScore.HiName[2], (LPSTR)szIniFile);

    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName1",
       structScore.SpreadName[0], (LPSTR)szIniFile);

    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName2",
       structScore.SpreadName[1], (LPSTR)szIniFile);

    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName3",
       structScore.SpreadName[2], (LPSTR)szIniFile);

    sprintf(tempnum, "%d",structScore.HiScore[0]);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiScore1",
tempnum, (LPSTR)szIniFile);

   sprintf(tempnum, "%d",structScore.HiScore[1]);
   WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiScore2",
       tempnum, (LPSTR)szIniFile);

  sprintf(tempnum, "%d",structScore.HiScore[2]);
  WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiScore3",
       tempnum, (LPSTR)szIniFile);

   sprintf(tempnum, "%d",structScore.Spread[0]);
   WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"Spread1",
       tempnum, (LPSTR)szIniFile);

    sprintf(tempnum, "%d",structScore.Spread[1]);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"Spread2",
       tempnum, (LPSTR)szIniFile);

    sprintf(tempnum, "%d",structScore.Spread[2]);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"Spread3",
       tempnum, (LPSTR)szIniFile);
}

void ClearHighScores()
{
char szIniFile[60], szKeyName[16];

 lstrcpy((LPSTR)szKeyName,"High Scores");

 GetWindowsDirectory((LPSTR)szIniFile, 48); /* look for INI file */
 lstrcat((LPSTR)szIniFile, "\\mb.ini");

    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName1",
       (LPSTR)" ", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName2",
       (LPSTR)" ", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName3",
       (LPSTR)" ", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName1",
       (LPSTR)" ", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName2",
       (LPSTR)" ", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName3",
       (LPSTR)" ", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiScore1",
       (LPSTR)"0", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiScore2",
       (LPSTR)"0", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiScore3",
       (LPSTR)"0", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"Spread1",
       (LPSTR)"0", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"Spread2",
       (LPSTR)"0", (LPSTR)szIniFile);
    WritePrivateProfileString((LPSTR)szKeyName, (LPSTR)"Spread3",
       (LPSTR)"0", (LPSTR)szIniFile);
}

void GetHighScores(hWnd)
HWND hWnd;
{
 OFSTRUCT ReOpenBuff;
 HANDLE   hIni;
 LPSTR    lpTmpString;
 char     szIniFile[60],
	  szKeyName[16],
	  szTmpString[40];
 int      i;

 lstrcpy((LPSTR)szKeyName, "High Scores");

 for(i = 0; i < 3; i++)
   {
    structScore.HiScore[i] = 0;
    structScore.Spread[i] = 0;
    lstrcpy((LPSTR)structScore.HiName[i]," ");
    lstrcpy((LPSTR)structScore.SpreadName[i]," ");
   }

 GetWindowsDirectory((LPSTR)szIniFile, 48); /* look for INI file */

 lstrcat((LPSTR)szIniFile, "\\mb.ini");
 hIni = OpenFile((LPSTR)szIniFile, (LPOFSTRUCT)&ReOpenBuff, OF_EXIST);
 if (hIni == -1)  ClearHighScores(); /* initializes .ini file */

   else /* already there, just read it */
     {
      structScore.HiScore[0] = GetPrivateProfileInt((LPSTR)szKeyName, (LPSTR)"HiScore1",
		   0, (LPSTR)szIniFile);
      structScore.HiScore[1] = GetPrivateProfileInt((LPSTR)szKeyName, (LPSTR)"HiScore2",
		   0, (LPSTR)szIniFile);
      structScore.HiScore[2] = GetPrivateProfileInt((LPSTR)szKeyName, (LPSTR)"HiScore3",
		   0, (LPSTR)szIniFile);
      structScore.Spread[0] = GetPrivateProfileInt((LPSTR)szKeyName, (LPSTR)"Spread1",
		   0, (LPSTR)szIniFile);
      structScore.Spread[1] = GetPrivateProfileInt((LPSTR)szKeyName, (LPSTR)"Spread2",
		   0, (LPSTR)szIniFile);
      structScore.Spread[2] = GetPrivateProfileInt((LPSTR)szKeyName, (LPSTR)"Spread3",
		   0, (LPSTR)szIniFile);

      lpTmpString = (LPSTR)szTmpString;
      GetPrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName1",
	 (LPSTR)" ", lpTmpString, 32, (LPSTR)szIniFile);
      lstrcpy((LPSTR)structScore.HiName[0], lpTmpString);

      GetPrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName2",
	 (LPSTR)" ", lpTmpString, 32, (LPSTR)szIniFile);
      lstrcpy((LPSTR)structScore.HiName[1], lpTmpString);
 
      GetPrivateProfileString((LPSTR)szKeyName, (LPSTR)"HiName3",
	 (LPSTR)" ", lpTmpString, 32, (LPSTR)szIniFile);
      lstrcpy((LPSTR)structScore.HiName[2], lpTmpString);

      GetPrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName1",
	 (LPSTR)" ", lpTmpString, 32, (LPSTR)szIniFile);
      lstrcpy((LPSTR)structScore.SpreadName[0], lpTmpString);

      GetPrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName2",
	 (LPSTR)" ", lpTmpString, 32, (LPSTR)szIniFile);
      lstrcpy((LPSTR)structScore.SpreadName[1], lpTmpString);

      GetPrivateProfileString((LPSTR)szKeyName, (LPSTR)"SpreadName3",
	 (LPSTR)" ", lpTmpString, 32, (LPSTR)szIniFile);
      lstrcpy((LPSTR)structScore.SpreadName[2], lpTmpString);
     }
}

void SortCards(HWND hWnd)   //sort the player's hand
{
 int i, j, tmpcard;
 for (i = 0; i < 7; i++) if (hand[0][i] < 0) hand[0][i] = 255;
 
 for (i = 0; i < 6; i++)
    for (j = i+1; j < 7; j++)
       if (hand[0][j] < hand[0][i])   //swap the cards
	 {
	  tmpcard = hand[0][j];
	  hand[0][j] = hand[0][i];
	  hand[0][i] = tmpcard;
	 }
 for (i = 0; i < 7; i++)
   {
    if (hand[0][i] == 255) 
      {
       hand[0][i] = -1;
       SetCard(hWnd, i, 1, 20);
      }
    else SetCard(hWnd, i, 1, hand[0][i]);
   }    
}
