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
//                     bin_geovinc.c                     //
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "storeorg.h"
#include "bin_geovinc.h"


/************************************************************************/
/* Funktioniert mit listfuncs.in2                                       */
/************************************************************************/

#define LISTELEMENT CHAR_PIXEL
#include "listfuncs.inc"
#undef LISTELEMENT




static int find_border_pixels (unsigned short numrows, 
                               unsigned short numcols,
                               simple_CHAR_PIXEL_list **first_pix,
                               simple_CHAR_PIXEL_list **last_pix,
                               char **mask,
							   char **marker)
{
   /*******************************************************************/
   /* Es wird eine Liste von Vordergrund-Pixeln (die selbst den Wert  */
   /* 1 haben) erzeugt, die mindestens ein Hintergrund-Pixel als      */
   /* Nachbarn aufweisen.                                             */
   /*******************************************************************/
   
   unsigned short row;
   unsigned short col;
   unsigned short nrow;
   unsigned short ncol;

   unsigned short rowmin;
   unsigned short colmin;
   unsigned short rowmax;
   unsigned short colmax;

   short fflag;


   for (row = 0; row < numrows; row ++)
   {
      for (col = 0; col < numcols; col ++)
      {
         if (marker [row][col] != 1)
            continue;

         rowmin = (row == 0) ? 0 : row - 1;
         colmin = (col == 0) ? 0 : col - 1;
         rowmax = (row == numrows - 1) ? row : row + 1;
         colmax = (col == numcols - 1) ? col : col + 1;
		 fflag = 0;

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
            
            
               if (marker [nrow][ncol] == 0 && mask [nrow][ncol] == 1)
			   {
				  /* Hier kann man propagieren */

                  fflag = 1;
				  break;
			   }
			}

			if (fflag == 1)
			   break;
		 }


         if (fflag == 0)
            continue;

         append_new_simple_CHAR_PIXEL_list (first_pix, last_pix);
		 (*last_pix) -> row = row;
		 (*last_pix) -> col = ncol;
	  }
   }

   return 0;
}




int binary_geodesic_morphological_reconstruction (
                unsigned short numrows,
                unsigned short numcols,
                char **mask,
                char **marker)
{
   simple_CHAR_PIXEL_list *first_pix = NULL;
   simple_CHAR_PIXEL_list *last_pix = NULL;
   simple_CHAR_PIXEL_list *curr_pix;

   unsigned short rowmin;
   unsigned short colmin;
   unsigned short rowmax;
   unsigned short colmax;
   unsigned short row;
   unsigned short col;
   unsigned short nrow;
   unsigned short ncol;


   find_border_pixels (numrows, numcols, &first_pix, &last_pix, mask, marker);
   

   /*********************************************************************/
   /* Hier kommt jetzt die alles entscheidende Schleife, in der die     */
   /* Dilatation vorgenommen wird. Das vorderste Element der Pixelliste */
   /* wird herausgenommen; seine Nachbarn werden in die Pixelliste      */
   /* einsortiert, falls ihr Wert geaendert werden kann.                */
   /*********************************************************************/

   for (curr_pix = first_pix; curr_pix != NULL;  curr_pix = first_pix)
   {
      row = curr_pix -> row;
      col = curr_pix -> col;
	  delete_first_simple_CHAR_PIXEL (&first_pix, &last_pix);
	  
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

	 	    if (marker [nrow][ncol] == 1 || mask [nrow][ncol] != 1)
				continue;

            marker [nrow][ncol] = 1;
            append_new_simple_CHAR_PIXEL_list (&first_pix, &last_pix);
		    last_pix -> row = nrow;
		    last_pix -> col = ncol;
         }
      }
   }

   return 0;
}


