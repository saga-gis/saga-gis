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
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             TIN_From_Grid_Specific_Points.h           //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__TIN_From_Grid_Specific_Points_H
#define HEADER_INCLUDED__TIN_From_Grid_Specific_Points_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTIN_From_Grid_Specific_Points : public CSG_Tool_Grid
{
public:
	CTIN_From_Grid_Specific_Points(void);
	virtual ~CTIN_From_Grid_Specific_Points(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("Conversion") );	}


protected:

	virtual bool				On_Execute		(void);


private:

	bool						Get_MarkHighestNB	(CSG_Grid *pResult, CSG_Grid *pGrid);
	bool						Get_OppositeNB		(CSG_Grid *pResult, CSG_Grid *pGrid, int Threshold);
	bool						Get_FlowDirection	(CSG_Grid *pResult, CSG_Grid *pGrid, int Min, int Max);
	bool						Get_FlowDirection2	(CSG_Grid *pResult, CSG_Grid *pGrid, int Threshold);
	bool						Get_Peucker			(CSG_Grid *pResult, CSG_Grid *pGrid, double Threshold);

};

#endif // #ifndef HEADER_INCLUDED__TIN_From_Grid_Specific_Points_H
