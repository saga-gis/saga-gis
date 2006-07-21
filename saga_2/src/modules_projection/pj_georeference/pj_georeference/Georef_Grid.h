
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Grid_Georeference                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Georef_Grid.h                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringel@saga-gis.org                   //
//                                                       //
//    contact:    Andre Ringeler                         //
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
#ifndef HEADER_INCLUDED__Georef_Grid_H
#define HEADER_INCLUDED__Georef_Grid_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

#include "Georef_Engine.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGeoref_Grid : public CModule 
{
public:
	CGeoref_Grid(void);
	virtual ~CGeoref_Grid(void);


protected:

	virtual int				On_Parameter_Changed		(CParameters *pParameters, CParameter *pParameter);

	virtual bool			On_Execute					(void);

private:

	CGeoref_Engine			m_Engine;


	bool					Get_Conversion				(void);

	void					Get_MinMax					(double &xMin, double &xMax, double &yMin, double &yMax, TSG_Point Point);
	CGrid *					Get_Target_Userdef			(CGrid *pSource, TGrid_Type Type);
	CGrid *					Get_Target_Autofit			(CGrid *pSource, double Grid_Size, int AutoExtMode, TGrid_Type Type);

	bool					Set_Grid					(CGrid *pSource, CGrid   *pTarget, int Interpolation);
	bool					Set_Shapes					(CGrid *pSource, CShapes *pTarget);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Georef_Grid_H
