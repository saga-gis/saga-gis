/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     listfuncs.inc                     //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     HfT Stuttgart                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     johannes.engels@hft-stuttgart.de       //
//                                                       //
//    contact:    Johannes Engels                        //
//                Hochschule fuer Technik Stuttgart      //
//                Schellingstr. 24                       //
//                70174 Stuttgart                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

/************************************************************************/
/* Vor diese Datei irgendwo includiert wird, muessen                    */
/*    LISTELEMENT             (z.B. #define LISTELEMENT REGION)         */
/* durch ein #define,                                                   */
/*    simple_LISTELEMENT_list (z.B. typedef struct simple_REGION_list)  */
/*    double_LISTELEMENT_list (z.B. typedef struct double_REGION_list)  */
/* durch ein typedef definiert sein!                                    */
/* Diese Strukturen muessen ein Feld simple_LISTELEMENT_list *next      */
/* bzw. zwei Felder simple_LISTELEMENT_list *next, *prev enthalten.     */
/* Funktioniert z.B. mit georec.c                                       */
/************************************************************************/

/* zu tun: Funktionen INSSORTSLIST2, INSSORTDLIST2 in denen der         */ 
/* Vergleich direkt vorgenommen wird:                                   */
/* if (curr -> content.sortkey >= new -> content.sortkey)      etc.     */


#if 0
#ifndef __LISTFUNCS_INC__

#define __LISTFUNCS_INC__

#endif
#endif


#define CONCAT(x,y)    x ## y
#define LCONCAT(x,y)  CONCAT(x,y)

#define SLIST          LCONCAT(LCONCAT(simple_,LISTELEMENT),_list)
#define DLIST          LCONCAT(LCONCAT(double_,LISTELEMENT),_list)
#define APPSLIST       LCONCAT(LCONCAT(append_simple_,LISTELEMENT),_list) 
#define APPDLIST       LCONCAT(LCONCAT(append_double_,LISTELEMENT),_list)
#define APPNEWSLIST    LCONCAT(LCONCAT(append_new_simple_,LISTELEMENT),_list) 
#define APPNEWDLIST    LCONCAT(LCONCAT(append_new_double_,LISTELEMENT),_list) 
#define PRESLIST       LCONCAT(LCONCAT(prepend_simple_,LISTELEMENT),_list) 
#define PREDLIST       LCONCAT(LCONCAT(prepend_double_,LISTELEMENT),_list) 
#define PRENEWSLIST    LCONCAT(LCONCAT(prepend_new_simple_,LISTELEMENT),_list) 
#define PRENEWDLIST    LCONCAT(LCONCAT(prepend_new_double_,LISTELEMENT),_list) 
#define CONCATSLIST    LCONCAT(LCONCAT(concat_simple_,LISTELEMENT),_list)
#define CONCATDLIST    LCONCAT(LCONCAT(concat_double_,LISTELEMENT),_list)
#define APPAFTERSLIST  LCONCAT(LCONCAT(app_after_simple_,LISTELEMENT),_list) 
#define APPAFTERDLIST  LCONCAT(LCONCAT(app_after_double_,LISTELEMENT),_list) 
#define APPBEFOREDLIST LCONCAT(LCONCAT(app_before_double_,LISTELEMENT),_list) 
#define APPBEFORESLIST LCONCAT(LCONCAT(app_before_simple_,LISTELEMENT),_list) 
#define DELETESFIRST   LCONCAT(delete_first_simple_,LISTELEMENT)
#define DELETEDFIRST   LCONCAT(delete_first_double_,LISTELEMENT)
#define DELETESLAST    LCONCAT(delete_last_simple_,LISTELEMENT)
#define DELETEDLAST    LCONCAT(delete_last_double_,LISTELEMENT)
#define REMOVESFIRST   LCONCAT(remove_first_simple_,LISTELEMENT) 
#define REMOVEDFIRST   LCONCAT(remove_first_double_,LISTELEMENT) 
#define REMOVESELEMENT LCONCAT(LCONCAT(remove_simple_,LISTELEMENT),_list) 
#define REMOVEDELEMENT LCONCAT(LCONCAT(remove_double_,LISTELEMENT),_list) 
#define FREESLIST      LCONCAT(LCONCAT(free_simple_,LISTELEMENT),_list)
#define FREEDLIST      LCONCAT(LCONCAT(free_double_,LISTELEMENT),_list)
#define INSSORTSLIST   LCONCAT(LCONCAT(inssort_simple_,LISTELEMENT),_list) 
#define INSSORTDLIST   LCONCAT(LCONCAT(inssort_double_,LISTELEMENT),_list) 
#define COMPELEMENT    LCONCAT(comp_,LISTELEMENT)


int APPSLIST (SLIST **first, SLIST **last, SLIST *new);

int APPDLIST (DLIST **first, DLIST **last, DLIST *new);

int APPNEWSLIST (SLIST **first, SLIST **last);

int APPNEWDLIST (DLIST **first, DLIST **last);

int PRESLIST (SLIST **first, SLIST **last, SLIST *new);

int PREDLIST (DLIST **first, DLIST **last, DLIST *new);

int PRENEWSLIST (SLIST **first, SLIST **last);

int PRENEWDLIST (DLIST **first, DLIST **last);

int CONCATSLIST (SLIST **first, SLIST **last, SLIST *fapp, SLIST *lapp);

int CONCATDLIST (DLIST **first, DLIST **last, DLIST *fapp, DLIST *lapp);

int APPAFTERSLIST (SLIST **first, SLIST **last, SLIST *old, SLIST *new);

int APPAFTERDLIST (DLIST **first, DLIST **last, DLIST *old, DLIST *new);

int APPBEFORESLIST (SLIST **first,
                    SLIST **last,
                    SLIST *old,
                    SLIST *oldprev,
                    SLIST *new);

int APPBEFOREDLIST (DLIST **first, DLIST **last, DLIST *old, DLIST *new);

int DELETESFIRST (SLIST **first, SLIST **last);

int DELETEDFIRST (DLIST **first, DLIST **last);

int DELETESLAST (SLIST **first, SLIST **last, SLIST *last_but_one);

int DELETEDLAST (DLIST **first, DLIST **last);

int REMOVESFIRST (SLIST **first, SLIST **last);

int REMOVEDFIRST (DLIST **first, DLIST **last);

int REMOVESELEMENT (SLIST **first, SLIST **last, SLIST *prev, SLIST *el);

int REMOVEDELEMENT (DLIST **first, DLIST **last, DLIST *el);

int FREESLIST (SLIST **first, SLIST **last);

int FREEDLIST (DLIST **first, DLIST **last);

int INSSORTSLIST (SLIST **first, 
                  SLIST **last, 
                  short ascdesc, 
                  SLIST *new,
                  void *add1,
                  void *add2,
                  int (*comp)(SLIST *, SLIST *, void *, void *));

int INSSORTDLIST (DLIST **first, 
                  DLIST **last, 
                  short ascdesc, 
                  DLIST *new,
                  void *add1,
                  void *add2,
                  int (*comp) (DLIST *, DLIST *, void *, void *));


/************************************************************************/
/* Neues Element an eine Liste hinten dranhaengen                       */
/************************************************************************/

int APPSLIST (SLIST **first,
              SLIST **last,
              SLIST *new)
{
   if (*first == NULL)
   {
      *first = new;
      *last  = new;
   }
   else
   {
      (*last) -> next = new;
   }
   
   *last = new;
   new -> next = NULL;
   
   return 0;
}




int APPDLIST (DLIST **first,
              DLIST **last,
              DLIST *new)
{
   if (*first == NULL)
   {
      *first = new;
      *last  = new;
      new -> prev = NULL;
   }
   else
   {
      (*last) -> next = new;
      new -> prev = *last;
   }
   
   *last = new;
   new -> next = NULL;
   
   return 0;
}



int APPNEWSLIST (SLIST **first,
                 SLIST **last)
{
   SLIST *new_el;

   new_el = (SLIST *) malloc (sizeof (SLIST));
   memset (new_el, 0x00, sizeof(SLIST));

   if (*first == NULL)
   {
      *first = new_el;
      *last  = new_el;
   }
   else
   {
      (*last) -> next = new_el;
   }
   
   *last = new_el;
   
   return 0;
}




int APPNEWDLIST (DLIST **first,
                 DLIST **last)
{
   DLIST *new_el;

   new_el = (DLIST *) malloc (sizeof (DLIST));
   memset (new_el, 0x00, sizeof(DLIST));

   if (*first == NULL)
   {
      *first = new_el;
      *last  = new_el;
   }
   else
   {
      (*last) -> next = new_el;
	  new_el -> prev = *last;
   }
   
   *last = new_el;
   
   return 0;
}







/************************************************************************/
/* Neues Element einer Liste voranstellen                               */
/************************************************************************/

int PRESLIST (SLIST **first,
              SLIST **last,
              SLIST *new)
{
   if (*first == NULL)
   {
      *first = new;
      *last  = new;
      new -> next = NULL;
   }
   else
   {
      new -> next = *first;
   }
   
   *first = new;
   
   return 0;
}




int PRENEWSLIST (SLIST **first,
                 SLIST **last)
{
   SLIST *new_el;

   new_el = (SLIST *) malloc (sizeof (SLIST));
   memset (new_el, 0x00, sizeof(SLIST));


   if (*first == NULL)
   {
      *first = new_el;
      *last  = new_el;
   }
   else
   {
      new_el -> next = *first;
   }
   
   *first = new_el;
   
   return 0;
}






int PREDLIST (DLIST **first,
              DLIST **last,
              DLIST *new)
{
   if (*first == NULL)
   {
      *first = new;
      *last  = new;
      new -> next = NULL;
   }
   else
   {
      new -> next = *first;
      (*first) -> prev = new;
   }
   
   *first = new;
   new -> prev = NULL;
   
   return 0;
}



int PRENEWDLIST (DLIST **first,
                 DLIST **last)
{
   DLIST *new_el;

   new_el = (DLIST *) malloc (sizeof (DLIST));
   memset (new_el, 0x00, sizeof(DLIST));

   if (*first == NULL)
   {
      *first = new_el;
      *last  = new_el;
   }
   else
   {
      new_el -> next = *first;
      (*first) -> prev = new_el;
   }
   
   *first = new_el;
   
   return 0;
}





int CONCATSLIST (SLIST **first, SLIST **last, SLIST *fapp, SLIST *lapp)
{
   /*********************************************************************/
   /* Hier wird die zweite Liste an die erste drangehaengt.             */
   /*********************************************************************/
   
   if (fapp == NULL)
      return 8;
   else if (*first == NULL)
   {
      *first = fapp;
      *last = lapp;
      return 4;
   }
   else
   {
      (*last) -> next = fapp;
      *last = lapp;
      return 0;
   }

   return 0;
}





int CONCATDLIST (DLIST **first, DLIST **last, DLIST *fapp, DLIST *lapp)
{
   /*********************************************************************/
   /* Hier wird die zweite Liste an die erste drangehaengt.             */
   /*********************************************************************/
   
   if (fapp == NULL)
      return 8;
   else if (*first == NULL)
   {
      *first = fapp;
      *last = lapp;
      return 4;
   }
   else
   {
      (*last) -> next = fapp;
      fapp -> prev = *last;
      *last = lapp;
      return 0;
   }

   return 0;
}





/************************************************************************/
/* Neues Element in eine Liste hinter einem bestimmten Element          */
/* einfuegen                                                            */
/************************************************************************/

int APPAFTERSLIST (SLIST **first,
                   SLIST **last,
                   SLIST *old,
                   SLIST *new)
{
   if (old == NULL)
      return 8;

   new -> next = old -> next;
   old -> next = new;
 
   if (old == *last)
      *last = new;
   
   return 0;
}




int APPAFTERDLIST (DLIST **first,
                   DLIST **last,
                   DLIST *old,
                   DLIST *new)
{
   if (old == NULL)
      return 8;

   new -> next = old -> next;
   new -> prev = old;

   if (old == *last)
      *last = new;
   else
      old -> next -> prev = new;
   
   old -> next = new;

   return 0;
}


/************************************************************************/
/* Neues Element in eine Liste vor einem bestimmten Element einfuegen   */
/************************************************************************/

int APPBEFORESLIST (SLIST **first,
                    SLIST **last,
                    SLIST *old,
                    SLIST *oldprev,
                    SLIST *new)
{
   if (old == NULL)
      return 8;

   new -> next = old;

   if (old == *first)
      *first = new;
   else
      oldprev -> next = new;
   
   return 0;
}




int APPBEFOREDLIST (DLIST **first,
                    DLIST **last,
                    DLIST *old,
                    DLIST *new)
{
   if (old == NULL)
      return 8;

   new -> next = old;
   new -> prev = old -> prev;

   if (old == *first)
      *first = new;
   else
      old -> prev -> next = new;

   old -> prev = new;
   
   return 0;
}





int DELETESFIRST (SLIST **first, SLIST **last)
{
   SLIST *zw;

   if (*first == NULL)
      return 8;

   if (*first == *last)
   {
      free (*first);
      *first = NULL;
      *last  = NULL;
      return 0;
   }

   zw = *first;
   *first = zw -> next;
   free (zw);

   return 0;
}


   

int DELETEDFIRST (DLIST **first, DLIST **last)
{
   DLIST *zw;

   if (*first == NULL)
      return 8;

   if (*first == *last)
   {
      free (*first);
      *first = NULL;
      *last  = NULL;
      return 0;
   }

   zw = *first;
   *first = zw -> next;
   free (zw);
   (*first) -> prev = NULL;

   return 0;
}


   


int DELETESLAST (SLIST **first, SLIST **last, SLIST *last_but_one)
{
   if (*first == NULL)
      return 8;

   if (*first == *last)
   {
      free (*first);
      *first = NULL;
      *last  = NULL;
      return 0;
   }

   free (*last);
   last_but_one -> next = NULL;
   *last = last_but_one;

   return 0;
}


   



int DELETEDLAST (DLIST **first, DLIST **last)
{
   DLIST *zw;

   if (*first == NULL)
      return 8;

   if (*first == *last)
   {
      free (*first);
      *first = NULL;
      *last  = NULL;
      return 0;
   }

   zw = (*last) -> prev;
   free (*last);
   zw -> next = NULL;
   *last = zw;

   return 0;
}


   

int REMOVESFIRST (SLIST **first, SLIST **last)
{
   SLIST *zw;

   if (*first == NULL)
      return 8;

   if (*first == *last)
   {
      *first = NULL;
      *last  = NULL;
   }
   else
   {
      zw = *first;
      *first = zw -> next;
      zw -> next = NULL;
   }

   return 0;
}


   


int REMOVEDFIRST (DLIST **first, DLIST **last)
{
   DLIST *zw;

   if (*first == NULL)
      return 8;

   if (*first == *last)
   {
      *first = NULL;
      *last  = NULL;
   }
   else
   {
      zw = *first;
      *first = zw -> next;
      zw -> next = NULL;
   }

   return 0;
}


   






/************************************************************************/
/* Element aus der Liste entfernen                                      */
/************************************************************************/

int REMOVESELEMENT (SLIST **first,
                    SLIST **last,
                    SLIST *prev,
                    SLIST *el)
{
   if (el == NULL)
      return 8;

   if (el == *first)
      *first = el -> next; 
   else
      prev -> next = el -> next;

   if (el == *last)
      *last = prev;

   el -> next = NULL;

   return 0;
}




int REMOVEDELEMENT (DLIST **first,
                    DLIST **last,
                    DLIST *el)
{   
   if (el == NULL)
      return 8;

   if (el == *first)
      *first = el -> next; 
   else
      el -> prev -> next = el -> next;

   if (el == *last)
      *last = el -> prev;
   else
      el -> next -> prev = el -> prev;

   el -> next = NULL;
   el -> prev = NULL;

   return 0;
}





/************************************************************************/
/* Liste freigeben                                                      */
/************************************************************************/

int FREESLIST (SLIST **first, SLIST **last)
{
   SLIST *currpix;
   SLIST *nextpix;

   for (currpix = *first; currpix != NULL; currpix = nextpix)
   {
      nextpix = currpix -> next;
      free (currpix);
   }  

   *first = NULL;
   *last  = NULL;

   return 0;
}




int FREEDLIST (DLIST **first, DLIST **last)
{
   DLIST *currpix;
   DLIST *nextpix;

   for (currpix = *first; currpix != NULL; currpix = nextpix)
   {
      nextpix = currpix -> next;
      free (currpix);
   }  

   *first = NULL;
   *last  = NULL;

   return 0;
}




int INSSORTSLIST (SLIST **first,
                  SLIST **last,
                  short ascdesc,
                  SLIST *new,
                  void *add1,
                  void *add2,
                  int (*comp) (SLIST *, SLIST *, void *, void *))
{
   SLIST *curr;
   SLIST *currprev;

   if (*first == NULL)
   {
      *first = new;
      *last  = new;

      return 0;
   }
   
   currprev = NULL; 

   if (ascdesc > 1)    /* die Liste soll aufsteigend sortiert werden */
   {
      for (curr = *first; curr != NULL; curr = curr -> next)
      {
         if ((*comp) (curr, new, add1, add2) >= 0)  /* curr >= new */
         {
             APPBEFORESLIST (first, last, curr, currprev, new);
             return 0;
         }

         currprev = curr;
     }
   }
   else                /* die Liste soll absteigend sortiert werden  */
   {  
      for (curr = *first; curr != NULL; curr = curr -> next)
      {
         if ((*comp) (curr, new, add1, add2) <= 0)  /* curr <= new */
         {
             APPBEFORESLIST (first, last, curr, currprev, new); 

             return 0;
         }

         currprev = curr;
      }
   }

   return (APPSLIST (first, last, new));
}




int INSSORTDLIST (DLIST **first,
                  DLIST **last,
                  short ascdesc,
                  DLIST *new,
                  void *add1,
                  void *add2,
                  int (*comp) (DLIST *, DLIST *, void *, void *))
{
   DLIST *curr;

   if (*first == NULL)
   {
      *first = new;
      *last  = new;

      return 0;
   }
   
   if (ascdesc > 1)    /* die Liste soll aufsteigend sortiert werden */
   {
      for (curr = *first; curr != NULL; curr = curr -> next)
      {
         if ((*comp) (curr, new, add1, add2) >= 0)  /* curr >= new */
         {
             APPBEFOREDLIST (first, last, curr, new);
             return 0;
         }
     }
   }
   else                /* die Liste soll absteigend sortiert werden  */
   {  
      for (curr = *first; curr != NULL; curr = curr -> next)
      {
         if ((*comp) (curr, new, add1, add2) <= 0)  /* curr <= new */
         {
             APPBEFOREDLIST (first, last, curr, new);
             return 0;
         }
      }
   }

   return (APPDLIST (first, last, new));
}


