/*  mbdlg.c
    Mile Bones Game
    1991 Andre Needham
 */

#include <windows.h>
#include <stdio.h>
#include "mb.h"

extern void UndoLastMove(BOOL);

extern char str1[60], str2[60], UserName[40], szHelpFile[], szIniFile[];
extern short helpcode, deck[101];
extern int GameState[74];
extern ScoreTable structScore;
extern BOOL bDragDrop, bSortCards, bNoLogon;
extern HANDLE hInst;
extern HWND hWndMain;

BOOL FAR PASCAL About(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    if (message == WM_COMMAND)
       {
	EndDialog(hDlg, TRUE);
	return TRUE;
       }
    else if (message == WM_INITDIALOG)
    return TRUE;
    else return FALSE;
}

BOOL FAR PASCAL MBHiScore(HWND hDlg, unsigned message, WORD wParam, LONG lParam )
 {
  char tempnum[6];
  int i;

    switch(message)
     {
      case PM_CALLHELP:
	 if (lParam == 22222) //don't just jump for any WM_USER
	 {
	 if (helpcode == 0)
	    WinHelp(hDlg, szHelpFile, HELP_INDEX, (DWORD)NULL);
	 else 
	    WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	 }
      break;

      case WM_INITDIALOG:
       helpcode = 601;
	 for(i=0;i<3;i++)
	   {
	    sprintf(tempnum, "%d", structScore.HiScore[i]);
	    SetDlgItemText(hDlg, 110+i, (LPSTR)tempnum);

	    sprintf(tempnum, "%d", structScore.Spread[i]);
	    SetDlgItemText(hDlg, 116+i, (LPSTR)tempnum);

	    SetDlgItemText(hDlg, 113+i, (LPSTR)structScore.HiName[i]);
	    SetDlgItemText(hDlg, 119+i, (LPSTR)structScore.SpreadName[i]);
	     } 
	 return TRUE;
      break;

      case WM_COMMAND:
	 switch(wParam)
	   {
	    case IDYES:
		helpcode = 0;
		EndDialog(hDlg,TRUE);
	    break;

	  case IDHELP:
	      WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	  break;

	      case  IDNO: // clear scores button
		 for(i=0; i<3; i++)
		     {
		       structScore.HiScore[i]=0;  /* erase score structure */
		       structScore.Spread[i]=0;
		       lstrcpy((LPSTR)structScore.HiName[i]," ");
		       lstrcpy((LPSTR)structScore.SpreadName[i]," ");

		     sprintf(tempnum, "%d", structScore.HiScore[i]);    /* clear dialog box */
		     SetDlgItemText(hDlg, 110+i, (LPSTR)tempnum);

		     sprintf(tempnum, "%d", structScore.Spread[i]);
		     SetDlgItemText(hDlg, 116+i, (LPSTR)tempnum);

		     SetDlgItemText(hDlg, 113+i, (LPSTR)structScore.HiName[i]);
		     SetDlgItemText(hDlg, 119+i, (LPSTR)structScore.SpreadName[i]);
			       }
		     //     ClearHighScores(); 
		       break;

			default:
			break;
	   }
	 return TRUE;
      break;

      case WM_CLOSE:
	 helpcode = 0;
	 EndDialog(hDlg,TRUE);
	 return TRUE;
      break;

      default:
	 return FALSE;
      break;
     }
}

BOOL FAR PASCAL MBName(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
  int nchars;

   switch(message)
     {
      case PM_CALLHELP:
	 if (lParam == 22222) //don't just jump for any WM_USER
	 {
	 if (helpcode == 0)
	    WinHelp(hDlg, szHelpFile, HELP_INDEX, (DWORD)NULL);
	 else 
	    WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	 }
      break;

      case WM_INITDIALOG:
	helpcode = 602;
	SendDlgItemMessage(hDlg, 101, EM_LIMITTEXT,  (WORD)31, (DWORD)0); 
		     return TRUE;
      break;

      case WM_COMMAND:
	 switch(wParam)
	   {
	       case IDOK:
		   nchars=GetDlgItemText(hDlg, 101, (LPSTR)UserName, 32);
		   if(nchars == 0) lstrcpy((LPSTR)UserName, "noname"); 
		   helpcode = 0;
		   EndDialog(hDlg,TRUE);
		   return TRUE;
	       break;

	  case IDHELP:
	      WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	      return FALSE;
	  break;

	       default:
		  return FALSE;
	       break;
	   }
	   break;

      case WM_CLOSE:
	    helpcode = 0;
	    EndDialog(hDlg,TRUE);
	    return TRUE;
      break;

      default:
	 return FALSE;
      break;
     }
}

BOOL FAR PASCAL MBOptions(HWND hDlg, unsigned message, WORD wParam,
		LONG lParam)
{
 static BOOL btmp, bsort, bnoname;
 char szOutStr[4], szDefName[32];
 int nchars;

 switch(message)
   {
    case PM_CALLHELP:
       if (lParam == 22222) //don't just jump for any WM_USER
	 {
	  if (helpcode == 0)
	     WinHelp(hDlg, szHelpFile, HELP_INDEX, (DWORD)NULL);
	  else 
	     WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	  }
       return TRUE;
    break;

    case WM_INITDIALOG:
       //turn on "drag-drop" button
       btmp = bDragDrop;
       SendDlgItemMessage(hDlg, btmp?(102):(103), BM_SETCHECK, (WORD)1, (LONG)0);

       bsort = bSortCards;
       SendDlgItemMessage(hDlg, 105, BM_SETCHECK, (WORD)bsort, (LONG)0);
       
       bnoname = bNoLogon;
       SendDlgItemMessage(hDlg, 106, BM_SETCHECK, (WORD)bnoname, (LONG)0);

       GetPrivateProfileString((LPSTR)"Options", (LPSTR)"DefName",
	  (LPSTR)"noname", (LPSTR)szDefName, 31, (LPSTR)szIniFile);               
       SetDlgItemText(hDlg, 108, szDefName);
       SendDlgItemMessage(hDlg, 108, EM_LIMITTEXT,  (WORD)31, (DWORD)0); 
       EnableWindow(GetDlgItem(hDlg,108),bNoLogon); 

       helpcode = 603;
       return TRUE;
    break;

    case WM_COMMAND:
       switch(wParam)
	 {
	  case 102:  //drag-drop
	     //SendDlgItemMessage(hDlg, 102, BM_SETCHECK, (WORD)1, (LONG)0);
	     btmp = TRUE;
	     return TRUE;
	  break;

	  case 103:  //click-click
	     //SendDlgItemMessage(hDlg, 103, BM_SETCHECK, (WORD)1, (LONG)0);
	     btmp = FALSE;
	     return TRUE;
	  break;

	  case 105:  //sort cards
	     bsort = SendDlgItemMessage(hDlg, 105, BM_GETCHECK, (WORD)0, (LONG)0);
	     return TRUE;
	  break;

	  case 106:  //disable name input
	     bnoname = SendDlgItemMessage(hDlg, 106, BM_GETCHECK, (WORD)0, (LONG)0);
	     EnableWindow(GetDlgItem(hDlg,108),bnoname); 
	     return TRUE;
	  break;

	case IDHELP:
	    WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	break;

	  case IDOK:
	     bDragDrop = btmp;
	     bSortCards = bsort;
	     bNoLogon = bnoname;

	     lstrcpy(szOutStr, bSortCards ? ("1"):("0"));
	     WritePrivateProfileString((LPSTR)"Options",
		(LPSTR)"SortCards", (LPSTR)szOutStr, (LPSTR)szIniFile);

	     lstrcpy(szOutStr, bDragDrop ? ("1"):("0"));
	     WritePrivateProfileString((LPSTR)"Options",
		(LPSTR)"DragDrop", (LPSTR)szOutStr, (LPSTR)szIniFile);

	     lstrcpy(szOutStr, bNoLogon ? ("1"):("0"));
	     WritePrivateProfileString((LPSTR)"Options",
		(LPSTR)"NoLogon", (LPSTR)szOutStr, (LPSTR)szIniFile);

	     if(bNoLogon)
	       {
		nchars=GetDlgItemText(hDlg, 108, (LPSTR)szDefName, 32);
		if(nchars == 0) lstrcpy((LPSTR)szDefName, "noname"); 
		WritePrivateProfileString((LPSTR)"Options",
		   (LPSTR)"DefName", (LPSTR)szDefName, (LPSTR)szIniFile);               
		lstrcpy(UserName,szDefName);
		InvalidateRect(hWndMain,NULL,FALSE);
	       }

	  //fall through
	  case IDCANCEL:
	     helpcode = 0;
	     EndDialog(hDlg, TRUE);
	     return TRUE;
	  break;

	  default:
	     return FALSE;
	  break;
	 }
       break;

    case WM_CLOSE:
       helpcode = 0;
       EndDialog(hDlg, TRUE);
       return TRUE;
    break;

    default:
       return FALSE;
    break;
   }
}

BOOL FAR PASCAL MBHelp(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
   switch(message)
     {
      case PM_CALLHELP:
	 if (lParam == 22222) //don't just jump for any WM_USER
	 {
	 if (helpcode == 0)
	    WinHelp(hDlg, szHelpFile, HELP_INDEX, (DWORD)NULL);
	 else 
	    WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	 }
      break;

      case WM_INITDIALOG:
	 SetDlgItemText(hDlg,100,(LPSTR)str1);
	 SetDlgItemText(hDlg,101,(LPSTR)str2);
	 return TRUE;
      break;

      case WM_COMMAND:
	   helpcode = 0;
	 EndDialog(hDlg,TRUE);
	 return TRUE;
      break;

      default:
	 return FALSE;
      break;
     }
}

BOOL FAR PASCAL MBSave(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
 static char szFileName[80],szBaseName[80], szTmpName[80];
 LPSTR lpTemp;
 static BOOL getdir;
 DWORD saveval;
 int hOutFile, i, check;
 
   switch(message)
     {
      case PM_CALLHELP:
	 if (lParam == 22222) //don't just jump for any WM_USER
	 {
	 if (helpcode == 0)
	    WinHelp(hDlg, szHelpFile, HELP_INDEX, (DWORD)NULL);
	 else 
	    WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	 }
      break;

      case WM_INITDIALOG:
	 getdir = FALSE;
	 helpcode = 605;
	 GetModuleFileName(hInst, (LPSTR)szBaseName, 80);
	 lstrcpy((LPSTR)szTmpName,(LPSTR)"1.mbg");
	 lpTemp = (LPSTR)szBaseName;
	 while (*lpTemp != '\0')lpTemp++; // find end of string
	 while (*lpTemp != '\\')lpTemp--; // find backslash
	 lpTemp++;
	 *lpTemp = '\0'; /* terminate string */
	 lstrcpy((LPSTR)szFileName,(LPSTR)szBaseName);
	 lstrcat((LPSTR)szFileName, "*.");
	 DlgDirList(hDlg,(LPSTR)szFileName,103,107,(WORD)0xC010);
  
	 return TRUE;
      break;
      case WM_COMMAND:
	 switch(wParam)
	   {
	    case 100:
	    case 101:   
	    break;

	    case 102:
	       SendDlgItemMessage(hDlg, 103, LB_SETCURSEL,
		  (WORD)1, (LONG)0);
	    break;

	    case 103:
	       switch(HIWORD(lParam))
		 {
		  case LBN_DBLCLK:
		     DlgDirSelectEx(hDlg,(LPSTR)szFileName,sizeof(szFileName),103);
		     /* if a dir, reset base path */
		     if(szFileName[1] == ':')
		       {
			lstrcpy((LPSTR)szBaseName,(LPSTR)szFileName);
			lstrcat((LPSTR)szFileName, "*.");
			getdir = TRUE;
		       }
		     /* else if .. , back up */
		     else if(szFileName[0] == '.')
		       {
			lpTemp = (LPSTR)szBaseName;
			while (*lpTemp != '\0')lpTemp++; /* find end */
			while (*lpTemp != '\\')lpTemp--; /* back up to */
			lpTemp--;
			while (*lpTemp != '\\')lpTemp--; /* 2nd bkslash */
			lpTemp++;
			*lpTemp = '\0'; /* terminate string */
			lstrcpy((LPSTR)szFileName, (LPSTR)szBaseName);
			lstrcat((LPSTR)szFileName, "*.");
		       }
		     /* else, tack onto pathname */
		     else
		       {
			lstrcat((LPSTR)szBaseName, (LPSTR)szFileName);
			lstrcpy((LPSTR)szFileName, (LPSTR)szBaseName);
			lstrcat((LPSTR)szFileName, "*.");
		       }
		     DlgDirList(hDlg, (LPSTR)szFileName, 103, 107,
			(WORD)0xC010);
		     if(getdir) /* gimme dir name */
		       {
			GetDlgItemText(hDlg,107,(LPSTR)szBaseName,70);
			lpTemp = (LPSTR)szBaseName;
			while (*lpTemp != '\0')lpTemp++; /* find end */
			lpTemp--; /* back up to find backslash */
			if (*lpTemp != '\\')
			  {
			   lpTemp++;
			   *lpTemp = '\\'; /* add backslash */
			   lpTemp++;
			   *lpTemp = '\0'; /* terminate string */
			  }
			getdir = FALSE;
		       }
		  break;
		 }
	    break;

	  case IDHELP:
	     WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	  break;

	    case IDOK:
	       GetDlgItemText(hDlg, 101, (LPSTR)szTmpName, 79);
	       lstrcat((LPSTR)szBaseName, (LPSTR)szTmpName);

	       hOutFile = _lcreat((LPSTR)szBaseName, 0);
		_lwrite(hOutFile, (LPSTR)"MB22",4);  //file header
		_lwrite(hOutFile, (LPSTR)&deck[0], 202);
		_lwrite(hOutFile, (LPSTR)&GameState[0], 74*sizeof(int));
		_lwrite(hOutFile, (LPSTR)UserName, 40);
	       _lclose(hOutFile);
 
	       EndDialog(hDlg, TRUE);
	       return TRUE;
	    break;

	    case IDCANCEL:
	      EndDialog(hDlg,TRUE);
	      return TRUE;
	    break;
	   }
      break;
      default:
	 return FALSE;
      break;
     }
}

BOOL FAR PASCAL MBLoad(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
 static char szFileName[80],szBaseName[80], szTmpName[80], szHeader[5];
 LPSTR lpTemp;
 static BOOL getdir;
 int hInFile, i, check;
 
   switch(message)
     {
      case PM_CALLHELP:
	 if (lParam == 22222) //don't just jump for any WM_USER
	 {
	 if (helpcode == 0)
	    WinHelp(hDlg, szHelpFile, HELP_INDEX, (DWORD)NULL);
	 else 
	    WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	 }
      break;

      case WM_INITDIALOG:
	 getdir = FALSE;
	 helpcode = 604;
	 GetModuleFileName(hInst, (LPSTR)szBaseName, 80);
	 lstrcpy((LPSTR)szTmpName,(LPSTR)"*.mbg");
	 lpTemp = (LPSTR)szBaseName;
	 while (*lpTemp != '\0')lpTemp++; // find end of string
	 while (*lpTemp != '\\')lpTemp--; // find backslash
	 lpTemp++;
	 *lpTemp = '\0'; /* terminate string */
	 lstrcpy((LPSTR)szFileName,(LPSTR)szBaseName);
	 lstrcat((LPSTR)szFileName, "*.mbg");
	 DlgDirList(hDlg,(LPSTR)szFileName,103,107,(WORD)0x4010);
  
	 return TRUE;
      break;
      case WM_COMMAND:
	 switch(wParam)
	   {
	    case 100:
	    case 101:
	    break;

	    case 102:
	       SendDlgItemMessage(hDlg, 103, LB_SETCURSEL,
		  (WORD)1, (LONG)0);
	    break;

	    case 103:
	       switch(HIWORD(lParam))
		 {
		  case LBN_SELCHANGE:
		     DlgDirSelectEx(hDlg,(LPSTR)szFileName,sizeof(szFileName),103);
		     if((szFileName[1]!=':') && (szFileName[0]!='.'))
		       {
			//if it's a filename, display in the edit box
			lpTemp = (LPSTR)szFileName;
			while (*lpTemp != '\0')lpTemp++; /* find end */
			while (*lpTemp != '.')lpTemp--; /* back up to . */
			lpTemp++;

			if(lstrcmp((LPSTR)lpTemp,"mbg")==0)
			   SetDlgItemText(hDlg, 101, (LPSTR)szFileName);
		       }
		  break;

		  case LBN_DBLCLK:
		     DlgDirSelectEx(hDlg,(LPSTR)szFileName,sizeof(szFileName),103);
		     /* if a drive letter, reset base path */
		     if(szFileName[1] == ':')
		       {
			lstrcpy((LPSTR)szBaseName,(LPSTR)szFileName);
			lstrcat((LPSTR)szFileName, "*.mbg");
			getdir = TRUE;
		       }
		     /* else if .. , back up */
		     else if(szFileName[0] == '.')
		       {
			lpTemp = (LPSTR)szBaseName;
			while (*lpTemp != '\0')lpTemp++; /* find end */
			while (*lpTemp != '\\')lpTemp--; /* back up to */
			lpTemp--;
			while (*lpTemp != '\\')lpTemp--; /* 2nd bkslash */
			lpTemp++;
			*lpTemp = '\0'; /* terminate string */
			lstrcpy((LPSTR)szFileName, (LPSTR)szBaseName);
			lstrcat((LPSTR)szFileName, "*.mbg");
		       }
		     /* else, tack onto pathname */
		     else
		       {
			lpTemp = (LPSTR)szFileName;
			while (*lpTemp != '\0')lpTemp++; /* find end */
			while (*lpTemp != '.')lpTemp--; /* back up to . */
			lpTemp++;
			if(lstrcmp((LPSTR)lpTemp,"mbg")==0)
			  {
			   //szBaseName must be a filename
			   SetDlgItemText(hDlg, 101, (LPSTR)szFileName);
			   SendMessage(hDlg, WM_COMMAND,IDOK,
			      MAKELONG(GetDlgItem(hDlg,IDOK),BN_CLICKED));
			   return TRUE; 
			  }
			lstrcat((LPSTR)szBaseName, (LPSTR)szFileName);
			lstrcpy((LPSTR)szFileName, (LPSTR)szBaseName);
			lstrcat((LPSTR)szFileName, "*.mbg");
		       }
		     DlgDirList(hDlg, (LPSTR)szFileName, 103, 107,
			(WORD)0x4010);
		     if(getdir) /* gimme dir name */
		       {
			GetDlgItemText(hDlg,107,(LPSTR)szBaseName,70);
			lpTemp = (LPSTR)szBaseName;
			while (*lpTemp != '\0')lpTemp++; /* find end */
			lpTemp--; /* back up to find backslash */
			if (*lpTemp != '\\')
			  {
			   lpTemp++;
			   *lpTemp = '\\'; /* add backslash */
			   lpTemp++;
			   *lpTemp = '\0'; /* terminate string */
			  }
			getdir = FALSE;
		       }
		  break;
		 }
	    break;

	  case IDHELP:
	     WinHelp(hDlg, szHelpFile, HELP_CONTEXT, (DWORD)helpcode);
	  break;

	    case IDOK:
	       GetDlgItemText(hDlg, 101, (LPSTR)szTmpName, 13);
	       //save base path in case load fails
	       lstrcpy((LPSTR)szFileName, (LPSTR)szBaseName);
	       lstrcat((LPSTR)szBaseName, (LPSTR)szTmpName);
	       hInFile = _lopen((LPSTR)szBaseName,OF_READ);
	       if(hInFile == -1)
		 {
		  MessageBox(hDlg, (LPSTR)szBaseName, "Can't open this file:", MB_OK);
		  lstrcpy((LPSTR)szBaseName, (LPSTR)szFileName);
		  return TRUE;
		 }               
	       _lread(hInFile, (LPSTR)szHeader,4);  //file header
	       if(lstrcmp((LPSTR)szHeader, (LPSTR)"MB22")==0)  //file OK
		 {
		  _lread(hInFile, (LPSTR)&deck[0], 202);
		  _lread(hInFile, (LPSTR)&GameState[0], 74*sizeof(int));
		  _lread(hInFile, (LPSTR)UserName, 40);
		 }
	       else 
		 {
		  MessageBox(hDlg, "This file is not a saved game.", "MB 2.2", MB_OK);
		  lstrcpy((LPSTR)szBaseName, (LPSTR)szFileName);
		  return TRUE;
		 }             
	       _lclose(hInFile);
	       UndoLastMove(TRUE); 
	       InvalidateRect(hWndMain, NULL, TRUE);
	       EndDialog(hDlg, TRUE);
	       return TRUE;
	    break;

	    case IDCANCEL:
	      EndDialog(hDlg,TRUE);
	      return TRUE;
	    break;
	   }
      break;
      default:
	 return FALSE;
      break;
     }
}



