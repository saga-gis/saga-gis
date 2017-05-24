/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       geovinc.c                       //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "storeorg.h"
#include "geovinc.h"


/************************************************************************/
/* Funktioniert mit listfuncs.in2                                       */
/************************************************************************/

#define LISTELEMENT PIXEL
#include "listfuncs.inc"
#undef LISTELEMENT

#define LISTELEMENT REGION
#include "listfuncs.inc"
#undef LISTELEMENT


static int comp_PIXEL (simple_PIXEL_list *curr,
                       simple_PIXEL_list *_new,
                       void *add1,
                       void *dummy)
{
   /*********************************************************************/
   /*  1 -- der Grauwert von curr ist groesser                          */
   /*  0 -- beide Grauwerte sind gleich gross                           */
   /* -1 -- der Grauwert von new ist groesser                           */
   /*********************************************************************/

   unsigned short row;   
   unsigned short col;   
   unsigned short nrow;   
   unsigned short ncol;  
   double **greyvalues;

   greyvalues = (double **) add1;

   row = curr -> row;
   col = curr -> col;

   nrow = _new -> row;
   ncol = _new -> col;

   if (greyvalues [row][col] > greyvalues [nrow][ncol])
      return 1;
   else if (greyvalues [row][col] == greyvalues [nrow][ncol])
      return 0;
   else
      return -1;
}
                       



static int new_pixel_in_region (simple_REGION_list *maxreg, 
                                unsigned short row, 
                                unsigned short col)
{
   simple_PIXEL_list *_new;

   _new = (simple_PIXEL_list *) malloc (sizeof (simple_PIXEL_list));
   memset (_new, 0x00, sizeof (simple_PIXEL_list));
   
   _new -> row = row;
   _new -> col = col;

   append_simple_PIXEL_list (&(maxreg -> first),
                             &(maxreg -> last),
                             _new);
   return 0;   
}                                     



static int new_region_in_region_list (simple_REGION_list **first,
                                      simple_REGION_list **last)
{
   simple_REGION_list *_new;
   
   _new = (simple_REGION_list *) malloc (sizeof (simple_REGION_list));
   memset (_new, 0x00, sizeof (simple_REGION_list));
  
   append_simple_REGION_list (first, last, _new);
   
   return 0;   
}                                     


static int free_pixlist_of_region (simple_REGION_list *mr)
{
   free_simple_PIXEL_list (&(mr -> first), &(mr -> last));  
 
   return 0;
}   



static int free_last_region (simple_REGION_list **mr_first,
                             simple_REGION_list **mr_last,
                             simple_REGION_list *last_but_one)
{
   /*******************************************************************/
   /* Hier wird vorausgesetzt, dass die Liste mindestens zwei         */
   /* Regionen beinhaltet                                             */
   /*******************************************************************/

   free_pixlist_of_region (*mr_last);
   return delete_last_simple_REGION (mr_first, mr_last, last_but_one);
}


static int flat_region_list (simple_REGION_list **mr_first,
                             simple_REGION_list **mr_last,
                             simple_PIXEL_list **flat_pixels_first,
                             simple_PIXEL_list **flat_pixels_last,
                             double **marker)
{
   simple_REGION_list *curr_reg;
   simple_PIXEL_list *_new;

   unsigned short row;
   unsigned short col;

   /*********************************************************************/
   /* Hier wird vorausgesetzt, dass es mindestens eine Region gibt      */
   /* Wir nehmen von jeder Maximalregion nur ein Pixel.                 */
   /*********************************************************************/

   for (curr_reg = *mr_first; curr_reg != NULL; curr_reg = curr_reg -> next)
   {
      row = curr_reg -> first -> row; 
      col = curr_reg -> first -> col; 

      _new = (simple_PIXEL_list *) malloc (sizeof (simple_PIXEL_list));
      memset (_new, 0x00, sizeof (simple_PIXEL_list));
   
      _new -> row = row;
      _new -> col = col;
      _new -> greyvalue = marker [row][col];

      append_simple_PIXEL_list (flat_pixels_first, flat_pixels_last, _new);

      free_pixlist_of_region (curr_reg);
   }

   free_simple_REGION_list (mr_first, mr_last);

   return 0;
}


static int single_check_neighbours (unsigned short numrows,
                                    unsigned short numcols,
                                    unsigned short row,
                                    unsigned short col,
                                    double **greyvalues,
                                    short *maxflag,
                                    short *ind_plateau)
{
   /*******************************************************************/
   /* Hier wird geprueft, ob Pixel (row, col) zu einem regionalen     */
   /* Maximum gehoeren koennte.                                       */
   /* maxflag =  1: Grauwert im Pixel groesser oder gleich wie die    */
   /*               Grauwerte aller Nachbarn                          */
   /* maxflag = -1: Grauwert im Pixel kleiner als in mindestens einem */
   /*                                                       Nachbarn  */
   /*                                                                 */
   /* maxflag sollte von der aufrufenden Funktion mit 1 initialisiert */
   /* sein!                                                           */
   /*                                                                 */
   /* ind_plateau = 1: Es existiert mindestens ein Nachbar mit        */
   /*                  demselben Grauwert                             */
   /*                                                                 */
   /* ind_plateau muss von der aufrufenden Funktion mit 0             */
   /* initialisiert sein!                                             */
   /*******************************************************************/
   
   unsigned short nrow;
   unsigned short ncol;
   unsigned short rowmin;
   unsigned short colmin;
   unsigned short rowmax;
   unsigned short colmax;
   

   /*******************************************************************/
   /* Hier wird vorerst nur die 4-er Nachbarschaft betrachtet.        */
   /*******************************************************************/

   rowmin = (row == 0) ? 0 : row - 1;
   colmin = (col == 0) ? 0 : col - 1;
   rowmax = (row == numrows - 1) ? row : row + 1;
   colmax = (col == numcols - 1) ? col : col + 1;
   
   /*******************************************************************/
   /* Hier wird nur geprueft, es wird keine Liste der Pixel des       */
   /* eventuellen Maximums angelegt.                                  */
   /*******************************************************************/

   for (nrow = rowmin; nrow <= rowmax; nrow ++)
   {
	  //#pragma omp parallel for
      for (ncol = colmin; ncol <= colmax; ncol ++)
      {
         /*************************************************************/
         /* Im ersten Fall liegen das aktuelle und das Nachbar-Pixel  */
         /* diagonal (8-Nachbarschaft), im zweiten sind sie identisch */
         /*************************************************************/

         if ((ncol != col && nrow != row) || (ncol == col && nrow == row))
            continue;
            
         if (greyvalues [row][col] < greyvalues [nrow][ncol])
            *maxflag = -1;
         else if (greyvalues [nrow][ncol] == greyvalues [row][col])
            *ind_plateau = 1;
      }
   }
   
   return 0;
}


static int check_plateau (unsigned short numrows,                   
                          unsigned short numcols,                   
                          double **greyvalues,
                          unsigned char **vis,
                          simple_REGION_list *mr,  
                          simple_PIXEL_list *currpix,
                          short *maxflag)
{
   /*******************************************************************/
   /* Hier wird eine Pixelliste eines Plateaus angelegt, also einer   */
   /* zusammenhaengenden Region von Pixeln, die alle denselben        */
   /* Grauwert besitzen. Gleichzeitig wird festgestellt ob es sich    */
   /* bei dieser Region um ein regionales Maximum handelt.            */
   /* Sobald ein Nachbarpixel des Plateaus angetroffen wird, dessen   */
   /* Grauwert den des Plateaus uebertrifft, ist klar, dass es sich   */
   /* bei dem Plateau nicht um ein regionales Maximum handeln kann.   */
   /* Das Plateau wird trotzdem weiter abgearbeitet, damit alle Pixel */
   /* des Plateaus als besucht gekennzeichnet werden koennen. Erst    */
   /* danach wird die Liste bzw. Region wieder aufgeloest.            */
   /*******************************************************************/
   
   unsigned short row;
   unsigned short col;
   unsigned short nrow;
   unsigned short ncol;
   unsigned short rowmin;
   unsigned short colmin;
   unsigned short rowmax;
   unsigned short colmax;


   row = currpix -> row;
   col = currpix -> col;

   /*******************************************************************/
   /* Hier wird vorerst nur die 4-er Nachbarschaft betrachtet.        */
   /*******************************************************************/

   rowmin = (row == 0) ? 0 : row - 1;
   colmin = (col == 0) ? 0 : col - 1;
   rowmax = (row == numrows - 1) ? row : row + 1;
   colmax = (col == numcols - 1) ? col : col + 1;
   
   
   for (nrow = rowmin; nrow <= rowmax; nrow ++)
   {
	  //#pragma omp parallel for
      for (ncol = colmin; ncol <= colmax; ncol ++)
      {
         /*************************************************************/
         /* Im ersten Fall liegen das aktuelle und das Nachbar-Pixel  */
         /* diagonal (8-Nachbarschaft), im zweiten sind sie identisch */
         /*************************************************************/
 
        if ((ncol != col && nrow != row) || (ncol == col && nrow == row))
            continue;
            
         /*************************************************************/
         /* Wir machen hier weiter, auch wenn das Nachbarpixel schon  */
         /* mal besucht wurde, da sonst eventuell maxflag nicht       */
         /* richtig gesetzt wird.                                     */
         /*************************************************************/
            
         if (greyvalues [nrow][ncol] == greyvalues [row][col])
         {
            if (vis [nrow][ncol] == 0)
            {
               new_pixel_in_region (mr, nrow, ncol);
               

               /*******************************************************/
               /* In die Liste gestellt ist hier so gut wie schon     */
               /* besucht. Das Pixel wird markiert, damit es nicht    */
               /* nochmal in die Liste reinkommt                      */
               /*******************************************************/
               
               vis [nrow][ncol] = 1;
            }   
         }   
         else if (greyvalues [nrow][ncol] > greyvalues [row][col])
         {
            /**********************************************************/
            /* Dieser Nachbarpunkt gehoert nicht zum Plateau, wird    */
            /* also nicht an die Liste angehaengt. Dennoch gibt es    */
            /* hier kein break. Falls ein Plateau gefunden wurde,     */
            /* wird es immer als Ganzes bearbeitet, auch wenn schon   */
            /* klar ist, dass es sich nicht um ein Maximum handelt!   */
            /**********************************************************/

            *maxflag = -1;
         }
         else
         {
            ;         

            /**********************************************************/
            /* Da passiert gar nichts.                                */
            /**********************************************************/
         }
      }
   }
   
   return 0;
}


static int find_regional_maxima (unsigned short numrows, 
                                 unsigned short numcols,
                                 simple_REGION_list **mr_first,
                                 simple_REGION_list **mr_last,
                                 short *num_regions,
                                 double **greyvalues)
{
   /*******************************************************************/
   /* Es werden Listen von regionalen Maxima erzeugt                  */
   /*******************************************************************/
   
   unsigned short row;
   unsigned short col;
   short maxflag;
   short ind_plateau;
   simple_PIXEL_list *currpix;
   simple_REGION_list *last_but_one = NULL;
  
   unsigned char **vis;


   vis = (unsigned char **) matrix_all_alloc (numrows, numcols, 'U', 0);

   new_region_in_region_list (mr_first, mr_last);

   *num_regions = 0;
  

   for (row = 0; row < numrows; row ++)
   {
	  //#pragma omp parallel for
      for (col = 0; col < numcols; col ++)
      {
         if (vis [row][col] != 0)
            continue;
            
         maxflag = 1;
         ind_plateau = 0;
   
         single_check_neighbours (numrows, numcols, row, col, greyvalues, &maxflag, &ind_plateau);

         /*************************************************************/
         /* Das aktuelle Pixel der Schleife wird hier als besucht     */
         /* markiert, auch wenn es das erste Pixel eines Plateaus     */
         /* ist (die uebrigen Pixel des Plateaus werden dann in       */
         /* check_plateau_us markiert).                               */
         /*************************************************************/

         vis [row][col] = 1;
                                     
         if (ind_plateau == 0)
         {
            /**********************************************************/
            /* Kein Nachbarpixel hat denselben Grauwert wie das       */
            /* aktuelle Pixel                                         */
            /**********************************************************/
            
            if (maxflag == 1)
            {
               /*******************************************************/
               /* Ein Maximum, bestehend aus einem einzigen Pixel     */
               /*******************************************************/
               
               last_but_one = *mr_last;    
               new_pixel_in_region (*mr_last, row, col);
               (*num_regions) ++;
               new_region_in_region_list (mr_first, mr_last);
            }
            else
            {
               ;
               
               /*******************************************************/
               /* Kein Maximum, da tun wir gar nichts                 */
               /*******************************************************/
            }
            
            continue;  
         }
         

         /*************************************************************/
         /* Es ist ein Plateau                                        */
         /*************************************************************/
         
         new_pixel_in_region (*mr_last, row, col);
         //#pragma omp parallel for
         for (currpix = (*mr_last) -> first; currpix != NULL; currpix = currpix -> next)
         {
            check_plateau (numrows, numcols, greyvalues, vis, *mr_last, currpix, &maxflag);
         }
		 
         if (maxflag == -1)
            free_pixlist_of_region (*mr_last);
         else
         {
            /**********************************************************/
            /* Ein neues Maximum-Plateau                              */
            /**********************************************************/
            last_but_one = *mr_last;    
            new_region_in_region_list (mr_first, mr_last);

            (*num_regions) ++;
         }
      }
   }

   free_last_region (mr_first, mr_last, last_but_one);
   matrix_all_free ((void **) vis);
   
   return 0;
}




#define MINIMUM(a,b) ((a)<(b))?a:b


int geodesic_morphological_reconstruction (
                unsigned short numrows,
                unsigned short numcols,
                double **mask,
                double **marker)
{
   int k;

   simple_REGION_list *mr_first = NULL;
   simple_REGION_list *mr_last = NULL;

   simple_PIXEL_list *first_pix = NULL;
   simple_PIXEL_list *last_pix = NULL;
   simple_PIXEL_list *curr_pix;
   simple_PIXEL_list *_new;

   short num_regions;

   unsigned short rowmin;
   unsigned short colmin;
   unsigned short rowmax;
   unsigned short colmax;
   unsigned short row;
   unsigned short col;
   unsigned short nrow;
   unsigned short ncol;

   find_regional_maxima (numrows, numcols, &mr_first, &mr_last, &num_regions, marker);
   
   /*********************************************************************/
   /* Hier wird jetzt die Regionen-Liste geplaettet, d.h. in eine Liste */
   /* von Pixeln ueberfuehrt. Diese Liste wird schon nach Grauwerten    */
   /* sortiert.                                                         */
   /*********************************************************************/

   flat_region_list (&mr_first, &mr_last, &first_pix, &last_pix, marker);
  
   //memset (*marker, 0x00, numrows * numcols * sizeof (double));	// this does not work with negative cell values, so we use the following:

   for (row = 0; row < numrows; row ++)
   {
	   for (col = 0; col < numcols; col ++)
	   {
		  marker [row][col] = -9.9e+19;
	   }
   }


   for (curr_pix = first_pix; curr_pix != NULL; curr_pix = curr_pix -> next)
   {
      row = curr_pix -> row;
      col = curr_pix -> col;
	  marker [row][col] = curr_pix -> greyvalue;
   }

   /*********************************************************************/
   /* Hier kommt jetzt die alles entscheidende Schleife, in der die     */
   /* Dilatation vorgenommen wird. Das vorderste Element der Pixelliste */
   /* wird herausgenommen; seine Nachbarn werden in die Pixelliste      */
   /* einsortiert, falls ihr Grauwert geaendert werden kann.            */
   /*********************************************************************/

   for (curr_pix = first_pix, k = 1; curr_pix != NULL;  curr_pix = first_pix, k ++)
   {
      row = curr_pix -> row;
      col = curr_pix -> col;
	  delete_first_simple_PIXEL (&first_pix, &last_pix);

      /****************************************************************/
      /* Hier wird vorerst nur die 4-er Nachbarschaft betrachtet.     */
      /****************************************************************/

      rowmin = (row == 0) ? 0 : row - 1;
      colmin = (col == 0) ? 0 : col - 1;
      rowmax = (row == numrows - 1) ? row : row + 1;
      colmax = (col == numcols - 1) ? col : col + 1;
      for (nrow = rowmin; nrow <= rowmax; nrow ++)
      {
		for (ncol = colmin; ncol <= colmax; ncol ++)
		{
		/*************************************************************/
		/* Im ersten Fall liegen das aktuelle und das Nachbar-Pixel  */
		/* diagonal (8-Nachbarschaft), im zweiten sind sie identisch */
		/*************************************************************/	
    	   if ((ncol != col && nrow != row) || (ncol == col && nrow == row))
				continue;
	 	   if (marker [nrow][ncol] >= marker [row][col] || marker [nrow][ncol] == mask [nrow][ncol])
				continue;

			marker [nrow][ncol] = MINIMUM((marker[row][col]),(mask[nrow][ncol]));
			_new = (simple_PIXEL_list *) malloc (sizeof(simple_PIXEL_list));
            memset (_new, 0x00, sizeof(simple_PIXEL_list));
			_new -> row = nrow;
			_new -> col = ncol;
            append_simple_PIXEL_list (&first_pix, &last_pix, _new);
         }
      }
   }

   return 0;
}


