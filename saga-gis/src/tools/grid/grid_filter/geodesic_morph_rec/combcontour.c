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
//                     combcontour.c                     //
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
#include "combcontour.h"


#ifdef MATLAB
#include "mex.h"
#endif


#define LISTELEMENT PIXELC
#include "listfuncs.inc"
#undef LISTELEMENT

#define LISTELEMENT REGIONC
#include "listfuncs.inc"
#undef LISTELEMENT

#define LISTELEMENT INNER_REGION
#include "listfuncs.inc"
#undef LISTELEMENT

/*
static int append_new_simple_PIXELC_to_list (simple_PIXELC_list **first_pix,
									         simple_PIXELC_list **last_pix)
{
   simple_PIXELC_list *new_pix;

   new_pix = (simple_PIXELC_list *) malloc (sizeof (simple_PIXELC_list));
   memset (new_pix, 0x00, sizeof(simple_PIXELC_list));
   append_simple_PIXELC_list (first_pix, last_pix, new_pix);

   return 0;
}
*/



int background_region_growing (unsigned char **bin_image, 
	                           unsigned short numrows, 
							   unsigned short numcols, 
							   unsigned short row, 
							   unsigned short col)
{
   /*****************************************************************/
   /* Starting with the seed point (row, col), the function updates */
   /* the pixels of the corresponding connected region of a binary  */
   /* image by means of a region growing to zero.				    */
   /* The region growing is based on a 4-neighbourhood.             */
   /*****************************************************************/

   unsigned short rowmin; 
   unsigned short rowmax;
   unsigned short colmin;
   unsigned short colmax;
   unsigned short nrow; 
   unsigned short ncol;

   simple_PIXELC_list *curr_pix;
   simple_PIXELC_list *first_pix = NULL;
   simple_PIXELC_list *last_pix = NULL;


   append_new_simple_PIXELC_list (&first_pix, &last_pix);
   last_pix -> row = row;
   last_pix -> col = col;
   
   for (curr_pix = first_pix; curr_pix != NULL;  curr_pix = first_pix)
   {
      row = curr_pix -> row;
      col = curr_pix -> col;
	  delete_first_simple_PIXELC (&first_pix, &last_pix);
	  bin_image [row][col] = 0;

      rowmin = (row == 0) ? 0 : row - 1;
      colmin = (col == 0) ? 0 : col - 1;
      rowmax = (row == numrows - 1) ? row : row + 1;
      colmax = (col == numcols - 1) ? col : col + 1;

      for (nrow = rowmin; nrow <= rowmax; nrow ++)
      {
		 for (ncol = colmin; ncol <= colmax; ncol ++)
		 {
   		    /*************************************************************/
		    /* In the first case the current pixel and the neighbouring  */
		    /* pixel are diagonal adjacents, in the second case they are */
		    /* identical, in the third case the neighbouring pixel is    */
		    /* a background pixel, in the fourth case it is a foreground */
		    /* pixel, but already in the list.                           */
		    /*************************************************************/	

    	    if ((ncol != col && nrow != row) || (ncol == col && nrow == row) ||
			    bin_image [nrow][ncol] == 0 || bin_image [nrow][ncol] == 99)
			   continue;

            append_new_simple_PIXELC_list (&first_pix, &last_pix);
			last_pix -> row = nrow;
			last_pix -> col = ncol;
			bin_image [nrow][ncol] = 99;
         }
      }
   }

   return 0;
}





int free_regions (simple_REGIONC_list **first_region, simple_REGIONC_list **last_region)
{
   simple_REGIONC_list *reg_curr;
   simple_REGIONC_list *reg_next;
   simple_INNER_REGION_list *inner_next;
   simple_INNER_REGION_list *inner_curr;

   for (reg_curr = *first_region; reg_curr != NULL; reg_curr = reg_next)
   {
      reg_next = reg_curr -> next;

      free_simple_PIXELC_list (&(reg_curr -> first_pix), &(reg_curr -> last_pix)); 

	  for (inner_curr = reg_curr -> inner_first; inner_curr != NULL; inner_curr = inner_next)
	  {
         inner_next = inner_curr -> next;
         free_simple_PIXELC_list (&(inner_curr -> first_pix), &(inner_curr -> last_pix));
		 free (inner_curr);
	  }

	  free (reg_curr);
   }

   return 0;
}   




/***********************************************************************/
/* Konventionen für Koordinatensystem und Richtungen:                  */
/*                                                                     */ 
/*                                     direction                       */ 
/* (row, col) --> (row, col + 1)          1                            */ 
/* (row, col) --> (row + 1, col)          2                            */ 
/* (row, col) --> (row, col - 1)          3                            */ 
/* (row, col) --> (row - 1, col)          4                            */ 
/***********************************************************************/

static int neighbour (unsigned short row,
                      unsigned short col,
                      unsigned char curr_dir,
                      unsigned short *nb_row,
                      unsigned short *nb_col)
{
   switch (curr_dir)
   {
      case 1:
         *nb_row = row;
         *nb_col = col + 1;
         break;

      case 2:
         *nb_row = row + 1;
         *nb_col = col;
         break;

      case 3:
         *nb_row = row;
         *nb_col = col - 1;
         break;

      case 4:
         *nb_row = row - 1;
         *nb_col = col;
         break;

      default:
         break;
   }


   return 0;
}




#define DIRECTION_RIGHT(x) (((x)==1)?4:(x)-1)
#define DIRECTION_LEFT(x) (((x)==4)?1:(x)+1)

#if 0
static int neighbour_right (unsigned short row,
                            unsigned short col,
                            unsigned char curr_dir,
                            unsigned short *nb_row,
                            unsigned short *nb_col)
{
   neighbour (row, col, (DIRECTION_RIGHT(curr_dir)), nb_row, nb_col);
   return 0;
}
#endif



static void set_contour_point (simple_PIXELC_list *currpix, 
	                            unsigned char direction, 
							    unsigned short row, 
							    unsigned short col)
{
   /***********************************************************************/
   /* "direction" indicates the direction of the edge between foreground  */
   /* and background as well as its location. The function sets the       */
   /* starting point within the pixel structure. If the coordinate of the */
   /* lower left corner of the pixel are (col, row), the start and end    */
   /* points of the edges are, in particular                              */
   /*                                                                     */
   /* direction   start point       end point                             */
   /*     1       (col, row)        (col+1, row)                          */
   /*     2       (col+1, row)      (col+1, row+1)                        */
   /*     3       (col+1, row+1)    (col, row+1)                          */
   /*     4       (col, row+1)      (col, row) ,                          */
   /*                                                                     */
   /* therefore:                                                          */
   /***********************************************************************/

   switch (direction)
   {
      case 1:
         currpix -> row = row;
         currpix -> col = col;
		 break;

	  case 2:
         currpix -> row = row;
         currpix -> col = col + 1;
		 break;

	  case 3:
         currpix -> row = row + 1;
         currpix -> col = col + 1;
		 break;

	  case 4:
         currpix -> row = row + 1;
         currpix -> col = col;
		 break;

	  default:
		  break;
   }

   return;
}




static int next_border_pixel_0 (unsigned short row,
                                unsigned short col,
                                unsigned char *curr_dir,
                                unsigned char **bin_image,
                                long **symb_image,
                                long inout,
                                long label,
                                simple_PIXELC_list **first,
                                simple_PIXELC_list **last,
                                unsigned short *next_row,
                                unsigned short *next_col)
{
   unsigned short nb_row;
   unsigned short nb_col;
   unsigned short im_row;
   unsigned short im_col;
   unsigned char zw_dir;
   short k;


   for (k = 0; k < 3; k ++)
   {
      neighbour (row, col, *curr_dir, &nb_row, &nb_col);

      if (bin_image [nb_row][nb_col] == 0)
      {
         /**************************************************************/
         /* Wir drehen so lange gegen den Uhrzeigersinn weiter, bis    */
         /* das Nachbarpixel auch zum Vordergrund gehört.              */
         /**************************************************************/

         symb_image [nb_row][nb_col] = inout;
         (*curr_dir) = DIRECTION_LEFT(*curr_dir);

         append_new_simple_PIXELC_list (first, last);
		 set_contour_point (*last, *curr_dir, row, col);
      }
      else
         break;
   }

   /*********************************************************************/
   /* Es muss geprüft werden, ob das rechte Nachbarpixel des neuen      */
   /* Pixels auch zum Hintergrund gehört. Wenn nicht gehen wir gleich   */
   /* noch eins weiter "rechts um die Ecke".                            */
   /*********************************************************************/

   zw_dir = DIRECTION_RIGHT(*curr_dir);
   neighbour (nb_row, nb_col, zw_dir, &im_row, &im_col);

   if (bin_image [im_row][im_col] != 0)
   {
      symb_image [nb_row][nb_col] = label;

      nb_row = im_row;
      nb_col = im_col;
      *curr_dir = zw_dir;
   }
   else
      symb_image [im_row][im_col] = inout;

   *next_row = nb_row;
   *next_col = nb_col;
   symb_image [nb_row][nb_col] = label;

   append_new_simple_PIXELC_list (first, last);
   set_contour_point (*last, *curr_dir, nb_row, nb_col);

   return 0;
}





static int next_border_pixel_1 (unsigned short row,
                                unsigned short col,
                                unsigned char *curr_dir,
                                unsigned char **bin_image,
                                long **symb_image,
                                long inout,
                                long label,
                                simple_PIXELC_list **first,
                                simple_PIXELC_list **last,
                                unsigned short *next_row,
                                unsigned short *next_col)
{
   unsigned short nb_row;
   unsigned short nb_col;
   unsigned short im_row;
   unsigned short im_col;
   unsigned char zw_dir;
   short k;


   for (k = 0; k < 3; k ++)
   {
      neighbour (row, col, *curr_dir, &nb_row, &nb_col);

      if (bin_image [nb_row][nb_col] == 0)
      {
         /**************************************************************/
         /* Wir drehen so lange gegen den Uhrzeigersinn weiter, bis    */
         /* das Nachbarpixel auch zum Vordergrund gehört.              */
         /**************************************************************/
         
         symb_image [nb_row][nb_col] = inout;
         (*curr_dir) = DIRECTION_LEFT(*curr_dir);
      }
      else
         break;
   }

   /*********************************************************************/
   /* Es muss geprüft werden, ob das rechte Nachbarpixel des neuen      */
   /* Pixels auch zum Hintergrund gehört. Wenn nicht gehen wir gleich   */
   /* noch eins weiter "rechts um die Ecke".                            */
   /*********************************************************************/

   zw_dir = DIRECTION_RIGHT(*curr_dir);
   neighbour (nb_row, nb_col, zw_dir, &im_row, &im_col);

   if (bin_image [im_row][im_col] != 0)
   {
      symb_image [nb_row][nb_col] = label;
      append_new_simple_PIXELC_list (first, last);
      (*last) -> row = nb_row;
	  (*last) -> col = nb_col;

      nb_row = im_row;
      nb_col = im_col;
      *curr_dir = zw_dir;
   }
   else
      symb_image [im_row][im_col] = inout;

   *next_row = nb_row;
   *next_col = nb_col;
   symb_image [nb_row][nb_col] = label;

   append_new_simple_PIXELC_list (first, last);
   (*last) -> row = nb_row;
   (*last) -> col = nb_col;

   return 0;
}








static int trace_contour_4 (
                unsigned short start_row,
                unsigned short start_col,
                unsigned char **bin_image,
                long **symb_image,
                long curr_reg_nr,
                simple_REGIONC_list *reg_curr,
                char contour_orientation,
				char center)
{
   /********************************************************************/
   /* Um die Subregion herumlaufen und die Randpunkte abspeichern.     */
   /* contour_orientation = 1 bedeutet, gegen den Uhrzeigersinn um die */ 
   /* Vordergrundregion herumlaufen, d.h. der Hintergrund liegt        */ 
   /* rechts; es muss sich um eine äußere Randkette handeln.           */
   /* contour_orientation = -1 bedeutet, mit dem Uhrzeigersinn um die  */ 
   /* Hintergrundregion herumlaufen, d.h. der Hintergrund liegt        */
   /* rechts; es muss sich um eine innere Randkette handeln.           */
   /********************************************************************/


   /********************************************************************/
   /* "direction" bedeutet die Richtung entlang der Randkette, so dass */
   /* der Hintergrund jeweils rechts liegt. Die Richtung nach außen    */ 
   /* ist also:                                                        */ 
   /*    outward_direction = direction - 1                             */
   /* (start_row, start_col) bezieht sich jeweils auf das Pixel das    */
   /* schon / noch zum Vordergrund gehört! Vorsicht beim Aufruf!       */ 
   /********************************************************************/

   unsigned short row;
   unsigned short col;
   unsigned short nb_row;
   unsigned short nb_col;
   unsigned short next_row;
   unsigned short next_col;
   long label;
   long inout;
   unsigned char curr_dir;
   unsigned char dir_try;
   unsigned char dir_right;
   unsigned char direction_start;
   char k;
   simple_PIXELC_list *first;
   simple_PIXELC_list *last;


   inout = 2 - contour_orientation; /* nur für die Markierung des Randes im symbolischen Bild  */
   row = start_row;
   col = start_col;

   last = (simple_PIXELC_list *) malloc (sizeof(simple_PIXELC_list));
   memset (last, 0x00, sizeof(simple_PIXELC_list));
   first = last;

   if (contour_orientation == 1)
   {
      direction_start = 4;
      label = curr_reg_nr * 10 + 8;
      reg_curr -> first_pix = last;
      reg_curr -> last_pix = last;
      symb_image [row][col-1] = inout;
   }
   else 
   {
      direction_start = 2;
      label = curr_reg_nr * 10 + 5;
      reg_curr -> inner_last -> first_pix = last;
      reg_curr -> inner_last -> last_pix = last;
      symb_image [row][col+1] = inout;
   }

   symb_image [row][col] = label;

   /********************************************************************/
   /* Wir drehen hier so lange zurück (im Uhrzeigersinn), bis das      */
   /* Nachbarpixel in Gegenrichtung von direction_start ein            */
   /* Vordergrundpixel ist. direction_start ist damit die Richtung,    */
   /* bei der wir am Ende wieder herauskommen müssen.                  */
   /********************************************************************/

   for (k = 0; k < 3; k ++)
   {
      dir_try = DIRECTION_RIGHT(direction_start);  
      dir_right = DIRECTION_RIGHT(dir_try);  

      neighbour (row, col, dir_right, &nb_row, &nb_col);
      
      if (bin_image [nb_row][nb_col] == 0)
      {
         symb_image [nb_row][nb_col] = inout;
         direction_start = dir_try;
      }
      else
         break;
   }

   last -> row = row;
   last -> col = col;
   
   if (k == 3)
   {
      /*****************************************************************/
      /* Es handelt sich um ein einzelnes Vordergrundpixel             */
      /*****************************************************************/
      
      if (center == 0)
	  {
         set_contour_point (last, 1, row, col);

         for (curr_dir = 2; curr_dir < 5; curr_dir ++)
	     {
            append_new_simple_PIXELC_list (&first, &last);
		    set_contour_point (last, curr_dir, row, col);
	     }
	  }

      return 0;
   }

   curr_dir = direction_start;

   if (center == 0)
   {
      set_contour_point (last, direction_start, row, col);
   }
   else
   {
      last -> row = row;
      last -> col = col;
   }

   do
   {
      if (center == 0)
	  {
         next_border_pixel_0 (row, 
                              col, 
                              &curr_dir, 
                              bin_image,
                              symb_image,
                              inout,
                              label,
							  &first,
                              &last,
                              &next_row,
                              &next_col);
	  }
	  else
	  {
         next_border_pixel_1 (row, 
                              col, 
                              &curr_dir, 
                              bin_image,
                              symb_image,
                              inout,
                              label,
							  &first,
                              &last,
                              &next_row,
                              &next_col);
	  }

      row = next_row;
      col = next_col;
   }
   while (row != start_row || col != start_col || 
          curr_dir != direction_start);

   if (contour_orientation == 1)
      reg_curr -> last_pix = last;
   else 
      reg_curr -> inner_last -> last_pix = last;

   return 0;
}




/************************************************************************/
/* Die folgende Funktion setzt Regionen-Labels für ein Binärbild und    */
/* extrahiert die zugehörigen Randketten. Siehe Burger, Burge (2006):   */
/* Digitale Bildverarbeitung. 2. Auflage Springer Verlag S.208.         */
/*                                                                      */
/* Voraussetzung ist, dass die Randzeilen und Randspalten des Bildes    */
/* alle aus Hintergrundpixeln bestehen!                                 */
/************************************************************************/

#define ALLOC_INC  1000

int comb_contour_region_marking (
                unsigned short numrows,
                unsigned short numcols,
                unsigned char **bin_image,
                long **symb_image,
				simple_REGIONC_list **reg_first,
				simple_REGIONC_list **reg_last,
				char center)
{
   unsigned short row;
   unsigned short col;
   unsigned short row_prev;
   unsigned short col_prev;
   int last_reg_nr;
   int curr_reg_nr;
   long anz_alloc;
   long label;
   char status;
   char status_alt;
  
   simple_REGIONC_list *reg_new;
   simple_REGIONC_list **reg_vec;
   simple_REGIONC_list *reg_curr;
   simple_INNER_REGION_list *new_inner_reg;

   last_reg_nr = 0;
   *reg_first = NULL;
   *reg_last = NULL;

   /*********************************************************************/
   /* Wir benutzen folgende Labels im symbolischen Bild:                */
   /*   0 - Hintergrund                                                 */
   /*  x2 - inneres Pixel einer Region                                  */
   /*  x8 - äußeres Randpixel einer Region                              */
   /*  x5 - Randpixel eines inneren Randes einer Region                 */
   /*                                                                   */
   /* Diese Labels dienen nur der Visualisierung. Manche Pixel können   */
   /* Randpixel sowohl eines inneren Randes als auch des äußeren Randes */
   /* sein                                                              */
   /*********************************************************************/

   anz_alloc = ALLOC_INC;

   /*********************************************************************/
   /*  Der Vektor reg_vec dient dazu, eine Region als Element der       */
   /*  verketteten Liste simple_REGIONC_list aus der curr_reg_nr         */
   /*  aufzufinden.                                                     */
   /*********************************************************************/

   reg_vec = (simple_REGIONC_list **) malloc (anz_alloc *
                           sizeof (simple_REGIONC_list *));
   memset (reg_vec, 0x00, anz_alloc * sizeof (simple_REGIONC_list *));

   for (row = 1; row < numrows - 1; row ++)
   {
      status = 0;
      status_alt = 0;
      curr_reg_nr = 0;

      for (col = 1; col < numcols - 1; col ++)
      {
         status = bin_image [row][col];

         if (status_alt != status)
         {
            if (status_alt == 0 && symb_image [row][col] == 0)
            {
               /*********************************************************/
               /* Fall A: Übergang von einem Hintergrundpixel auf ein   */
               /* bisher nicht markiertes Vordergrundpixel              */
               /* Das muss ein äußerer Rand sein                        */
               /*********************************************************/

               last_reg_nr ++;
               curr_reg_nr = last_reg_nr;

               reg_new = (simple_REGIONC_list *)
                                     malloc (sizeof(simple_REGIONC_list));
               memset (reg_new, 0x00, sizeof(simple_REGIONC_list));
               reg_new -> region_nr = last_reg_nr;
               reg_curr = reg_new;

               append_simple_REGIONC_list (reg_first, reg_last, reg_new);

               if (last_reg_nr >= anz_alloc)
               {
                  reg_vec = (simple_REGIONC_list **) realloc (
                        reg_vec,
                        (anz_alloc + ALLOC_INC) *
                        sizeof (simple_REGIONC_list *));
                  memset (((char *) reg_vec) + 
                                anz_alloc * sizeof (simple_REGIONC_list *), 
                          0x00, 
                          ALLOC_INC);
                  anz_alloc += ALLOC_INC;
               }

               reg_vec [last_reg_nr] = reg_new;

               trace_contour_4 (row,
                                col, 
                                bin_image, 
                                symb_image, 
                                curr_reg_nr, 
                                reg_curr,
                                1,
								center);
            }
            else if (status_alt == 0 && symb_image [row][col] != 0)
            {
               /*********************************************************/
               /* Ein Randpixel, das früher mal erzeugt wurde. Das      */
               /* könnte ein äußerer oder auch ein innerer Rand sein.   */
               /*********************************************************/

               label = symb_image [row][col];

               if (label % 10 == 8)        /* äußerer Rand    */
                  curr_reg_nr = (label - 8) / 10;
               else if (label % 10 == 5)   /* innerer Rand    */
                  curr_reg_nr = (label - 5) / 10;

               reg_curr = reg_vec [curr_reg_nr];
            }
            else if (status_alt != 0 && status == 0)
            {
               row_prev = row;
               col_prev = col - 1;
               label = symb_image [row_prev][col_prev];

               if (symb_image [row][col] == 0)
               {
                  /******************************************************/
                  /* Es ist ein Randpixel (wegen status = 0), wurde     */
                  /* aber bisher nicht als solches erkannt.             */
                  /* Das muss ein Randpixel eines inneren Randes sein,  */
                  /* andernfalls wäre es früher schon erfasst worden    */
                  /* bzw. das Hintergrundpixel wäre schon markiert.     */
                  /******************************************************/
                 
                  curr_reg_nr = (label - 2) / 10;
                  reg_curr = reg_vec [curr_reg_nr];

                  new_inner_reg = (simple_INNER_REGION_list *)
                            malloc (sizeof (simple_INNER_REGION_list));
                  memset (new_inner_reg, 
                          0x00, 
                          sizeof (simple_INNER_REGION_list));

                  (reg_curr -> num_holes) ++;
                  append_simple_INNER_REGION_list (
                             &(reg_curr -> inner_first), 
                             &(reg_curr -> inner_last),
                             new_inner_reg);
                   
                  trace_contour_4 (row_prev,
                                   col_prev, 
                                   bin_image, 
                                   symb_image, 
                                   curr_reg_nr, 
                                   reg_curr,
                                   -1,
								   center);
               }
               else
               {
                  /******************************************************/
                  /* Ein Randpixel, das wir früher schon mal als        */
                  /* solches erfasst hatten. Da müssen wir nichts tun.  */
                  /******************************************************/

                  ;
               }
            }

            status_alt = status;
         } /* Der status hatte sich geändert    */
         else if (status_alt == 0)
            continue;
         else if (symb_image [row][col] != 0)
            continue;
         else
            symb_image [row][col] = curr_reg_nr * 10 + 2; 
      }
   }
   
   free (reg_vec);

   return 0;
}





