
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    fill_minima.cpp                    //
//                                                       //
//              Copyright (C) 2015-2023 by               //
//                   	Neil Flood                       //
//                  Justus Spitzmüller                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 3 of the     //
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
//    e-mail: j.spitzmueller@uni-goettingen.de           //
//                                                       //
//    contact:    Justus Spitzmüller                     //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////



/* This file is part of 'python-fmask' - a cloud masking module
* Copyright (C) 2015  Neil Flood
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*
Module to implement filling of local minima in a raster surface. 

The algorithm is from 
    Soille, P., and Gratin, C. (1994). An efficient algorithm for drainage network
        extraction on DEMs. J. Visual Communication and Image Representation. 
        5(2). 181-189. 
        
The algorithm is intended for hydrological processing of a DEM, but is used by the 
Fmask cloud shadow algorithm as part of its process for finding local minima which 
represent potential shadow objects. 
*/
#include "fill_minima.h"
#include <math.h>


/* Routines for handling the hierarchical pixel queue which the
   algorithm requires.
*/
typedef struct PQstruct {
    int i, j;
    struct PQstruct *next;
} PQel;
    
typedef struct {
    PQel *first, *last;
    int n;
} PQhdr;
    
typedef struct PQ {
    int hMin;
    int numLevels;
    PQhdr *q;
} PixelQueue;

/* A new pixel structure */
static PQel *newPix(int i, int j) {
    PQel *p;
        
    p = (PQel *)calloc(1, sizeof(PQel));
    p->i = i;
    p->j = j;
    p->next = NULL;
    if (i>20000) {
        //printf("i=%d\\n", i);
        exit(1);
    }
    return p;
}
    
/* Initialize pixel queue */
static PixelQueue *PQ_init(int hMin, int hMax) {
    PixelQueue *pixQ;
    int numLevels, i;
        
    pixQ = (PixelQueue *)calloc(1, sizeof(PixelQueue));
    numLevels = hMax - hMin + 1;
    pixQ->hMin = hMin;
    pixQ->numLevels = numLevels;
    pixQ->q = (PQhdr *)calloc(numLevels, sizeof(PQhdr));
    for (i=0; i<numLevels; i++) {
        pixQ->q[i].first = NULL;
        pixQ->q[i].last = NULL;
        pixQ->q[i].n = 0;
    }
    return pixQ;
}
    
/* Add a pixel at level h */
static void PQ_add(PixelQueue *pixQ, PQel *p, int h) {
    int ndx;
    PQel *current, *newP;
    PQhdr *thisQ;
        
    /* Take a copy of the pixel structure */
    newP = newPix(p->i, p->j);
        
    ndx = h - pixQ->hMin;
    if (ndx > pixQ->numLevels) {
        //printf("Level h=%d too large. ndx=%d, numLevels=%d\\n", h, ndx, pixQ->numLevels);
        exit(1);
    }
    if (ndx < 0) {
        //printf("Ndx is negative, which is not allowed. ndx=%d, h=%d, hMin=%d\n", ndx, h, pixQ->hMin);
        exit(1);
    }
    thisQ = &(pixQ->q[ndx]);
    /* Add to end of queue at this level */
    current = thisQ->last;
    if (current != NULL) {
        current->next = newP;
    }
    thisQ->last = newP;
    thisQ->n++;
    /* If head of queue is NULL, make the new one the head */
    if (thisQ->first == NULL) {
        thisQ->first = newP;
    }
}
    
/* Return TRUE if queue at level h is empty */
static int PQ_empty(PixelQueue *pixQ, int h) {
    int ndx, empty, n;
    PQel *current;
        
    ndx = h - pixQ->hMin;
    current = pixQ->q[ndx].first;
    n = pixQ->q[ndx].n;
    empty = (current == NULL);
    if (empty && (n != 0)) {
        //printf("Empty, but n=%d\\n", n);
        exit(1);
    }
    if ((n == 0) && (! empty)) {
        //printf("n=0, but not empty\\n");
        while (current != NULL) {
            //printf("    h=%d i=%d j=%d\\n", h, current->i, current->j);
            current = current->next;
        }
        exit(1);
    }
    return empty;
}
    
/* Return the first element in the queue at level h, and remove it
   from the queue */
static PQel *PQ_first(PixelQueue *pixQ, int h) {
    int ndx;
    PQel *current;
    PQhdr *thisQ;
        
    ndx = h - pixQ->hMin;
    thisQ = &(pixQ->q[ndx]);
    current = thisQ->first;
    /* Remove from head of queue */
    if (current != NULL) {
        thisQ->first = current->next;
        if (thisQ->first == NULL) {
            thisQ->last = NULL;
        }
        thisQ->n--;
        if (thisQ->n < 0) {
            //printf("n=%d in PQ_first()\\n", thisQ->n);
            exit(1);
        } else if (thisQ->n == 0) {
            if (thisQ->first != NULL) {
                //printf("n=0, but 'first' != NULL. first(i,j) = %d,%d\\n", 
                //   thisQ->first->i, thisQ->first->j);
            }
        }
    }
    return current;
}
    
/* Return a list of neighbouring pixels to given pixel p.  */
static PQel *neighbours(PQel *p, int nRows, int nCols) {
    int ii, jj, i, j;
    PQel *pl, *pNew;
        
    pl = NULL;
    for (ii=-1; ii<=1; ii++) {
        for (jj=-1; jj<=1; jj++) {
            if ((ii != 0) && (jj != 0)) {
                i = p->i + ii;
                j = p->j + jj;
                if ((i >= 0) && (i < nRows) && (j >= 0) && (j < nCols)) {
                    pNew = newPix(i, j);
                    pNew->next = pl;
                    pl = pNew;
                }
            }
        }
    }
    return pl;
}


CFillMinima::CFillMinima(void)
{
	Set_Name 		(_TL("Fill Minima"));

	Set_Author 		("Neil Flood (c) 2015, Justus Spitzmueller (c) 2023");

	Set_Version 	("1.0");

	Set_Description(_TW(
		"Minima filling. Currently only for unsigned 1 byte integer grids."
	));

	Add_Reference("https://www.pythonfmask.org/",
		SG_T("Python Fmask")
	);

    Add_Reference("Soille, P., & C. Gratin", "1994", 
		"An efficient algorithm for drainage network extraction on DEMs.",
		"J. Visual Communication and Image Representation. 5(2): 181-189."
	);
	
	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("DEM"),
		_TL("digital elevation model [m]"),
		PARAMETER_INPUT, true, SG_DATATYPE_Char 
	);

	Parameters.Add_Grid(
		"", "RESULT"	, _TL("Filled DEM"),
		_TL("processed DEM"),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);
}


#define max(a,b) ((a) > (b) ? (a) : (b))


static int	xnb[] = { 0,-1, 1, 0,-1, 1,-1, 1};
static int	ynb[] = {-1, 0, 0, 1,-1,-1, 1, 1};

bool CFillMinima::On_Execute(void)
{
	CSG_Grid *pInput = Parameters("DEM")->asGrid();
	CSG_Grid *pImg2 = Parameters("RESULT")->asGrid();

	pImg2->Set_NoData_Value(pInput->Get_NoData_Value());
	pImg2->Set_Scaling( pInput->Get_Scaling(), pInput->Get_Offset() );

	// boundaryval?
	int hMin = 0;
	int hMax = 0;
	for( sLong i=0; i<pInput->Get_NCells(); i++ )
	{
		int value = pInput->asInt(i, false);
		hMin = std::min( hMin, value );
		hMax = max( hMax, value );
	}
	//double dBoundaryVal = pInput->Get_Scaling();
	double dBoundaryVal = hMin;



	//for( int i=0; i<Get_NX(); i ++ )
	//{
	//	int y = Get_NY()-1;
	//	pImg2->Set_Value( i, 0, pInput->asDouble(i, 0));		
	//	pImg2->Set_Value( i, y, pInput->asDouble(i, y));		
	//}

	//for( int i=0; i<Get_NY(); i ++ )
	//{
	//	int x = Get_NX()-1;
	//	pImg2->Set_Value( 0, i, pInput->asDouble(0, i));		
	//	pImg2->Set_Value( x, i, pInput->asDouble(x, i));		
	//}
	
	PixelQueue *pixQ = PQ_init(hMin,hMax);
	for( int y=0; y<pInput->Get_NY(); y++ )
	{
		for( int x=0; x<pInput->Get_NX(); x++ )
		{
			if( pInput->is_NoData(x,y) )
			{
				int max = 0;
				int count = 0;
				for(int i=0; i<8; i++)
				{
					int xn = x + xnb[i];
					int yn = y + ynb[i];

					if( xn >= 0 && xn < Get_NX() && yn >= 0 && yn < Get_NY() )
					{
						if( !pInput->is_NoData(xn,yn) )
						{
							count++;
							max = max( max, pInput->asInt(xn, yn, false) );
						}
					}
				}
				if( count > 0 )
				{
					//pImg2->Set_Value(x,y, max); 
					//p = newPix(r, c);
					PQel *p = newPix(y, x); // does the row/cols matter?
					//h = img(r, c);
					//PQ_add(pixQ, p, img(r, c));
					PQ_add(pixQ, p, max);
				}
				else
	  			{
					pImg2->Set_NoData(x,y);
				}
			}
			else
	  		{
				pImg2->Set_Value(x,y, hMax, false);
			}
		}
	}

	//for( int y=0; y<Get_NY(); y ++ )
	//{
	//	for( int x=0; x<Get_NX(); x ++ )
	//	{
	//		if( pInput->is_NoData(x,y) )
	//		{
	//			//p = newPix(r, c);
	//			PQel *p = newPix(y, x); // does the row/cols matter?
	//			//h = img(r, c);
	//			//PQ_add(pixQ, p, img(r, c));
	//			PQ_add(pixQ, p, dBoundaryVal);

	//		}
	//	}
	//}

    /* Process until stability */
    int hCrt = (int)hMin;
    do 
	{
        while (! PQ_empty(pixQ, hCrt)) 
		{
            PQel *p = PQ_first(pixQ, hCrt);
            PQel *nbrs = neighbours(p, Get_NY(), Get_NX());
            PQel *pNbr = nbrs;
            while (pNbr != NULL) 
			{
                int r = pNbr->i;
                int c = pNbr->j;
                /* Exclude null area of original image */
                //if (! *((npy_bool*)PyArray_GETPTR2(pNullMask, r, c)))
                if ( !pInput->is_NoData(c, r) )
				{
                    int imgval = pInput->asInt(c,r, false);
                    int img2val = pImg2->asInt(c,r, false);
                    if (img2val == hMax) 
					{
                        img2val = max(hCrt, imgval);
                        //*((npy_int16*)PyArray_GETPTR2(pimg2, r, c)) = img2val;
						pImg2->Set_Value(c,r, img2val, false);
                        PQ_add(pixQ, pNbr, img2val);
                    }
                }
                PQel *pNext = pNbr->next;
                free(pNbr);
                pNbr = pNext;
            }
            free(p);
        }
        hCrt++;
    } while (hCrt < hMax);
    
    free(pixQ);

	return true;
}
