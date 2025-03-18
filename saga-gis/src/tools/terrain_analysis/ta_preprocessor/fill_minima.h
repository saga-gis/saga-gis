
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
//                     fill_minima.h                     //
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

//---------------------------------------------------------
#pragma once


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>
#include <limits.h>
#include <memory>
#include <vector>
#include <queue>
#include <utility>

/*
template<typename T>
struct pixel 
{
    sLong x, y;
	T height;
};

template<typename T>
struct pixelQueue
{
	T height_min;
	T number_levels;
	
	std::vector<std::shared_ptr<std::queue<pixel<T>>>> data;
};

//using pixelQueue = std::vector<std::queue<pixel<int>>>;
using pixelQueuePtr = std::shared_ptr<pixelQueue<int>>;
*/


//pixelQueuePtr PQ_init(int hMin, int hMax) {
//    pixelQueuePtr pixQ = std::make_shared<pixelQueue<int>>();
//        
//    int numLevels = hMax - hMin + 1;
//    
//	pixQ->height_min = hMin;
//    pixQ->number_levels = numLevels;
//    pixQ->data.reserve(numLevels);
//    
//	for( std::shared_ptr<std::queue<pixel<int>>> height : pixQ->data )
//		height = std::make_shared<std::queue<pixel<int>>>();
//		
//	return pixQ;
//}





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
static PQel *neighbours(PQel *p, int bRows, int nRows, int bCols, int nCols) {
    int ii, jj, i, j;
    PQel *pl, *pNew;
        
    pl = NULL;
    for (ii=-1; ii<=1; ii++) {
        for (jj=-1; jj<=1; jj++) {
            if (!(ii == 0 && jj == 0)) {
                i = p->i + ii;
                j = p->j + jj;
                if ((i >= bRows) && (i < nRows) && (j >= bCols) && (j < nCols)) {
                    pNew = newPix(i, j);
                    pNew->next = pl;
                    pl = pNew;
                }
            }
        }
    }
    return pl;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFillMinima : public CSG_Tool_Grid
{
public:
	CFillMinima(void);


protected:

	virtual bool		On_Execute		(void);
	
	PixelQueue* 		Create_Queue	( CSG_Grid *pInput, CSG_Grid *pOutput, int hMin, int hMax, int BoundaryVal, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd);
	bool 				Fill_Queue 		( CSG_Grid *pInput, CSG_Grid *pOutput, PixelQueue *pixQ, int hMin, int hMax, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd );


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
