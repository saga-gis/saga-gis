
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Flow_RecursiveDown.h                  //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Flow_RecursiveDown_H
#define HEADER_INCLUDED__Flow_RecursiveDown_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class ta_hydrology_EXPORT CFlow_RecursiveDown : public CFlow  
{
public:
	CFlow_RecursiveDown(void);
	virtual ~CFlow_RecursiveDown(void);


protected:

	virtual void			On_Initialize	(void);
	virtual void			On_Finalize		(void);

	virtual void			Calculate		(void);
	virtual void			Calculate		(int x, int y);


private:

	bool					bFlowPathWeight;

	int						Method;

	double					Src_Height, Src_Slope, DEMON_minDQV;

	CSG_Grid					*pLinear, *pDir, *pDif;


	void					Add_Flow		(int x, int y, double qFlow);

	void					Rho8_Start		(int x, int y, double qFlow);

	void					KRA_Start		(int x, int y, double qFlow);
	void					KRA_Trace		(int x, int y, double qFlow, int Direction, double from);

	void					DEMON_Start		(int x, int y, double qFlow);
	void					DEMON_Trace		(int x, int y, double qFlow, int Direction, double from_A, double from_B);
};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Flow_RecursiveDown_H
