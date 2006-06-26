
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Kriging                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Kriging_Ordinary_Global.h               //
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
#ifndef HEADER_INCLUDED__Kriging_Ordinary_Global_H
#define HEADER_INCLUDED__Kriging_Ordinary_Global_H

//---------------------------------------------------------


//---------------------------------------------------------
#include "Kriging_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CKriging_Ordinary_Global : public CModule
{
public:
	CKriging_Ordinary_Global(void);
	virtual ~CKriging_Ordinary_Global(void);


protected:

	virtual bool			On_Execute(void);


private:

	bool					bLogarithmic;

	int						Model, nPoints;

	double					*Gammas, **Weights,
							Nugget, Scale, Range,
							BLIN, BEXP, APOW, BPOW,
							NoData;

	TGEO_Position			*Points;

	CGrid					*pGrid, *pVariance;


	CGrid *					Get_Target_Grid(CParameters *pParameters, CShapes *pShapes);

	double					Get_Weight (double Distance);
	int						Get_Weights(CShapes *pShapes, int zField);

	bool					Get_Grid_Value(int x, int y);

};

#endif // #ifndef HEADER_INCLUDED__Kriging_Ordinary_Global_H
