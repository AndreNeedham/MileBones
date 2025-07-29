#include <windows.h>
#include "mb.h"

extern short xgap, ygap, xcard, ycard, xsize, ysize, compturn,
	     curcard, gameover, winmiles, helpcode, cclicked,
	     cdrawn;
extern short battle[2], cango[2], rofw[2], splim[2], nsplim[2],
	     hand[2][7], ncoup[2], nsafe[2], safety[2][4],
	     deck[101], two200s[2], ncards[2], mileval[19],
	     nbattle[2];
extern int   score[2], miles[2], scard[9][3], htype;

//in mbcore.c:
void SetCard(HWND, int, int, int);

//GoComp
//
//Computer's card playing logic
//

void GoComp(hWnd)
HWND hWnd;
{
 int i, t, tx, tempcard, checknum, totmiles;
 nbattle[0] = 0;
 nsplim[0] = 0;
 if ((splim[1] == 1) && (nsplim[1] == 1))
   {                             /* speed limit coup fourre */
    for(i=0;i<6;i++)
       if (hand[1][i] ==3)
	 {
	  rofw[1] = 1;
	  ncoup[1]++;
	  score[1] += 400;
	  splim[1] = 0;
	  SetCard(hWnd,1,2,20);
	  if (battle[1] < 8)
	    {
	     battle[1] = 3;
	     SetCard(hWnd,0,2,3);
	    }
	  safety[1][nsafe[1]++] = 3;
	  SetCard(hWnd,nsafe[1]+1,2,35);
	  if (curcard < 101)
	     hand[1][i] = deck[curcard++];
	     else hand[1][i] = -1;
	 }
   }
 if ((nbattle[1] == 1))           /* other coup fourre */
   {
    for(i=0;i<6;i++)
       if ((hand[1][i] == battle[1] - 9) && (battle[1] < 13))
	 {
	  if (battle[1] == 12)
	    {
	     rofw[1] = 1;
	     splim[1] = 0;
	     SetCard(hWnd,1,2,20);
	    }
	  if (rofw[1] == 1)
	    {
	     battle[1] = 3;
	     SetCard(hWnd,0,2,3);
	    }
	  else
	    {
	     battle[1] = 7;
	     SetCard(hWnd,0,2,7);
	    }
	  ncoup[1]++;
	  safety[1][nsafe[1]++] = hand[1][i];
	  score[1] += 400;
	  SetCard(hWnd,nsafe[1]+1,2,32+hand[1][i]);
	  if (curcard < 101)
	     hand[1][i] = deck[curcard++];
	     else hand[1][i] = -1;
	 }
   }

 if (curcard <101) hand[1][6] = deck[curcard++];
 /*if (curcard == 101) */ SetCard(hWnd,7,0,21);
 if ((battle[1] <7) && (cango[1] ==1))    /* check if rofw to play */
   {
    for(i=0;i<7;i++)
       if (hand[1][i] == 3)
	 {
	  compturn = 0;
	  rofw[1] = 1;
	  battle[1] = 3;
	  score[1] += 100;
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	  SetCard(hWnd,0,2,3);
	  safety[1][nsafe[1]++] = 3;
	  SetCard(hWnd,nsafe[1]+1,2,3);
	  splim[1] = 0;
	  SetCard(hWnd,1,2,20);
	 }
    }
 if ((battle[1] < 7) && (cango[1] == 1) && (rofw[1] == 0))
    /* check if a go to play */
   {
    i = 0;
    while((i < 7) && (compturn == 1))
      {
       if (hand[1][i] == 7)
	 {
	  compturn = 0;
	  battle[1] = 7;
	  SetCard(hWnd,0,2,7);
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	 }
       i++;
      }
   }
 if((compturn == 1) && ((battle[1] == 3) || (battle[1] == 7)))
   {
    totmiles = miles[1];
    for (i = 0; i < 7; i++)
      {
       tempcard = hand[1][i];
       if((tempcard > 13) && (tempcard < 19)) totmiles += mileval[tempcard];
      }
			  /* play 200 mileage */
    i = 0;
    while ((i < 7) && (compturn == 1))
      {
       if((hand[1][i] == 14) && (two200s[1] < 2)
	  && (miles[1] <= (winmiles-200)) && (splim[1] == 0))
	 {
	  if (miles[1] + 200 == winmiles)
	    {
	     if((totmiles >= 1000) && (winmiles == 700)) winmiles = 1000;
	     else
	       {
		int j = 0;
		while (j<7)  /* before winning, play all safeties */
		  {
		   if ((hand[1][j] <3) && (hand[1][j] > -1))
		     {
		      safety[1][nsafe[1]++] = hand[1][j];
		      SetCard(hWnd, nsafe[1]+1, 2, hand[1][j]);
		      score[1] += 100;
		      if (curcard < 101)        /* draw another card */
			 hand[1][j] = deck[curcard++];
		      else hand[1][j] = -1;
		     }
		   j++;
		  }
		}
	     }
	  miles[1] += 200;
	  if (miles[1] == winmiles) gameover = 2;
	  compturn = 0;
	  two200s[1]++;
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	  SetCard(hWnd, 6, 2, 23);
	 }
       i++;
      }
    checknum = 15;
    while(checknum < 19)
      {
       i = 0;
       while ((i < 7) && (compturn == 1))  /* other mileage */
	 {
	  tempcard = hand[1][i];
	  if((tempcard == checknum) && ((tempcard > 16) || (splim[1] == 0)))
	    if(miles[1] + mileval[tempcard] <= winmiles)
	      {
	       if (miles[1] + mileval[tempcard] == winmiles)
		 {
		  if((totmiles >= 1000) && (winmiles == 700)) winmiles = 1000;
		  else
		    {
		     int j = 0;
		     while (j < 7)   /* before winning, play all safeties */
		       {
			if ((hand[1][j] < 3) && (hand[1][j] > -1))
			  {
			   safety[1][nsafe[1]++] = hand[1][j];
			   SetCard(hWnd, nsafe[1]+1, 2, hand[1][j]);
			   score[1] += 100;
			   if (curcard < 101)        /* draw another card */
			     hand[1][j] = deck[curcard++];
			   else hand[1][j] = -1;
			  }
			j++;
		       }
		    }
		 }
	       miles[1] += mileval[tempcard];
	       if (miles[1] == winmiles) gameover = 2;
	       compturn = 0;
	       hand[1][i] = hand[1][6];
	       hand[1][6] = -1;
	       SetCard(hWnd, 6, 2, 23);
	       checknum = 18;
	      }
	  i++;
	 }
       checknum++;
      }
   }
 if((compturn == 1) && (cango[1] == 0))
   {
    i = 0;
    while ((i < 7) && (compturn == 1))
      {
       tempcard = hand[1][i];
       if ((tempcard == battle[1] - 9) && (tempcard < 4))
	 {                             /* play safety as remedy */
	  battle[1] = 0;
	  score[1] += 100;
	  safety[1][nsafe[1]++] = tempcard;
	  SetCard(hWnd, nsafe[1]+1, 2, tempcard);
	  cango[1] = 1;
	  compturn = 0;
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	  if (tempcard == 3) rofw[1] = 1;
	  if (rofw[1] == 1)
	    {                          /* play rofw as remedy */
	     battle[1] = 3;
	     SetCard(hWnd, 0, 2, 3);
	     splim[1] = 0;
	     SetCard(hWnd, 1, 2, 20);
	    }
	  else SetCard(hWnd, 0, 2, 20);
	 }
       if ((compturn == 1) && (tempcard == battle[1] - 5)
	  && (tempcard > 3) && (tempcard < 8))
	 {                              /* play remedy */
	  battle[1] = 0;
	  cango[1] = 1;
	  compturn = 0;
	  if (rofw[1] == 1) tempcard = 3;
	  if (tempcard == 3) battle[1] = 3;
	  if (tempcard == 7) battle[1] = 7;
	  SetCard(hWnd, 0, 2, tempcard);
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	 }
       i++;
      }
   }
 if((compturn == 1) && (splim[1] == 1))
   {                                     /* play eolim */
    i = 0;
    while((i < 7) && (compturn == 1))
      {
       if (hand[1][i] == 8)
	 {
	  splim[1] = 0;
	  compturn = 0;
	  SetCard(hWnd, 1, 2, 8);
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	 }
       i++;
      }
   }
 if(compturn == 1)
   {                                     /* play dirt */
    i = 0;
    while((i < 7) && (compturn == 1) && ((battle[0] == 3) || (battle[0] == 7)))
      {
       tempcard = hand[1][i];
       if ((tempcard > 8) && (tempcard < 13))
	 {
	  tx = 0;
	  for(t = 0; t<4;t++) if (safety[0][t] == tempcard -9)tx = 3;
	  if (tx == 0)
	    {
	     compturn = 0;
	     cango[0] = 0;
	     battle[0] = tempcard;
	     nbattle[0] = 1;
	     SetCard(hWnd, 0, 0, tempcard);
	    }
	 } else if ((tempcard == 13) && (rofw[0] == 0) && (splim[0] == 0))
	    {
	     compturn = 0;
	     splim[0] = 1;
	     nsplim[0] = 1;
	     SetCard(hWnd, 1, 0, 13);
	    }
       if (compturn == 0)
	 {
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	 }
       i++;
      }
   }
/* new 2.1 code */
if((compturn == 1) && (curcard == 101))
   for (i = 6; i >=0; i--)
     /* play out safeties if deck empty and nothing else to do */
   {
    if((hand[1][i] < 4) && (hand[1][i] > -1))
      {
      if (hand[1][i] == 3)
	 {
	  rofw[1] = 1;
	  splim[1] = 0;
	  SetCard(hWnd, 1, 2, 20); //erase splim
	 }
       score[1] += 100;
       safety[1][nsafe[1]++] = hand[1][i];
       SetCard(hWnd, nsafe[1]+1, 2, hand[1][i]);
       hand[1][i] = -1;
       compturn = 0;
      }
   }

 if((compturn == 1) && (nsafe[0] > 0)) /* throw out useless dirt */
   {
    i = 0;
    while((i < 7) && (compturn == 1))
      {
       for(tx = 0;tx < nsafe[0];tx++)
       if (hand[1][i] == safety[0][tx] + 9)
	 {
	  compturn = 0;
	  scard[7][2] = hand[1][i];
	  SetCard(hWnd, 7, 2, hand[1][i]); //draw discard
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	 }
       i++;
      }
   }

 if((compturn == 1) && (nsafe[1] > 0)) /* throw out useless remedy */
   {
    i = 0;
    while((i < 7) && (compturn == 1))
      {
       for(tx = 0;tx < nsafe[1];tx++)
       if (hand[1][i] == safety[1][tx] + 4)
	 {
	  compturn = 0;
	  scard[7][2] = hand[1][i];
	  SetCard(hWnd, 7, 2, hand[1][i]); //draw discard
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	 }
       i++;
      }
   }

 if(compturn == 1) for (i = 6; i >= 0; i--)     /* discard all but safety */
   {
    if ((compturn == 1) && (hand[1][i] > 3))
      {
       compturn = 0;
       if (hand [1][6] > -1)
	 {
	  scard[7][2] = hand[1][i];
	  SetCard(hWnd, 7, 2, hand[1][i]); //draw discard
	  hand[1][i] = hand[1][6];
	  hand[1][6] = -1;
	 }
       else hand[1][i] = -1;
      }
   }

 if(compturn == 1) for (i = 6; i >=0; i--)     /* play out safeties */
   {
    if (hand[1][i] >-1)
      {
       score[1] += 100;
       safety[1][nsafe[1]++] = hand[1][i];
       SetCard(hWnd, nsafe[1] + 1, 2, hand[1][i]);
       hand[1][i] = -1;
      }
   }
 compturn = 0;
 nbattle[1] = 0;
 nsplim[1] = 0;
 ncards[1] = 0;
 for (i=0; i < 7; i++) if (hand[1][i] > -1) ncards[1]++;
 if ((ncards[1] > 0) && (ncards[0] == 0)) compturn = 1;  /* finish game properly */
 if ((gameover == 0) && (ncards[0] == 0) && (ncards[1] == 0)) gameover = 4;
}
