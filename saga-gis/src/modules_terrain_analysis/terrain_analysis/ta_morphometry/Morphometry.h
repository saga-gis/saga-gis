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
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Morphometry.h                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Morphometry_H
#define HEADER_INCLUDED__Morphometry_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class ta_morphometry_EXPORT CMorphometry : public CSG_Module_Grid
{
public:
	CMorphometry(void);
	virtual ~CMorphometry(void);


protected:

	virtual bool			On_Execute				(void);


private:

	double					_DX_2, _4DX_2, _6DX, _2DX;

	CSG_Grid					*pDTM, *pSlope, *pAspect, *pCurvature, *pCurv_Horz, *pCurv_Vert, *pCurv_Tang;

	//-----------------------------------------------------
	void					Set_Parameters			(int x, int y, double Slope, double Aspect, double Curv = 0.0, double vCurv = 0.0, double hCurv = 0.0, double tCurv = 0.0);
	void					Set_Parameters_Derive	(int x, int y, double D, double E, double F, double G, double H);
	void					Set_Parameters_NoData	(int x, int y, bool bCompletely = false);

	bool					Get_SubMatrix3x3		(int x, int y, double SubMatrix[ 9]);
	bool					Get_SubMatrix5x5		(int x, int y, double SubMatrix[25]);

	//-----------------------------------------------------
	void					Do_MaximumSlope			(int x, int y );
	void					Do_Tarboton				(int x, int y );

	void					Do_LeastSquare			(int x, int y );

	void					Do_FD_BRM				(int x, int y );
	void					Do_FD_Heerdegen			(int x, int y );
	void					Do_FD_Zevenbergen		(int x, int y );

	void					Do_FD_Haralick			(int x, int y );

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Morphometry_H
