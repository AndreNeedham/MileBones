#include <windows.h>
#include "mb.h"

extern short xgap, ygap, xcard, ycard, xsize, ysize, compturn,
	     curcard, gameover, winmiles, helpcode, cclicked,
	     cdrawn;
extern short battle[2], cango[2], rofw[2], splim[2], nsplim[2],
	     hand[2][7], ncoup[2], nsafe[2], safety[2][4],
	     deck[101], two200s[2], ncards[2], mileval[19],
	     nbattle[2];
extern char  str1[], str2[];
extern POINT dragpt;
extern int   score[2], miles[2], scard[9][3], htype;
extern BOOL  bDblClick, bDragDrop, bInDrag, bJustInDrag, bButtonUp,
             bSortCards, bHelp;

//in mbcore.c:
HBITMAP CopyBitmap(HDC, short, short);
void DrawBitmap(HDC, short, short, HBITMAP);
void EraseCard(HDC, int, int);
void SetCard(HWND, int, int, int);
void MoveCur(HWND);
void SortCards();
void SetStatusBar1(LPSTR);
void SetStatusBar2();


void GoPlayer(hWnd) 
HWND hWnd; 
{ 
 HDC hDC; 
 DWORD mousepos; 
 POINT mousept, tmppt; 
 LPPOINT lpmousept;
 BOOL bRealDiscard = FALSE; 
 int   tempcard, tx, t, tmpflg, square, oldcurcard; 
 PAINTSTRUCT ps; 
 
 if ((gameover == 0) && (compturn == 0)) 
   { 
    mousepos = GetMessagePos(); 
    mousept.x = LOWORD(mousepos);  
    mousept.y = HIWORD(mousepos);  
    lpmousept = (LPPOINT)&mousept; 
    ScreenToClient(hWnd, lpmousept); 
    mousept = *lpmousept; 
    dragpt = mousept; 
    tmppt = mousept; 
    tmppt.x -= xgap; tmppt.y -= ygap; 
    if ((tmppt.x < 0) || (tmppt.y < 0)  
       || (tmppt.x % xsize >= xcard) || (tmppt.y % ysize >= ycard)) 
      {tmppt.x = 7; tmppt.y = 1;} 
    else 
      {tmppt.x /= xsize; tmppt.y /= ysize;} 
    mousept.x = tmppt.x * xsize + xgap; 
    mousept.y = tmppt.y * ysize + ygap; 
 
    square = tmppt.y * 8 + tmppt.x;
    if (square == 23) bRealDiscard = TRUE; 
    oldcurcard = curcard;

    if(bDblClick)
      {
       bDblClick = FALSE;
       if ((square > 7) && (square < 15))
         {
          cclicked = tmppt.x + 1;
          tempcard = hand[0][cclicked - 1]; 

          if (tempcard < 4)       //safety
             square = tmppt.x = tmppt.y = 0;

          else if (tempcard < 7)  //remedy
            {
             if (battle[0] == (tempcard + 5))
                square = tmppt.x = tmppt.y = 0;
             else 
               {
                square = 23;  //discard
                bRealDiscard = TRUE;
               }
            }

          else if (tempcard == 7)  //go
            {
             if ((rofw[0] == 0) && ((battle[0] < 7) || (battle[0] == 12)))
                square = tmppt.x = tmppt.y = 0;
             else
               {
                square = 23;  //discard
                bRealDiscard = TRUE;
               }
            }

          else if (tempcard == 8)  //eolim
            {
             if (splim[0] == 1)
                square = tmppt.x = tmppt.y = 0;
             else
               {
                square = 23;  //discard
                bRealDiscard = TRUE;
               }
            }

          else if (tempcard < 13)  //hazard    
            {
      	     tx = 0;
             for(t = 0; t < 4;t++) if (safety[1][t] == (tempcard - 9)) tx++; 
	     if(((battle[1] == 3) || (battle[1] == 7)) && (tx == 0)) 
               {
                square = 16;
                tmppt.x = 0;
                tmppt.y = 2;
               }
             else
               {
                square = 23;  //discard
                bRealDiscard = TRUE;
               }
            }

          else if (tempcard == 13)  //splim
            {
             if ((rofw[1] == 0) && (splim[1] == 0))
               {
                square = 16;
                tmppt.x = 0;
                tmppt.y = 2;
               }
             else
               {
                square = 23;  //discard
                bRealDiscard = TRUE;
               }
            }

          else //mileage
            {
             if ((cango[0] == 0)
	        || ((tempcard <17) && (tempcard > 13) && (splim[0] == 1)) 
	        || ((tempcard == 14) && (two200s[0] == 2)) 
	        || (mileval[tempcard] + miles[0] > winmiles))
               {
                square = 23;  //discard
                bRealDiscard = TRUE;
               }
             else
               {
                square = tmppt.x = 6;
                tmppt.y = 0;
               }
            }
         }  // if card is in our hand
      }  // if bDblClick    

    /* coup fourre */ 
    if((square < 2) && (cdrawn == 0) && (cclicked > 0)) 
      { 
       tmpflg = 0; 
       tempcard = hand[0][cclicked - 1]; 
       if (tempcard < 4) 
	 {              
	  if (((tempcard == 3) && (splim[0] == 1) && (nsplim[0] == 1)) || ((tempcard == 3) && 
(battle[0] == 12) && (nbattle[0] == 1)))           //right of way 
	    { 
	     splim[0] = 0; 
	     rofw[0] = 1; 
	     tmpflg  = 1; 
	     SetCard(hWnd, 1, 0, 20);  //erase splim
	     if ((battle[0] < 8) || (battle[0] == 12))  //remedy or stop on battle
	       { 
		battle[0] = 3; 
		cango[0] = 1; 
		SetCard(hWnd, 0, 0, 3); 
	       } 
	    } 
	  if ((tmpflg == 1) || ((tempcard == battle[0] - 9) && (nbattle[0] == 1))) 
	    { 
	     score[0] += 400; 
	     ncoup[0]++; 
	     if (tmpflg == 0) 
	       { 
		cango[0] = 1; 
		if (rofw[0] == 0) battle[0] = 7; else battle[0] = 3; 
		SetCard(hWnd, 0, 0, battle[0]); 
	       } 
	     compturn = 0; 
	     safety[0][nsafe[0]++] = tempcard; 
	     SetCard(hWnd, nsafe[0] + 1, 0, 32 + tempcard); 
	     if (curcard < 101) 
	       { 
		hand[0][cclicked - 1] = deck[curcard]; 
		SetCard(hWnd, 7, 0, 21);
		SetCard(hWnd, cclicked - 1, 1, deck[curcard]);
		curcard++; 
	       } 
	     else  //out of cards 
	       { 
		hand[0][cclicked - 1] = -1; 
		SetCard(hWnd, cclicked - 1, 1, 20); 
	       } 
	     if (bSortCards) SortCards(hWnd);
	     cclicked = 0; cdrawn = 0; 
	     tmppt.x = 7; square = 7; // draw card 
	    } 
	  else 
	    { 
	     lstrcpy((LPSTR)str1, "You must draw a card before playing"); 
	     lstrcpy((LPSTR)str2, "unless you can coup-fourré."); 
	     HelpUser(hWnd, htype); 
	    } 
	 } 
      } 
 
    /* draw card */ 
    if(square == 7) 
      { 
       if(cdrawn == 1) 
	 { 
	  lstrcpy((LPSTR)str1, "You have already drawn a card."); 
	  lstrcpy((LPSTR)str2, " "); 
	 }         
       if(curcard == 101) 
	 { 
	  lstrcpy((LPSTR)str1, "The draw deck is empty."); 
	  lstrcpy((LPSTR)str2, "Either play or discard a card."); 
	 } 
       if((cdrawn == 0) && (curcard < 101)) 
	 { 
	  cdrawn = 1; 
	  hand[0][6] =deck[curcard]; 
	  SetCard(hWnd, 6, 1, deck[curcard]); 
	  curcard++; 
	  SetCard(hWnd, 7, 0, 21);
	  if (bSortCards) SortCards(hWnd);
	 } 
       else HelpUser(hWnd, htype); 
      } 
 
    if((cdrawn == 1) && (cclicked > 0)) 
      { 
       tempcard = hand[0][cclicked-1]; 
 
       // move cards in hand
       if((square > 7) && (square < 15) && (!bSortCards) && bDragDrop
	  && ((cclicked - 1) != tmppt.x))
	 {
	  hand[0][cclicked-1] = hand[0][tmppt.x];
	  hand[0][tmppt.x] = tempcard;
	  SetCard(hWnd, tmppt.x, 1, hand[0][tmppt.x]);
	  SetCard(hWnd, cclicked-1, 1, hand[0][cclicked-1]);
	 }

       if((square > 15) && (square < 18)) 
	 { 
	  if(tempcard == 13) 
	    { 
	     if(splim[1] == 1) 
	       { 
		lstrcpy((LPSTR)str1, "The computer already has a Speed Limit."); 
		lstrcpy((LPSTR)str2, " "); 
		HelpUser(hWnd, htype); 
	       } 
	     if(rofw[1] == 1) 
	       { 
		lstrcpy((LPSTR)str1, "You can't play a Speed Limit."); 
		lstrcpy((LPSTR)str2, "The computer has a Right of Way."); 
		HelpUser(hWnd, htype); 
	       } 
	     if((rofw[1] == 0) && (splim[1] == 0)) 
	       { 
		splim[1] = 1; 
		nsplim[1] = 1; 
		SetCard(hWnd, 1, 2, 13); 
		square = 23; /* discard */ 
	       } 
	    }
	  if(((battle[1] != 3) && (battle[1] != 7))) 
	    { 
	     if((tempcard > 8) && (tempcard < 13)) 
	       { 
		lstrcpy((LPSTR)str1, "You can't play a Hazard on the computer"); 
		lstrcpy((LPSTR)str2, "since it is already stopped."); 
		HelpUser(hWnd, htype); 
	       } 
	     if((tempcard < 9) || (tempcard > 13)) 
	       { 
		lstrcpy((LPSTR)str1, "You can play only Hazard cards"); 
		lstrcpy((LPSTR)str2, "on the computer."); 
		HelpUser(hWnd, htype); 
	       } 
	    } 
	  else 
	    { 
	     if((tempcard > 8) && (tempcard < 13)) 
	    { 
	     tx = 0; for(t = 0; t < 4;t++) if (safety[1][t] == tempcard-9) tx = 3; 
	     if (tx == 0) 
	       { 
		SetCard(hWnd, 0, 2, tempcard); 
		cango[1] = 0; 
		nbattle[1] = 1; 
		battle[1] = tempcard; 
		    square = 23; /* discard */ 
		   } 
		else 
		  { 
		   lstrcpy((LPSTR)str1, "You can't play that Hazard on the"); 
		   lstrcpy((LPSTR)str2, "computer because it has a Safety."); 
		   HelpUser(hWnd, htype); 
		   } 
		} 
	     else if (tempcard != 13) 
	       { 
		lstrcpy((LPSTR)str1, "You can only play Hazard cards"); 
		lstrcpy((LPSTR)str2, "on the computer."); 
		HelpUser(hWnd, htype); 
	       } 
	    } 
	 } 
 
       // play remedy or safety 
       if (square < 2) 
	 { 
	  if (tempcard > 13) 
	    { 
	     lstrcpy((LPSTR)str1, "You must play mileage on your Mileage pile."); 
	     lstrcpy((LPSTR)str2, " "); 
	     HelpUser(hWnd, htype); 
	    } 
	  else if (tempcard > 8) 
	    { 
	     lstrcpy((LPSTR)str1, "You can't play a Hazard on yourself!"); 
	     lstrcpy((LPSTR)str2, " "); 
	     HelpUser(hWnd, htype); 
	    } 
	  else if (tempcard == 8) 
	    { 
	     if (splim[0] == 0)  
	      { 
		lstrcpy((LPSTR)str1, "There is no Speed Limit to remove."); 
		lstrcpy((LPSTR)str2, " "); 
		HelpUser(hWnd, htype); 
	       } 
	     else 
	   { 
	    splim[0] = 0; 
	    SetCard(hWnd, 1, 0, 8); 
	    square = 23; /* discard */ 
	   } 
	    } 
	  else if (tempcard == 7) 
	    { 
	     if((battle[0] > 8) && (battle[0] != 12)) 
	       { 
		lstrcpy((LPSTR)str1, "You can't play a Go until you remove the Hazard."); 
		lstrcpy((LPSTR)str2, " "); 
		HelpUser(hWnd, htype); 
	       } 
	     if (battle[0] == 7) 
	       { 
		lstrcpy((LPSTR)str1, "You have already played a Go."); 
		lstrcpy((LPSTR)str2, " "); 
		HelpUser(hWnd, htype); 
	       }  
	     if ((battle[0] < 7) || (battle[0] == 12)) 
	       { 
		if (rofw[0] == 0) 
	      { 
	       battle[0] = 7; 
	       cango[0] = 1; 
	       SetCard(hWnd, 0, 0, 7); 
	       square = 23;  /* discard */ 
		  } 
		else 
		  { 
		   lstrcpy((LPSTR)str1, "You don't need to play a Go"); 
		   lstrcpy((LPSTR)str2, "because you already have a Right of Way."); 
		   HelpUser(hWnd, htype); 
		  } 
	       }    
	    } 
	  else if (tempcard < 4) 
	    {                                      /* safety */ 
	     compturn = 2; /* give player extra turn */ 
	     safety[0][nsafe[0]++] = tempcard; 
	     if (battle[0] == tempcard + 9) 
	       { 
		battle[0] = 0; 
		SetCard(hWnd, 0, 0, 20); 
	       } 
	     if (tempcard == 3) rofw[0] = 1; 
	     if (rofw[0] ==1) 
	       { 
		if(battle[0] < 8) battle[0] = 0; 
		if(battle[0] == 0) 
		  { 
	       cango[0] = 1; 
	       battle[0] = 3; 
	       SetCard(hWnd, 0, 0, 3); 
		   } 
		 SetCard(hWnd, 1, 0, 20); 
		 splim[0] = 0; 
		} 
	     score[0] += 100; 
	     SetCard(hWnd,nsafe[0] + 1, 0, tempcard); 
	     square = 23; /* discard */ 
	    }  
	  else  //tmpcard between 4 and 7 
	    {                               /* remedy */ 
	     if ((battle[0] == tempcard+5) && (tempcard < 7)) 
	       { 
		if (rofw[0] == 1) 
		  { 
		   cango[0] = 1; 
		   tempcard = 3; 
		  } 
		battle[0] = tempcard; 
		if (battle[0] == 7) cango[0] = 1; 
		SetCard(hWnd, 0, 0, tempcard); 
		square = 23; /* discard */ 
	       } 
	     else if (tempcard < 7) 
	       { 
		if((battle[0] > 8) && (battle[0] < 13)) 
		  { 
		   lstrcpy((LPSTR)str1, "That isn't the right Remedy for the Hazard."); 
		   lstrcpy((LPSTR)str2, " "); 
		   HelpUser(hWnd, htype); 
		  } 
		else 
		  { 
		   lstrcpy((LPSTR)str1, "You don't need to play a Remedy."); 
		   lstrcpy((LPSTR)str2, " "); 
		   HelpUser(hWnd, htype); 
		  } 
	       }  //tmpcard < 7 
	    }  //tmpcard between 4 and 7 
	 }  //square < 2 
 
       /* play mileage */ 
       if(square == 6) 
	 { 
	  if((tempcard < 14) || (tempcard > 18)) 
	    { 
	     lstrcpy((LPSTR)str1, "You can only play Mileage cards"); 
	     lstrcpy((LPSTR)str2, "onto your Mileage pile."); 
	     HelpUser(hWnd, htype); 
	    } 
	  if((tempcard <17) && (tempcard > 13) && (splim[0] == 1)) 
	    { 
	     lstrcpy((LPSTR)str1, "You are under a Speed Limit."); 
	     lstrcpy((LPSTR)str2, "You can only play 25 or 50 mile cards."); 
	     HelpUser(hWnd, htype);          
	    } 
	  if((tempcard == 14) && (two200s[0] == 2)) 
	    { 
	     lstrcpy((LPSTR)str1, "You have already played two 200 mile"); 
	     lstrcpy((LPSTR)str2, "cards, which is the maximum allowed."); 
	     HelpUser(hWnd, htype); 
	    } 
	  if(mileval[tempcard] + miles[0] > winmiles) 
	    { 
	     lstrcpy((LPSTR)str1, "You must play the exact mileage needed"); 
	     lstrcpy((LPSTR)str2, "to complete the game without going over."); 
	     HelpUser(hWnd, htype); 
	    }   
	  if((cango[0] == 0) && (tempcard > 13)) 
	    { 
	     lstrcpy((LPSTR)str1, "You can't play mileage if you are stopped."); 
	     lstrcpy((LPSTR)str2, " "); 
	     HelpUser(hWnd, htype); 
	    } 
	  if((tempcard < 14) || (cango[0] == 0) 
	    || ((splim[0] == 1) && (tempcard < 17)) 
	    || ((tempcard == 14) && (two200s[0] == 2)) 
	    || (mileval[tempcard] + miles[0] > winmiles)) 
	    { /* don't play mileage */ } 
	  else 
	    { 
	     miles[0] += mileval[tempcard]; 
	     if (miles[0] == winmiles) gameover = 3; 
	     SetCard(hWnd, 6, 0, 23); 
	     if (tempcard == 14) two200s[0]++; 
	     square = 23;     /* force discard */ 
	} 
	 }  //square == 6 
      }  //cclicked & cdrawn 
    if (curcard > 100) cdrawn = 1; 
 
    /* discard (or cleanup for other areas) */ 
    if((square == 23) && (cdrawn == 1) && (cclicked > 0)) 
      { 
       if (hand[0][6] > -1) 
	 {
          if(bRealDiscard)
            {
             scard[7][2] = hand[0][cclicked-1]; //set discard
             SetCard(hWnd, 7, 2, scard[7][2]);
             bRealDiscard = FALSE; 
	    }
          hand[0][cclicked-1] = hand[0][6]; 
	  SetCard(hWnd, cclicked - 1, 1, hand[0][6]); 
	  hand[0][6] = -1; 
	  cclicked = 7; //for the SetCard just below here
	 } 
       else hand[0][cclicked-1] = -1; 
 
       if (bSortCards) SortCards(hWnd);
       else SetCard(hWnd, cclicked - 1, 1, 20); 
       cclicked = 0; 
       cdrawn = 0; 
       if (compturn == 2) compturn = 0; 
       else if (ncards[1] > 0) compturn = 1; 
      } 

     else if (cclicked > 0)  /* if we dropped somewhere useless */ 
      { 
       SetCard(hWnd, cclicked - 1, 1, hand[0][cclicked - 1]); 
       cclicked = 0; 
       bInDrag = FALSE; 
      } 
 
     /* select card */  
    if((square > 7) && (square < 15) && (hand[0][tmppt.x] > -1) &&!bButtonUp) 
      {
       if (bDragDrop && (cclicked == 0))  // someone just click-dragged a card 
	 { 
	  bInDrag = TRUE; 
	  bJustInDrag = TRUE; 
	 } 
       if((cclicked > 0) && (bInDrag = TRUE))  //already dragging a selected card 
	 { 
	  cclicked = 0; 
	  bInDrag = FALSE; 
	 } 
       else if (cclicked == 0)
	 {
	  SetCard(hWnd, tmppt.x, 1, scard[tmppt.x][1] + 64); 
	  cclicked = tmppt.x + 1; 
	 }
 
       if ((hand[0][cclicked - 1] > 3) && (cdrawn == 0)) 
	 { 
	  SetCard(hWnd, tmppt.x, 1, scard[tmppt.x][1] - 64); 
	  cclicked = 0;
	  bInDrag = bJustInDrag = FALSE; 
	  lstrcpy((LPSTR)str1, "You must draw a card before you can play one."); 
	  lstrcpy((LPSTR)str2, " "); 
	  HelpUser(hWnd, htype); 
	 } 
      } 
 
    bButtonUp = FALSE; 
    if((winmiles == 700) && (gameover == 3)) 
      { 
       helpcode = 500; 
       if(MessageBox(hWnd, "Do you want to extend the game to 1000 miles?", 
		"Extension", MB_YESNO | MB_DEFBUTTON2) == IDYES) 
	   { 
	    winmiles = 1000; 
	    gameover = 0; 
	   } 
       else gameover = 1; 
       helpcode = 0; 
      } 
    if (gameover == 3)gameover = 1; 
    ncards[0] = 0; 
    for (tx = 0;tx < 7;tx++) if (hand[0][tx] > -1) ncards[0]++; 
   } 
 if ((gameover == 0) && (ncards[0] == 0) && (ncards[1] == 0)) gameover = 4;
 if (oldcurcard != curcard) SetStatusBar2();
} 
 
