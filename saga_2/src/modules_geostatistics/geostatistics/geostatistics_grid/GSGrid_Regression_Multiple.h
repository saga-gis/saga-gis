
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              GSGrid_Regression_Multiple.h             //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
#ifndef HEADER_INCLUDED__GSGrid_Regression_Multiple_H
#define HEADER_INCLUDED__GSGrid_Regression_Multiple_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGSGrid_Regression_Multiple : public CSG_Module_Grid
{
public:
	CGSGrid_Regression_Multiple(void);
	virtual ~CGSGrid_Regression_Multiple(void);

	virtual const SG_Char *		Get_MenuPath		(void)	{	return( _TL("R:Regression Analysis") );	}


protected:

	virtual bool				On_Execute			(void);


private:

	int							m_Interpolation;

	CSG_Regression_Multiple		m_Regression;


	bool						Get_Regression		(CSG_Parameter_Grid_List *pGrids, CSG_Shapes *pShapes, int iAttribute);
	bool						Set_Regression		(CSG_Parameter_Grid_List *pGrids, CSG_Grid *pRegression);
	bool						Set_Residuals		(CSG_Shapes *pShapes, int iAttribute, CSG_Shapes *pResiduals, CSG_Grid *pRegression);
	void						Set_Message			(CSG_Parameter_Grid_List *pGrids);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__GSGrid_Regression_Multiple_H
