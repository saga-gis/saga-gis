/**********************************************************
 * Version $Id: skeletonization.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   skeletonization.h                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef HEADER_INCLUDED__skeletonization_H
#define HEADER_INCLUDED__skeletonization_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSkeletonization : public CSG_Module_Grid
{
public:
	CSkeletonization(void);


protected:

	virtual bool		On_Execute			(void);


private:

	CSG_Grid			*pResult;


	int					Get_Neighbours		(int x, int y, CSG_Grid *pGrid, bool Neighbours[8]);

	int					Vectorize			(CSG_Shapes *pShapes);
	bool				Vectorize_Trace		(int x, int y, CSG_Shape *pShape);

	void				Standard_Execute	(void);
	int					Standard_Step		(int iDir, CSG_Grid *pPrev, CSG_Grid *pNext);
	bool				Standard_Check		(int iDir, bool z[8]);

	void				Hilditch_Execute	(void);
	int					Hilditch_Step		(CSG_Grid *pPrev, CSG_Grid *pNext, CSG_Grid *pNC_Gaps);
	bool				Hilditch_Check		(CSG_Grid *pNC_Gaps, int x, int y, int i0, bool z[8]);

	void				SK_Execute			(void);
	int					SK_Connectivity		(int NB[8]);
	bool				SK_Filter			(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__skeletonization_H
