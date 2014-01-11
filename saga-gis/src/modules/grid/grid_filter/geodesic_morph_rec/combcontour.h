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
//                     combcontour.h                     //
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
typedef struct simple_PIXELC_list
{
   unsigned short row;
   unsigned short col;
   
   struct simple_PIXELC_list *next;
}
simple_PIXELC_list;


typedef struct double_PIXELC_list
{
   unsigned short row;
   unsigned short col;
   
   struct double_PIXELC_list *prev;
   struct double_PIXELC_list *next;
}
double_PIXELC_list;




typedef struct simple_REGIONC_list
{
   struct simple_REGIONC_list *next;
   long region_nr;

   struct simple_PIXELC_list *first_pix;
   struct simple_PIXELC_list *last_pix;
   short num_holes;
   struct simple_INNER_REGION_list *inner_first;
   struct simple_INNER_REGION_list *inner_last;
}
simple_REGIONC_list;



typedef struct double_REGIONC_list
{
   struct double_REGIONC_list *prev;
   struct double_REGIONC_list *next;
   long region_nr;

   struct double_PIXELC_list *first_pix;
   struct double_PIXELC_list *last_pix;
   short num_holes;
   struct double_INNER_REGION_list *inner_first;
   struct double_INNER_REGION_list *inner_last;
}
double_REGIONC_list;


typedef struct simple_INNER_REGION_list
{
   struct simple_INNER_REGION_list *next;

   struct simple_PIXELC_list *first_pix;
   struct simple_PIXELC_list *last_pix;
}
simple_INNER_REGION_list;



typedef struct double_INNER_REGION_list
{
   struct double_INNER_REGION_list *prev;
   struct double_INNER_REGION_list *next;

   struct double_PIXELC_list *first_pix;
   struct double_PIXELC_list *last_pix;
}
double_INNER_REGION_list;


int background_region_growing (unsigned char **bin_image, 
	                           unsigned short numrows, 
							   unsigned short numcols, 
							   unsigned short row, 
							   unsigned short col);

int free_regions (simple_REGIONC_list **first_region, simple_REGIONC_list **last_region);

int comb_contour_region_marking (
                unsigned short numrows,
                unsigned short numcols,
                unsigned char **bin_image,
                long **symb_image,
                simple_REGIONC_list **reg_first,
				simple_REGIONC_list **reg_last,
				char center);


